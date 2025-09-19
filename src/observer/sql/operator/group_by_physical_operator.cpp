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
// Created by WangYunlai on 2024/06/11.
//

#include "common/log/log.h"
#include "common/lang/ranges.h"
#include "sql/operator/group_by_physical_operator.h"
#include "sql/expr/expression_tuple.h"
#include "sql/expr/composite_tuple.h"
#include "sql/stmt/filter_stmt.h"

using namespace std;
using namespace common;

GroupByPhysicalOperator::GroupByPhysicalOperator(vector<Expression *> &&expressions, FilterStmt *having_filter_stmt)
 : aggregate_expressions_(std::move(expressions)), having_filter_stmt_(having_filter_stmt)
{
  value_expressions_.reserve(aggregate_expressions_.size());
  ranges::for_each(aggregate_expressions_, [this](Expression *expr) {
    auto       *aggregate_expr = static_cast<AggregateExpr *>(expr);
    Expression *child_expr     = aggregate_expr->child().get();
    ASSERT(child_expr != nullptr, "aggregate expression must have a child expression");
    value_expressions_.emplace_back(child_expr);
  });
}

void GroupByPhysicalOperator::create_aggregator_list(AggregatorList &aggregator_list)
{
  aggregator_list.clear();
  aggregator_list.reserve(aggregate_expressions_.size());
  std::ranges::for_each(aggregate_expressions_, [&aggregator_list](Expression *expr) {
    auto *aggregate_expr = static_cast<AggregateExpr *>(expr);
    aggregator_list.emplace_back(aggregate_expr->create_aggregator());
  });
}

RC GroupByPhysicalOperator::aggregate(AggregatorList &aggregator_list, const Tuple &tuple)
{
  ASSERT(static_cast<int>(aggregator_list.size()) == tuple.cell_num(), 
         "aggregator list size must be equal to tuple size. aggregator num: %d, tuple num: %d",
         aggregator_list.size(), tuple.cell_num());

  RC        rc = RC::SUCCESS;
  Value     value;
  const int size = static_cast<int>(aggregator_list.size());
  for (int i = 0; i < size; i++) {
    Aggregator *aggregator = aggregator_list[i].get();

    rc = tuple.cell_at(i, value);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to get value from expression. rc=%s", strrc(rc));
      return rc;
    }

    rc = aggregator->accumulate(value);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to accumulate value. rc=%s", strrc(rc));
      return rc;
    }
  }

  return rc;
}

RC GroupByPhysicalOperator::evaluate(GroupValueType &group_value)
{
  RC rc = RC::SUCCESS;

  vector<TupleCellSpec> aggregator_names;
  for (Expression *expr : aggregate_expressions_) {
    aggregator_names.emplace_back(expr->name());
  }

  AggregatorList &aggregators           = get<0>(group_value);
  CompositeTuple &composite_value_tuple = get<1>(group_value);

  ValueListTuple evaluated_tuple;
  vector<Value>  values;
  for (unique_ptr<Aggregator> &aggregator : aggregators) {
    Value value;
    rc = aggregator->evaluate(value);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to evaluate aggregator. rc=%s", strrc(rc));
      return rc;
    }
    values.emplace_back(value);
  }

  evaluated_tuple.set_cells(values);
  evaluated_tuple.set_names(aggregator_names);

  composite_value_tuple.add_tuple(make_unique<ValueListTuple>(std::move(evaluated_tuple)));

  return rc;
}

bool GroupByPhysicalOperator::check_having_condition(const GroupValueType &group_value)
{
  if (having_filter_stmt_ == nullptr) {
    return true;
  }

  const CompositeTuple &composite_tuple = std::get<1>(group_value);
  
  for (const auto &filter_unit : having_filter_stmt_->filter_units()) {
    Value left_value, right_value;
    
    if (!get_filter_value(filter_unit->left(), composite_tuple, left_value) ||
        !get_filter_value(filter_unit->right(), composite_tuple, right_value)) {
      return false;
    }
    
    if (!evaluate_comparison(left_value, right_value, filter_unit->comp())) {
      return false;
    }
  }

  return true;
}

bool GroupByPhysicalOperator::get_filter_value(const FilterObj &filter_obj, 
                                            const CompositeTuple &composite_tuple, 
                                            Value &result_value)
{
  switch (filter_obj.type_) {
    case FilterObj::Type::EXPRESSION: {
      int aggregate_pos = composite_tuple.cell_num() - 1;
      return composite_tuple.cell_at(aggregate_pos, result_value) == RC::SUCCESS;
    }
    case FilterObj::Type::VALUE: {
      result_value = filter_obj.value;
      return true;
    }
    default:
      return false;
  }
}

bool GroupByPhysicalOperator::evaluate_comparison(const Value &left_value, 
                                                  const Value &right_value, 
                                                  CompOp comp_op)
{
  int compare_result = left_value.compare(right_value);
  
  switch (comp_op) {
    case EQUAL_TO:    return compare_result == 0;
    case LESS_THAN:   return compare_result < 0;
    case GREAT_THAN:  return compare_result > 0;
    case LESS_EQUAL:  return compare_result <= 0;
    case GREAT_EQUAL: return compare_result >= 0;
    case NOT_EQUAL:   return compare_result != 0;
    default:          return false;
  }
}