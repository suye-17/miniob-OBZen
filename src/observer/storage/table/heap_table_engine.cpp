/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "storage/table/heap_table_engine.h"
#include "storage/record/heap_record_scanner.h"
#include "common/log/log.h"
#include "common/types.h"
#include "storage/index/bplus_tree_index.h"
#include "storage/common/meta_util.h"
#include "storage/db/db.h"
#include "storage/buffer/page_type.h"

HeapTableEngine::~HeapTableEngine()
{
  if (record_handler_ != nullptr) {
    delete record_handler_;
    record_handler_ = nullptr;
  }

  if (data_buffer_pool_ != nullptr) {
    data_buffer_pool_->close_file();
    data_buffer_pool_ = nullptr;
  }

  for (vector<Index *>::iterator it = indexes_.begin(); it != indexes_.end(); ++it) {
    Index *index = *it;
    delete index;
  }
  indexes_.clear();

  LOG_DEBUG("Table has been closed: %s", table_meta_->name());
}
RC HeapTableEngine::insert_record(Record &record)
{
  RC rc = RC::SUCCESS;
  
  // 处理TEXT字段的overflow pages
  // 如果record中包含extended TEXT格式，需要先创建overflow pages
  const std::vector<FieldMeta> *field_metas = table_meta_->field_metas();
  int base_record_size = table_meta_->record_size();
  
  // 创建实际要插入的record（只包含base_record_size）
  char *final_record_data = (char *)malloc(base_record_size);
  memcpy(final_record_data, record.data(), base_record_size);
  
  for (const FieldMeta &field_meta : *field_metas) {
    if (field_meta.type() != AttrType::TEXTS) {
      continue;
    }
    
    const char *field_data = record.data() + field_meta.offset();
    uint32_t marker = 0;
    memcpy(&marker, field_data, sizeof(uint32_t));
    
    
    // 处理extended TEXT格式（来自Table::make_record）
    if (marker == 0xFFFFFFFF) {
      uint32_t full_length = 0;
      uint32_t data_offset_in_record = 0;
      memcpy(&full_length, field_data + 4, sizeof(uint32_t));
      memcpy(&data_offset_in_record, field_data + 8, sizeof(uint32_t));
      
      const char *actual_text_data = record.data() + data_offset_in_record;
      size_t inline_capacity = std::min(static_cast<size_t>(INLINE_TEXT_CAPACITY), 
                                        static_cast<size_t>(field_meta.len() - 16));
      
      if (full_length <= inline_capacity) {
        memcpy(final_record_data + field_meta.offset(), actual_text_data, full_length);
        if (full_length < static_cast<size_t>(field_meta.len())) {
          final_record_data[field_meta.offset() + full_length] = '\0';
        }
      } else {
        // 长TEXT：写入inline + 创建overflow pages
        // 确保不会读取超过TEXT_MAX_LENGTH的数据
        uint32_t safe_length = std::min(full_length, static_cast<uint32_t>(TEXT_MAX_LENGTH));
        size_t remaining = safe_length;
        const char *src_ptr = actual_text_data;
        
        memcpy(final_record_data + field_meta.offset(), src_ptr, inline_capacity);
        src_ptr += inline_capacity;
        remaining -= inline_capacity;
        
        PageNum first_page_num = -1;
        PageNum current_page_num = -1;
        PageNum prev_page_num = -1;
        
        while (remaining > 0) {
          Frame *frame = nullptr;
          rc = record_handler_->allocate_overflow_page(&frame);
          if (rc != RC::SUCCESS) {
            LOG_ERROR("Failed to allocate overflow page for TEXT field");
            free(final_record_data);
            return rc;
          }
          
          current_page_num = frame->page_num();
          
          if (first_page_num == -1) {
            first_page_num = current_page_num;
          }
          
          char *page_data = frame->data();
          OverflowPageHeader *header = reinterpret_cast<OverflowPageHeader *>(page_data);
          header->page_type = PageType::TEXT_OVERFLOW;
          header->next_page = -1;
          
          size_t page_capacity = record_handler_->get_overflow_page_capacity();
          size_t to_write = (remaining > page_capacity) ? page_capacity : remaining;
          
          header->data_length = static_cast<uint32_t>(to_write);
          memcpy(page_data + sizeof(OverflowPageHeader), src_ptr, to_write);
          
          frame->mark_dirty();
          data_buffer_pool_->unpin_page(frame);
          
          if (prev_page_num != -1) {
            Frame *prev_frame = nullptr;
            rc = data_buffer_pool_->get_this_page(prev_page_num, &prev_frame);
            if (rc == RC::SUCCESS) {
              OverflowPageHeader *prev_header = reinterpret_cast<OverflowPageHeader *>(prev_frame->data());
              prev_header->next_page = current_page_num;
              prev_frame->mark_dirty();
              data_buffer_pool_->unpin_page(prev_frame);
            }
          }
          
          prev_page_num = current_page_num;
          src_ptr += to_write;
          remaining -= to_write;
        }
        
        // 写入overflow pointer: [table_id][page_num][offset][total_length]
        char *overflow_ptr = final_record_data + field_meta.offset() + inline_capacity;
        uint32_t table_id = table_meta_->table_id();
        uint32_t offset = sizeof(OverflowPageHeader);
        
        memcpy(overflow_ptr, &table_id, sizeof(uint32_t));
        memcpy(overflow_ptr + 4, &first_page_num, sizeof(PageNum));
        memcpy(overflow_ptr + 8, &offset, sizeof(uint32_t));
        memcpy(overflow_ptr + 12, &safe_length, sizeof(uint32_t));
      }
    }
  }
  
  // 插入最终的record
  rc = record_handler_->insert_record(final_record_data, base_record_size, &record.rid());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Insert record failed. table name=%s, rc=%s", table_meta_->name(), strrc(rc));
    free(final_record_data);
    return rc;
  }
  
  // 更新record的data为final_record_data（用于后续的索引操作）
  record.set_data_owner(final_record_data, base_record_size);

  rc = insert_entry_of_indexes(record.data(), record.rid());
  if (rc != RC::SUCCESS) {  // 可能出现了键值重复
    RC rc2 = delete_entry_of_indexes(record.data(), record.rid(), false /*error_on_not_exists*/);
    if (rc2 != RC::SUCCESS) {
      LOG_ERROR("Failed to rollback index data when insert index entries failed. table name=%s, rc=%d:%s",
                table_meta_->name(), rc2, strrc(rc2));
    }
    rc2 = record_handler_->delete_record(&record.rid());
    if (rc2 != RC::SUCCESS) {
      LOG_PANIC("Failed to rollback record data when insert index entries failed. table name=%s, rc=%d:%s",
                table_meta_->name(), rc2, strrc(rc2));
    }
  }
  return rc;
}

