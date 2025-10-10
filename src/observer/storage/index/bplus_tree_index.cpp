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
bool BplusTreeIndex::is_null_key(const char *key_data, int key_len) const
{
  // MySQL标准：NULL值在miniob中用0xFF填充整个字段
  
  if (field_metas_.size() == 1) {
    // 单字段索引：检查是否全为0xFF
    for (int i = 0; i < key_len; i++) {
      if ((unsigned char)key_data[i] != 0xFF) {
        return false;
      }
    }
    return true;
  } else {
    // 多字段索引：只要有一个字段为NULL，整个键就包含NULL
    int offset = 0;
    for (const FieldMeta &field_meta : field_metas_) {
      int field_len = field_meta.len();
      const char *field_data = key_data + offset;
      
      // 检查这个字段是否为NULL（全为0xFF）
      bool is_field_null = true;
      for (int i = 0; i < field_len; i++) {
        if ((unsigned char)field_data[i] != 0xFF) {
          is_field_null = false;
          break;
        }
      }
      
      if (is_field_null) {
        return true;  // 任意字段为NULL，整个键包含NULL
      }
      
      offset += field_len;
    }
    return false;
  }
}

RC BplusTreeIndex::check_key_exists(const char *key_data, int key_len, bool &exists)
{
  exists = false;
  list<RID> rids;
  RC rc = index_handler_.get_entry(key_data, key_len, rids);
  if (rc == RC::SUCCESS) {
    exists = !rids.empty();
  }
  else if (rc == RC::RECORD_NOT_EXIST) {
    exists = false;
    rc = RC::SUCCESS;
  }
  return rc;
}

RC BplusTreeIndex::build_index(const char *record, char *&composite_key, int &key_len)
{
  if (field_metas_.size() == 1) {
    const FieldMeta &field_meta = field_metas_[0];
    key_len = field_meta.len();  
    composite_key = new char[key_len];
    memcpy(composite_key, record + field_meta.offset(), key_len);
  } else {
    key_len = 0;
    for (const FieldMeta &field_meta : field_metas_) {
      key_len += field_meta.len();  
    }
    composite_key = new char[key_len];
    int offset = 0;
    for (const FieldMeta &field_meta : field_metas_) {
      const char *field_data = record + field_meta.offset();
      int field_len = field_meta.len();  
      memcpy(composite_key + offset, field_data, field_len);
      offset += field_len;
    }
  }
  return RC::SUCCESS;
}

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
  char *key_data = nullptr;
  int key_len = 0;
  RC rc = build_index(record, key_data, key_len);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to build index. rc=%d:%s", rc, strrc(rc));
    return rc;
  }
  
  // UNIQUE索引：MySQL标准行为
  if (index_meta_.is_unique()) {
    if (is_null_key(key_data, key_len)) {
      // NULL值不插入索引（MySQL标准：允许多个NULL共存）
      delete[] key_data;
      LOG_DEBUG("UNIQUE index: NULL key not inserted into index (MySQL behavior)");
      return RC::SUCCESS;
    } else {
      // 非NULL值需要检查重复
      bool exists = false;
      rc = check_key_exists(key_data, key_len, exists);
      if (rc != RC::SUCCESS) {
        delete[] key_data;
        LOG_WARN("Failed to check key existence. rc=%d:%s", rc, strrc(rc));
        return rc;
      }
      
      if (exists) {
        delete[] key_data;
        LOG_DEBUG("Duplicate key found in unique index");
        return RC::RECORD_DUPLICATE_KEY;
      }
    }
  }
  
  // 插入索引
  rc = index_handler_.insert_entry(key_data, rid);
  delete[] key_data;
  
  return rc;
}

RC BplusTreeIndex::delete_entry(const char *record, const RID *rid)
{
  // UNIQUE索引：如果是NULL值，不需要从索引删除（因为根本没插入）
  if (index_meta_.is_unique()) {
    char *key_data = nullptr;
    int key_len = 0;
    RC rc = build_index(record, key_data, key_len);
    if (rc != RC::SUCCESS) {
      LOG_WARN("Failed to build index for delete. rc=%d:%s", rc, strrc(rc));
      return rc;
    }
    
    if (is_null_key(key_data, key_len)) {
      delete[] key_data;
      LOG_DEBUG("UNIQUE index: NULL key not in index, skip delete");
      return RC::SUCCESS;
    }
    delete[] key_data;
  }
  
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
  
  if (inited_) {
    LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s",
        file_name, index_meta.name(), field_metas[0]->name());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);

  // 计算组合键的总长度
  int total_key_len = 0;
  for (const FieldMeta *field_meta : field_metas) {
    total_key_len += field_meta->len();
  }
  
  // 多字段索引使用CHARS类型存储组合键（字节串拼接）
  AttrType key_type = (field_metas.size() == 1) ? field_metas[0]->type() : AttrType::CHARS;
  
  LOG_INFO("Creating multi-field index. file_name:%s, index:%s, field_count:%zu, total_key_len:%d, first_field:%s",
      file_name, index_meta.name(), field_metas.size(), total_key_len, field_metas[0]->name());

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();
  RC rc = index_handler_.create(table->db()->log_handler(), bpm, file_name, key_type, total_key_len);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), field_metas[0]->name(), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully create multi-field index, file_name:%s, index:%s, field_count:%zu, total_key_len:%d",
    file_name, index_meta.name(), field_metas.size(), total_key_len);
  return RC::SUCCESS;
}

RC BplusTreeIndex::open(Table *table, const char *file_name, const IndexMeta &index_meta, const vector<const FieldMeta *> &field_metas)
{
  if (field_metas.empty()) {
    LOG_WARN("Failed to open index due to empty field_metas. file_name:%s, index:%s", file_name, index_meta.name());
    return RC::INVALID_ARGUMENT;
  }
  
  if (inited_) {
    LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s",
        file_name, index_meta.name(), field_metas[0]->name());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);

  // 计算组合键的总长度（用于日志）
  int total_key_len = 0;
  for (const FieldMeta *field_meta : field_metas) {
    total_key_len += field_meta->len();
  }
  
  LOG_INFO("Opening multi-field index. file_name:%s, index:%s, field_count:%zu, total_key_len:%d, first_field:%s",
      file_name, index_meta.name(), field_metas.size(), total_key_len, field_metas[0]->name());

  BufferPoolManager &bpm = table->db()->buffer_pool_manager();
  RC rc = index_handler_.open(table->db()->log_handler(), bpm, file_name);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name, index_meta.name(), field_metas[0]->name(), strrc(rc));
    return rc;
  }

  inited_ = true;
  table_  = table;
  LOG_INFO("Successfully open multi-field index, file_name:%s, index:%s, field_count:%zu, total_key_len:%d",
    file_name, index_meta.name(), field_metas.size(), total_key_len);
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

