/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/operator/join_logical_operator.h"

JoinLogicalOperator::JoinLogicalOperator(JoinType join_type, Expression *condition)
    : join_type_(join_type), condition_(condition)
{}

JoinLogicalOperator::~JoinLogicalOperator()
{
  if (condition_) {
    delete condition_;
    condition_ = nullptr;
  }
}
