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

#include "sql/stmt/filter_stmt.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/sys/rc.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/parser/expression_binder.h"
#include "sql/expr/expression.h"

// FilterObj 拷贝构造函数实现
FilterObj::FilterObj(const FilterObj& other) 
  : is_attr(other.is_attr), field(other.field), value(other.value),
    value_list(other.value_list), has_value_list(other.has_value_list),
    has_subquery(other.has_subquery)
{
  if (other.subquery) {
    subquery = SelectSqlNode::create_copy(other.subquery.get());
  }
  if (other.expr) {
    expr = other.expr->copy().release();
  }
}

// FilterObj 拷贝赋值操作符实现
FilterObj& FilterObj::operator=(const FilterObj& other)
{
  if (this != &other) {
    // 清理原有表达式
    if (expr) {
      delete expr;
      expr = nullptr;
    }
    
    is_attr = other.is_attr;
    field = other.field;
    value = other.value;
    value_list = other.value_list;
    has_value_list = other.has_value_list;
    has_subquery = other.has_subquery;
    
    if (other.subquery) {
      subquery = SelectSqlNode::create_copy(other.subquery.get());
    } else {
      subquery = nullptr;
    }
    
    if (other.expr) {
      expr = other.expr->copy().release();
    }
  }
  return *this;
}

// FilterObj 析构函数实现
FilterObj::~FilterObj()
{
  if (expr) {
    delete expr;
    expr = nullptr;
  }
}

FilterStmt::~FilterStmt()
{
  for (FilterUnit *unit : filter_units_) {
    delete unit;
  }
  filter_units_.clear();
}

RC FilterStmt::create(Db *db, Table *default_table, unordered_map<string, Table *> *tables,
    const ConditionSqlNode *conditions, int condition_num, FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt  = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();
  for (int i = 0; i < condition_num; i++) {
    FilterUnit *filter_unit = nullptr;

    rc = create_filter_unit(db, default_table, tables, conditions[i], filter_unit);
    if (rc != RC::SUCCESS) {
      delete tmp_stmt;
      LOG_WARN("failed to create filter unit. condition index=%d", i);
      return rc;
    }
    tmp_stmt->filter_units_.push_back(filter_unit);
  }

  stmt = tmp_stmt;
  return rc;
}

