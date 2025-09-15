/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/type/boolean_type.h"
#include "common/value.h"
#include "common/log/log.h"

RC BooleanType::to_string(const Value &val, string &result) const
{
  if (val.is_null()) {
    result = "NULL";
    return RC::SUCCESS;
  }
  
  bool value = val.get_boolean();
  result = value ? "1" : "0";  // MySQL风格：true显示为1，false显示为0
  return RC::SUCCESS;
}

RC BooleanType::cast_to(const Value &val, AttrType type, Value &result) const
{
  if (val.is_null()) {
    result.set_null();
    return RC::SUCCESS;
  }
  
  bool value = val.get_boolean();
  switch (type) {
    case AttrType::BOOLEANS: {
      result = val;
      return RC::SUCCESS;
    }
    case AttrType::INTS: {
      result.set_int(value ? 1 : 0);
      return RC::SUCCESS;
    }
    case AttrType::FLOATS: {
      result.set_float(value ? 1.0f : 0.0f);
      return RC::SUCCESS;
    }
    case AttrType::CHARS: {
      result.set_string(value ? "1" : "0");
      return RC::SUCCESS;
    }
    default: {
      return RC::UNIMPLEMENTED;
    }
  }
}

int BooleanType::compare(const Value &left, const Value &right) const
{
  if (left.is_null() && right.is_null()) {
    return 0;
  }
  if (left.is_null()) {
    return -1;
  }
  if (right.is_null()) {
    return 1;
  }
  
  bool left_val = left.get_boolean();
  bool right_val = right.get_boolean();
  
  if (left_val == right_val) {
    return 0;
  }
  return left_val ? 1 : -1;  // true > false
}
