/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/optimizer/predicate_to_join_rule.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/expr/expression.h"
#include "common/log/log.h"

RC PredicateToJoinRewriter::rewrite(unique_ptr<LogicalOperator> &oper, bool &change_made)
{
  RC rc = RC::SUCCESS;
  
  // 先递归处理子节点
  for (auto &child : oper->children()) {
    rc = rewrite(child, change_made);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  
  // 只处理Predicate算子
  if (oper->type() != LogicalOperatorType::PREDICATE) {
    return RC::SUCCESS;
  }
  
  // 获取谓词表达式
  auto &predicates = oper->expressions();
  if (predicates.empty()) {
    return RC::SUCCESS;
  }
  
  // 分析每个谓词并尝试下推
  vector<unique_ptr<Expression>> remaining_predicates;
  
  for (auto &pred : predicates) {
    if (!pred) {
      continue;
    }
    
    bool pushed = false;
    
    // 尝试下推到子节点
    if (!oper->children().empty()) {
      rc = try_push_down_predicate(oper, pred.get(), pushed);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to push down predicate. rc=%s", strrc(rc));
        return rc;
      }
    }
    
    if (!pushed) {
      // 无法下推，保留在当前位置
      remaining_predicates.push_back(std::move(pred));
    } else {
      LOG_INFO("Successfully pushed down predicate");
      change_made = true;
    }
  }
  
  // 如果所有谓词都下推了，用子节点替换当前Predicate算子
  if (remaining_predicates.empty() && !oper->children().empty()) {
    LOG_INFO("All predicates pushed down, removing Predicate operator");
    oper = std::move(oper->children()[0]);
    change_made = true;
  } else if (!remaining_predicates.empty()) {
    // 更新Predicate算子的表达式列表
    predicates = std::move(remaining_predicates);
  }
  
  return RC::SUCCESS;
}

RC PredicateToJoinRewriter::try_push_down_predicate(
    unique_ptr<LogicalOperator> &oper,
    Expression *predicate,
    bool &pushed)
{
  pushed = false;
  
  if (!predicate) {
    return RC::SUCCESS;
  }
  
  // 获取谓词涉及的表
  auto pred_tables = predicate->get_involved_tables();
  
  if (pred_tables.empty()) {
    // 常量条件，不下推
    LOG_INFO("Predicate involves no tables (constant), not pushing down");
    return RC::SUCCESS;
  }
  
  // 检查子节点
  if (oper->children().empty()) {
    return RC::SUCCESS;
  }
  
  auto &child = oper->children()[0];
  
  // 情况1：下推到TableScan
  if (child->type() == LogicalOperatorType::TABLE_GET) {
    auto table_scan = dynamic_cast<TableGetLogicalOperator*>(child.get());
    if (can_push_to_operator(table_scan, pred_tables)) {
      LOG_INFO("Pushing predicate to TableScan");
      RC rc = push_to_table_scan(table_scan, predicate);
      if (rc == RC::SUCCESS) {
        pushed = true;
      }
      return rc;
    }
  }
  
  // 情况2：下推到Join
  if (child->type() == LogicalOperatorType::JOIN) {
    auto join_oper = dynamic_cast<JoinLogicalOperator*>(child.get());
    if (can_push_to_operator(join_oper, pred_tables)) {
      LOG_INFO("Pushing predicate to Join operator (tables=%zu)", pred_tables.size());
      RC rc = push_to_join(join_oper, predicate);
      if (rc == RC::SUCCESS) {
        pushed = true;
      }
      return rc;
    }
  }
  
  // 情况3：尝试递归下推到更深的层次
  // 如果子节点是Predicate，可以继续下推
  if (child->type() == LogicalOperatorType::PREDICATE) {
    RC rc = try_push_down_predicate(child, predicate, pushed);
    return rc;
  }
  
  return RC::SUCCESS;
}

bool PredicateToJoinRewriter::can_push_to_operator(
    LogicalOperator *target_oper,
    const std::unordered_set<std::string> &pred_tables)
{
  if (!target_oper) {
    return false;
  }
  
  // 获取目标算子涉及的表
  auto oper_tables = target_oper->get_involved_tables();
  
  if (oper_tables.empty()) {
    return false;
  }
  
  // 检查谓词涉及的所有表是否都在目标算子的表集合中
  for (const auto &pred_table : pred_tables) {
    if (oper_tables.find(pred_table) == oper_tables.end()) {
      return false;
    }
  }
  
  return true;
}

RC PredicateToJoinRewriter::push_to_join(JoinLogicalOperator *join_oper, Expression *predicate)
{
  if (!join_oper || !predicate) {
    return RC::INVALID_ARGUMENT;
  }
  
  // 复制谓词表达式并添加到JOIN条件
  auto pred_copy = predicate->copy();
  if (!pred_copy) {
    LOG_WARN("Failed to copy predicate expression");
    return RC::INTERNAL;
  }
  
  join_oper->add_condition(pred_copy.release());
  
  LOG_INFO("Added predicate to Join condition");
  return RC::SUCCESS;
}

RC PredicateToJoinRewriter::push_to_table_scan(
    TableGetLogicalOperator *scan_oper,
    Expression *predicate)
{
  if (!scan_oper || !predicate) {
    return RC::INVALID_ARGUMENT;
  }
  
  // 复制谓词表达式
  auto pred_copy = predicate->copy();
  if (!pred_copy) {
    LOG_WARN("Failed to copy predicate expression");
    return RC::INTERNAL;
  }
  
  // 将谓词添加到TableScan的predicates
  vector<unique_ptr<Expression>> preds;
  preds.push_back(std::move(pred_copy));
  
  // 注意：set_predicates会替换现有的predicates
  // 我们需要先获取现有的，然后追加新的
  auto &existing_preds = scan_oper->predicates();
  for (auto &existing_pred : existing_preds) {
    preds.push_back(std::move(existing_pred));
  }
  
  scan_oper->set_predicates(std::move(preds));
  
  LOG_INFO("Added predicate to TableScan");
  return RC::SUCCESS;
}
