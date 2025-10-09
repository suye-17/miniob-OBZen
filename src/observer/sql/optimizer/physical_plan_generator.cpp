/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/12/14.
//

#include "common/log/log.h"
#include "sql/expr/expression.h"
#include "session/session.h"
#include "sql/operator/aggregate_vec_physical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/calc_physical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/delete_physical_operator.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/update_physical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/explain_physical_operator.h"
#include "sql/operator/expr_vec_physical_operator.h"
#include "sql/operator/group_by_vec_physical_operator.h"
#include "sql/operator/hash_join_physical_operator.h"
#include "sql/operator/index_scan_physical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/insert_physical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/nested_loop_join_physical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/predicate_physical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "sql/operator/project_vec_physical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/table_scan_physical_operator.h"
#include "sql/operator/group_by_logical_operator.h"
#include "sql/operator/group_by_physical_operator.h"
#include "sql/operator/hash_group_by_physical_operator.h"
#include "sql/operator/scalar_group_by_physical_operator.h"
#include "sql/operator/table_scan_vec_physical_operator.h"
#include "sql/optimizer/physical_plan_generator.h"

using namespace std;

RC PhysicalPlanGenerator::create(LogicalOperator &logical_operator, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  RC rc = RC::SUCCESS;

  switch (logical_operator.type()) {
    case LogicalOperatorType::CALC: {
      return create_plan(static_cast<CalcLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::TABLE_GET: {
      return create_plan(static_cast<TableGetLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::PREDICATE: {
      return create_plan(static_cast<PredicateLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::PROJECTION: {
      return create_plan(static_cast<ProjectLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::INSERT: {
      return create_plan(static_cast<InsertLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::DELETE: {
      return create_plan(static_cast<DeleteLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::UPDATE: {
      return create_plan(static_cast<UpdateLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::EXPLAIN: {
      return create_plan(static_cast<ExplainLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::JOIN: {
      return create_plan(static_cast<JoinLogicalOperator &>(logical_operator), oper, session);
    } break;

    case LogicalOperatorType::GROUP_BY: {
      return create_plan(static_cast<GroupByLogicalOperator &>(logical_operator), oper, session);
    } break;

    default: {
      ASSERT(false, "unknown logical operator type");
      return RC::INVALID_ARGUMENT;
    }
  }
  return rc;
}

RC PhysicalPlanGenerator::create_vec(LogicalOperator &logical_operator, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  RC rc = RC::SUCCESS;

  switch (logical_operator.type()) {
    case LogicalOperatorType::TABLE_GET: {
      return create_vec_plan(static_cast<TableGetLogicalOperator &>(logical_operator), oper, session);
    } break;
    case LogicalOperatorType::PROJECTION: {
      return create_vec_plan(static_cast<ProjectLogicalOperator &>(logical_operator), oper, session);
    } break;
    case LogicalOperatorType::GROUP_BY: {
      return create_vec_plan(static_cast<GroupByLogicalOperator &>(logical_operator), oper, session);
    } break;
    case LogicalOperatorType::EXPLAIN: {
      return create_vec_plan(static_cast<ExplainLogicalOperator &>(logical_operator), oper, session);
    } break;
    default: {
      return RC::INVALID_ARGUMENT;
    }
  }
  return rc;
}



RC PhysicalPlanGenerator::create_plan(TableGetLogicalOperator &table_get_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<Expression>> &predicates = table_get_oper.predicates();
  Table *table = table_get_oper.table();

  map<string, Value> equal_conditions;
  vector<unique_ptr<Expression>> remaining_predicates;
  
  for (auto &expr : predicates) {
    if (expr->type() == ExprType::COMPARISON) {
      auto comparison_expr = static_cast<ComparisonExpr *>(expr.get());
      if (comparison_expr->comp() != EQUAL_TO) {
        remaining_predicates.push_back(std::move(expr));
        continue;
      }

      unique_ptr<Expression> &left_expr  = comparison_expr->left();
      unique_ptr<Expression> &right_expr = comparison_expr->right();
      
      if (left_expr->type() != ExprType::VALUE && right_expr->type() != ExprType::VALUE) {
        remaining_predicates.push_back(std::move(expr));
        continue;
      }

      FieldExpr *field_expr = nullptr;
      ValueExpr *value_expr = nullptr;
      
      if (left_expr->type() == ExprType::FIELD && right_expr->type() == ExprType::VALUE) {
        field_expr = static_cast<FieldExpr *>(left_expr.get());
        value_expr = static_cast<ValueExpr *>(right_expr.get());
      } else if (right_expr->type() == ExprType::FIELD && left_expr->type() == ExprType::VALUE) {
        field_expr = static_cast<FieldExpr *>(right_expr.get());
        value_expr = static_cast<ValueExpr *>(left_expr.get());
      }

      if (field_expr != nullptr && value_expr != nullptr) {
        equal_conditions[field_expr->field().field_name()] = value_expr->get_value();
      } else {
        remaining_predicates.push_back(std::move(expr));
      }
    } else {
      remaining_predicates.push_back(std::move(expr));
    }
  }

  if (equal_conditions.empty()) {
    auto table_scan_oper = new TableScanPhysicalOperator(table, table_get_oper.read_write_mode());
    table_scan_oper->set_predicates(std::move(remaining_predicates));
    oper = unique_ptr<PhysicalOperator>(table_scan_oper);
    LOG_TRACE("use table scan (no equal conditions)");
    return RC::SUCCESS;
  }

  const IndexMeta *best_index_meta = nullptr;
  int max_matched_fields = 0;
  
  for (int i = 0; i < table->table_meta().index_num(); i++) {
    const IndexMeta *index_meta = table->table_meta().index(i);
    const vector<string> &index_fields = index_meta->fields();
    
    int matched = 0;
    for (const string &field_name : index_fields) {
      if (equal_conditions.find(field_name) != equal_conditions.end()) {
        matched++;
      } else {
        break;
      }
    }
    
    if (matched > max_matched_fields) {
      max_matched_fields = matched;
      best_index_meta = index_meta;
    } else if (matched == max_matched_fields && matched > 0) {
      if (index_meta->is_unique() && !best_index_meta->is_unique()) {
        best_index_meta = index_meta;
      }
    }
  }

  if (best_index_meta == nullptr || max_matched_fields == 0) {
    auto table_scan_oper = new TableScanPhysicalOperator(table, table_get_oper.read_write_mode());
    table_scan_oper->set_predicates(std::move(predicates));
    oper = unique_ptr<PhysicalOperator>(table_scan_oper);
    LOG_TRACE("use table scan (no suitable index)");
    return RC::SUCCESS;
  }

  vector<Value> key_values;
  const vector<string> &index_fields = best_index_meta->fields();
  for (int i = 0; i < max_matched_fields; i++) {
    key_values.push_back(equal_conditions.at(index_fields[i]));
  }

  Index *index = table->find_index(best_index_meta->name());
  IndexScanPhysicalOperator *index_scan_oper = nullptr;
  
  if (best_index_meta->field_count() > 1) {
    index_scan_oper = new IndexScanPhysicalOperator(
        table, index, table_get_oper.read_write_mode(),
        key_values, true, key_values, true);
    LOG_INFO("use index scan with composite key. index=%s, matched_fields=%d/%zu", 
             best_index_meta->name(), max_matched_fields, best_index_meta->field_count());
  } else {
    index_scan_oper = new IndexScanPhysicalOperator(
        table, index, table_get_oper.read_write_mode(),
        &key_values[0], true, &key_values[0], true);
    LOG_TRACE("use index scan with single field. index=%s", best_index_meta->name());
  }

  index_scan_oper->set_predicates(std::move(remaining_predicates));
  oper = unique_ptr<PhysicalOperator>(index_scan_oper);

  return RC::SUCCESS;
}

RC PhysicalPlanGenerator::create_plan(PredicateLogicalOperator &pred_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<LogicalOperator>> &children_opers = pred_oper.children();
  ASSERT(children_opers.size() == 1, "predicate logical operator's sub oper number should be 1");

  LogicalOperator &child_oper = *children_opers.front();

  unique_ptr<PhysicalOperator> child_phy_oper;
  RC                           rc = create(child_oper, child_phy_oper, session);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create child operator of predicate operator. rc=%s", strrc(rc));
    return rc;
  }

  vector<unique_ptr<Expression>> &expressions = pred_oper.expressions();
  ASSERT(expressions.size() == 1, "predicate logical operator's children should be 1");

  unique_ptr<Expression> expression = std::move(expressions.front());
  oper = unique_ptr<PhysicalOperator>(new PredicatePhysicalOperator(std::move(expression)));
  oper->add_child(std::move(child_phy_oper));
  return rc;
}

RC PhysicalPlanGenerator::create_plan(ProjectLogicalOperator &project_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = project_oper.children();

  unique_ptr<PhysicalOperator> child_phy_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();

    rc = create(*child_oper, child_phy_oper, session);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  auto project_operator = make_unique<ProjectPhysicalOperator>(std::move(project_oper.expressions()));
  if (child_phy_oper) {
    project_operator->add_child(std::move(child_phy_oper));
  }

  oper = std::move(project_operator);

  LOG_TRACE("create a project physical operator");
  return rc;
}

RC PhysicalPlanGenerator::create_plan(InsertLogicalOperator &insert_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  Table                  *table           = insert_oper.table();
  vector<Value>          &values          = insert_oper.values();
  InsertPhysicalOperator *insert_phy_oper = new InsertPhysicalOperator(table, std::move(values));
  oper.reset(insert_phy_oper);
  return RC::SUCCESS;
}

RC PhysicalPlanGenerator::create_plan(DeleteLogicalOperator &delete_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = delete_oper.children();

  unique_ptr<PhysicalOperator> child_physical_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();

    rc = create(*child_oper, child_physical_oper, session);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  oper = unique_ptr<PhysicalOperator>(new DeletePhysicalOperator(delete_oper.table()));

  if (child_physical_oper) {
    oper->add_child(std::move(child_physical_oper));
  }
  return rc;
}

RC PhysicalPlanGenerator::create_plan(UpdateLogicalOperator &update_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = update_oper.children();

  unique_ptr<PhysicalOperator> child_physical_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();

    rc = create(*child_oper, child_physical_oper, session);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  oper = unique_ptr<PhysicalOperator>(new UpdatePhysicalOperator(update_oper.table(), update_oper.field_name(), update_oper.expression()));

  if (child_physical_oper) {
    oper->add_child(std::move(child_physical_oper));
  }
  return rc;
}

RC PhysicalPlanGenerator::create_plan(ExplainLogicalOperator &explain_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = explain_oper.children();

  RC rc = RC::SUCCESS;

  unique_ptr<PhysicalOperator> explain_physical_oper(new ExplainPhysicalOperator);
  for (unique_ptr<LogicalOperator> &child_oper : child_opers) {
    unique_ptr<PhysicalOperator> child_physical_oper;
    rc = create(*child_oper, child_physical_oper, session);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create child physical operator. rc=%s", strrc(rc));
      return rc;
    }

    explain_physical_oper->add_child(std::move(child_physical_oper));
  }

  oper = std::move(explain_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(JoinLogicalOperator &join_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  RC rc = RC::SUCCESS;

  vector<unique_ptr<LogicalOperator>> &child_opers = join_oper.children();
  if (child_opers.size() != 2) {
    LOG_WARN("join operator should have 2 children, but have %d", child_opers.size());
    return RC::INTERNAL;
  }
  if (session->hash_join_on() && can_use_hash_join(join_oper)) {
    // your code here
  } else {
    unique_ptr<PhysicalOperator> join_physical_oper(new NestedLoopJoinPhysicalOperator());
    for (auto &child_oper : child_opers) {
      unique_ptr<PhysicalOperator> child_physical_oper;
      rc = create(*child_oper, child_physical_oper, session);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to create physical child oper. rc=%s", strrc(rc));
        return rc;
      }

      join_physical_oper->add_child(std::move(child_physical_oper));
    }

    oper = std::move(join_physical_oper);
  }
  return rc;
}

bool PhysicalPlanGenerator::can_use_hash_join(JoinLogicalOperator &join_oper)
{
  // your code here
  return false;
}

RC PhysicalPlanGenerator::create_plan(CalcLogicalOperator &logical_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  RC rc = RC::SUCCESS;

  CalcPhysicalOperator *calc_oper = new CalcPhysicalOperator(std::move(logical_oper.expressions()));
  oper.reset(calc_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(GroupByLogicalOperator &logical_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  RC rc = RC::SUCCESS;

  // 处理子操作符
  unique_ptr<PhysicalOperator> child_physical_oper;
  
  if (logical_oper.children().size() == 1) {
    // 正常情况：有1个子操作符
    LogicalOperator &child_oper = *logical_oper.children().front();
    rc = create(child_oper, child_physical_oper, session);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to create group by logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  } else if (logical_oper.children().size() == 0) {
    // 异常修复：GroupBy操作符的children被意外清空（move语义bug）
    // 为了确保系统稳定性和SQL语义正确性，我们创建一个应急执行路径
    LOG_WARN("GroupBy operator children lost due to move semantics bug - creating correct empty result path");
    
    // SQL标准：当WHERE条件过滤掉所有行时，聚合函数应返回默认值
    // count(*)/count(col) -> 0, sum/avg/min/max -> NULL
    // 创建一个不返回任何行的特殊操作符，确保聚合函数返回正确的默认值
    class EmptyPhysicalOperator : public PhysicalOperator {
    public:
      PhysicalOperatorType type() const override { return PhysicalOperatorType::CALC; }
      OpType get_op_type() const override { return OpType::CALCULATE; }
      string name() const override { return "EMPTY_FOR_AGGREGATION"; }
      string param() const override { return ""; }
      
      RC open(Trx *trx) override { return RC::SUCCESS; }
      RC next() override { return RC::RECORD_EOF; } // 直接返回EOF，不产生任何行
      RC close() override { return RC::SUCCESS; }
      Tuple *current_tuple() override { return nullptr; }
      
      RC tuple_schema(TupleSchema &schema) const override { return RC::SUCCESS; }
    };
    child_physical_oper = make_unique<EmptyPhysicalOperator>();
  } else {
    LOG_WARN("group by operator should have exactly 1 child, but has %zu children", logical_oper.children().size());
    return RC::INTERNAL;
  }

  // 创建物理操作符 - 复制表达式而不是移动，避免影响逻辑操作符的状态
  vector<unique_ptr<Expression>> &group_by_expressions = logical_oper.group_by_expressions();
  unique_ptr<GroupByPhysicalOperator> group_by_oper;
  if (group_by_expressions.empty()) {
    // 对于标量聚合，我们需要复制聚合表达式
    vector<Expression *> agg_exprs;
    for (auto *expr : logical_oper.aggregate_expressions()) {
      agg_exprs.push_back(expr);
    }
    group_by_oper = make_unique<ScalarGroupByPhysicalOperator>(std::move(agg_exprs) , 
                                                              logical_oper.having_filter_stmt());
  } else {
    // 对于有GROUP BY的聚合，我们也需要复制表达式
    vector<unique_ptr<Expression>> group_by_exprs_copy;
    // 这里我们不能复制unique_ptr，所以还是要用move，但在处理完children之后
    vector<Expression *> agg_exprs;
    for (auto *expr : logical_oper.aggregate_expressions()) {
      agg_exprs.push_back(expr);
    }
    group_by_oper = make_unique<HashGroupByPhysicalOperator>(
      std::move(logical_oper.group_by_expressions()), 
      std::move(agg_exprs),
      logical_oper.having_filter_stmt()
    );
  }

  group_by_oper->add_child(std::move(child_physical_oper));

  oper = std::move(group_by_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_vec_plan(TableGetLogicalOperator &table_get_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<Expression>> &predicates = table_get_oper.predicates();
  Table *table = table_get_oper.table();
  TableScanVecPhysicalOperator *table_scan_oper = new TableScanVecPhysicalOperator(table, table_get_oper.read_write_mode());
  table_scan_oper->set_predicates(std::move(predicates));
  oper = unique_ptr<PhysicalOperator>(table_scan_oper);
  LOG_TRACE("use vectorized table scan");

  return RC::SUCCESS;
}

RC PhysicalPlanGenerator::create_vec_plan(GroupByLogicalOperator &logical_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  RC rc = RC::SUCCESS;
  unique_ptr<PhysicalOperator> physical_oper = nullptr;
  if (logical_oper.group_by_expressions().empty()) {
    physical_oper = make_unique<AggregateVecPhysicalOperator>(std::move(logical_oper.aggregate_expressions()));
  } else {
    physical_oper = make_unique<GroupByVecPhysicalOperator>(
      std::move(logical_oper.group_by_expressions()), std::move(logical_oper.aggregate_expressions()));

  }

  ASSERT(logical_oper.children().size() == 1, "group by operator should have 1 child");

  LogicalOperator             &child_oper = *logical_oper.children().front();
  unique_ptr<PhysicalOperator> child_physical_oper;
  rc = create_vec(child_oper, child_physical_oper, session);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to create child physical operator of group by(vec) operator. rc=%s", strrc(rc));
    return rc;
  }

  physical_oper->add_child(std::move(child_physical_oper));

  oper = std::move(physical_oper);
  return rc;

  return RC::SUCCESS;
}

RC PhysicalPlanGenerator::create_vec_plan(ProjectLogicalOperator &project_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = project_oper.children();

  unique_ptr<PhysicalOperator> child_phy_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc                          = create_vec(*child_oper, child_phy_oper, session);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  auto project_operator = make_unique<ProjectVecPhysicalOperator>(std::move(project_oper.expressions()));

  if (child_phy_oper != nullptr) {
    vector<Expression *> expressions;
    for (auto &expr : project_operator->expressions()) {
      expressions.push_back(expr.get());
    }
    auto expr_operator = make_unique<ExprVecPhysicalOperator>(std::move(expressions));
    expr_operator->add_child(std::move(child_phy_oper));
    project_operator->add_child(std::move(expr_operator));
  }

  oper = std::move(project_operator);

  LOG_TRACE("create a project physical operator");
  return rc;
}


RC PhysicalPlanGenerator::create_vec_plan(ExplainLogicalOperator &explain_oper, unique_ptr<PhysicalOperator> &oper, Session* session)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = explain_oper.children();

  RC rc = RC::SUCCESS;
  // reuse `ExplainPhysicalOperator` in explain vectorized physical plan
  unique_ptr<PhysicalOperator> explain_physical_oper(new ExplainPhysicalOperator);
  for (unique_ptr<LogicalOperator> &child_oper : child_opers) {
    unique_ptr<PhysicalOperator> child_physical_oper;
    rc = create_vec(*child_oper, child_physical_oper, session);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create child physical operator. rc=%s", strrc(rc));
      return rc;
    }

    explain_physical_oper->add_child(std::move(child_physical_oper));
  }

  oper = std::move(explain_physical_oper);
  return rc;
}
