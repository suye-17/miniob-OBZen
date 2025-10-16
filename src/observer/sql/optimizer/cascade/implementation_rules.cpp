/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/log/log.h"
#include "sql/optimizer/cascade/implementation_rules.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/table_scan_physical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/insert_physical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/explain_physical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/calc_physical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/delete_physical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/predicate_physical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/nested_loop_join_physical_operator.h"
#include "sql/operator/hash_join_physical_operator.h"
#include "sql/optimizer/cascade/join_cost_calculator.h"

// -------------------------------------------------------------------------------------------------
// PhysicalSeqScan
// -------------------------------------------------------------------------------------------------
LogicalGetToPhysicalSeqScan::LogicalGetToPhysicalSeqScan()
{
  type_          = RuleType::GET_TO_SEQ_SCAN;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALGET));
}

void LogicalGetToPhysicalSeqScan::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  TableGetLogicalOperator *table_get_oper = dynamic_cast<TableGetLogicalOperator *>(input);

  vector<unique_ptr<Expression>> &log_preds = table_get_oper->predicates();
  vector<unique_ptr<Expression>>  phys_preds;
  for (auto &pred : log_preds) {
    phys_preds.push_back(pred->copy());
  }

  Table *table           = table_get_oper->table();
  auto   table_scan_oper = new TableScanPhysicalOperator(table, table_get_oper->read_write_mode());
  table_scan_oper->set_predicates(std::move(phys_preds));
  auto oper = unique_ptr<OperatorNode>(table_scan_oper);

  transformed->emplace_back(std::move(oper));
}

// -------------------------------------------------------------------------------------------------
//  LogicalProjectionToProjection
// -------------------------------------------------------------------------------------------------
LogicalProjectionToProjection::LogicalProjectionToProjection()
{
  type_          = RuleType::PROJECTION_TO_PHYSOCAL;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALPROJECTION));
  auto child     = new Pattern(OpType::LEAF);
  match_pattern_->add_child(child);
}

void LogicalProjectionToProjection::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  auto                                 project_oper = dynamic_cast<ProjectLogicalOperator *>(input);
  vector<unique_ptr<LogicalOperator>> &child_opers  = project_oper->children();
  ASSERT(child_opers.size() == 1, "only one child is supported for now");

  unique_ptr<PhysicalOperator> child_phy_oper;

  auto project_operator = make_unique<ProjectPhysicalOperator>(std::move(project_oper->expressions()));
  if (project_operator) {
    project_operator->add_general_child(child_opers.front().get());
  }

  transformed->emplace_back(std::move(project_operator));
}

// -------------------------------------------------------------------------------------------------
// PhysicalInsert
// -------------------------------------------------------------------------------------------------
LogicalInsertToInsert::LogicalInsertToInsert()
{
  type_          = RuleType::INSERT_TO_PHYSICAL;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALINSERT));
}

void LogicalInsertToInsert::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  InsertLogicalOperator *insert_oper = dynamic_cast<InsertLogicalOperator *>(input);

  Table         *table           = insert_oper->table();
  vector<Value> &values          = insert_oper->values();
  auto           insert_phy_oper = make_unique<InsertPhysicalOperator>(table, std::move(values));

  transformed->emplace_back(std::move(insert_phy_oper));
}

// -------------------------------------------------------------------------------------------------
// PhysicalExplain
// -------------------------------------------------------------------------------------------------
LogicalExplainToExplain::LogicalExplainToExplain()
{
  type_          = RuleType::EXPLAIN_TO_PHYSICAL;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALEXPLAIN));
  auto child     = new Pattern(OpType::LEAF);
  match_pattern_->add_child(child);
}

void LogicalExplainToExplain::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  auto                         explain_oper = dynamic_cast<ExplainLogicalOperator *>(input);
  unique_ptr<PhysicalOperator> explain_physical_oper(new ExplainPhysicalOperator());
  for (auto &child : explain_oper->children()) {
    explain_physical_oper->add_general_child(child.get());
  }

  transformed->emplace_back(std::move(explain_physical_oper));
}

// -------------------------------------------------------------------------------------------------
// PhysicalCalc
// -------------------------------------------------------------------------------------------------
LogicalCalcToCalc::LogicalCalcToCalc()
{
  type_          = RuleType::CALC_TO_PHYSICAL;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALCALCULATE));
}

