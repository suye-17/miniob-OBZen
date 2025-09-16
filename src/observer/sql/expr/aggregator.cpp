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
// Created by Wangyunlai on 2024/05/29.
//

#include "sql/expr/aggregator.h"
#include "common/log/log.h"

RC SumAggregator::accumulate(const Value &value)
{
  if (value.is_null()) {
    return RC::SUCCESS; // SUM忽略NULL值
  }

  if (value_.attr_type() == AttrType::UNDEFINED) {
    value_ = value;
    return RC::SUCCESS;
  }
  
  // 类型兼容性检查
  if (value.attr_type() != value_.attr_type()) {
    LOG_WARN("type mismatch in SUM. value type: %s, accumulated type: %s", 
            attr_type_to_string(value.attr_type()), attr_type_to_string(value_.attr_type()));
    return RC::INVALID_ARGUMENT;
  }
  
  Value::add(value, value_, value_);
  return RC::SUCCESS;
}

RC SumAggregator::evaluate(Value& result)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    result.set_null(); // 没有非NULL值参与计算
  } else {
    result = value_;
  }
  return RC::SUCCESS;
}

// CountAggregator implementation
RC CountAggregator::accumulate(const Value &value)
{
  // COUNT总是计数非NULL值，COUNT(*)已被转换为COUNT(1)
  if (!value.is_null()) {
    count_++;
  }
  return RC::SUCCESS;
}

RC CountAggregator::evaluate(Value &result)
{
  result.set_int(count_);
  return RC::SUCCESS;
}

// AvgAggregator implementation
RC AvgAggregator::accumulate(const Value &value)
{
  if (value.is_null()) {
    return RC::SUCCESS; // AVG忽略NULL值
  }

  if (!sum_initialized_) {
    sum_ = value;
    count_ = 1;
    sum_initialized_ = true;
  } else {
    Value::add(value, sum_, sum_);
    count_++;
  }
  return RC::SUCCESS;
}

RC AvgAggregator::evaluate(Value &result)
{
  if (count_ == 0 || !sum_initialized_) {
    result.set_null();
    return RC::SUCCESS;
  }

  Value count_value;
  // 确保除数类型与被除数兼容，AVG结果总是浮点数
  count_value.set_float((float)count_);
  
  // 先设置result的类型，然后进行除法
  result.set_type(AttrType::FLOATS);
  RC rc = Value::divide(sum_, count_value, result);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to calculate average. rc=%s", strrc(rc));
    result.set_null();
    return rc;
  }
  
  return RC::SUCCESS;
}

// MaxAggregator implementation
RC MaxAggregator::accumulate(const Value &value)
{
  if (value.is_null()) {
    return RC::SUCCESS; // MAX忽略NULL值
  }

  if (!has_value_ || value.compare(max_value_) > 0) {
    max_value_ = value;
    has_value_ = true;
  }
  return RC::SUCCESS;
}

RC MaxAggregator::evaluate(Value &result)
{
  if (!has_value_) {
    result.set_null();
  } else {
    result = max_value_;
  }
  return RC::SUCCESS;
}

// MinAggregator implementation
RC MinAggregator::accumulate(const Value &value)
{
  if (value.is_null()) {
    return RC::SUCCESS; // MIN忽略NULL值
  }

  if (!has_value_ || value.compare(min_value_) < 0) {
    min_value_ = value;
    has_value_ = true;
  }
  return RC::SUCCESS;
}

RC MinAggregator::evaluate(Value &result)
{
  if (!has_value_) {
    result.set_null();
  } else {
    result = min_value_;
  }
  return RC::SUCCESS;
}