RC HeapTableEngine::visit_record(const RID &rid, function<bool(Record &)> visitor)
{
  return record_handler_->visit_record(rid, visitor);
}

RC HeapTableEngine::get_record(const RID &rid, Record &record)
{
  RC rc = record_handler_->get_record(rid, record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to visit record. rid=%s, table=%s, rc=%s", rid.to_string().c_str(), table_meta_->name(), strrc(rc));
    return rc;
  }

  return rc;
}

RC HeapTableEngine::delete_record(const Record &record)
{
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->delete_entry(record.data(), &record.rid());
    if (RC::SUCCESS != rc) {
      LOG_WARN("failed to delete entry from index. table=%s, index=%s, rid=%s, rc=%s", 
               table_meta_->name(), index->index_meta().name(), 
               record.rid().to_string().c_str(), strrc(rc));
      return rc;
    }
  }

  rc = record_handler_->delete_record(&record.rid());
  return rc;
}

/**
 * @brief 更新记录
 * @details 更新指定的记录，包含事务支持和索引维护
 *
 * 步骤：
 * 1. 删除旧记录在所有索引中的条目
 * 2. 更新记录的实际数据
 * 3. 在所有索引中插入新记录的条目
 *
 * @param old_record 旧记录
 * @param new_record 新记录（包含更新后的数据）
 * @param trx 事务对象
 * @return RC 操作结果码
 */
