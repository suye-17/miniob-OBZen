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

UpdateStmt::UpdateStmt(Table *table, const std::string &field_name, Expression *expression, FilterStmt *filter_stmt) 
  : table_(table), field_name_(field_name), expression_(expression), filter_stmt_(filter_stmt) 
{}

UpdateStmt::~UpdateStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
  if (nullptr != expression_) {
    delete expression_;
    expression_ = nullptr;
  }
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

  // 第一步：验证目标表是否存在
  // 从数据库中查找指定的表，如果不存在则返回错误
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // 第二步：验证要更新的字段是否存在
  // 从表的元数据中查找指定的字段，确保字段名有效
  const FieldMeta *field_meta = table->table_meta().field(update.attribute_name.c_str());
  if (nullptr == field_meta) {
    LOG_WARN("no such field in table. db=%s, table=%s, field=%s", 
        db->name(), table_name, update.attribute_name.c_str());
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  // 第三步：验证和处理更新表达式
  // 注意：Expression的类型在运行时才能确定，这里不做类型检查
  // 类型兼容性将在执行时进行验证
  if (update.expression == nullptr) {
    LOG_WARN("update expression is null. table=%s, field=%s", table_name, update.attribute_name.c_str());
    return RC::INVALID_ARGUMENT;
  }

  // 第四步：创建表绑定上下文并绑定表达式
  BinderContext binder_context;
  binder_context.add_table(table);
  ExpressionBinder expression_binder(binder_context);
  
  vector<unique_ptr<Expression>> bound_expressions;
  unique_ptr<Expression> expression_copy(update.expression);
  
  RC rc = expression_binder.bind_expression(expression_copy, bound_expressions);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to bind expression. table=%s, field=%s, rc=%s", 
             table_name, update.attribute_name.c_str(), strrc(rc));
    return rc;
  }
  
  if (bound_expressions.size() != 1) {
    LOG_WARN("unexpected bound expression count: %lu", bound_expressions.size());
    return RC::INTERNAL;
  }
  
  Expression *bound_expression = bound_expressions[0].release();

  // 第五步：处理WHERE条件（如果存在）
  // 创建表映射，用于WHERE条件中的表引用解析
  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));

  // 创建过滤条件语句，用于处理WHERE子句
  FilterStmt *filter_stmt = nullptr;
  rc = FilterStmt::create(
      db, table, &table_map, update.conditions.data(), static_cast<int>(update.conditions.size()), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    delete bound_expression;
    return rc;
  }

  // 第六步：创建UpdateStmt对象
  // 将验证通过的表、字段名、绑定后的表达式和过滤条件封装到UpdateStmt中
  stmt = new UpdateStmt(table, update.attribute_name, bound_expression, filter_stmt);
  return RC::SUCCESS;
}
