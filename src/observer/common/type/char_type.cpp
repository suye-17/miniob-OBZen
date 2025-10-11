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
#include "common/type/attr_type.h"
#include "common/type/char_type.h"
#include "common/value.h"
#include "common/utils.h"

int CharType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::CHARS, "left value must be CHARS type");
  if (right.attr_type() == AttrType::CHARS || right.attr_type() == AttrType::TEXTS) {
    return common::compare_string(
        (void *)left.value_.pointer_value_, left.length_, 
        (void *)right.value_.pointer_value_, right.length_);
  }
  LOG_WARN("CharType::compare: unsupported right type %d", static_cast<int>(right.attr_type()));
  return INT32_MAX; 
}

RC CharType::set_value_from_str(Value &val, const string &data) const
{
  val.set_string(data.c_str());
  return RC::SUCCESS;
}

RC CharType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::DATES: {
      int date;
      RC rc = parse_date(val.value_.pointer_value_, date);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      result.set_int(date);
    } break;
    case AttrType::VECTORS: {
      std::vector<float> elements;
      RC rc = parse_vector_literal(val.value_.pointer_value_, elements);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      result.set_vector(elements);
    } break;
    case AttrType::TEXTS: {
      result.set_text(val.value_.pointer_value_, val.length_);
    } break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
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
  if (type == AttrType::VECTORS) {
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