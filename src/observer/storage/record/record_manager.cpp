/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Meiyi & Longda on 2021/4/13.
//
#include "storage/record/record_manager.h"
#include "common/log/log.h"
#include "storage/buffer/frame.h"
#include "storage/buffer/page.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"
#include "storage/clog/log_handler.h"
#include "storage/buffer/page_type.h"
#include "common/types.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace {
  inline OverflowPageHeader *hdr(Frame *frame) {
    return reinterpret_cast<OverflowPageHeader *>(frame->data());
  }

  inline uint32_t ov_cap() {
    return BP_PAGE_DATA_SIZE - sizeof(OverflowPageHeader);
  }

  RC ov_write(Frame *frame, const char* buf, uint32_t len)
  {
    if (frame == nullptr || buf == nullptr) {
      return RC::INVALID_ARGUMENT;
    }

    OverflowPageHeader *h = hdr(frame);
    if (h == nullptr || h->page_type != PageType::TEXT_OVERFLOW) {
      LOG_ERROR("Invalid overflow page header. page_num %d:%d.", frame->page_num(), frame->page_num());
      return RC::INVALID_ARGUMENT;
    }

    const uint32_t cap = ov_cap();
    if (len > cap) {
      return RC::INVALID_ARGUMENT;
    }
    if (len == 0) {
      h->data_length = 0;
      frame->mark_dirty();
      return RC::SUCCESS;
    }

    char *payload = frame->data() + sizeof(OverflowPageHeader);
    memcpy(payload, buf, len);

    h->data_length = len;
    frame->mark_dirty();    // 标记脏页

    return RC::SUCCESS;
  }

   RC ov_read(Frame *frame, char* buf, uint32_t len) 
  {
    if (frame == nullptr || buf == nullptr) {
      return RC::INVALID_ARGUMENT;
    }

    const OverflowPageHeader *h = hdr(frame);
    if (h == nullptr || h->page_type != PageType::TEXT_OVERFLOW) {
      LOG_ERROR("Invalid overflow page header. page_num %d:%d.", frame->page_num(), frame->page_num());
      return RC::INVALID_ARGUMENT;
    }

    if (len > h->data_length) {
      return RC::INVALID_ARGUMENT;
    }
    if (len == 0) {
      return RC::SUCCESS; 
    }

    const char *payload = frame->data() + sizeof(OverflowPageHeader);
    memcpy(buf, payload, len);

    return RC::SUCCESS;
  } 

  /* RC ov_set_next(Frame *frame, PageNum next)
  {
    if (frame == nullptr) return RC::INVALID_ARGUMENT;

    OverflowPageHeader *h = hdr(frame);
    if (h == nullptr || h->page_type != PageType::TEXT_OVERFLOW) {
      LOG_ERROR("Invalid overflow page header. page_num %d:%d.", frame->page_num(), frame->page_num());
      return RC::INVALID_ARGUMENT;
    }

    h->next_page = next;
    frame->mark_dirty();
    return RC::SUCCESS;
  }

  PageNum ov_get_next(Frame *frame)
  {
    if (frame == nullptr) return BP_INVALID_PAGE_NUM;

    const OverflowPageHeader *h = hdr(frame);
    if (h == nullptr || h->page_type != PageType::TEXT_OVERFLOW) {
      LOG_ERROR("Invalid overflow page header. page_num %d:%d.", frame->page_num(), frame->page_num());
      return BP_INVALID_PAGE_NUM;
    }

    return h->next_page;
  } */

  bool is_overflow_pointer(const char* field_data, uint32_t field_len, uint32_t expected_table_id)
  {
    if (field_data == nullptr || field_len < 20) {
      return false;
    }

    uint32_t table_id = *reinterpret_cast<const uint32_t*>(field_data);
    if (table_id != expected_table_id) {
      return false;
    }

    uint32_t page_num = *reinterpret_cast<const uint32_t*>(field_data + 4);
    if (page_num == 0 || page_num == static_cast<uint32_t>(BP_INVALID_PAGE_NUM)) {
        return false;
    }

    uint32_t offset = *reinterpret_cast<const uint32_t*>(field_data + 8);
    if (offset != sizeof(OverflowPageHeader)) {
        return false;
    }

    uint64_t length = *reinterpret_cast<const uint64_t*>(field_data + 12);
    if (length == 0 || length > TEXT_MAX_LENGTH) {
        return false;
    }

    return true;
  }

}  // end anonymous namespace