RC HeapTableEngine::update_record_with_trx(const Record &old_record, const Record &new_record, Trx *trx)
{
  RC rc = RC::SUCCESS;

  // 先读取当前记录的实际数据，确保索引操作使用正确的数据
  Record actual_old_record;
  rc = get_record(old_record.rid(), actual_old_record);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to get actual record data before update. table name=%s, rid=%s, rc=%s",
              table_meta_->name(), old_record.rid().to_string().c_str(), strrc(rc));
    return rc;
  }
  
  // 删除旧记录的索引条目
  for (Index *index : indexes_) {
    rc = index->delete_entry(actual_old_record.data(), &actual_old_record.rid());
    if (rc != RC::SUCCESS) {
      if (rc == RC::RECORD_NOT_EXIST) {
        LOG_WARN("index entry not found when deleting, may be inconsistent. table=%s, index=%s, rid=%s",
                 table_meta_->name(), index->index_meta().name(), actual_old_record.rid().to_string().c_str());
      } else {
        LOG_ERROR("failed to delete entry from index when updating. table name=%s, index name=%s, rid=%s, rc=%s",
                  table_meta_->name(), index->index_meta().name(), actual_old_record.rid().to_string().c_str(), strrc(rc));
        return rc;
      }
    }
  }

  // 释放TEXT overflow pages
  rc = record_handler_->free_text_overflow_pages(actual_old_record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to free old TEXT overflow pages. table name=%s, rid=%s, rc=%s",
             table_meta_->name(), old_record.rid().to_string().c_str(), strrc(rc));
  }
  
  // 更新record并处理TEXT字段
  rc = record_handler_->visit_record(old_record.rid(), [this, &new_record](Record &record) -> bool {
    
    // 处理TEXT字段：检测格式并创建overflow pages（如需要）
    for (int i = 0; i < table_meta_->field_num(); i++) {
      const FieldMeta *field = table_meta_->field(i);
      if (field->type() != AttrType::TEXTS) {
        continue;
      }
      
      char *field_ptr = record.data() + field->offset();
      const char *new_field_ptr = new_record.data() + field->offset();
      
      uint32_t magic = *reinterpret_cast<const uint32_t*>(new_field_ptr);
      bool is_overflow_ptr = (magic == static_cast<uint32_t>(table_meta_->table_id()));
      bool is_extended_text = (magic == 0xFFFFFFFF);
      bool is_raw_string = !is_overflow_ptr && !is_extended_text;
      
      if (is_raw_string || is_extended_text) {
        const char *actual_text_data = nullptr;
        uint32_t text_len = 0;
        
        if (is_extended_text) {
          uint32_t full_length = *reinterpret_cast<const uint32_t*>(new_field_ptr + 4);
          uint32_t data_offset_in_record = *reinterpret_cast<const uint32_t*>(new_field_ptr + 8);
          // 确保不超过TEXT_MAX_LENGTH
          text_len = std::min(full_length, static_cast<uint32_t>(TEXT_MAX_LENGTH));
          actual_text_data = new_record.data() + data_offset_in_record;
        } else {
          uint32_t field_offset = field->offset();
          uint32_t max_readable = new_record.len() - field_offset;
          text_len = strnlen(new_field_ptr, std::min(max_readable, static_cast<uint32_t>(field->len())));
          actual_text_data = new_field_ptr;
        }
        
        const uint32_t inline_capacity = field->len() - 16;
        
        if (text_len <= inline_capacity) {
          memset(field_ptr, 0, field->len());
          if (text_len > 0) {
            memcpy(field_ptr, actual_text_data, text_len);
          }
        } else {
          // 长TEXT：写入inline + 创建overflow pages
          memcpy(field_ptr, actual_text_data, inline_capacity);
          
          char *overflow_ptr = field_ptr + inline_capacity;
          uint32_t *magic_ptr = reinterpret_cast<uint32_t*>(overflow_ptr);
          PageNum *page_num_ptr = reinterpret_cast<PageNum*>(overflow_ptr + 4);
          uint32_t *offset_ptr = reinterpret_cast<uint32_t*>(overflow_ptr + 8);
          uint32_t *total_len_ptr = reinterpret_cast<uint32_t*>(overflow_ptr + 12);
          
          *magic_ptr = table_meta_->table_id();
          *offset_ptr = sizeof(OverflowPageHeader);
          *total_len_ptr = text_len;
          
          uint32_t remain = text_len - inline_capacity;
          const char *overflow_data = actual_text_data + inline_capacity;
          
          const uint32_t PAGE_CAPACITY = record_handler_->get_overflow_page_capacity();
          int num_pages = (remain + PAGE_CAPACITY - 1) / PAGE_CAPACITY;
          PageNum first_page_num = BP_INVALID_PAGE_NUM;
          Frame *prev_frame = nullptr;
          uint32_t data_offset = 0;
          
          for (int p = 0; p < num_pages; p++) {
            Frame *frame = nullptr;
            RC alloc_rc = record_handler_->allocate_overflow_page(&frame);
            if (alloc_rc != RC::SUCCESS) {
              LOG_ERROR("Failed to allocate overflow page for TEXT field update. rc=%s", strrc(alloc_rc));
              return false;
            }
            
            frame->write_latch();
            OverflowPageHeader *header = reinterpret_cast<OverflowPageHeader*>(frame->data());
            header->page_type = PageType::TEXT_OVERFLOW;
            header->next_page = BP_INVALID_PAGE_NUM;
            
            uint32_t write_size = std::min(PAGE_CAPACITY, remain - data_offset);
            header->data_length = write_size;
            if (p == 0) {
              header->total_length = text_len;
              first_page_num = frame->page_num();
            } else {
              header->total_length = 0;
            }
            
            memcpy(frame->data() + sizeof(OverflowPageHeader), overflow_data + data_offset, write_size);
            
            if (prev_frame != nullptr) {
              OverflowPageHeader *prev_header = reinterpret_cast<OverflowPageHeader*>(prev_frame->data());
              prev_header->next_page = frame->page_num();
              prev_frame->mark_dirty();
              prev_frame->write_unlatch();
              prev_frame->unpin();
            }
            
            frame->mark_dirty();
            prev_frame = frame;
            data_offset += write_size;
          }
          
          if (prev_frame != nullptr) {
            prev_frame->write_unlatch();
            prev_frame->unpin();
          }
          
          *page_num_ptr = first_page_num;
        }
      } else {
        memcpy(field_ptr, new_field_ptr, field->len());
      }
    }
    
    // 复制非TEXT字段
    for (int i = 0; i < table_meta_->field_num(); i++) {
      const FieldMeta *field = table_meta_->field(i);
      if (field->type() == AttrType::TEXTS) {
        continue;
      }
      
      char *field_ptr = record.data() + field->offset();
      const char *new_field_ptr = new_record.data() + field->offset();
      memcpy(field_ptr, new_field_ptr, field->len());
    }
    
    return true;
  });
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to update record data. table name=%s, rid=%s, rc=%s",
              table_meta_->name(), old_record.rid().to_string().c_str(), strrc(rc));
    return rc;
  }

  // 插入新记录的索引条目
  rc = insert_entry_of_indexes(new_record.data(), old_record.rid());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to insert new entry into indexes when updating. table name=%s, rid=%s, rc=%s",
              table_meta_->name(), new_record.rid().to_string().c_str(), strrc(rc));

    // 如果插入新索引条目失败，尝试恢复旧索引条目
    LOG_INFO("attempting to restore old index entries after failed update");
    for (Index *index : indexes_) {
      RC restore_rc = index->insert_entry(actual_old_record.data(), &actual_old_record.rid());
      if (restore_rc != RC::SUCCESS) {
        LOG_ERROR("failed to restore old index entry. table=%s, index=%s, rid=%s, rc=%s",
                  table_meta_->name(), index->index_meta().name(), 
                  actual_old_record.rid().to_string().c_str(), strrc(restore_rc));
      }
    }
    return rc;
  }

  return RC::SUCCESS;
}

