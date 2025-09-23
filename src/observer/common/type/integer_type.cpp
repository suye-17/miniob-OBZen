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
#include "common/type/integer_type.h"
#include "common/value.h"

int IntegerType::compare(const Value &left, const Value &right) const
{
  // 确保左操作数是整数类型
  if (left.attr_type() != AttrType::INTS) {
    LOG_WARN("Left operand is not an integer type: %d", static_cast<int>(left.attr_type()));
    return INT32_MAX;
  }

  switch (right.attr_type()) {
    case AttrType::INTS: {
      // 整数与整数比较
      return common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
    }
    case AttrType::FLOATS: {
      // 整数与浮点数比较
      float left_val  = left.get_float();
      float right_val = right.get_float();
      return common::compare_float((void *)&left_val, (void *)&right_val);
    }
    case AttrType::CHARS: {
      // 整数与字符串比较：将字符串转为整数进行比较
      int left_int = left.get_int();
      int right_as_int = right.get_int();  // 使用Value类的转换方法
      if (left_int < right_as_int) return -1;
      if (left_int > right_as_int) return 1;
      return 0;
    }
    default: {
      LOG_WARN("Unsupported type comparison between INTS and %d", static_cast<int>(right.attr_type()));
      return INT32_MAX;
    }
  }
}

int IntegerType::cast_cost(AttrType type)
{
  if (type == AttrType::INTS) {
    return 0;  // 同类型转换成本为0
  }
  if (type == AttrType::FLOATS) {
    return 1;  // 整数转浮点数成本较低
  }
  return INT32_MAX;  // 其他类型转换不支持
}

RC IntegerType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
  case AttrType::FLOATS: {
    float float_value = val.get_int();
    result.set_float(float_value);
    return RC::SUCCESS;
  }
  default:
    LOG_WARN("unsupported type %d", type);
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
}

RC IntegerType::add(const Value &left, const Value &right, Value &result) const
{
  result.set_int(left.get_int() + right.get_int());
  return RC::SUCCESS;
}

RC IntegerType::subtract(const Value &left, const Value &right, Value &result) const
{
  result.set_int(left.get_int() - right.get_int());
  return RC::SUCCESS;
}

RC IntegerType::multiply(const Value &left, const Value &right, Value &result) const
{
  result.set_int(left.get_int() * right.get_int());
  return RC::SUCCESS;
}

RC IntegerType::negative(const Value &val, Value &result) const
{
  result.set_int(-val.get_int());
  return RC::SUCCESS;
}

RC IntegerType::set_value_from_str(Value &val, const string &data) const
{
  RC                rc = RC::SUCCESS;
  stringstream deserialize_stream;
  deserialize_stream.clear();  // 清理stream的状态，防止多次解析出现异常
  deserialize_stream.str(data);
  int int_value;
  deserialize_stream >> int_value;
  if (!deserialize_stream || !deserialize_stream.eof()) {
    rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
  } else {
    val.set_int(int_value);
  }
  return rc;
}

RC IntegerType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.int_value_;
  result = ss.str();
  return RC::SUCCESS;
}