// TEXT字段溢出处理函数（放在common命名空间以供外部调用）
namespace common {

RC process_text_fields_on_read(TableMeta *table_meta, DiskBufferPool *buffer_pool, const Record &raw_record, Record &output_record) 
  {
    LOG_ERROR("==== 开始处理TEXT字段读取 ====");
    LOG_ERROR("raw_record: len=%d, data=%p", raw_record.len(), raw_record.data());
    
    const char *raw_data = raw_record.data();
    int raw_len = raw_record.len();
    uint32_t table_id = table_meta->table_id();
    
    // 安全检查：记录长度必须匹配表的记录大小
    int expected_len = table_meta->record_size();
    if (raw_len != expected_len) {
        LOG_WARN("Record length mismatch: got %d, expected %d. This record doesn't belong to this table.", 
                raw_len, expected_len);
        // 返回错误，让调用者处理
        return RC::RECORD_INVALID_RID;
    }
    
    // 第1步：快速检查是否有TEXT溢出字段
    bool has_overflow = false;
    for (int i = 0; i < table_meta->field_num(); i++) {
        const FieldMeta *field = table_meta->field(i);
        
        if (field->type() != AttrType::TEXTS) {
            continue;
        }
        
        const char *field_data = raw_data + field->offset();
        if (is_overflow_pointer(field_data, field->len(), table_id)) {
            has_overflow = true;
            break;
        }
    }
    
    // 第2步：没有溢出，直接复制
    if (!has_overflow) {
        LOG_DEBUG("没有溢出字段，直接复制数据");
        output_record.copy_data(raw_data, raw_len);
        return RC::SUCCESS;
    }
    
    LOG_DEBUG("发现溢出字段，开始展开");
    
    // 第3步：有溢出，需要展开数据（但保持记录长度不变）
    auto new_data = std::make_unique<char[]>(raw_len);
    memcpy(new_data.get(), raw_data, raw_len);  // 先复制原始数据
    
    // 第4步：逐个处理TEXT字段，将溢出数据展开到字段空间中
    for (int i = 0; i < table_meta->field_num(); i++) {
        const FieldMeta *field = table_meta->field(i);
        if (field->type() != AttrType::TEXTS) {
            continue;
        }
        
        char *field_ptr = new_data.get() + field->offset();
        
        // 确保TEXT字段以\0结尾（对于非溢出的TEXT也很重要）
        if (!is_overflow_pointer(field_ptr, field->len(), table_id)) {
            // 非溢出TEXT：确保在字段末尾有\0
            // 注意：field_ptr可能不是以\0结尾的，需要找到实际长度
            size_t text_len = 0;
            for (size_t j = 0; j < static_cast<size_t>(field->len()); j++) {
                if (field_ptr[j] == '\0') {
                    text_len = j;
                    break;
                }
            }
            if (text_len == 0 && field_ptr[0] != '\0') {
                text_len = field->len() - 1;  // 整个字段都是数据
                field_ptr[text_len] = '\0';
            }
            LOG_DEBUG("Non-overflow TEXT field %s: %zu bytes", field->name(), text_len);
            continue;
        }
        
        // 溢出TEXT：需要读取溢出页并展开
        {
            uint32_t first_page_num = *reinterpret_cast<const uint32_t*>(field_ptr + 4);
            uint64_t total_length = *reinterpret_cast<const uint64_t*>(field_ptr + 12);
            
            uint32_t inline_len = std::min(static_cast<uint32_t>(INLINE_TEXT_CAPACITY),
                                         static_cast<uint32_t>(field->len() - 20));
            uint32_t overflow_length = total_length > inline_len ? total_length - inline_len : 0;
            
            auto overflow_buffer = std::make_unique<char[]>(overflow_length);
            uint32_t buffer_offset = 0;  
            
            // 步骤1：循环读取所有溢出页
            PageNum current_page = first_page_num;
            int page_count = 0;
            
            while (current_page != BP_INVALID_PAGE_NUM && buffer_offset < overflow_length) {
                Frame *frame = nullptr;
                RC rc = buffer_pool->get_this_page(current_page, &frame);
                if (rc != RC::SUCCESS) {
                    LOG_ERROR("Failed to get overflow page %d for field %s", current_page, field->name());
                    return rc;
                }
                
                frame->read_latch();
                
                const OverflowPageHeader *header = hdr(frame);
                if (header->page_type != PageType::TEXT_OVERFLOW) {
                    frame->read_unlatch();
                    frame->unpin();
                    LOG_ERROR("Page %d is not overflow page for field %s", current_page, field->name());
                    return RC::INVALID_ARGUMENT;
                }
                
                uint32_t copy_size = std::min(header->data_length, overflow_length - buffer_offset);
                rc = ov_read(frame, overflow_buffer.get() + buffer_offset, copy_size);
                
                PageNum next_page = header->next_page;
                
                frame->read_unlatch();
                frame->unpin();
                
                if (rc != RC::SUCCESS) {
                    LOG_ERROR("Failed to read overflow data from page %d for field %s", current_page, field->name());
                    return rc;
                }

                buffer_offset += copy_size;
                current_page = next_page; 
                page_count++;
                
                LOG_DEBUG("Read overflow page %d: %u bytes, next_page=%d", 
                         page_count, copy_size, next_page);
            }
            
            // 验证读取长度
            if (buffer_offset != overflow_length) {
                LOG_WARN("TEXT field %s: read %u bytes, expected %u bytes", 
                        field->name(), buffer_offset, overflow_length);
            }
            
            LOG_DEBUG("Read %d overflow pages, %u bytes total for field %s", 
                    page_count, buffer_offset, field->name());
            
            // 步骤2：重组TEXT字段数据（内联部分 + 溢出部分）
            // 将完整TEXT展开到固定的field->len()空间中
            uint32_t full_text_len = inline_len + buffer_offset;
            auto full_text = std::make_unique<char[]>(full_text_len + 1);
            
            // 复制内联部分
            memcpy(full_text.get(), field_ptr + 20, inline_len);
            // 复制溢出部分
            memcpy(full_text.get() + inline_len, overflow_buffer.get(), buffer_offset);
            full_text[full_text_len] = '\0';
            
            // 将完整TEXT写入字段空间（最多field->len()字节）
            // 注意：数据库字段是定长的，不需要为\0预留空间
            uint32_t write_len = std::min(full_text_len, static_cast<uint32_t>(field->len()));
            memcpy(field_ptr, full_text.get(), write_len);
            
            // 清空剩余空间
            if (write_len < static_cast<uint32_t>(field->len())) {
                memset(field_ptr + write_len, 0, field->len() - write_len);
            }
            
            if (full_text_len > write_len) {
                LOG_WARN("TEXT field %s truncated: %d -> %d bytes", field->name(), full_text_len, write_len);
            }
            
            LOG_DEBUG("TEXT字段 %s 展开完成: %d字节 (内联%d + 溢出%d)", 
                     field->name(), write_len, inline_len, buffer_offset);
        }
    }
    
    // 第5步：设置输出记录
    RC rc = output_record.copy_data(new_data.get(), raw_len);
    if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to copy data to output record");
        return rc;
    }
    
    LOG_ERROR("==== process_text_fields_on_read完成 ====");
    LOG_ERROR("output_record: len=%d, data=%p", output_record.len(), output_record.data());
    
    // 验证输出数据
    if (output_record.data() == nullptr) {
        LOG_ERROR("ERROR: output_record.data() is NULL!");
        return RC::INTERNAL;
    }
    
    return RC::SUCCESS;
  }

}  // end namespace common

using namespace common;

static constexpr int PAGE_HEADER_SIZE = (sizeof(PageHeader));
RecordPageHandler   *RecordPageHandler::create(StorageFormat format)
{
  if (format == StorageFormat::ROW_FORMAT) {
    return new RowRecordPageHandler();
  } else {
    return new PaxRecordPageHandler();
  }
}
/**
 * @brief 8字节对齐
 * 注: ceiling(a / b) = floor((a + b - 1) / b)
 *
 * @param size 待对齐的字节数
 */
int align8(int size) { return (size + 7) & ~7; }

/**
 * @brief 计算指定大小的页面，可以容纳多少个记录
 *
 * @param page_size   页面的大小
 * @param record_size 记录的大小
 * @param fixed_size  除 PAGE_HEADER 外，页面中其余固定长度占用，目前为PAX存储格式中的
 *                    列偏移索引大小（column index）。
 */
