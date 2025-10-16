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
#include "sql/expr/expression.h"

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

void JoinLogicalOperator::set_condition(Expression *condition)
{
  if (condition_) {
    delete condition_;
  }
  condition_ = condition;
}

void JoinLogicalOperator::add_condition(Expression *additional_cond)
{
  if (condition_ == nullptr) {
    condition_ = additional_cond;
  } else {
    // 使用AND连接现有条件和新条件
    vector<unique_ptr<Expression>> children;
    children.push_back(unique_ptr<Expression>(condition_));
    children.push_back(unique_ptr<Expression>(additional_cond));
    condition_ = new ConjunctionExpr(ConjunctionExpr::Type::AND, children);
  }
}
