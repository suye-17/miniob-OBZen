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
#include "common/log/log.h"
#include "common/type/char_type.h"
#include "common/value.h"
#include "common/utils.h"

int CharType::compare(const Value &left, const Value &right) const
{
  // 确保左操作数是字符串类型
  if (left.attr_type() != AttrType::CHARS) {
    LOG_WARN("Left operand is not a string type: %d", static_cast<int>(left.attr_type()));
    return INT32_MAX;
  }

  switch (right.attr_type()) {
    case AttrType::CHARS: {
      // 字符串与字符串比较
      return common::compare_string(
          (void *)left.value_.pointer_value_, left.length_, 
          (void *)right.value_.pointer_value_, right.length_);
    }
    case AttrType::INTS: {
      // 字符串与整数比较：将字符串转为整数进行比较
      int left_as_int = left.get_int();  // 使用Value类已有的转换方法
      int right_int = right.get_int();
      if (left_as_int < right_int) return -1;
      if (left_as_int > right_int) return 1;
      return 0;
    }
    case AttrType::FLOATS: {
      // 字符串与浮点数比较：将字符串转为浮点数进行比较
      float left_as_float = left.get_float();
      float right_float = right.get_float();
      if (left_as_float < right_float) return -1;
      if (left_as_float > right_float) return 1;
      return 0;
    }
    default: {
      LOG_WARN("Unsupported type comparison between CHARS and %d", static_cast<int>(right.attr_type()));
      return INT32_MAX;
    }
  }
}

RC CharType::set_value_from_str(Value &val, const string &data) const
{
  val.set_string(data.c_str());
  return RC::SUCCESS;
}

RC CharType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::CHARS: {
      // 字符串到字符串，直接复制
      result = val;
      return RC::SUCCESS;
    }
    case AttrType::INTS: {
      // 字符串转整数
      try {
        int int_value = std::stoi(val.value_.pointer_value_);
        result.set_int(int_value);
        return RC::SUCCESS;
      } catch (const std::exception& e) {
        LOG_WARN("Failed to convert string '%s' to integer: %s", val.value_.pointer_value_, e.what());
        result.set_int(0);  // MySQL行为：转换失败时返回0
        return RC::SUCCESS;
      }
    }
    case AttrType::FLOATS: {
      // 字符串转浮点数
      try {
        float float_value = std::stof(val.value_.pointer_value_);
        result.set_float(float_value);
        return RC::SUCCESS;
      } catch (const std::exception& e) {
        LOG_WARN("Failed to convert string '%s' to float: %s", val.value_.pointer_value_, e.what());
        result.set_float(0.0f);  // MySQL行为：转换失败时返回0.0
        return RC::SUCCESS;
      }
    }
    case AttrType::DATES: {
      int date;
      RC rc = parse_date(val.value_.pointer_value_, date);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      result.set_int(date);
      return RC::SUCCESS;
    }
    default: 
      LOG_WARN("Unsupported cast from CHARS to type %d", static_cast<int>(type));
      return RC::UNIMPLEMENTED;
  }
}

int CharType::cast_cost(AttrType type)
{
  if (type == AttrType::CHARS) {
    return 0;
  }
  if (type == AttrType::DATES) {
    return 1;
  }
  if (type == AttrType::INTS) {
    return 1;
  }
  if (type == AttrType::FLOATS) {
    return 1;
  }
  return INT32_MAX;
}

RC CharType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}