int page_record_capacity(int page_size, int record_size, int fixed_size)
{
  // (record_capacity * record_size) + record_capacity/8 + 1 <= (page_size - fix_size)
  // ==> record_capacity = ((page_size - fix_size) - 1) / (record_size + 0.125)
  return (int)((page_size - PAGE_HEADER_SIZE - fixed_size - 1) / (record_size + 0.125));
}

/**
 * @brief bitmap 记录了某个位置是否有有效的记录数据，这里给定记录个数时需要多少字节来存放bitmap数据
 * 注: ceiling(a / b) = floor((a + b - 1) / b)
 *
 * @param record_capacity 想要存放多少记录
 */
int page_bitmap_size(int record_capacity) { return (record_capacity + 7) / 8; }

string PageHeader::to_string() const
{
  stringstream ss;
  ss << "record_num:" << record_num << ",column_num:" << column_num << ",record_real_size:" << record_real_size
     << ",record_size:" << record_size << ",record_capacity:" << record_capacity << ",data_offset:" << data_offset;
  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////
RecordPageIterator::RecordPageIterator() {}
RecordPageIterator::~RecordPageIterator() {}

void RecordPageIterator::init(RecordPageHandler *record_page_handler, SlotNum start_slot_num /*=0*/)
{
  record_page_handler_ = record_page_handler;
  page_num_            = record_page_handler->get_page_num();
  bitmap_.init(record_page_handler->bitmap_, record_page_handler->page_header_->record_capacity);
  next_slot_num_ = bitmap_.next_setted_bit(start_slot_num);
}

bool RecordPageIterator::has_next() { return -1 != next_slot_num_; }

RC RecordPageIterator::next(Record &record)
{
  record_page_handler_->get_record(RID(page_num_, next_slot_num_), record);

  if (next_slot_num_ >= 0) {
    next_slot_num_ = bitmap_.next_setted_bit(next_slot_num_ + 1);
  }
  return record.rid().slot_num != -1 ? RC::SUCCESS : RC::RECORD_EOF;
}

////////////////////////////////////////////////////////////////////////////////

RecordPageHandler::~RecordPageHandler() { cleanup(); }

RC RecordPageHandler::init(DiskBufferPool &buffer_pool, LogHandler &log_handler, PageNum page_num, ReadWriteMode mode)
{
  if (disk_buffer_pool_ != nullptr) {
    if (frame_->page_num() == page_num) {
      LOG_WARN("Disk buffer pool has been opened for page_num %d.", page_num);
      return RC::RECORD_OPENNED;
    } else {
      cleanup();
    }
  }

  RC ret = RC::SUCCESS;
  if ((ret = buffer_pool.get_this_page(page_num, &frame_)) != RC::SUCCESS) {
    LOG_ERROR("Failed to get page handle from disk buffer pool. ret=%d:%s", ret, strrc(ret));
    return ret;
  }

  char *data = frame_->data();

  if (mode == ReadWriteMode::READ_ONLY) {
    frame_->read_latch();
  } else {
    frame_->write_latch();
  }
  disk_buffer_pool_ = &buffer_pool;

  rw_mode_     = mode;
  page_header_ = (PageHeader *)(data);
  bitmap_      = data + PAGE_HEADER_SIZE;

  (void)log_handler_.init(log_handler, buffer_pool.id(), page_header_->record_real_size, storage_format_);

  LOG_TRACE("Successfully init page_num %d.", page_num);
  return ret;
}

RC RecordPageHandler::recover_init(DiskBufferPool &buffer_pool, PageNum page_num)
{
  if (disk_buffer_pool_ != nullptr) {
    LOG_WARN("Disk buffer pool has been opened for page_num %d.", page_num);
    return RC::RECORD_OPENNED;
  }

  RC ret = RC::SUCCESS;
  if ((ret = buffer_pool.get_this_page(page_num, &frame_)) != RC::SUCCESS) {
    LOG_ERROR("Failed to get page handle from disk buffer pool. ret=%d:%s", ret, strrc(ret));
    return ret;
  }

  char *data = frame_->data();

  frame_->write_latch();
  disk_buffer_pool_ = &buffer_pool;
  rw_mode_          = ReadWriteMode::READ_WRITE;
  page_header_      = (PageHeader *)(data);
  bitmap_           = data + PAGE_HEADER_SIZE;

  buffer_pool.recover_page(page_num);

  LOG_TRACE("Successfully init page_num %d.", page_num);
  return ret;
}

RC RecordPageHandler::init_empty_page(
    DiskBufferPool &buffer_pool, LogHandler &log_handler, PageNum page_num, int record_size, TableMeta *table_meta)
{
  RC rc = init(buffer_pool, log_handler, page_num, ReadWriteMode::READ_WRITE);
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to init empty page page_num:record_size %d:%d. rc=%s", page_num, record_size, strrc(rc));
    return rc;
  }

  (void)log_handler_.init(log_handler, buffer_pool.id(), record_size, storage_format_);

  int column_num = 0;
  // only pax format need column index
  if (table_meta != nullptr && storage_format_ == StorageFormat::PAX_FORMAT) {
    column_num = table_meta->field_num();
  }
  page_header_->record_num       = 0;
  page_header_->column_num       = column_num;
  page_header_->record_real_size = record_size;
  page_header_->record_size      = align8(record_size);
  page_header_->record_capacity  = page_record_capacity(
      BP_PAGE_DATA_SIZE, page_header_->record_size, column_num * sizeof(int) /* other fixed size*/);
  page_header_->col_idx_offset = align8(PAGE_HEADER_SIZE + page_bitmap_size(page_header_->record_capacity));
  page_header_->data_offset    = align8(PAGE_HEADER_SIZE + page_bitmap_size(page_header_->record_capacity)) +
                              column_num * sizeof(int) /* column index*/;
  this->fix_record_capacity();
  ASSERT(page_header_->data_offset + page_header_->record_capacity * page_header_->record_size 
              <= BP_PAGE_DATA_SIZE, 
         "Record overflow the page size");

  bitmap_ = frame_->data() + PAGE_HEADER_SIZE;
  memset(bitmap_, 0, page_bitmap_size(page_header_->record_capacity));
  // column_index[i] store the end offset of column `i` or the start offset of column `i+1`
  int *column_index = reinterpret_cast<int *>(frame_->data() + page_header_->col_idx_offset);
  for (int i = 0; i < column_num; ++i) {
    ASSERT(i == table_meta->field(i)->field_id(), "i should be the col_id of fields[i]");
    if (i == 0) {
      column_index[i] = table_meta->field(i)->len() * page_header_->record_capacity;
    } else {
      column_index[i] = table_meta->field(i)->len() * page_header_->record_capacity + column_index[i - 1];
    }
  }

  rc = log_handler_.init_new_page(frame_, page_num, span((const char *)column_index, column_num * sizeof(int)));
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to init empty page: write log failed. page_num:record_size %d:%d. rc=%s", 
              page_num, record_size, strrc(rc));
    return rc;
  }

  return RC::SUCCESS;
}

