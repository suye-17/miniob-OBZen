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
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/parser/expression_binder.h"

UpdateStmt::UpdateStmt(Table *table, const std::vector<std::string> &field_names,
    std::vector<Expression *> &&expressions, FilterStmt *filter_stmt)
    : table_(table), field_names_(field_names), expressions_(std::move(expressions)), filter_stmt_(filter_stmt)
{}

UpdateStmt::~UpdateStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
  for (Expression *expr : expressions_) {
    if (expr != nullptr) {
      delete expr;
    }
  }
  expressions_.clear();
}

/**
 * @brief 创建UPDATE语句的语义解析对象
 * @details 这个函数负责验证UPDATE语句的合法性，包括表存在性、字段存在性、类型匹配性等
 *
 * @param db 数据库对象，用于查找表信息
 * @param update 从语法解析得到的UPDATE节点，包含表名、字段名、新值和WHERE条件
 * @param stmt 输出参数，成功时返回创建的UpdateStmt对象
 * @return RC 操作结果码
 */
RC UpdateStmt::create(Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();

  // 参数有效性检查：确保数据库对象和表名都不为空
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // 验证字段数量和表达式数量是否相等
  if (update.attribute_names.size() != update.expressions.size()) {
    LOG_WARN("field count mismatch expression count. fields=%lu, expressions=%lu",
             update.attribute_names.size(), update.expressions.size());
    return RC::INVALID_ARGUMENT;
  }

  if (update.attribute_names.empty()) {
    LOG_WARN("no fields to update");
    return RC::INVALID_ARGUMENT;
  }

  // 验证目标表是否存在
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // 验证要更新的字段是否存在并检查重复字段
  std::unordered_set<std::string> field_set;
  for (const std::string &field_name : update.attribute_names) {
    const FieldMeta *field_meta = table->table_meta().field(field_name.c_str());
    if (nullptr == field_meta) {
      LOG_WARN("no such field in table. db=%s, table=%s, field=%s", 
          db->name(), table_name, field_name.c_str());
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }

    if (field_set.count(field_name) > 0) {
      LOG_WARN("duplicate field in update statement. field=%s", field_name.c_str());
      return RC::INVALID_ARGUMENT;
    }
    field_set.insert(field_name);
  }

  BinderContext binder_context;
  binder_context.add_table(table);
  ExpressionBinder expression_binder(binder_context);

  std::vector<Expression *> bound_expressions;
  for (size_t i = 0; i < update.expressions.size(); ++i) {
    if (update.expressions[i] == nullptr) {
      LOG_WARN("update expression is null. table=%s, field=%s", 
               table_name, update.attribute_names[i].c_str());
      // 清理已绑定的表达式
      for (Expression *expr : bound_expressions) {
        delete expr;
      }
      return RC::INVALID_ARGUMENT;
    }
    vector<unique_ptr<Expression>> temp_bound_expressions;
    unique_ptr<Expression>         expression_copy(update.expressions[i]);

    RC rc = expression_binder.bind_expression(expression_copy, temp_bound_expressions);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to bind expression. table=%s, field=%s, rc=%s", 
               table_name, update.attribute_names[i].c_str(), strrc(rc));
      // 清理已绑定的表达式
      for (Expression *expr : bound_expressions) {
        delete expr;
      }
      return rc;
    }

    if (temp_bound_expressions.size() != 1) {
      LOG_WARN("unexpected bound expression count: %lu", temp_bound_expressions.size());
      // 清理已绑定的表达式
      for (Expression *expr : bound_expressions) {
        delete expr;
      }
      return RC::INTERNAL;
    }

    bound_expressions.push_back(temp_bound_expressions[0].release());
  }

  // 处理where 条件（如果存在）
  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));
  FilterStmt *filter_stmt = nullptr;
  RC          rc          = FilterStmt::create(
      db, table, &table_map, update.conditions.data(), static_cast<int>(update.conditions.size()), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    for (Expression *expr : bound_expressions) {
      delete expr;
    }
    return rc;
  }

  // 创建UpdateStmt对象
  stmt = new UpdateStmt(table, update.attribute_names, std::move(bound_expressions), filter_stmt);
  return RC::SUCCESS;
}
