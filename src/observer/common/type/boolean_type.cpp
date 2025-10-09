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
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/value.h"

int BooleanType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::BOOLEANS, "left type is not boolean");
  ASSERT(right.attr_type() == AttrType::BOOLEANS, "right type is not boolean");
  
  bool left_val = left.get_boolean();
  bool right_val = right.get_boolean();
  
  if (left_val == right_val) {
    return 0;
  }
  return left_val ? 1 : -1;
}

RC BooleanType::to_string(const Value &val, string &result) const
{
  ASSERT(val.attr_type() == AttrType::BOOLEANS, "value type is not boolean");
  
  stringstream ss;
  ss << (val.get_boolean() ? "TRUE" : "FALSE");
  result = ss.str();
  return RC::SUCCESS;
}

int BooleanType::cast_cost(AttrType type)
{
  if (type == AttrType::BOOLEANS) {
    return 0;
  }
  // 布尔类型不支持隐式类型转换
  return INT32_MAX;
}