RC RecordPageHandler::init_empty_page(DiskBufferPool &buffer_pool, LogHandler &log_handler, PageNum page_num,
    int record_size, int column_num, const char *col_idx_data)
{
  RC rc = init(buffer_pool, log_handler, page_num, ReadWriteMode::READ_WRITE);
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to init empty page page_num:record_size %d:%d. rc=%s", page_num, record_size, strrc(rc));
    return rc;
  }

  (void)log_handler_.init(log_handler, buffer_pool.id(), record_size, storage_format_);

  page_header_->record_num       = 0;
  page_header_->column_num       = column_num;
  page_header_->record_real_size = record_size;
  page_header_->record_size      = align8(record_size);
  page_header_->record_capacity =
      page_record_capacity(BP_PAGE_DATA_SIZE, page_header_->record_size, page_header_->column_num * sizeof(int));
  page_header_->col_idx_offset = align8(PAGE_HEADER_SIZE + page_bitmap_size(page_header_->record_capacity));
  page_header_->data_offset    = align8(PAGE_HEADER_SIZE + page_bitmap_size(page_header_->record_capacity)) +
                              column_num * sizeof(int) /* column index*/;
  this->fix_record_capacity();
  ASSERT(page_header_->data_offset + page_header_->record_capacity * page_header_->record_size 
              <= BP_PAGE_DATA_SIZE, 
         "Record overflow the page size");

  bitmap_ = frame_->data() + PAGE_HEADER_SIZE;
  memset(bitmap_, 0, page_bitmap_size(page_header_->record_capacity));
  // column_index[i] store the end offset of column `i` the start offset of column `i+1`
  int *column_index = reinterpret_cast<int *>(frame_->data() + page_header_->col_idx_offset);
  memcpy(column_index, col_idx_data, column_num * sizeof(int));

  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to init empty page: write log failed. page_num:record_size %d:%d. rc=%s", 
              page_num, record_size, strrc(rc));
    return rc;
  }

  return RC::SUCCESS;
}

RC RecordPageHandler::cleanup()
{
  if (disk_buffer_pool_ != nullptr) {
    if (rw_mode_ == ReadWriteMode::READ_ONLY) {
      frame_->read_unlatch();
    } else {
      frame_->write_unlatch();
    }
    disk_buffer_pool_->unpin_page(frame_);
    disk_buffer_pool_ = nullptr;
  }

  return RC::SUCCESS;
}

RC RowRecordPageHandler::insert_record(const char *data, RID *rid)
{
  ASSERT(rw_mode_ != ReadWriteMode::READ_ONLY, 
         "cannot insert record into page while the page is readonly");

  if (page_header_->record_num == page_header_->record_capacity) {
    LOG_WARN("Page is full, page_num %d:%d.", disk_buffer_pool_->file_desc(), frame_->page_num());
    return RC::RECORD_NOMEM;
  }

  // 找到空闲位置
  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  int    index = bitmap.next_unsetted_bit(0);
  bitmap.set_bit(index);
  page_header_->record_num++;

  RC rc = log_handler_.insert_record(frame_, RID(get_page_num(), index), data);
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to insert record. page_num %d:%d. rc=%s", disk_buffer_pool_->file_desc(), frame_->page_num(), strrc(rc));
    // return rc; // ignore errors
  }

  // assert index < page_header_->record_capacity
  char *record_data = get_record_data(index);
  memcpy(record_data, data, page_header_->record_real_size);

  frame_->mark_dirty();

  if (rid) {
    rid->page_num = get_page_num();
    rid->slot_num = index;
  }

  // LOG_TRACE("Insert record. rid page_num=%d, slot num=%d", get_page_num(), index);
  return RC::SUCCESS;
}

RC RowRecordPageHandler::recover_insert_record(const char *data, const RID &rid)
{
  if (rid.slot_num >= page_header_->record_capacity) {
    LOG_WARN("slot_num illegal, slot_num(%d) > record_capacity(%d).", rid.slot_num, page_header_->record_capacity);
    return RC::RECORD_INVALID_RID;
  }

  // 更新位图
  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (!bitmap.get_bit(rid.slot_num)) {
    bitmap.set_bit(rid.slot_num);
    page_header_->record_num++;
  }

  // 恢复数据
  char *record_data = get_record_data(rid.slot_num);
  memcpy(record_data, data, page_header_->record_real_size);

  frame_->mark_dirty();

  return RC::SUCCESS;
}

RC RowRecordPageHandler::delete_record(const RID *rid)
{
  ASSERT(rw_mode_ != ReadWriteMode::READ_ONLY, 
         "cannot delete record from page while the page is readonly");

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (bitmap.get_bit(rid->slot_num)) {
    bitmap.clear_bit(rid->slot_num);
    page_header_->record_num--;
    frame_->mark_dirty();

    RC rc = log_handler_.delete_record(frame_, *rid);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to delete record. page_num %d:%d. rc=%s", disk_buffer_pool_->file_desc(), frame_->page_num(), strrc(rc));
      // return rc; // ignore errors
    }

    return RC::SUCCESS;
  } else {
    LOG_DEBUG("Invalid slot_num %d, slot is empty, page_num %d.", rid->slot_num, frame_->page_num());
    return RC::RECORD_NOT_EXIST;
  }
}

RC RowRecordPageHandler::update_record(const RID &rid, const char *data)
{
  ASSERT(rw_mode_ != ReadWriteMode::READ_ONLY, "cannot delete record from page while the page is readonly");

  if (rid.slot_num >= page_header_->record_capacity) {
    LOG_ERROR("Invalid slot_num %d, exceed page's record capacity, frame=%s, page_header=%s",
              rid.slot_num, frame_->to_string().c_str(), page_header_->to_string().c_str());
    return RC::INVALID_ARGUMENT;
  }

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (bitmap.get_bit(rid.slot_num)) {
    frame_->mark_dirty();

    char *record_data = get_record_data(rid.slot_num);
    if (record_data == data) {
      // nothing to do
    } else {
      memcpy(record_data, data, page_header_->record_real_size);
    }

    RC rc = log_handler_.update_record(frame_, rid, data);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to update record. page_num %d:%d. rc=%s", 
                disk_buffer_pool_->file_desc(), frame_->page_num(), strrc(rc));
      // return rc; // ignore errors
    }

    return RC::SUCCESS;
  } else {
    LOG_DEBUG("Invalid slot_num %d, slot is empty, page_num %d.", rid.slot_num, frame_->page_num());
    return RC::RECORD_NOT_EXIST;
  }
}

