/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by wangyunlai.wyl on 2021/5/19.
//

#include "storage/index/bplus_tree_index.h"
#include <cstring>
#include "common/log/log.h"
#include "storage/table/table.h"
#include "storage/db/db.h"

BplusTreeIndex::~BplusTreeIndex() noexcept { close(); }

RC BplusTreeIndex::create(Table *table, const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  if (inited_) {
    LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s",
        file_name, index_meta.name(), index_meta.field());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_meta);

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();
  RC rc = index_handler_.create(table->db()->log_handler(), bpm, file_name, field_meta.type(), field_meta.len());
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), index_meta.field(), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully create index, file_name:%s, index:%s, field:%s",
    file_name, index_meta.name(), index_meta.field());
  return RC::SUCCESS;
}

RC BplusTreeIndex::open(Table *table, const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  if (inited_) {
    LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s",
        file_name, index_meta.name(), index_meta.field());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_meta);

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();
  RC rc = index_handler_.open(table->db()->log_handler(), bpm, file_name);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), index_meta.field(), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully open index, file_name:%s, index:%s, field:%s",
    file_name, index_meta.name(), index_meta.field());
  return RC::SUCCESS;
}

RC BplusTreeIndex::close()
{
  if (inited_) {
    LOG_INFO("Begin to close index, index:%s, field:%s", index_meta_.name(), index_meta_.field());
    index_handler_.close();
    inited_ = false;
  }
  LOG_INFO("Successfully close index.");
  return RC::SUCCESS;
}

RC BplusTreeIndex::insert_entry(const char *record, const RID *rid)
{
  if (field_metas_.size() == 1) {
    // 单字段索引（向后兼容）
    const char *key_data = record + field_meta_.offset();
    return index_handler_.insert_entry(key_data, rid);
  } else {
    // 多字段索引 - 构造组合键值
    LOG_DEBUG("Multi-field index insert, field count: %zu", field_metas_.size());
    
    // 计算组合键值的总长度
    int total_key_length = 0;
    for (const FieldMeta &field_meta : field_metas_) {
      total_key_length += field_meta.len();
    }
    
    // 分配内存存储组合键值
    char *composite_key = new char[total_key_length];
    int offset = 0;
    
    // 构造组合键值
    for (const FieldMeta &field_meta : field_metas_) {
      const char *field_data = record + field_meta.offset();
      int field_len = field_meta.len();
      
      // 复制字段数据到组合键值中
      memcpy(composite_key + offset, field_data, field_len);
      offset += field_len;
    }
    
    RC rc = index_handler_.insert_entry(composite_key, rid);
    delete[] composite_key;
    return rc;
  }
}

RC BplusTreeIndex::delete_entry(const char *record, const RID *rid)
{
  if (field_metas_.size() == 1) {
    // 单字段索引（向后兼容）
    return index_handler_.delete_entry(record + field_meta_.offset(), rid);
  } else {
    // 多字段索引 - 构造组合键值
    // 计算组合键值的总长度
    int total_key_length = 0;
    for (const FieldMeta &field_meta : field_metas_) {
      total_key_length += field_meta.len();
    }
    
    // 分配内存存储组合键值
    char *composite_key = new char[total_key_length];
    int offset = 0;
    
    // 构造组合键值
    for (const FieldMeta &field_meta : field_metas_) {
      const char *field_data = record + field_meta.offset();
      int field_len = field_meta.len();
      
      // 复制字段数据到组合键值中
      memcpy(composite_key + offset, field_data, field_len);
      offset += field_len;
    }
    
    RC rc = index_handler_.delete_entry(composite_key, rid);
    delete[] composite_key;
    return rc;
  }
}

IndexScanner *BplusTreeIndex::create_scanner(
    const char *left_key, int left_len, bool left_inclusive, const char *right_key, int right_len, bool right_inclusive)
{
  BplusTreeIndexScanner *index_scanner = new BplusTreeIndexScanner(index_handler_);
  RC rc = index_scanner->open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete index_scanner;
    return nullptr;
  }
  return index_scanner;
}

RC BplusTreeIndex::sync() { return index_handler_.sync(); }

RC BplusTreeIndex::create(Table *table, const char *file_name, const IndexMeta &index_meta, const vector<const FieldMeta *> &field_metas)
{
  if (field_metas.empty()) {
    LOG_WARN("Failed to create index due to empty field_metas. file_name:%s, index:%s", file_name, index_meta.name());
    return RC::INVALID_ARGUMENT;
  }
  
  // 当前B+树索引实现暂时只支持第一个字段，多字段支持将在后续实现
  LOG_INFO("Creating multi-field index using first field. file_name:%s, index:%s, field_count:%zu, first_field:%s",
      file_name, index_meta.name(), field_metas.size(), field_metas[0]->name());
  
  if (inited_) {
    LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s",
        file_name, index_meta.name(), field_metas[0]->name());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();
  RC rc = index_handler_.create(table->db()->log_handler(), bpm, file_name, field_metas[0]->type(), field_metas[0]->len());
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), field_metas[0]->name(), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully create multi-field index, file_name:%s, index:%s, field_count:%zu",
    file_name, index_meta.name(), field_metas.size());
  return RC::SUCCESS;
}

RC BplusTreeIndex::open(Table *table, const char *file_name, const IndexMeta &index_meta, const vector<const FieldMeta *> &field_metas)
{
  if (field_metas.empty()) {
    LOG_WARN("Failed to open index due to empty field_metas. file_name:%s, index:%s", file_name, index_meta.name());
    return RC::INVALID_ARGUMENT;
  }
  
  // 当前B+树索引实现暂时只支持第一个字段，多字段支持将在后续实现
  LOG_INFO("Opening multi-field index using first field. file_name:%s, index:%s, field_count:%zu, first_field:%s",
      file_name, index_meta.name(), field_metas.size(), field_metas[0]->name());
  
  if (inited_) {
    LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s",
        file_name, index_meta.name(), field_metas[0]->name());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();
  RC rc = index_handler_.open(table->db()->log_handler(), bpm, file_name);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), field_metas[0]->name(), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully open multi-field index, file_name:%s, index:%s, field_count:%zu",
    file_name, index_meta.name(), field_metas.size());
  return RC::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
BplusTreeIndexScanner::BplusTreeIndexScanner(BplusTreeHandler &tree_handler) : tree_scanner_(tree_handler) {}

BplusTreeIndexScanner::~BplusTreeIndexScanner() noexcept { tree_scanner_.close(); }

RC BplusTreeIndexScanner::open(
    const char *left_key, int left_len, bool left_inclusive, const char *right_key, int right_len, bool right_inclusive)
{
  return tree_scanner_.open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
}

RC BplusTreeIndexScanner::next_entry(RID *rid) { return tree_scanner_.next_entry(*rid); }

RC BplusTreeIndexScanner::destroy()
{
  delete this;
  return RC::SUCCESS;
}
