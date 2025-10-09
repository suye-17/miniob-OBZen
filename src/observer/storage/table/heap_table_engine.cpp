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

  LOG_INFO("Table has been closed: %s", table_meta_->name());
}
RC HeapTableEngine::insert_record(Record &record)
{
  RC rc = RC::SUCCESS;
  rc    = record_handler_->insert_record(record.data(), table_meta_->record_size(), &record.rid());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Insert record failed. table name=%s, rc=%s", table_meta_->name(), strrc(rc));
    return rc;
  }

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
    ASSERT(RC::SUCCESS == rc, 
           "failed to delete entry from index. table name=%s, index name=%s, rid=%s, rc=%s",
           table_meta_->name(), index->index_meta().name(), record.rid().to_string().c_str(), strrc(rc));
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
  
  // 第一步：删除旧记录在所有索引中的条目（使用实际记录数据）
  for (Index *index : indexes_) {
    rc = index->delete_entry(actual_old_record.data(), &actual_old_record.rid());
    if (rc != RC::SUCCESS) {
      // 如果索引条目不存在，可能是之前就有不一致的情况，记录警告但继续
      if (rc == RC::RECORD_NOT_EXIST) {
        LOG_WARN("index entry not found when deleting, may be inconsistent. table=%s, index=%s, rid=%s",
                 table_meta_->name(), index->index_meta().name(), actual_old_record.rid().to_string().c_str());
      } else {
        LOG_ERROR("failed to delete entry from index when updating. table name=%s, index name=%s, rid=%s, rc=%s",
                  table_meta_->name(), index->index_meta().name(), actual_old_record.rid().to_string().c_str(), strrc(rc));
        // 对于严重错误，需要返回失败
        return rc;
      }
    }
  }

  // 第二步：更新实际的记录数据
  // 注意：对于TEXT字段，需要先释放旧溢出页，然后重新处理新数据
  
  // 2.1 先释放旧记录的TEXT溢出页
  rc = record_handler_->free_text_overflow_pages(actual_old_record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to free old TEXT overflow pages. table name=%s, rid=%s, rc=%s",
             table_meta_->name(), old_record.rid().to_string().c_str(), strrc(rc));
    // 继续执行更新，即使释放失败
  }
  
  // 2.2 使用visit_record更新记录数据，并重新处理TEXT字段
  rc = record_handler_->visit_record(old_record.rid(), [this, &new_record](Record &record) -> bool {
    
    // 第一步：检测并处理TEXT字段
    // 如果new_record中的TEXT字段包含原始字符串（不是overflow pointer），
    // 我们需要创建overflow pages
    for (int i = 0; i < table_meta_->field_num(); i++) {
      const FieldMeta *field = table_meta_->field(i);
      if (field->type() != AttrType::TEXTS) {
        continue;
      }
      
      char *field_ptr = record.data() + field->offset();
      const char *new_field_ptr = new_record.data() + field->offset();
      
      // 检查new_field_ptr是否是原始字符串（不是overflow pointer）
      // overflow pointer的magic是table_id, 而原始字符串通常是可打印字符或\0
      // 特殊标记0xFFFFFFFF表示TEXT数据在record末尾
      uint32_t magic = *reinterpret_cast<const uint32_t*>(new_field_ptr);
      bool is_overflow_ptr = (magic == static_cast<uint32_t>(table_meta_->table_id()));
      bool is_extended_text = (magic == 0xFFFFFFFF);
      bool is_raw_string = !is_overflow_ptr && !is_extended_text;
      
      if (is_raw_string || is_extended_text) {
        // 需要创建overflow pages
        const char *actual_text_data = nullptr;
        uint32_t text_len = 0;
        
        if (is_extended_text) {
          // TEXT数据在record末尾
          uint64_t full_length = *reinterpret_cast<const uint64_t*>(new_field_ptr + 4);
          uint32_t data_offset_in_record = *reinterpret_cast<const uint32_t*>(new_field_ptr + 12);
          text_len = full_length;
          actual_text_data = new_record.data() + data_offset_in_record;
        } else {
          // 普通的短TEXT，在field内部
          uint32_t field_offset = field->offset();
          uint32_t max_readable = new_record.len() - field_offset;
          text_len = strnlen(new_field_ptr, std::min(max_readable, static_cast<uint32_t>(field->len())));
          actual_text_data = new_field_ptr;
        }
        
        // 调用RecordFileHandler的insert逻辑来处理TEXT字段
        // 但我们需要一个辅助函数...实际上，让我们直接在这里实现逻辑
        const uint32_t inline_capacity = field->len() - 20;  // 20 bytes for header
        
        if (text_len <= inline_capacity) {
          // 短TEXT：不需要overflow，直接写入inline格式
          memset(field_ptr, 0, field->len());
          if (text_len > 0) {
            memcpy(field_ptr, actual_text_data, text_len);
          }
        } else {
          // 长TEXT：需要创建overflow pages
          // 写入header（必须与INSERT时的结构完全一致！）
          uint32_t *magic_ptr = reinterpret_cast<uint32_t*>(field_ptr);
          uint32_t *page_num_ptr = reinterpret_cast<uint32_t*>(field_ptr + 4);
          uint32_t *offset_ptr = reinterpret_cast<uint32_t*>(field_ptr + 8);  // 注意：这是offset，不是inline_len！
          uint64_t *total_len_ptr = reinterpret_cast<uint64_t*>(field_ptr + 12);
          
          *magic_ptr = table_meta_->table_id();
          *offset_ptr = sizeof(OverflowPageHeader);  // 修正：应该是OverflowPageHeader的大小
          *total_len_ptr = text_len;
          
          // 写入inline部分
          memcpy(field_ptr + 20, actual_text_data, inline_capacity);
          
          // 创建overflow pages
          uint32_t remain = text_len - inline_capacity;
          const char *overflow_data = actual_text_data + inline_capacity;
          
          // 计算需要多少个overflow pages
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
            
            // 写入数据
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
          
          // 更新main record中的page_num指针
          *page_num_ptr = first_page_num;
        }
      } else {
        // 已经是overflow pointer格式，直接复制
        memcpy(field_ptr, new_field_ptr, field->len());
      }
    }
    
    // 第二步：复制其他非TEXT字段的数据
    for (int i = 0; i < table_meta_->field_num(); i++) {
      const FieldMeta *field = table_meta_->field(i);
      if (field->type() == AttrType::TEXTS) {
        continue;  // TEXT字段已经处理过了
      }
      
      char *field_ptr = record.data() + field->offset();
      const char *new_field_ptr = new_record.data() + field->offset();
      memcpy(field_ptr, new_field_ptr, field->len());
    }
    
    return true;  // 返回true表示记录被修改了
  });
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to update record data. table name=%s, rid=%s, rc=%s",
              table_meta_->name(), old_record.rid().to_string().c_str(), strrc(rc));
    return rc;
  }

  // 第三步：在所有索引中插入新记录的条目
  // 注意：UPDATE是原地更新，使用old_record的RID
  rc = insert_entry_of_indexes(new_record.data(), old_record.rid());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to insert new entry into indexes when updating. table name=%s, rid=%s, rc=%s",
              table_meta_->name(), old_record.rid().to_string().c_str(), strrc(rc));
    
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