RC RowRecordPageHandler::get_record(const RID &rid, Record &record)
{
  if (rid.slot_num >= page_header_->record_capacity) {
    LOG_ERROR("Invalid slot_num %d, exceed page's record capacity, frame=%s, page_header=%s",
              rid.slot_num, frame_->to_string().c_str(), page_header_->to_string().c_str());
    return RC::RECORD_INVALID_RID;
  }

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (!bitmap.get_bit(rid.slot_num)) {
    LOG_ERROR("Invalid slot_num:%d, slot is empty, page_num %d.", rid.slot_num, frame_->page_num());
    return RC::RECORD_NOT_EXIST;
  }

  record.set_rid(rid);
  record.set_data(get_record_data(rid.slot_num), page_header_->record_real_size);
  return RC::SUCCESS;
}

PageNum RecordPageHandler::get_page_num() const
{
  if (nullptr == page_header_) {
    return (PageNum)(-1);
  }
  return frame_->page_num();
}

bool RecordPageHandler::is_full() const { return page_header_->record_num >= page_header_->record_capacity; }

RC PaxRecordPageHandler::insert_record(const char *data, RID *rid)
{
  // your code here
  exit(-1);
}

RC PaxRecordPageHandler::delete_record(const RID *rid)
{
  ASSERT(rw_mode_ != ReadWriteMode::READ_ONLY, 
         "cannot delete record from page while the page is readonly");

  Bitmap bitmap(bitmap_, page_header_->record_capacity);
  if (bitmap.get_bit(rid->slot_num)) {
    bitmap.clear_bit(rid->slot_num);
    page_header_->record_num--;
    frame_->mark_dirty();

    RC rc = log_handler_.delete_record(frame_, *rid);
    if (OB_FAIL(rc)) {
      LOG_ERROR("Failed to delete record. page_num %d:%d. rc=%s", disk_buffer_pool_->file_desc(), frame_->page_num(), strrc(rc));
      // return rc; // ignore errors
    }

    return RC::SUCCESS;
  } else {
    LOG_DEBUG("Invalid slot_num %d, slot is empty, page_num %d.", rid->slot_num, frame_->page_num());
    return RC::RECORD_NOT_EXIST;
  }
}

RC PaxRecordPageHandler::get_record(const RID &rid, Record &record)
{
  // your code here
  exit(-1);
}

// TODO: specify the column_ids that chunk needed. currenly we get all columns
RC PaxRecordPageHandler::get_chunk(Chunk &chunk)
{
  // your code here
  exit(-1);
}

char *PaxRecordPageHandler::get_field_data(SlotNum slot_num, int col_id)
{
  int *col_idx = reinterpret_cast<int *>(frame_->data() + page_header_->col_idx_offset);
  if (col_id == 0) {
    return frame_->data() + page_header_->data_offset + (get_field_len(col_id) * slot_num);
  } else {
    return frame_->data() + page_header_->data_offset + col_idx[col_id - 1] + (get_field_len(col_id) * slot_num);
  }
}

int PaxRecordPageHandler::get_field_len(int col_id)
{
  int *col_idx = reinterpret_cast<int *>(frame_->data() + page_header_->col_idx_offset);
  if (col_id == 0) {
    return col_idx[col_id] / page_header_->record_capacity;
  } else {
    return (col_idx[col_id] - col_idx[col_id - 1]) / page_header_->record_capacity;
  }
}

////////////////////////////////////////////////////////////////////////////////

RecordFileHandler::~RecordFileHandler() { this->close(); }

RC RecordFileHandler::init(DiskBufferPool &buffer_pool, LogHandler &log_handler, TableMeta *table_meta)
{
  if (disk_buffer_pool_ != nullptr) {
    LOG_ERROR("record file handler has been openned.");
    return RC::RECORD_OPENNED;
  }

  disk_buffer_pool_ = &buffer_pool;
  log_handler_      = &log_handler;
  table_meta_       = table_meta;

  RC rc = init_free_pages();

  LOG_INFO("open record file handle done. rc=%s", strrc(rc));
  return RC::SUCCESS;
}

void RecordFileHandler::close()
{
  if (disk_buffer_pool_ != nullptr) {
    free_pages_.clear();
    disk_buffer_pool_ = nullptr;
    log_handler_      = nullptr;
    table_meta_       = nullptr;
  }
}

RC RecordFileHandler::init_free_pages()
{
  // 遍历当前文件上所有页面，找到没有满的页面
  // 这个效率很低，会降低启动速度
  // NOTE: 由于是初始化时的动作，所以不需要加锁控制并发

  RC rc = RC::SUCCESS;

  BufferPoolIterator bp_iterator;
  bp_iterator.init(*disk_buffer_pool_, 1);
  unique_ptr<RecordPageHandler> record_page_handler(RecordPageHandler::create(storage_format_));
  PageNum                       current_page_num = 0;

  while (bp_iterator.has_next()) {
    current_page_num = bp_iterator.next();
    
    // 检查是否是TEXT溢出页（通过读取页面的第一个字段）
    Frame *check_frame = nullptr;
    if (disk_buffer_pool_->get_this_page(current_page_num, &check_frame) == RC::SUCCESS) {
      uint32_t first_field = *reinterpret_cast<uint32_t*>(check_frame->data());
      disk_buffer_pool_->unpin_page(check_frame);
      
      if (first_field == static_cast<uint32_t>(PageType::TEXT_OVERFLOW)) {
        // 这是TEXT溢出页，记录并跳过
        overflow_pages_.insert(current_page_num);
        LOG_TRACE("Identified TEXT overflow page %d during initialization", current_page_num);
        continue;
      }
    }

    rc = record_page_handler->init(*disk_buffer_pool_, *log_handler_, current_page_num, ReadWriteMode::READ_ONLY);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to init record page handler. page num=%d, rc=%d:%s", current_page_num, rc, strrc(rc));
      return rc;
    }

    if (!record_page_handler->is_full()) {
      free_pages_.insert(current_page_num);
    }
    record_page_handler->cleanup();
  }
  LOG_INFO("record file handler init free pages done. free page num=%d, overflow page num=%d, rc=%s", 
           free_pages_.size(), overflow_pages_.size(), strrc(rc));
  return rc;
}

