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
  
  // 处理左侧表达式 - 优先使用表达式字段
  if (condition.left_expression != nullptr) {
    // 直接使用表达式（需要拷贝，因为解析器创建的表达式会被管理）
    left_expr.reset(condition.left_expression->copy().release());
  } else if (condition.left_is_attr) {
    const RelAttrSqlNode &attr = condition.left_attr;
    
    // ✅ 验证字段存在性
    if (!attr.relation_name.empty()) {
      auto it = table_map.find(attr.relation_name);
      if (it == table_map.end()) {
        LOG_WARN("table not found in JOIN condition: %s", attr.relation_name.c_str());
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      Table *table = it->second;
      const FieldMeta *field_meta = table->table_meta().field(attr.attribute_name.c_str());
      if (!field_meta) {
        LOG_WARN("field not found in table: %s.%s", attr.relation_name.c_str(), attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    }
    
    left_expr = make_unique<UnboundFieldExpr>(attr.relation_name, attr.attribute_name);
  } else {
    left_expr = make_unique<ValueExpr>(condition.left_value);
  }
  
  // 检查是否为IN/NOT IN操作
  if (condition.comp == IN_OP || condition.comp == NOT_IN_OP) {
    // 使用值列表构造ComparisonExpr
    vector<Value> right_values = {condition.right_value};
    expr = new ComparisonExpr(condition.comp, std::move(left_expr), right_values);
    return RC::SUCCESS;
  }
  
  // 处理普通比较操作的右侧表达式
  unique_ptr<Expression> right_expr;
  // 处理右侧表达式 - 优先使用表达式字段
  if (condition.right_expression != nullptr) {
    // 直接使用表达式（需要拷贝）
    right_expr.reset(condition.right_expression->copy().release());
  } else if (condition.right_is_attr) {
    const RelAttrSqlNode &attr = condition.right_attr;
    
    // ✅ 验证字段存在性
    if (!attr.relation_name.empty()) {
      auto it = table_map.find(attr.relation_name);
      if (it == table_map.end()) {
        LOG_WARN("table not found in JOIN condition: %s", attr.relation_name.c_str());
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      Table *table = it->second;
      const FieldMeta *field_meta = table->table_meta().field(attr.attribute_name.c_str());
      if (!field_meta) {
        LOG_WARN("field not found in table: %s.%s", attr.relation_name.c_str(), attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    }
    
    right_expr = make_unique<UnboundFieldExpr>(attr.relation_name, attr.attribute_name);
  } else {
    right_expr = make_unique<ValueExpr>(condition.right_value);
  }
  
  expr = new ComparisonExpr(condition.comp, std::move(left_expr), std::move(right_expr));
  return RC::SUCCESS;
}

/**
 * @brief 将多个ConditionSqlNode转换为ConjunctionExpr (AND连接)
 */
RC create_join_conditions_expression(const vector<ConditionSqlNode> &conditions, Expression *&expr, 
                                    const unordered_map<string, Table *> &table_map)
{
  if (conditions.empty()) {
    expr = nullptr;
    return RC::SUCCESS;
  }
  
  if (conditions.size() == 1) {
    return create_condition_expression(conditions[0], expr, table_map);
  }
  
  // 多个条件用AND连接
  vector<unique_ptr<Expression>> condition_exprs;
  for (const ConditionSqlNode &condition : conditions) {
    Expression *condition_expr = nullptr;
    RC rc = create_condition_expression(condition, condition_expr, table_map);
    if (rc != RC::SUCCESS) {
      // ✅ 失败时清理已创建的表达式（unique_ptr会自动清理）
      return rc;
    }
    condition_exprs.emplace_back(condition_expr);
  }
  
  expr = new ConjunctionExpr(ConjunctionExpr::Type::AND, condition_exprs);
  return RC::SUCCESS;
}

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
  if (nullptr != having_filter_stmt_) {
    delete having_filter_stmt_;
    having_filter_stmt_ = nullptr;
  }
  // ✅ 清理JOIN条件表达式
  for (const JoinTable &join_table : join_tables_) {
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
    const char *table_name = join_sql.relation.c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. join table name is null");
      delete select_stmt;
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table in join. db=%s, table_name=%s", db->name(), table_name);
      delete select_stmt;
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    // ✅ 关键修复：先将JOIN表加入table_map，再创建和验证JOIN条件
    table_map.insert({table_name, table});

    // 创建JOIN条件表达式
    Expression *join_condition = nullptr;
    LOG_INFO("Creating JOIN condition for table %s with %zu conditions", 
             table_name, join_sql.conditions.size());
    RC rc = create_join_conditions_expression(join_sql.conditions, join_condition, table_map);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create join condition expression");
      // ✅ 清理已创建的JOIN条件表达式
      if (join_condition != nullptr) {
        delete join_condition;
      }
      // ✅ 清理已创建的其他JOIN表的条件
      for (const JoinTable &jt : join_tables) {
        if (jt.condition != nullptr) {
          delete jt.condition;
        }
      }
      delete select_stmt;
      return rc;
    }
    LOG_INFO("JOIN condition created successfully, type=%d", 
             join_condition ? static_cast<int>(join_condition->type()) : -1);

    JoinTable join_table;
    join_table.table = table;
    join_table.join_type = join_sql.type;
    join_table.condition = join_condition;
    join_tables.push_back(join_table);
  }

  // collect query fields in `select` statement
  vector<unique_ptr<Expression>> bound_expressions;
  BinderContext binder_context;
  
  // 添加主表到绑定上下文中
  for (Table *table : tables) {
    binder_context.add_table(table);
  }
  
  // 添加JOIN表到绑定上下文中（用于SELECT * 投影和字段绑定）
  for (const JoinTable &join_table : join_tables) {
    binder_context.add_table(join_table.table);
  }
  
  ExpressionBinder               expression_binder(binder_context);

  for (unique_ptr<Expression> &expression : select_sql.expressions) {
    RC rc = expression_binder.bind_expression(expression, bound_expressions);
    if (OB_FAIL(rc)) {
      LOG_INFO("bind expression failed. rc=%s", strrc(rc));
      // ✅ 清理JOIN条件表达式
      for (const JoinTable &jt : join_tables) {
        if (jt.condition != nullptr) {
          delete jt.condition;
        }
      }
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
      // ✅ 清理JOIN条件表达式
      for (const JoinTable &jt : join_tables) {
        if (jt.condition != nullptr) {
          delete jt.condition;
        }
      }
      delete select_stmt;
      return rc;
    }
  }

  // 第七步：处理WHERE条件
  Table *default_table = nullptr;
  if (tables.size() == 1 && join_tables.empty()) {
    default_table = tables[0];
  }

  // create having filter statement
  FilterStmt *having_filter_stmt = nullptr;
  if (!select_sql.having.empty()) {
    RC rc = FilterStmt::create(db,
        default_table,
        &table_map,
        select_sql.having.data(),
        static_cast<int>(select_sql.having.size()),
        having_filter_stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct having filter stmt");
      // ✅ 清理JOIN条件表达式
      for (const JoinTable &jt : join_tables) {
        if (jt.condition != nullptr) {
          delete jt.condition;
        }
      }
      delete select_stmt;
      return rc;
    }
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
                            default_table,
                            &table_map,
                            select_sql.conditions.data(),
                            static_cast<int>(select_sql.conditions.size()),
                            filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    // ✅ 清理JOIN条件表达式
    for (const JoinTable &jt : join_tables) {
      if (jt.condition != nullptr) {
        delete jt.condition;
      }
    }
    // ✅ 清理having filter stmt
    if (having_filter_stmt != nullptr) {
      delete having_filter_stmt;
    }
    delete select_stmt;
    return rc;
  }

  // 设置SelectStmt的成员变量
  select_stmt->tables_.swap(tables);
  select_stmt->join_tables_.swap(join_tables);
  select_stmt->query_expressions_.swap(bound_expressions);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->group_by_.swap(group_by_expressions);
  select_stmt->having_filter_stmt_ = having_filter_stmt;
  stmt                             = select_stmt;
  return RC::SUCCESS;
}