RC HeapTableEngine::create_index(Trx *trx, const FieldMeta *field_meta, const char *index_name)
{
  if (common::is_blank(index_name) || nullptr == field_meta) {
    LOG_INFO("Invalid input arguments, table name is %s, index_name is blank or attribute_name is blank", table_meta_->name());
    return RC::INVALID_ARGUMENT;
  }

  IndexMeta new_index_meta;

  RC rc = new_index_meta.init(index_name, *field_meta);
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
  rc = get_record_scanner(scanner, trx, ReadWriteMode::READ_ONLY);
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
  LOG_INFO("Sync table over. table=%s", table_meta_->name());
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
    const FieldMeta *field_meta = table_meta_->field(index_meta->field());
    if (field_meta == nullptr) {
      LOG_ERROR("Found invalid index meta info which has a non-exists field. table=%s, index=%s, field=%s",
                table_meta_->name(), index_meta->name(), index_meta->field());
      // skip cleanup
      //  do all cleanup action in destructive Table function
      return RC::INTERNAL;
    }

    BplusTreeIndex *index      = new BplusTreeIndex();
    string          index_file = table_index_file(db_->path().c_str(), table_meta_->name(), index_meta->name());

    rc = index->open(table_, index_file.c_str(), *index_meta, *field_meta);
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