RC RecordFileHandler::insert_record(const char *data, int record_size, RID *rid)
{
  RC ret = RC::SUCCESS;

  unique_ptr<RecordPageHandler> record_page_handler(RecordPageHandler::create(storage_format_));
  bool                          page_found       = false;
  PageNum                       current_page_num = 0;

  auto new_data = make_unique<char[]>(record_size);
  memcpy(new_data.get(), data, record_size);
  bool rewritten = false;   // 标记是否修改了数据

  // 检查每个字段，处理TEXT类型
  for (int i = 0; i < table_meta_->field_num(); i++) {
    const FieldMeta *field = table_meta_->field(i);
    
    if (field->type() != AttrType::TEXTS) {
      continue;
    }
    
    const char *field_data = new_data.get() + field->offset();
    
    int actual_len = strnlen(field_data, field->len());
    
    LOG_ERROR("📏 TEXT字段 '%s' 实际长度: %d, 字段最大长度: %d", 
              field->name(), actual_len, field->len());
    
    int inline_len = std::min(actual_len, INLINE_TEXT_CAPACITY);  // INLINE_TEXT_CAPACITY = 768
    int remain = actual_len - inline_len;
    
    // 🔧 修复内存重叠：提前保存内联数据
    auto inline_data_backup = make_unique<char[]>(inline_len);
    memcpy(inline_data_backup.get(), field_data, inline_len);
    
    LOG_ERROR("存储分析: 总长度=%d, 内联长度=%d, 溢出长度=%d", 
              actual_len, inline_len, remain);
    
    if (remain == 0) {
      LOG_ERROR("纯内联存储：无需溢出页");
    } else if (static_cast<uint32_t>(remain) <= ov_cap()) {
      LOG_ERROR("单页溢出存储：需要1个溢出页 (容量=%d)", ov_cap());

      // 1. 分配溢出页
      Frame *frame = nullptr;
      if ((ret = disk_buffer_pool_->allocate_page(&frame)) != RC::SUCCESS) {
        LOG_ERROR("Failed to allocate page while inserting record. ret:%d", ret);
        return ret;
      }
      
      // 标记为溢出页，以便scanner跳过
      mark_overflow_page(frame->page_num());

      // 2. 接下来要初始化溢出页头部
      frame->write_latch();
      OverflowPageHeader *header = hdr(frame);
      header->page_type = PageType::TEXT_OVERFLOW;
      header->next_page = BP_INVALID_PAGE_NUM;
      header->data_length = 0;  
      header->total_length = actual_len;

      // 3. 然后写入溢出数据
      ret = ov_write(frame, field_data + inline_len, remain);
      if (ret != RC::SUCCESS) {
        frame->write_unlatch();
        frame->unpin();
        LOG_ERROR("write overflow data failed: %s", strrc(ret));
        return ret;
      }

      PageNum overflow_page_num = frame->page_num();
      LOG_ERROR("成功写入 %d 字节溢出数据到页面 %d", remain, overflow_page_num);
      frame->write_unlatch();
      frame->unpin();

      // 4. 最后重写主记录的TEXT字段
      char *field_ptr = new_data.get() + field->offset();
      *reinterpret_cast<uint32_t*>(field_ptr + 0) = table_meta_->table_id();    // 表ID
      *reinterpret_cast<uint32_t*>(field_ptr + 4) = overflow_page_num;          // 页号
      *reinterpret_cast<uint32_t*>(field_ptr + 8) = sizeof(OverflowPageHeader); // 偏移
      *reinterpret_cast<uint64_t*>(field_ptr + 12) = actual_len;                // 长度

      // 5. 保留内联数据（前768字节）
      memcpy(field_ptr + 20, inline_data_backup.get(), inline_len);
      
      // 6. 清空字段剩余空间
      if (field->len() > 20 + inline_len) {
        memset(field_ptr + 20 + inline_len, 0, field->len() - 20 - inline_len);
      }

      rewritten = true;  // 标记数据被修改了
      LOG_ERROR("成功创建TEXT溢出指针：表ID=%d, 页号=%d, 偏移=%d, 总长度=%d", 
                table_meta_->table_id(), overflow_page_num, sizeof(OverflowPageHeader), actual_len);
    } else {
      LOG_INFO("Multi-page overflow: total=%d bytes, overflow=%d bytes", actual_len, remain);
      
      const uint32_t PAGE_CAPACITY = ov_cap(); 
      int num_pages = (remain + PAGE_CAPACITY - 1) / PAGE_CAPACITY;  
      
      LOG_INFO("需要分配 %d 个溢出页来存储 %d 字节的溢出数据", num_pages, remain);
      
      // 步骤2：准备循环分配溢出页
      PageNum first_page_num = BP_INVALID_PAGE_NUM;  // 记录第一页的页号
      Frame *prev_frame = nullptr;                    // 记录前一页的Frame指针
      uint32_t data_offset = inline_len;              // 当前在field_data中的偏移
      
      // 步骤3：循环分配每一个溢出页
      for (int i = 0; i < num_pages; i++) {
        // 3.1 分配新的溢出页
        Frame *frame = nullptr;
        if ((ret = disk_buffer_pool_->allocate_page(&frame)) != RC::SUCCESS) {
          LOG_ERROR("Failed to allocate overflow page %d/%d", i+1, num_pages);
          // TODO: 这里应该清理已分配的页面（步骤4会完善）
          return ret;
        }
        
        // 标记为溢出页，以便scanner跳过
        mark_overflow_page(frame->page_num());
        
        frame->write_latch();  // 加写锁
        
        // 3.2 初始化溢出页头部
        OverflowPageHeader *header = hdr(frame);
        header->page_type = PageType::TEXT_OVERFLOW;
        header->next_page = BP_INVALID_PAGE_NUM;  // 先设为结束，后面可能会被前一页更新
        
        uint32_t remaining_data = actual_len - data_offset;  
        uint32_t write_size = std::min(PAGE_CAPACITY, remaining_data);
        header->data_length = write_size;
        
        if (i == 0) {
          header->total_length = actual_len;
          first_page_num = frame->page_num();  
        } else {
          header->total_length = 0;
        }
        
        ret = ov_write(frame, field_data + data_offset, write_size);
        if (ret != RC::SUCCESS) {
          frame->write_unlatch();
          frame->unpin();
          LOG_ERROR("Failed to write data to overflow page %d", i+1);
          return ret;
        }
        
        if (prev_frame != nullptr) {
          OverflowPageHeader *prev_header = hdr(prev_frame);
          prev_header->next_page = frame->page_num();  
          prev_frame->mark_dirty();
          
          prev_frame->write_unlatch();
          prev_frame->unpin();
        }
        
        frame->mark_dirty();
        prev_frame = frame;  
        data_offset += write_size;  
        
        LOG_DEBUG("溢出页 %d/%d: page_num=%d, size=%u bytes", 
                 i+1, num_pages, frame->page_num(), write_size);
      }
      
      if (prev_frame != nullptr) {
        prev_frame->write_unlatch();
        prev_frame->unpin();
      }
      
      // 步骤4：在记录中写入溢出指针
      char *field_ptr = new_data.get() + field->offset();
      *reinterpret_cast<uint32_t*>(field_ptr + 0) = table_meta_->table_id();
      *reinterpret_cast<uint32_t*>(field_ptr + 4) = first_page_num;  // 指向第一个溢出页
      *reinterpret_cast<uint32_t*>(field_ptr + 8) = sizeof(OverflowPageHeader);
      *reinterpret_cast<uint64_t*>(field_ptr + 12) = actual_len;
      
      // 内联数据
      memcpy(field_ptr + 20, inline_data_backup.get(), inline_len);
      
      // 清空剩余空间
      if (field->len() > 20 + inline_len) {
        memset(field_ptr + 20 + inline_len, 0, field->len() - 20 - inline_len);
      }
      
      rewritten = true;
      LOG_INFO("Multi-page overflow created: first_page=%d, %d pages, %d bytes total", 
              first_page_num, num_pages, actual_len);
    }
  } 

  // 当前要访问free_pages对象，所以需要加锁。在非并发编译模式下，不需要考虑这个锁
  lock_.lock();

  // 找到没有填满的页面
  while (!free_pages_.empty()) {
    current_page_num = *free_pages_.begin();

    ret = record_page_handler->init(*disk_buffer_pool_, *log_handler_, current_page_num, ReadWriteMode::READ_WRITE);
    if (OB_FAIL(ret)) {
      lock_.unlock();
      LOG_WARN("failed to init record page handler. page num=%d, rc=%d:%s", current_page_num, ret, strrc(ret));
      return ret;
    }

    if (!record_page_handler->is_full()) {
      page_found = true;
      break;
    }
    record_page_handler->cleanup();
    free_pages_.erase(free_pages_.begin());
  }
  lock_.unlock();  // 如果找到了一个有效的页面，那么此时已经拿到了页面的写锁

  // 找不到就分配一个新的页面
  if (!page_found) {
    Frame *frame = nullptr;
    if ((ret = disk_buffer_pool_->allocate_page(&frame)) != RC::SUCCESS) {
      LOG_ERROR("Failed to allocate page while inserting record. ret:%d", ret);
      return ret;
    }

    current_page_num = frame->page_num();

    ret = record_page_handler->init_empty_page(
        *disk_buffer_pool_, *log_handler_, current_page_num, record_size, table_meta_);
    if (OB_FAIL(ret)) {
      frame->unpin();
      LOG_ERROR("Failed to init empty page. ret:%d", ret);
      // this is for allocate_page
      return ret;
    }

    // frame 在allocate_page的时候，是有一个pin的，在init_empty_page时又会增加一个，所以这里手动释放一个
    frame->unpin();

    // 这里的加锁顺序看起来与上面是相反的，但是不会出现死锁
    // 上面的逻辑是先加lock锁，然后加页面写锁，这里是先加上
    // 了页面写锁，然后加lock的锁，但是不会引起死锁。
    // 为什么？
    lock_.lock();
    free_pages_.insert(current_page_num);
    lock_.unlock();
  }

  // 找到空闲位置
  if (rewritten) {
    return record_page_handler->insert_record(new_data.get(), rid);
  } else {
    return record_page_handler->insert_record(data, rid);
  }
}

