/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/float_type.h"
#include "common/value.h"
#include "common/lang/limits.h"
#include "common/value.h"

int FloatType::compare(const Value &left, const Value &right) const
{
  // 确保左操作数是浮点数类型
  if (left.attr_type() != AttrType::FLOATS) {
    LOG_WARN("Left operand is not a float type: %d", static_cast<int>(left.attr_type()));
    return INT32_MAX;
  }

  switch (right.attr_type()) {
    case AttrType::FLOATS: {
      // 浮点数与浮点数比较
      float left_val  = left.get_float();
      float right_val = right.get_float();
      return common::compare_float((void *)&left_val, (void *)&right_val);
    }
    case AttrType::INTS: {
      // 浮点数与整数比较
      float left_val  = left.get_float();
      float right_val = right.get_float();  // get_float()会自动转换整数为浮点数
      return common::compare_float((void *)&left_val, (void *)&right_val);
    }
    case AttrType::CHARS: {
      // 浮点数与字符串比较：将字符串转为浮点数进行比较
      float left_float = left.get_float();
      float right_as_float = right.get_float();  // 使用Value类的转换方法
      if (left_float < right_as_float) return -1;
      if (left_float > right_as_float) return 1;
      return 0;
    }
    default: {
      LOG_WARN("Unsupported type comparison between FLOATS and %d", static_cast<int>(right.attr_type()));
      return INT32_MAX;
    }
  }
}

RC FloatType::add(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
    return RC::SUCCESS;
  }
  result.set_float(left.get_float() + right.get_float());
  return RC::SUCCESS;
}
RC FloatType::subtract(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
    return RC::SUCCESS;
  }
  result.set_float(left.get_float() - right.get_float());
  return RC::SUCCESS;
}
RC FloatType::multiply(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
    return RC::SUCCESS;
  }
  result.set_float(left.get_float() * right.get_float());
  return RC::SUCCESS;
}

RC FloatType::divide(const Value &left, const Value &right, Value &result) const
{
  if (left.is_null() || right.is_null()) {
    result.set_null();
    return RC::SUCCESS;
  }

  float left_val  = left.get_float();
  float right_val = right.get_float();
  LOG_INFO("FloatType::divide: %f / %f", left_val, right_val);

  if (right_val > -EPSILON && right_val < EPSILON) {
    // 除零返回NULL（符合SQL标准）
    result.set_null();
    result.set_type(AttrType::FLOATS);  // 保持运算结果类型
    LOG_INFO("FloatType::divide: Division by zero, returning NULL");
  } else {
    float div_result = left_val / right_val;
    result.set_float(div_result);
    LOG_INFO("FloatType::divide: Result = %f", div_result);
  }
  return RC::SUCCESS;
}

RC FloatType::negative(const Value &val, Value &result) const
{
  if (val.is_null()) {
    result.set_null();
    return RC::SUCCESS;
  }
  result.set_float(-val.get_float());
  return RC::SUCCESS;
}

int FloatType::cast_cost(AttrType type)
{
  if (type == AttrType::FLOATS) {
    return 0;  // 同类型转换成本为0
  }
  if (type == AttrType::INTS) {
    return 2;  // 浮点数转整数成本较高（可能丢失精度）
  }
  return INT32_MAX;  // 其他类型转换不支持
}

RC FloatType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::INTS: {
      int int_value = (int)val.get_float();  // 截断小数部分
      result.set_int(int_value);
      return RC::SUCCESS;
    }
    default: LOG_WARN("unsupported type %d", type); return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
}

RC FloatType::set_value_from_str(Value &val, const string &data) const
{
  RC           rc = RC::SUCCESS;
  stringstream deserialize_stream;
  deserialize_stream.clear();
  deserialize_stream.str(data);

  float float_value;
  deserialize_stream >> float_value;
  if (!deserialize_stream || !deserialize_stream.eof()) {
    rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
  } else {
    val.set_float(float_value);
  }
  return rc;
}

RC FloatType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << common::double_to_str(val.value_.float_value_);
  result = ss.str();
  return RC::SUCCESS;
}
