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
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/parser/expression_binder.h"
#include "sql/expr/expression.h"

using namespace std;
using namespace common;

/**
 * @brief 将ConditionSqlNode转换为ComparisonExpr
 */
RC create_condition_expression(const ConditionSqlNode &condition, Expression *&expr, 
                              const unordered_map<string, Table *> &table_map)
{
  unique_ptr<Expression> left_expr;
  unique_ptr<Expression> right_expr;
  
  // 处理左侧表达式
  if (condition.left_is_attr) {
    const RelAttrSqlNode &attr = condition.left_attr;
    left_expr = make_unique<UnboundFieldExpr>(attr.relation_name, attr.attribute_name);
  } else {
    left_expr = make_unique<ValueExpr>(condition.left_value);
  }
  
  // 处理右侧表达式
  if (condition.right_is_attr) {
    const RelAttrSqlNode &attr = condition.right_attr;
    right_expr = make_unique<UnboundFieldExpr>(attr.relation_name, attr.attribute_name);
  } else {
    right_expr = make_unique<ValueExpr>(condition.right_value);
  }
  
  expr = new ComparisonExpr(condition.comp, std::move(left_expr), std::move(right_expr));
  return RC::SUCCESS;
}

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
  
  for (auto &join_table : join_tables_) {
    if (join_table.condition != nullptr) {
      delete join_table.condition;
    }
  }
}

RC SelectStmt::create(Db *db, SelectSqlNode &select_sql, Stmt *&stmt)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  SelectStmt *select_stmt = new SelectStmt();

  // 第一步：处理主表
  vector<Table *> tables;
  unordered_map<string, Table *> table_map;
  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      delete select_stmt;
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      delete select_stmt;
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert({table_name, table});
  }

  // 第二步：处理JOIN表
  vector<JoinTable> join_tables;
  for (const JoinSqlNode &join_sql : select_sql.joins) {
    Table *join_table = db->find_table(join_sql.relation.c_str());
    if (nullptr == join_table) {
      LOG_WARN("no such join table. db=%s, table_name=%s", 
               db->name(), join_sql.relation.c_str());
      delete select_stmt;
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    JoinTable join_info;
    join_info.table = join_table;
    join_info.alias = join_sql.relation;
    join_info.join_type = join_sql.type;
    join_info.condition = nullptr;  // 稍后绑定
    
    join_tables.push_back(join_info);
    table_map.insert({join_sql.relation, join_table});
  }

  // 第三步：构建多表绑定上下文
  BinderContext binder_context;
  
  // 添加主表到绑定上下文
  for (Table *table : tables) {
    binder_context.add_table(table);
  }
  
  // 添加JOIN表到绑定上下文
  for (const JoinTable &join_table : join_tables) {
    binder_context.add_table(join_table.table);
  }

  // 第四步：绑定JOIN条件表达式
  ExpressionBinder expression_binder(binder_context);
  for (size_t i = 0; i < select_sql.joins.size(); i++) {
    const JoinSqlNode &join_sql = select_sql.joins[i];
    
    // 转换ConditionSqlNode为Expression
    Expression *condition_expr = nullptr;
    RC rc = create_condition_expression(join_sql.condition, condition_expr, table_map);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create join condition expression");
      delete select_stmt;
      return rc;
    }
    
    // 绑定表达式
    vector<unique_ptr<Expression>> bound_expressions;
    unique_ptr<Expression> condition_copy(condition_expr);
    rc = expression_binder.bind_expression(condition_copy, bound_expressions);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to bind join condition expression");
      delete select_stmt;
      return rc;
    }
    
    join_tables[i].condition = bound_expressions[0].release();
  }

  // 第五步：绑定查询表达式
  vector<unique_ptr<Expression>> bound_expressions;
  for (unique_ptr<Expression> &expression : select_sql.expressions) {
    RC rc = expression_binder.bind_expression(expression, bound_expressions);
    if (OB_FAIL(rc)) {
      LOG_INFO("bind expression failed. rc=%s", strrc(rc));
      delete select_stmt;
      return rc;
    }
  }

  // 第六步：绑定group by表达式
  vector<unique_ptr<Expression>> group_by_expressions;
  for (unique_ptr<Expression> &expression : select_sql.group_by) {
    RC rc = expression_binder.bind_expression(expression, group_by_expressions);
    if (OB_FAIL(rc)) {
      LOG_INFO("bind expression failed. rc=%s", strrc(rc));
      delete select_stmt;
      return rc;
    }
  }

  // 第七步：处理WHERE条件
  Table *default_table = nullptr;
  if (tables.size() == 1 && join_tables.empty()) {
    default_table = tables[0];
  }

  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
                            default_table,
                            &table_map,
                            select_sql.conditions.data(),
                            static_cast<int>(select_sql.conditions.size()),
                            filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    delete select_stmt;
    return rc;
  }

  // 设置SelectStmt的成员变量
  select_stmt->tables_.swap(tables);
  select_stmt->join_tables_.swap(join_tables);
  select_stmt->query_expressions_.swap(bound_expressions);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->group_by_.swap(group_by_expressions);
  
  stmt = select_stmt;
  return RC::SUCCESS;
}