RC HeapTableEngine::get_record_scanner(RecordScanner *&scanner, Trx *trx, ReadWriteMode mode)
{

  scanner = new HeapRecordScanner(table_, *data_buffer_pool_, record_handler_, trx, db_->log_handler(), mode, nullptr);
  RC rc = scanner->open_scan();
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. rc=%s", strrc(rc));
  }
  return rc;
}

RC HeapTableEngine::get_chunk_scanner(ChunkFileScanner &scanner, Trx *trx, ReadWriteMode mode)
{
  RC rc = scanner.open_scan_chunk(table_, *data_buffer_pool_, db_->log_handler(), mode);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. rc=%s", strrc(rc));
  }
  return rc;
}

RC HeapTableEngine::create_index(Trx *trx, const FieldMeta *field_meta, const char *index_name, bool is_unique)
{
  if (common::is_blank(index_name) || nullptr == field_meta) {
    LOG_INFO("Invalid input arguments, table name is %s, index_name is blank or attribute_name is blank", table_meta_->name());
    return RC::INVALID_ARGUMENT;
  }

  IndexMeta new_index_meta;

  RC rc = new_index_meta.init(index_name, *field_meta, is_unique);
  if (rc != RC::SUCCESS) {
    LOG_INFO("Failed to init IndexMeta in table:%s, index_name:%s, field_name:%s", 
             table_meta_->name(), index_name, field_meta->name());
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index      = new BplusTreeIndex();
  string          index_file = table_index_file(db_->path().c_str(), table_meta_->name(), index_name);

  rc = index->create(table_, index_file.c_str(), new_index_meta, *field_meta);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
  RecordScanner *scanner = nullptr;
  rc                     = get_record_scanner(scanner, trx, ReadWriteMode::READ_ONLY);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create scanner while creating index. table=%s, index=%s, rc=%s", 
             table_meta_->name(), index_name, strrc(rc));
    return rc;
  }

  Record record;
  while (OB_SUCC(rc = scanner->next(record))) {
    rc = index->insert_entry(record.data(), &record.rid());
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to insert record into index while creating index. table=%s, index=%s, rc=%s",
               table_meta_->name(), index_name, strrc(rc));
      return rc;
    }
  }
  if (RC::RECORD_EOF == rc) {
    rc = RC::SUCCESS;
  } else {
    LOG_WARN("failed to insert record into index while creating index. table=%s, index=%s, rc=%s",
             table_meta_->name(), index_name, strrc(rc));
    return rc;
  }
  scanner->close_scan();
  delete scanner;
  LOG_INFO("inserted all records into new index. table=%s, index=%s", table_meta_->name(), index_name);

  indexes_.push_back(index);

  /// 接下来将这个索引放到表的元数据中
  TableMeta new_table_meta(*table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, table_meta_->name(), rc, strrc(rc));
    return rc;
  }

  /// 内存中有一份元数据，磁盘文件也有一份元数据。修改磁盘文件时，先创建一个临时文件，写入完成后再rename为正式文件
  /// 这样可以防止文件内容不完整
  // 创建元数据临时文件
  string  tmp_file = table_meta_file(db_->path().c_str(), table_meta_->name()) + ".tmp";
  fstream fs;
  fs.open(tmp_file, ios_base::out | ios_base::binary | ios_base::trunc);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR_OPEN;  // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0) {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }
  fs.close();

  // 覆盖原始元数据文件
  string meta_file = table_meta_file(db_->path().c_str(), table_meta_->name());

  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0) {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
              "system error=%d:%s",
              tmp_file.c_str(), meta_file.c_str(), index_name, table_meta_->name(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }

  table_meta_->swap(new_table_meta);

  LOG_INFO("Successfully added a new index (%s) on the table (%s)", index_name, table_meta_->name());
  return rc;
}
RC HeapTableEngine::create_index(
    Trx *trx, const vector<const FieldMeta *> &field_metas, const char *index_name, bool is_unique)
{
  if (field_metas.empty()) {
    LOG_INFO("Invalid input arguments, table name is %s, field_metas is empty", table_meta_->name());
    return RC::INVALID_ARGUMENT;
  }
  if (common::is_blank(index_name)) {
    LOG_INFO("Invalid input arguments, table name is %s, index_name is blank", table_meta_->name());
    return RC::INVALID_ARGUMENT;
  }

  IndexMeta new_index_meta;
  RC        rc = new_index_meta.init(index_name, field_metas, is_unique);
  if (rc != RC::SUCCESS) {
    LOG_INFO("Failed to init IndexMeta in table:%s, index_name:%s, field_metas=%zu", 
             table_meta_->name(), index_name, field_metas.size());
    return rc;
  }
  BplusTreeIndex *index      = new BplusTreeIndex();
  string          index_file = table_index_file(db_->path().c_str(), table_meta_->name(), index_name);
  rc                         = index->create(table_, index_file.c_str(), new_index_meta, field_metas);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s", index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 为表中现有数据建立索引条目
  LOG_INFO("Building index for existing records in table %s", table_meta_->name());
  printf("=== Building index for existing records ===\n");

  RecordScanner *scanner = nullptr;
  rc                     = get_record_scanner(scanner, nullptr, ReadWriteMode::READ_ONLY);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to open table scanner for reindexing. table=%s, rc=%s", table_meta_->name(), strrc(rc));
    return rc;
  }

  Record record;
  int    record_count = 0;
  while (OB_SUCC(rc = scanner->next(record))) {
    rc = index->insert_entry(record.data(), &record.rid());
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to insert existing record into new index. table=%s, index=%s, rid=%s, rc=%s", 
                table_meta_->name(), index_name, record.rid().to_string().c_str(), strrc(rc));
      scanner->close_scan();
      delete scanner;
      delete index;
      return rc;
    }
    record_count++;
    printf("Indexed record %d: RID=%s\n", record_count, record.rid().to_string().c_str());
  }

  scanner->close_scan();
  delete scanner;

  if (rc != RC::RECORD_EOF) {
    delete index;
    LOG_ERROR("Failed to scan table for reindexing. table=%s, rc=%s", table_meta_->name(), strrc(rc));
    return rc;
  }

  LOG_INFO("Successfully built index for %d existing records in table %s", record_count, table_meta_->name());
  printf("=== Finished building index for %d records ===\n", record_count);

  // 创建新的表元数据副本
  TableMeta new_table_meta(*table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name, table_meta_->name(), rc, strrc(rc));
    return rc;
  }

  /// 内存中有一份元数据，磁盘文件也有一份元数据。修改磁盘文件时，先创建一个临时文件，写入完成后再rename为正式文件
  /// 这样可以防止文件内容不完整
  // 创建元数据临时文件
  string  tmp_file = table_meta_file(db_->path().c_str(), table_meta_->name()) + ".tmp";
  fstream fs;
  fs.open(tmp_file, ios_base::out | ios_base::binary | ios_base::trunc);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", tmp_file.c_str(), strerror(errno));
    return RC::IOERR_OPEN;  // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0) {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s", tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }
  fs.close();

  // 覆盖原始元数据文件
  string meta_file = table_meta_file(db_->path().c_str(), table_meta_->name());

  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0) {
    LOG_ERROR("Failed to rename tmp meta file (%s) to normal meta file (%s) while creating index (%s) on table (%s). "
            "system error=%d:%s",
            tmp_file.c_str(), meta_file.c_str(), index_name, table_meta_->name(), errno, strerror(errno));
    return RC::IOERR_WRITE;
  }

  table_meta_->swap(new_table_meta);

  indexes_.push_back(index);
  LOG_INFO("Successfully added a new index (%s) on the table (%s)", index_name, table_meta_->name());
  return RC::SUCCESS;
}
RC HeapTableEngine::insert_entry_of_indexes(const char *record, const RID &rid)
{
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->insert_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      break;
    }
  }
  return rc;
}