RC RecordFileHandler::recover_insert_record(const char *data, int record_size, const RID &rid)
{
  RC ret = RC::SUCCESS;

  unique_ptr<RecordPageHandler> record_page_handler(RecordPageHandler::create(storage_format_));

  ret = record_page_handler->recover_init(*disk_buffer_pool_, rid.page_num);
  if (OB_FAIL(ret)) {
    LOG_WARN("failed to init record page handler. page num=%d, rc=%s", rid.page_num, strrc(ret));
    return ret;
  }

  return record_page_handler->recover_insert_record(data, rid);
}

RC RecordFileHandler::free_text_overflow_pages(const Record &record)
{
  RC rc = RC::SUCCESS;
  
  // 遍历所有TEXT字段
  for (int i = 0; i < table_meta_->field_num(); i++) {
    const FieldMeta *field = table_meta_->field(i);
    if (field->type() != AttrType::TEXTS) {
      continue;
    }
    
    const char *field_ptr = record.data() + field->offset();
    
    // 检查是否是溢出指针
    if (!is_overflow_pointer(field_ptr, field->len(), table_meta_->table_id())) {
      continue;  // 不是溢出指针，跳过
    }
    
    // 读取溢出指针
    PageNum page_num = *reinterpret_cast<const PageNum*>(field_ptr + 4);
    
    // 沿着溢出页链释放所有页面
    PageNum current_page = page_num;
    int freed_count = 0;
    
    while (current_page != BP_INVALID_PAGE_NUM) {
      Frame *frame = nullptr;
      rc = disk_buffer_pool_->get_this_page(current_page, &frame);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to get overflow page %d for freeing", current_page);
        return rc;
      }
      
      // 读取下一页页号
      OverflowPageHeader *header = hdr(frame);
      PageNum next_page = header->next_page;
      
      disk_buffer_pool_->unpin_page(frame);
      
      // 释放当前页面
      rc = disk_buffer_pool_->dispose_page(current_page);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to dispose overflow page %d", current_page);
        return rc;
      }
      
      // 从溢出页集合中移除
      overflow_pages_.erase(current_page);
      freed_count++;
      
      current_page = next_page;
    }
    
    LOG_DEBUG("Freed %d overflow pages for TEXT field %s", freed_count, field->name());
  }
  
  return RC::SUCCESS;
}

uint32_t RecordFileHandler::get_overflow_page_capacity() const
{
  // 溢出页的容量 = 页面大小 - 溢出页头部大小
  return BP_PAGE_DATA_SIZE - sizeof(OverflowPageHeader);
}

RC RecordFileHandler::allocate_overflow_page(Frame **frame)
{
  RC rc = disk_buffer_pool_->allocate_page(frame);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to allocate overflow page");
    return rc;
  }
  
  // 标记为溢出页
  mark_overflow_page((*frame)->page_num());
  
  LOG_DEBUG("Allocated overflow page: %d", (*frame)->page_num());
  return RC::SUCCESS;
}

