/* Copyright (c) 2023 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2023/08/16.
//

#include "sql/optimizer/logical_plan_generator.h"

#include "common/log/log.h"

#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/group_by_logical_operator.h"

#include "sql/stmt/calc_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/explain_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/stmt.h"
#include "sql/parser/expression_binder.h"

#include "sql/expr/expression_iterator.h"

using namespace std;
using namespace common;

// 函数前向声明
RC bind_expression_fields(unique_ptr<Expression> &expr, const vector<Table *> &tables);
RC bind_unbound_field(unique_ptr<Expression> &expr, const vector<Table *> &tables);
RC bind_arithmetic_expression(unique_ptr<Expression> &expr, const vector<Table *> &tables);
RC bind_comparison_expression(unique_ptr<Expression> &expr, const vector<Table *> &tables);

// 辅助函数：绑定单个未绑定字段
RC bind_unbound_field(unique_ptr<Expression> &expr, const vector<Table *> &tables) {
  auto unbound_field = static_cast<UnboundFieldExpr*>(expr.get());
  const char* field_name = unbound_field->field_name();
  const char* table_name = unbound_field->table_name();
  
  // 查找目标表
  Table* target_table = nullptr;
  if (table_name && strlen(table_name) > 0) {
    // 指定了表名，直接查找
    auto it = find_if(tables.begin(), tables.end(), 
                      [table_name](Table* table) { return strcmp(table->name(), table_name) == 0; });
    target_table = (it != tables.end()) ? *it : nullptr;
  } else {
    // 没有指定表名，在所有表中查找字段
    auto it = find_if(tables.begin(), tables.end(), 
                      [field_name](Table* table) { 
                        return table->table_meta().field(field_name) != nullptr; 
                      });
    target_table = (it != tables.end()) ? *it : nullptr;
  }
  
  if (!target_table) {
    LOG_WARN("field not found: %s", field_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }
  
  const FieldMeta* field_meta = target_table->table_meta().field(field_name);
  if (!field_meta) {
    LOG_WARN("field not found in table: %s.%s", target_table->name(), field_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }
  
  // 替换为FieldExpr
  Field field(target_table, field_meta);
  expr = make_unique<FieldExpr>(field);
  return RC::SUCCESS;
}

// 辅助函数：绑定算术表达式
RC bind_arithmetic_expression(unique_ptr<Expression> &expr, const vector<Table *> &tables) {
  auto arithmetic_expr = static_cast<ArithmeticExpr*>(expr.get());
  
  // 获取并绑定左右子表达式
  auto left = arithmetic_expr->left()->copy();
  auto right = arithmetic_expr->right() ? arithmetic_expr->right()->copy() : nullptr;
  
  RC rc = bind_expression_fields(left, tables);
  if (rc != RC::SUCCESS) return rc;
  
  if (right) {
    rc = bind_expression_fields(right, tables);
    if (rc != RC::SUCCESS) return rc;
  }
  
  // 重新构造算术表达式
  ArithmeticExpr::Type op_type = arithmetic_expr->arithmetic_type();
  expr = make_unique<ArithmeticExpr>(op_type, left.release(), right.release());
  return RC::SUCCESS;
}

// 辅助函数：绑定比较表达式
RC bind_comparison_expression(unique_ptr<Expression> &expr, const vector<Table *> &tables) {
  auto comparison_expr = static_cast<ComparisonExpr*>(expr.get());
  
  // 获取并绑定左右子表达式
  auto left = comparison_expr->left()->copy();
  auto right = comparison_expr->right()->copy();
  
  RC rc = bind_expression_fields(left, tables);
  if (rc != RC::SUCCESS) return rc;
  
  rc = bind_expression_fields(right, tables);
  if (rc != RC::SUCCESS) return rc;
  
  // 重新构造比较表达式
  CompOp comp_op = comparison_expr->comp();
  expr = make_unique<ComparisonExpr>(comp_op, std::move(left), std::move(right));
  return RC::SUCCESS;
}

// 辅助函数：递归绑定表达式中的字段
RC bind_expression_fields(unique_ptr<Expression> &expr, const vector<Table *> &tables) {
  if (!expr) {
    return RC::SUCCESS;
  }
  
  switch (expr->type()) {
    case ExprType::UNBOUND_FIELD: {
      return bind_unbound_field(expr, tables);
    }
    
    case ExprType::ARITHMETIC: {
      return bind_arithmetic_expression(expr, tables);
    }
    
    case ExprType::COMPARISON: {
      return bind_comparison_expression(expr, tables);
    }
    
    case ExprType::FIELD:
    case ExprType::VALUE:
    case ExprType::STAR:
      // 这些表达式已经绑定或不需要绑定
      return RC::SUCCESS;
      
    default:
      // 其他类型的表达式暂时不处理
      return RC::SUCCESS;
  }
}

RC LogicalPlanGenerator::create(Stmt *stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  switch (stmt->type()) {
    case StmtType::CALC: {
      CalcStmt *calc_stmt = static_cast<CalcStmt *>(stmt);

      rc = create_plan(calc_stmt, logical_operator);
    } break;

    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);

      rc = create_plan(select_stmt, logical_operator);
    } break;

    case StmtType::INSERT: {
      InsertStmt *insert_stmt = static_cast<InsertStmt *>(stmt);

      rc = create_plan(insert_stmt, logical_operator);
    } break;

    case StmtType::DELETE: {
      DeleteStmt *delete_stmt = static_cast<DeleteStmt *>(stmt);

      rc = create_plan(delete_stmt, logical_operator);
    } break;

    case StmtType::UPDATE: {
      UpdateStmt *update_stmt = static_cast<UpdateStmt *>(stmt);
      
      rc = create_plan(update_stmt, logical_operator);
    } break;

    case StmtType::EXPLAIN: {
      ExplainStmt *explain_stmt = static_cast<ExplainStmt *>(stmt);

      rc = create_plan(explain_stmt, logical_operator);
    } break;
    default: {
      rc = RC::UNIMPLEMENTED;
    }
  }
  return rc;
}

RC LogicalPlanGenerator::create_plan(CalcStmt *calc_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  logical_operator.reset(new CalcLogicalOperator(std::move(calc_stmt->expressions())));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  unique_ptr<LogicalOperator> *last_oper = nullptr;

  unique_ptr<LogicalOperator> table_oper(nullptr);
  last_oper = &table_oper;
  unique_ptr<LogicalOperator> predicate_oper;

  const vector<Table *> &tables = select_stmt->tables();
  
  RC rc = create_plan(select_stmt->filter_stmt(), tables, predicate_oper);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
    return rc;
  }
  for (Table *table : tables) {

    unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, ReadWriteMode::READ_ONLY));
    if (table_oper == nullptr) {
      table_oper = std::move(table_get_oper);
    } else {
      JoinLogicalOperator *join_oper = new JoinLogicalOperator;
      join_oper->add_child(std::move(table_oper));
      join_oper->add_child(std::move(table_get_oper));
      table_oper = unique_ptr<LogicalOperator>(join_oper);
    }
  }

  // 如果没有FROM子句，创建一个CALC操作符来生成单行数据
  if (tables.empty()) {
    // 创建一个空的表达式列表，只是为了生成一行数据
    vector<unique_ptr<Expression>> dummy_expressions;
    table_oper = make_unique<CalcLogicalOperator>(std::move(dummy_expressions));
  }


  if (predicate_oper) {
    if (*last_oper) {
      predicate_oper->add_child(std::move(*last_oper));
    }

    last_oper = &predicate_oper;
  }

  unique_ptr<LogicalOperator> group_by_oper;
  rc = create_group_by_plan(select_stmt, group_by_oper);
  if (OB_FAIL(rc)) {
    LOG_WARN("failed to create group by logical plan. rc=%s", strrc(rc));
    return rc;
  }

  if (group_by_oper) {
    if (*last_oper) {
      group_by_oper->add_child(std::move(*last_oper));
    }

    last_oper = &group_by_oper;
  }

  auto project_oper = make_unique<ProjectLogicalOperator>(std::move(select_stmt->query_expressions()));
  if (*last_oper) {
    project_oper->add_child(std::move(*last_oper));
  }

  logical_operator = std::move(project_oper);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(FilterStmt *filter_stmt, const vector<Table *> &tables, unique_ptr<LogicalOperator> &logical_operator)
{
  RC                                  rc = RC::SUCCESS;
  vector<unique_ptr<Expression>> cmp_exprs;
  const vector<FilterUnit *>    &filter_units = filter_stmt->filter_units();
  for (const FilterUnit *filter_unit : filter_units) {
    const FilterObj &filter_obj_left  = filter_unit->left();
    const FilterObj &filter_obj_right = filter_unit->right();

    unique_ptr<Expression> left;
    if (filter_obj_left.is_expression()) {
      left = unique_ptr<Expression>(filter_obj_left.expression->copy().release());
      // 绑定表达式中的字段
      rc = bind_expression_fields(left, tables);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to bind fields in left expression. rc=%s", strrc(rc));
        return rc;
      }
    } else if (filter_obj_left.is_attr()) {
      left = unique_ptr<Expression>(new FieldExpr(filter_obj_left.field));
    } else {
      left = unique_ptr<Expression>(new ValueExpr(filter_obj_left.value));
    }

    unique_ptr<Expression> right;
    if (filter_obj_right.is_expression()) {
      right = unique_ptr<Expression>(filter_obj_right.expression->copy().release());
      // 绑定表达式中的字段
      rc = bind_expression_fields(right, tables);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to bind fields in right expression. rc=%s", strrc(rc));
        return rc;
      }
    } else if (filter_obj_right.is_attr()) {
      right = unique_ptr<Expression>(new FieldExpr(filter_obj_right.field));
    } else {
      right = unique_ptr<Expression>(new ValueExpr(filter_obj_right.value));
    }

    if (left->value_type() != right->value_type()) {
      auto left_to_right_cost = implicit_cast_cost(left->value_type(), right->value_type());
      auto right_to_left_cost = implicit_cast_cost(right->value_type(), left->value_type());
      if (left_to_right_cost <= right_to_left_cost && left_to_right_cost != INT32_MAX) {
        ExprType left_type = left->type();
        auto cast_expr = make_unique<CastExpr>(std::move(left), right->value_type());
        if (left_type == ExprType::VALUE) {
          Value left_val;
          if (OB_FAIL(rc = cast_expr->try_get_value(left_val)))
          {
            LOG_WARN("failed to get value from left child", strrc(rc));
            return rc;
          }
          left = make_unique<ValueExpr>(left_val);
        } else {
          left = std::move(cast_expr);
        }
      } else if (right_to_left_cost < left_to_right_cost && right_to_left_cost != INT32_MAX) {
        ExprType right_type = right->type();
        auto cast_expr = make_unique<CastExpr>(std::move(right), left->value_type());
        if (right_type == ExprType::VALUE) {
          Value right_val;
          if (OB_FAIL(rc = cast_expr->try_get_value(right_val)))
          {
            LOG_WARN("failed to get value from right child", strrc(rc));
            return rc;
          }
          right = make_unique<ValueExpr>(right_val);
        } else {
          right = std::move(cast_expr);
        }

      } else {
        rc = RC::UNSUPPORTED;
        LOG_WARN("unsupported cast from %s to %s", attr_type_to_string(left->value_type()), attr_type_to_string(right->value_type()));
        return rc;
      }
    }

    ComparisonExpr *cmp_expr = new ComparisonExpr(filter_unit->comp(), std::move(left), std::move(right));
    cmp_exprs.emplace_back(cmp_expr);
  }

  unique_ptr<PredicateLogicalOperator> predicate_oper;
  if (!cmp_exprs.empty()) {
    unique_ptr<ConjunctionExpr> conjunction_expr(new ConjunctionExpr(ConjunctionExpr::Type::AND, cmp_exprs));
    predicate_oper = unique_ptr<PredicateLogicalOperator>(new PredicateLogicalOperator(std::move(conjunction_expr)));
  }

  logical_operator = std::move(predicate_oper);
  return rc;
}

int LogicalPlanGenerator::implicit_cast_cost(AttrType from, AttrType to)
{
  if (from == to) {
    return 0;
  }
  return DataType::type_instance(from)->cast_cost(to);
}

RC LogicalPlanGenerator::create_plan(InsertStmt *insert_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table        *table = insert_stmt->table();
  vector<Value> values(insert_stmt->values(), insert_stmt->values() + insert_stmt->value_amount());

  InsertLogicalOperator *insert_operator = new InsertLogicalOperator(table, values);
  logical_operator.reset(insert_operator);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(DeleteStmt *delete_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table                      *table       = delete_stmt->table();
  FilterStmt                 *filter_stmt = delete_stmt->filter_stmt();
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, ReadWriteMode::READ_WRITE));

  unique_ptr<LogicalOperator> predicate_oper;
  vector<Table *> tables = {table};

  RC rc = create_plan(filter_stmt, tables, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  unique_ptr<LogicalOperator> delete_oper(new DeleteLogicalOperator(table));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    delete_oper->add_child(std::move(predicate_oper));
  } else {
    delete_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(delete_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(ExplainStmt *explain_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  unique_ptr<LogicalOperator> child_oper;

  Stmt *child_stmt = explain_stmt->child();

  RC rc = create(child_stmt, child_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create explain's child operator. rc=%s", strrc(rc));
    return rc;
  }

  logical_operator = unique_ptr<LogicalOperator>(new ExplainLogicalOperator);
  logical_operator->add_child(std::move(child_oper));
  return rc;
}

RC LogicalPlanGenerator::create_group_by_plan(SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  vector<unique_ptr<Expression>> &group_by_expressions = select_stmt->group_by();
  vector<Expression *> aggregate_expressions;
  vector<unique_ptr<Expression>> &query_expressions = select_stmt->query_expressions();
  function<RC(unique_ptr<Expression>&)> collector = [&](unique_ptr<Expression> &expr) -> RC {
    RC rc = RC::SUCCESS;
    if (expr->type() == ExprType::AGGREGATION) {
      expr->set_pos(aggregate_expressions.size() + group_by_expressions.size());
      aggregate_expressions.push_back(expr.get());
    }
    rc = ExpressionIterator::iterate_child_expr(*expr, collector);
    return rc;
  };

  function<RC(unique_ptr<Expression>&)> bind_group_by_expr = [&](unique_ptr<Expression> &expr) -> RC {
    RC rc = RC::SUCCESS;
    for (size_t i = 0; i < group_by_expressions.size(); i++) {
      auto &group_by = group_by_expressions[i];
      if (expr->type() == ExprType::AGGREGATION) {
        break;
      } else if (expr->equal(*group_by)) {
        expr->set_pos(i);
        continue;
      } else {
        rc = ExpressionIterator::iterate_child_expr(*expr, bind_group_by_expr);
      }
    }
    return rc;
  };

 bool found_unbound_column = false;
  function<RC(unique_ptr<Expression>&)> find_unbound_column = [&](unique_ptr<Expression> &expr) -> RC {
    RC rc = RC::SUCCESS;
    if (expr->type() == ExprType::AGGREGATION) {
      // do nothing
    } else if (expr->pos() != -1) {
      // do nothing
    } else if (expr->type() == ExprType::FIELD) {
      found_unbound_column = true;
    }else {
      rc = ExpressionIterator::iterate_child_expr(*expr, find_unbound_column);
    }
    return rc;
  };
  

  for (unique_ptr<Expression> &expression : query_expressions) {
    bind_group_by_expr(expression);
  }

  for (unique_ptr<Expression> &expression : query_expressions) {
    find_unbound_column(expression);
  }

  // collect all aggregate expressions
  for (unique_ptr<Expression> &expression : query_expressions) {
    collector(expression);
  }

  if (group_by_expressions.empty() && aggregate_expressions.empty()) {
    // 既没有group by也没有聚合函数，不需要group by
    return RC::SUCCESS;
  }

  if (found_unbound_column) {
    LOG_WARN("column must appear in the GROUP BY clause or must be part of an aggregate function");
    return RC::INVALID_ARGUMENT;
  }

  // 如果只需要聚合，但是没有group by 语句，需要生成一个空的group by 语句

  auto group_by_oper = make_unique<GroupByLogicalOperator>(std::move(group_by_expressions),
                                                           std::move(aggregate_expressions));
  logical_operator = std::move(group_by_oper);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(UpdateStmt *update_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table                      *table       = update_stmt->table();
  FilterStmt                 *filter_stmt = update_stmt->filter_stmt();
  std::unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, ReadWriteMode::READ_WRITE));

  std::unique_ptr<LogicalOperator> predicate_oper;
  vector<Table *> tables = {table};

  RC rc = create_plan(filter_stmt, tables, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  std::unique_ptr<LogicalOperator> update_oper(new UpdateLogicalOperator(table, update_stmt->field_name(), update_stmt->expression()));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    update_oper->add_child(std::move(predicate_oper));
  } else {
    update_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(update_oper);
  return RC::SUCCESS;
}
