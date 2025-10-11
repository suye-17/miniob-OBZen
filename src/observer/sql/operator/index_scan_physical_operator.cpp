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
// Created by Wangyunlai on 2022/07/08.
//

#include "sql/operator/index_scan_physical_operator.h"
#include "storage/index/index.h"
#include "storage/trx/trx.h"

IndexScanPhysicalOperator::IndexScanPhysicalOperator(Table *table, Index *index, ReadWriteMode mode,
    const Value *left_value, bool left_inclusive, const Value *right_value, bool right_inclusive)
    : table_(table),
      index_(index),
      mode_(mode),
      use_composite_key_(false),
      left_inclusive_(left_inclusive),
      right_inclusive_(right_inclusive)
{
  if (left_value) {
    left_value_ = *left_value;
  }
  if (right_value) {
    right_value_ = *right_value;
  }
}

IndexScanPhysicalOperator::IndexScanPhysicalOperator(Table *table, Index *index, ReadWriteMode mode,
    const vector<Value> &left_values, bool left_inclusive, const vector<Value> &right_values, bool right_inclusive)
    : table_(table),
      index_(index),
      mode_(mode),
      use_composite_key_(true),
      left_values_(left_values),
      right_values_(right_values),
      left_inclusive_(left_inclusive),
      right_inclusive_(right_inclusive)
{}

RC IndexScanPhysicalOperator::open(Trx *trx)
{
  if (nullptr == table_ || nullptr == index_) {
    return RC::INTERNAL;
  }

  IndexScanner *index_scanner = nullptr;

  if (use_composite_key_) {
    char *left_key  = nullptr;
    char *right_key = nullptr;
    int   left_len  = 0;
    int   right_len = 0;

    if (!left_values_.empty()) {
      RC rc = build_composite_key(left_values_, left_key, left_len, true);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to build left composite key");
        return rc;
      }
    }

    if (!right_values_.empty()) {
      RC rc = build_composite_key(right_values_, right_key, right_len, false);
      if (rc != RC::SUCCESS) {
        delete[] left_key;
        LOG_WARN("failed to build right composite key");
        return rc;
      }
    }

    index_scanner = index_->create_scanner(left_key, left_len, left_inclusive_, right_key, right_len, right_inclusive_);

    delete[] left_key;
    delete[] right_key;
  } else {
    index_scanner = index_->create_scanner(left_value_.data(),
        left_value_.length(),
        left_inclusive_,
        right_value_.data(),
        right_value_.length(),
        right_inclusive_);
  }

  if (nullptr == index_scanner) {
    LOG_WARN("failed to create index scanner");
    return RC::INTERNAL;
  }
  index_scanner_ = index_scanner;

  tuple_.set_schema(table_, table_->table_meta().field_metas());

  trx_ = trx;
  return RC::SUCCESS;
}

RC IndexScanPhysicalOperator::next()
{
  // TODO: 需要适配 lsm-tree 引擎
  RID rid;
  RC  rc = RC::SUCCESS;

  bool filter_result = false;
  while (RC::SUCCESS == (rc = index_scanner_->next_entry(&rid))) {
    rc = table_->get_record(rid, current_record_);
    if (OB_FAIL(rc)) {
      LOG_TRACE("failed to get record. rid=%s, rc=%s", rid.to_string().c_str(), strrc(rc));
      return rc;
    }

    LOG_TRACE("got a record. rid=%s", rid.to_string().c_str());

    tuple_.set_record(&current_record_);
    rc = filter(tuple_, filter_result);
    if (OB_FAIL(rc)) {
      LOG_TRACE("failed to filter record. rc=%s", strrc(rc));
      return rc;
    }

    if (!filter_result) {
      LOG_TRACE("record filtered");
      continue;
    }

    rc = trx_->visit_record(table_, current_record_, mode_);
    if (rc == RC::RECORD_INVISIBLE) {
      LOG_TRACE("record invisible");
      continue;
    } else {
      return rc;
    }
  }

  if (rc != RC::RECORD_EOF && rc != RC::SUCCESS) {
    LOG_WARN("index scanner error: rc=%s", strrc(rc));
  }

  return rc;
}

RC IndexScanPhysicalOperator::close()
{
  index_scanner_->destroy();
  index_scanner_ = nullptr;
  return RC::SUCCESS;
}

Tuple *IndexScanPhysicalOperator::current_tuple()
{
  tuple_.set_record(&current_record_);
  return &tuple_;
}

void IndexScanPhysicalOperator::set_predicates(vector<unique_ptr<Expression>> &&exprs)
{
  predicates_ = std::move(exprs);
}

RC IndexScanPhysicalOperator::filter(RowTuple &tuple, bool &result)
{
  RC    rc = RC::SUCCESS;
  Value value;
  for (unique_ptr<Expression> &expr : predicates_) {
    rc = expr->get_value(tuple, value);
    if (rc != RC::SUCCESS) {
      return rc;
    }

    bool tmp_result = value.get_boolean();
    if (!tmp_result) {
      result = false;
      return rc;
    }
  }

  result = true;
  return rc;
}

RC IndexScanPhysicalOperator::build_composite_key(
    const vector<Value> &values, char *&key, int &key_len, bool is_left_key)
{
  const IndexMeta      &index_meta  = index_->index_meta();
  const vector<string> &field_names = index_meta.fields();

  if (values.size() > field_names.size()) {
    LOG_WARN("too many values for composite key. values=%zu, fields=%zu", 
             values.size(), field_names.size());
    return RC::INVALID_ARGUMENT;
  }

  key_len = 0;
  for (size_t i = 0; i < field_names.size(); i++) {
    const FieldMeta *field = table_->table_meta().field(field_names[i].c_str());
    if (field == nullptr) {
      LOG_WARN("field not found: %s", field_names[i].c_str());
      return RC::INTERNAL;
    }
    key_len += field->len();
  }

  key        = new char[key_len];
  int offset = 0;

  for (size_t i = 0; i < values.size(); i++) {
    const FieldMeta *field      = table_->table_meta().field(field_names[i].c_str());
    int              field_len  = field->len();
    int              actual_len = std::min(values[i].length(), field_len);

    memcpy(key + offset, values[i].data(), actual_len);
    if (actual_len < field_len) {
      memset(key + offset + actual_len, 0, field_len - actual_len);
    }

    offset += field_len;
  }

  if (offset < key_len) {
    unsigned char fill_byte = is_left_key ? 0x00 : 0xFF;
    memset(key + offset, fill_byte, key_len - offset);
  }

  return RC::SUCCESS;
}

string IndexScanPhysicalOperator::param() const
{
  return string(index_->index_meta().name()) + " ON " + table_->name();
}