RC RecordFileHandler::delete_record(const RID *rid)
{
  RC rc = RC::SUCCESS;

  unique_ptr<RecordPageHandler> record_page_handler(RecordPageHandler::create(storage_format_));

  rc = record_page_handler->init(*disk_buffer_pool_, *log_handler_, rid->page_num, ReadWriteMode::READ_WRITE);
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to init record page handler.page number=%d. rc=%s", rid->page_num, strrc(rc));
    return rc;
  }

  // 在删除记录之前，先获取记录数据以释放TEXT溢出页
  Record record;
  rc = record_page_handler->get_record(*rid, record);
  if (OB_SUCC(rc)) {
    // 释放TEXT字段关联的所有溢出页
    rc = free_text_overflow_pages(record);
    if (OB_FAIL(rc)) {
      LOG_WARN("Failed to free TEXT overflow pages before delete, rc=%s", strrc(rc));
      // 继续删除主记录，即使溢出页释放失败
    }
  }

  rc = record_page_handler->delete_record(rid);
  // 📢 这里注意要清理掉资源，否则会与insert_record中的加锁顺序冲突而可能出现死锁
  // delete record的加锁逻辑是拿到页面锁，删除指定记录，然后加上和释放record manager锁
  // insert record是加上 record manager锁，然后拿到指定页面锁再释放record manager锁
  record_page_handler->cleanup();
  if (OB_SUCC(rc)) {
    // 因为这里已经释放了页面锁，并发时，其它线程可能又把该页面填满了，那就不应该再放入 free_pages_
    // 中。但是这里可以不关心，因为在查找空闲页面时，会自动过滤掉已经满的页面
    lock_.lock();
    free_pages_.insert(rid->page_num);
    LOG_TRACE("add free page %d to free page list", rid->page_num);
    lock_.unlock();
  }
  return rc;
}

RC RecordFileHandler::get_record(const RID &rid, Record &record)
{
  LOG_ERROR("RecordFileHandler::get_record 被调用，RID: %s", rid.to_string().c_str());
  
  unique_ptr<RecordPageHandler> page_handler(RecordPageHandler::create(storage_format_));

  RC rc = page_handler->init(*disk_buffer_pool_, *log_handler_, rid.page_num, ReadWriteMode::READ_WRITE);
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to init record page handler.page number=%d", rid.page_num);
    return rc;
  }

  Record inplace_record;
  rc = page_handler->get_record(rid, inplace_record);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to get record from record page handle. rid=%s, rc=%s", rid.to_string().c_str(), strrc(rc));
    return rc;
  }

  LOG_ERROR("准备调用 process_text_fields_on_read");
  rc = process_text_fields_on_read(table_meta_, disk_buffer_pool_, inplace_record, record);
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to process TEXT overflow fields. rid=%s", rid.to_string().c_str());
    return rc;
  }
  LOG_ERROR("process_text_fields_on_read 调用完成");
  record.set_rid(rid);
  
  LOG_ERROR("get_record 返回前检查: record.len=%d, record.data=%p", 
           record.len(), record.data());
  
  if (record.data() == nullptr) {
      LOG_ERROR("ERROR: record.data() is NULL before return!");
      return RC::INTERNAL;
  }
  
  return rc;
}

RC RecordFileHandler::visit_record(const RID &rid, function<bool(Record &)> updater)
{
  unique_ptr<RecordPageHandler> page_handler(RecordPageHandler::create(storage_format_));

  RC rc = page_handler->init(*disk_buffer_pool_, *log_handler_, rid.page_num, ReadWriteMode::READ_WRITE);
  if (OB_FAIL(rc)) {
    LOG_ERROR("Failed to init record page handler.page number=%d", rid.page_num);
    return rc;
  }

  Record inplace_record;
  rc = page_handler->get_record(rid, inplace_record);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to get record from record page handle. rid=%s, rc=%s", rid.to_string().c_str(), strrc(rc));
    return rc;
  }

  // 需要将数据复制出来再修改，否则update_record调用失败但是实际上数据却更新成功了，
  // 会导致数据库状态不正确
  Record record;
  record.copy_data(inplace_record.data(), inplace_record.len());
  record.set_rid(rid);

  bool updated = updater(record);
  if (updated) {
    rc = page_handler->update_record(rid, record.data());
  }
  return rc;
}

ChunkFileScanner::~ChunkFileScanner() { close_scan(); }

RC ChunkFileScanner::close_scan()
{
  if (disk_buffer_pool_ != nullptr) {
    disk_buffer_pool_ = nullptr;
  }

  if (record_page_handler_ != nullptr) {
    record_page_handler_->cleanup();
    delete record_page_handler_;
    record_page_handler_ = nullptr;
  }

  return RC::SUCCESS;
}

RC ChunkFileScanner::open_scan_chunk(
    Table *table, DiskBufferPool &buffer_pool, LogHandler &log_handler, ReadWriteMode mode)
{
  close_scan();

  table_            = table;
  disk_buffer_pool_ = &buffer_pool;
  log_handler_      = &log_handler;
  rw_mode_          = mode;

  RC rc = bp_iterator_.init(buffer_pool, 1);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to init bp iterator. rc=%d:%s", rc, strrc(rc));
    return rc;
  }
  if (table == nullptr || table->table_meta().storage_format() == StorageFormat::ROW_FORMAT) {
    record_page_handler_ = new RowRecordPageHandler();
  } else {
    record_page_handler_ = new PaxRecordPageHandler();
  }

  return rc;
}

RC ChunkFileScanner::next_chunk(Chunk &chunk)
{
  RC rc = RC::SUCCESS;

  while (bp_iterator_.has_next()) {
    PageNum page_num = bp_iterator_.next();
    record_page_handler_->cleanup();
    rc = record_page_handler_->init(*disk_buffer_pool_, *log_handler_, page_num, rw_mode_);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to init record page handler. page_num=%d, rc=%s", page_num, strrc(rc));
      return rc;
    }
    rc = record_page_handler_->get_chunk(chunk);
    if (rc == RC::SUCCESS) {
      return rc;
    } else if (rc == RC::RECORD_EOF) {
      break;
    } else {
      LOG_WARN("failed to get chunk from page. page_num=%d, rc=%s", page_num, strrc(rc));
      return rc;
    }
  }

  record_page_handler_->cleanup();
  return RC::RECORD_EOF;
}