void LogicalCalcToCalc::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  auto                             calc_oper = dynamic_cast<CalcLogicalOperator *>(input);
  unique_ptr<CalcPhysicalOperator> calc_phys_oper(new CalcPhysicalOperator(std::move(calc_oper->expressions())));

  transformed->emplace_back(std::move(calc_phys_oper));
}

// -------------------------------------------------------------------------------------------------
// PhysicalDelete
// -------------------------------------------------------------------------------------------------
LogicalDeleteToDelete::LogicalDeleteToDelete()
{
  type_          = RuleType::DELETE_TO_PHYSICAL;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALDELETE));
  auto child     = new Pattern(OpType::LEAF);
  match_pattern_->add_child(child);
}

void LogicalDeleteToDelete::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  auto delete_oper = dynamic_cast<DeleteLogicalOperator *>(input);

  auto delete_phys_oper = unique_ptr<PhysicalOperator>(new DeletePhysicalOperator(delete_oper->table()));
  for (auto &child : delete_oper->children()) {
    delete_phys_oper->add_general_child(child.get());
  }

  transformed->emplace_back(std::move(delete_phys_oper));
}

// -------------------------------------------------------------------------------------------------
// Physical Predicate
// -------------------------------------------------------------------------------------------------
LogicalPredicateToPredicate::LogicalPredicateToPredicate()
{
  type_          = RuleType::PREDICATE_TO_PHYSICAL;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALFILTER));
  auto child     = new Pattern(OpType::LEAF);
  match_pattern_->add_child(child);
}

void LogicalPredicateToPredicate::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  auto predicate_oper = dynamic_cast<PredicateLogicalOperator *>(input);

  vector<unique_ptr<Expression>> &expressions = predicate_oper->expressions();
  ASSERT(expressions.size() == 1, "predicate logical operator's children should be 1");

  unique_ptr<Expression>       expression = std::move(expressions.front());
  unique_ptr<PhysicalOperator> oper =
      unique_ptr<PhysicalOperator>(new PredicatePhysicalOperator(std::move(expression)));
  for (auto &child : predicate_oper->children()) {
    oper->add_general_child(child.get());
  }
  transformed->emplace_back(std::move(oper));
}

// -------------------------------------------------------------------------------------------------
// Physical Aggregation
// -------------------------------------------------------------------------------------------------
// LogicalGroupByToAggregation::LogicalGroupByToAggregation()
// {
//   type_ = RuleType::GROUP_BY_TO_PHYSICAL_AGGREGATION;
//   match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALGROUPBY));
//   auto child = new Pattern(OpType::LEAF);
//   match_pattern_->add_child(child);
// }

// void LogicalGroupByToAggregation::transform(OperatorNode* input,
//                          std::vector<std::unique_ptr<OperatorNode>> *transformed,
//                          OptimizerContext *context) const
// {
//   auto groupby_oper = dynamic_cast<GroupByLogicalOperator*>(input);
//   vector<unique_ptr<Expression>> &group_by_expressions = groupby_oper->group_by_expressions();
//   unique_ptr<GroupByPhysicalOperator> groupby_phys_oper;
//   if (group_by_expressions.empty()) {
//     groupby_phys_oper = make_unique<ScalarGroupByPhysicalOperator>(std::move(groupby_oper->aggregate_expressions()));
//   } else {
//     return;
//   }
//   for (auto &child : groupby_oper->children()) {
//     groupby_phys_oper->add_general_child(child.get());
//   }

//   transformed->emplace_back(std::move(groupby_phys_oper));
// }

// -------------------------------------------------------------------------------------------------
// Physical Hash Group By
// -------------------------------------------------------------------------------------------------
// LogicalGroupByToHashGroupBy::LogicalGroupByToHashGroupBy()
// {
//   type_ = RuleType::GROUP_BY_TO_PHYSICL_HASH_GROUP_BY;
//   match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALGROUPBY));
//   auto child = new Pattern(OpType::LEAF);
//   match_pattern_->add_child(child);
// }