RC get_table_and_field(Db *db, Table *default_table, unordered_map<string, Table *> *tables,
    const RelAttrSqlNode &attr, Table *&table, const FieldMeta *&field)
{
  if (common::is_blank(attr.relation_name.c_str())) {
    table = default_table;
  } else if (nullptr != tables) {
    auto iter = tables->find(attr.relation_name);
    if (iter != tables->end()) {
      table = iter->second;
    }
  } else {
    table = db->find_table(attr.relation_name.c_str());
  }
  if (nullptr == table) {
    LOG_WARN("No such table: attr.relation_name: %s", attr.relation_name.c_str());
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  field = table->table_meta().field(attr.attribute_name.c_str());
  if (nullptr == field) {
    LOG_WARN("no such field in table: table %s, field %s", table->name(), attr.attribute_name.c_str());
    table = nullptr;
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  return RC::SUCCESS;
}

RC FilterStmt::create_filter_unit(Db *db, Table *default_table, unordered_map<string, Table *> *tables,
    const ConditionSqlNode &condition, FilterUnit *&filter_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  filter_unit = new FilterUnit;

  // 创建绑定上下文，用于绑定表达式
  BinderContext binder_context;
  // 先添加所有tables map中的表
  if (tables != nullptr) {
    for (auto &pair : *tables) {
      binder_context.add_table(pair.second);
    }
  }
  // 如果default_table存在且不在tables map中，则添加它
  if (default_table != nullptr) {
    bool found = false;
    if (tables != nullptr) {
      for (auto &pair : *tables) {
        if (pair.second == default_table) {
          found = true;
          break;
        }
      }
    }
    if (!found) {
      binder_context.add_table(default_table);
    }
  }
  ExpressionBinder expression_binder(binder_context);

  // 处理左侧表达式：可能是表达式、属性、值或子查询
  if (condition.left_expr != nullptr) {
    // 左侧是表达式（如算术表达式）
    unique_ptr<Expression> left_expr(condition.left_expr->copy().release());
    LOG_DEBUG("Left expression type before binding: %d, value_type: %d", 
              static_cast<int>(left_expr->type()), static_cast<int>(left_expr->value_type()));
    vector<unique_ptr<Expression>> bound_expressions;
    rc = expression_binder.bind_expression(left_expr, bound_expressions);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to bind left expression");
      delete filter_unit;
      return rc;
    }
    if (bound_expressions.size() != 1) {
      LOG_WARN("invalid bound expression size: %d", bound_expressions.size());
      delete filter_unit;
      return RC::INVALID_ARGUMENT;
    }
    LOG_DEBUG("Left expression type after binding: %d, value_type: %d", 
              static_cast<int>(bound_expressions[0]->type()), static_cast<int>(bound_expressions[0]->value_type()));
    FilterObj filter_obj;
    filter_obj.init_expr(bound_expressions[0].release());
    filter_unit->set_left(filter_obj);
  } else if (condition.left_is_attr) {
    Table           *table = nullptr;
    const FieldMeta *field = nullptr;
    rc                     = get_table_and_field(db, default_table, tables, condition.left_attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      delete filter_unit;
      return rc;
    }
    FilterObj filter_obj;
    filter_obj.init_attr(Field(table, field));
    filter_unit->set_left(filter_obj);
  } else if (condition.has_subquery && condition.subquery) {
    // 左侧是子查询的情况（例如: (SELECT ...) = attr 或 (SELECT ...) = value）
    FilterObj filter_obj;
    filter_obj.init_subquery(condition.subquery.get());
    filter_unit->set_left(filter_obj);
  } else {
    // 左侧是常量值
    FilterObj filter_obj;
    filter_obj.init_value(condition.left_value);
    filter_unit->set_left(filter_obj);
  }

  // 处理IN/NOT IN操作的值列表或子查询
  if (comp == IN_OP || comp == NOT_IN_OP) {
    if (condition.has_subquery && condition.subquery) {
      // 处理子查询
      FilterObj filter_obj;
      filter_obj.init_subquery(condition.subquery.get());
      filter_unit->set_right(filter_obj);
    } else if (!condition.right_values.empty()) {
      // 处理值列表
      FilterObj filter_obj;
      filter_obj.init_value_list(condition.right_values);
      filter_unit->set_right(filter_obj);
    } else {
      LOG_WARN("IN/NOT IN operation requires value list or subquery");
      delete filter_unit;
      return RC::INVALID_ARGUMENT;
    }
  } else if (condition.right_expr != nullptr) {
    // 右侧是表达式（如算术表达式）
    unique_ptr<Expression> right_expr(condition.right_expr->copy().release());
    vector<unique_ptr<Expression>> bound_expressions;
    rc = expression_binder.bind_expression(right_expr, bound_expressions);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to bind right expression");
      delete filter_unit;
      return rc;
    }
    if (bound_expressions.size() != 1) {
      LOG_WARN("invalid bound expression size: %d", bound_expressions.size());
      delete filter_unit;
      return RC::INVALID_ARGUMENT;
    }
    FilterObj filter_obj;
    filter_obj.init_expr(bound_expressions[0].release());
    filter_unit->set_right(filter_obj);
  } else if (condition.right_is_attr) {
    Table           *table = nullptr;
    const FieldMeta *field = nullptr;
    rc                     = get_table_and_field(db, default_table, tables, condition.right_attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      delete filter_unit;
      return rc;
    }
    FilterObj filter_obj;
    filter_obj.init_attr(Field(table, field));
    filter_unit->set_right(filter_obj);
  } else if (condition.has_subquery && condition.subquery) {
    // 右侧是子查询的情况（例如: attr = (SELECT ...)）
    FilterObj filter_obj;
    filter_obj.init_subquery(condition.subquery.get());
    filter_unit->set_right(filter_obj);
  } else {
    FilterObj filter_obj;
    filter_obj.init_value(condition.right_value);
    filter_unit->set_right(filter_obj);
  }

  filter_unit->set_comp(comp);

  // 检查两个类型是否能够比较
  return rc;
}
