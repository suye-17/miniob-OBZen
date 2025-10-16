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

#include "sql/optimizer/rewrite_rule.h"
#include <unordered_set>
#include <string>

class JoinLogicalOperator;
class TableGetLogicalOperator;
class Expression;

/**
 * @brief 将一些谓词表达式下推到join中
 * @ingroup Rewriter
 * @details 将WHERE子句中的条件根据涉及的表数量，下推到TableScan或Join算子
 */
class PredicateToJoinRewriter : public RewriteRule
{
public:
  PredicateToJoinRewriter()          = default;
  virtual ~PredicateToJoinRewriter() = default;

  RC rewrite(unique_ptr<LogicalOperator> &oper, bool &change_made) override;

private:
  /**
   * @brief 尝试将谓词下推到子节点
   * @param oper 当前算子
   * @param predicate 要下推的谓词
   * @param pushed 输出参数，表示是否成功下推
   */
  RC try_push_down_predicate(unique_ptr<LogicalOperator> &oper, Expression *predicate, bool &pushed);

  /**
   * @brief 将谓词添加到JOIN条件
   */
  RC push_to_join(JoinLogicalOperator *join_oper, Expression *predicate);

  /**
   * @brief 将谓词添加到TableScan
   */
  RC push_to_table_scan(TableGetLogicalOperator *scan_oper, Expression *predicate);

  /**
   * @brief 检查条件能否下推到指定算子
   */
  bool can_push_to_operator(LogicalOperator *target_oper, const std::unordered_set<std::string> &pred_tables);
};
