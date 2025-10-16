/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "sql/operator/logical_operator.h"
#include "sql/parser/parse_defs.h"

/**
 * @brief JOIN逻辑算子
 * @details 表示JOIN操作的逻辑执行计划，不涉及具体的JOIN算法实现
 */
class JoinLogicalOperator : public LogicalOperator
{
public:
  JoinLogicalOperator(JoinType join_type, Expression *condition);
  virtual ~JoinLogicalOperator();
  
  LogicalOperatorType type() const override { return LogicalOperatorType::JOIN; }
  
  JoinType     join_type() const { return join_type_; }
  Expression  *condition() const { return condition_; }

  /**
   * @brief 设置JOIN条件（替换现有条件）
   * @param condition 新的JOIN条件表达式（调用者转移所有权）
   */
  void set_condition(Expression *condition);

  /**
   * @brief 添加额外的JOIN条件（使用AND连接）
   * @param additional_cond 额外的条件表达式（调用者转移所有权）
   */
  void add_condition(Expression *additional_cond);

private:
  JoinType     join_type_;  ///< JOIN类型
  Expression  *condition_;  ///< JOIN条件
};