// void LogicalGroupByToHashGroupBy::transform(OperatorNode* input,
//                          std::vector<std::unique_ptr<OperatorNode>> *transformed,
//                          OptimizerContext *context) const
// {
//   auto groupby_oper = dynamic_cast<GroupByLogicalOperator*>(input);
//   vector<unique_ptr<Expression>> &group_by_expressions = groupby_oper->group_by_expressions();
//   unique_ptr<GroupByPhysicalOperator> groupby_phys_oper;
//   if (group_by_expressions.empty()) {
//     return;
//   } else {
//     groupby_phys_oper = make_unique<HashGroupByPhysicalOperator>(std::move(groupby_oper->group_by_expressions()),
//         std::move(groupby_oper->aggregate_expressions()));
//   }
//   for (auto &child : groupby_oper->children()) {
//     groupby_phys_oper->add_general_child(child.get());
//   }

//   transformed->emplace_back(std::move(groupby_phys_oper));
// }
// -------------------------------------------------------------------------------------------------
// LogicalJoinToNestedLoopJoin
// -------------------------------------------------------------------------------------------------
LogicalJoinToNestedLoopJoin::LogicalJoinToNestedLoopJoin()
{
  type_          = RuleType::INNER_JOIN_TO_NL_JOIN;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALINNERJOIN));
  
  // Join算子有两个子节点
  auto left_child  = new Pattern(OpType::LEAF);
  auto right_child = new Pattern(OpType::LEAF);
  match_pattern_->add_child(left_child);
  match_pattern_->add_child(right_child);
}

void LogicalJoinToNestedLoopJoin::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  JoinLogicalOperator *join_oper = dynamic_cast<JoinLogicalOperator *>(input);
  ASSERT(join_oper != nullptr, "input operator is not a JoinLogicalOperator");
  
  // 获取JOIN条件并复制
  Expression *condition = join_oper->condition();
  Expression *phys_condition = nullptr;
  if (condition) {
    auto condition_copy = condition->copy();
    phys_condition = condition_copy.release();
  }
  
  // 创建NestedLoopJoin物理算子
  auto nlj_oper = make_unique<NestedLoopJoinPhysicalOperator>(phys_condition);
  
  // 添加左右子节点
  vector<unique_ptr<LogicalOperator>> &children = join_oper->children();
  ASSERT(children.size() == 2, "Join operator must have exactly 2 children");
  
  nlj_oper->add_general_child(children[0].get());
  nlj_oper->add_general_child(children[1].get());
  
  LOG_INFO("Created NestedLoopJoin physical operator");
  
  transformed->emplace_back(std::move(nlj_oper));
}

// -------------------------------------------------------------------------------------------------
// LogicalJoinToHashJoin
// -------------------------------------------------------------------------------------------------
LogicalJoinToHashJoin::LogicalJoinToHashJoin()
{
  type_          = RuleType::INNER_JOIN_TO_HASH_JOIN;
  match_pattern_ = unique_ptr<Pattern>(new Pattern(OpType::LOGICALINNERJOIN));
  
  // Join算子有两个子节点
  auto left_child  = new Pattern(OpType::LEAF);
  auto right_child = new Pattern(OpType::LEAF);
  match_pattern_->add_child(left_child);
  match_pattern_->add_child(right_child);
}

void LogicalJoinToHashJoin::transform(
    OperatorNode *input, std::vector<std::unique_ptr<OperatorNode>> *transformed, OptimizerContext *context) const
{
  JoinLogicalOperator *join_oper = dynamic_cast<JoinLogicalOperator *>(input);
  ASSERT(join_oper != nullptr, "input operator is not a JoinLogicalOperator");
  
  // 获取JOIN条件
  Expression *condition = join_oper->condition();
  
  // 检查是否为等值JOIN（只有等值JOIN才能使用HashJoin）
  if (!JoinCostCalculator::is_equi_join(condition)) {
    LOG_INFO("Join condition is not equi-join, cannot use HashJoin");
    // 不生成HashJoin算子
    return;
  }
  
  // 复制条件表达式
  Expression *phys_condition = nullptr;
  if (condition) {
    auto condition_copy = condition->copy();
    phys_condition = condition_copy.release();
  }
  
  // 创建HashJoin物理算子
  auto hash_join_oper = make_unique<HashJoinPhysicalOperator>(phys_condition);
  
  // 添加左右子节点
  vector<unique_ptr<LogicalOperator>> &children = join_oper->children();
  ASSERT(children.size() == 2, "Join operator must have exactly 2 children");
  
  hash_join_oper->add_general_child(children[0].get());
  hash_join_oper->add_general_child(children[1].get());
  
  LOG_INFO("Created HashJoin physical operator");
  
  transformed->emplace_back(std::move(hash_join_oper));
}