RC HeapTableEngine::delete_entry_of_indexes(const char *record, const RID &rid, bool error_on_not_exists)
{
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->delete_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      if (rc != RC::RECORD_INVALID_KEY || !error_on_not_exists) {
        break;
      }
    }
  }
  return rc;
}

RC HeapTableEngine::sync()
{
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->sync();
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to flush index's pages. table=%s, index=%s, rc=%d:%s",
          table_meta_->name(),
          index->index_meta().name(),
          rc,
          strrc(rc));
      return rc;
    }
  }

  rc = data_buffer_pool_->flush_all_pages();
  LOG_DEBUG("Sync table over. table=%s", table_meta_->name());
  return rc;
}

Index *HeapTableEngine::find_index(const char *index_name) const
{
  for (Index *index : indexes_) {
    if (0 == strcmp(index->index_meta().name(), index_name)) {
      return index;
    }
  }
  return nullptr;
}
Index *HeapTableEngine::find_index_by_field(const char *field_name) const
{
  const IndexMeta *index_meta = table_meta_->find_index_by_field(field_name);
  if (index_meta != nullptr) {
    return this->find_index(index_meta->name());
  }
  return nullptr;
}

RC HeapTableEngine::init()
{
  string data_file = table_data_file(db_->path().c_str(), table_meta_->name());

  BufferPoolManager &bpm = db_->buffer_pool_manager();
  RC                 rc  = bpm.open_file(db_->log_handler(), data_file.c_str(), data_buffer_pool_);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open disk buffer pool for file:%s. rc=%d:%s", data_file.c_str(), rc, strrc(rc));
    return rc;
  }

  record_handler_ = new RecordFileHandler(table_meta_->storage_format());

  rc = record_handler_->init(*data_buffer_pool_, db_->log_handler(), table_meta_);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to init record handler. rc=%s", strrc(rc));
    delete record_handler_;
    record_handler_ = nullptr;
    return rc;
  }

  return rc;
}

RC HeapTableEngine::open()
{
  RC rc = RC::SUCCESS;
  init();
  const int index_num = table_meta_->index_num();
  for (int i = 0; i < index_num; i++) {
    const IndexMeta *index_meta = table_meta_->index(i);

    BplusTreeIndex *index      = new BplusTreeIndex();
    string          index_file = table_index_file(db_->path().c_str(), table_meta_->name(), index_meta->name());

    // 处理多字段索引和单字段索引
    if (index_meta->is_multi_field()) {
      // 多字段索引：获取所有字段元数据
      vector<const FieldMeta *> field_metas;
      for (const string &field_name : index_meta->fields()) {
        const FieldMeta *field_meta = table_meta_->field(field_name.c_str());
        if (field_meta == nullptr) {
          LOG_ERROR("Found invalid index meta info which has a non-exists field. table=%s, index=%s, field=%s",
                    table_meta_->name(), index_meta->name(), field_name.c_str());
          delete index;
          return RC::INTERNAL;
        }
        field_metas.push_back(field_meta);
      }
      rc = index->open(table_, index_file.c_str(), *index_meta, field_metas);
    } else {
      // 单字段索引：使用原有方式
      const FieldMeta *field_meta = table_meta_->field(index_meta->field());
      if (field_meta == nullptr) {
        LOG_ERROR("Found invalid index meta info which has a non-exists field. table=%s, index=%s, field=%s",
                  table_meta_->name(), index_meta->name(), index_meta->field());
        delete index;
        return RC::INTERNAL;
      }
      rc = index->open(table_, index_file.c_str(), *index_meta, *field_meta);
    }

    if (rc != RC::SUCCESS) {
      delete index;
      LOG_ERROR("Failed to open index. table=%s, index=%s, file=%s, rc=%s",
                table_meta_->name(), index_meta->name(), index_file.c_str(), strrc(rc));
      // skip cleanup
      //  do all cleanup action in destructive Table function.
      return rc;
    }
    indexes_.push_back(index);
  }
  return rc;
}
