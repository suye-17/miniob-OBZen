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
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "storage/record/record.h"
#include "sql/parser/expression_binder.h"
#include "sql/parser/parse_defs.h"

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

// 辅助函数：创建恒假条件（用于NULL处理）
RC FilterStmt::create_always_false_condition(FilterObj& left_obj, FilterObj& right_obj, FilterUnit* filter_unit)
{
  // 创建 1 = 0 的条件，确保总是返回FALSE
  Value one_value;
  one_value.set_int(1);
  left_obj.init_value(one_value);
  
  Value zero_value;
  zero_value.set_int(0);
  right_obj.init_value(zero_value);
  
  filter_unit->set_left(left_obj);
  filter_unit->set_right(right_obj);
  filter_unit->set_comp(EQUAL_TO);
  
  return RC::SUCCESS;
}

// 辅助函数：创建表达式等于真值的条件
RC FilterStmt::create_expression_equals_true_condition(FilterObj& left_obj, FilterObj& right_obj, FilterUnit* filter_unit)
{
  filter_unit->set_left(left_obj);
  
  // 右侧设置为true值
  Value true_value;
  true_value.set_boolean(true);
  right_obj.init_value(true_value);
  filter_unit->set_right(right_obj);
  filter_unit->set_comp(EQUAL_TO);
  
  return RC::SUCCESS;
}

// 辅助函数：处理单独表达式条件
RC FilterStmt::handle_single_expression_condition(FilterObj& left_obj, FilterObj& right_obj, FilterUnit* filter_unit)
{
  // 检查是否为NULL值：如果是，创建恒假条件
  if (left_obj.is_value() && left_obj.value.is_null()) {
    LOG_INFO("WHERE condition is NULL, creating always-false condition for empty result");
    return create_always_false_condition(left_obj, right_obj, filter_unit);
  } else {
    // 非NULL的单独表达式，转换为 expression = true
    return create_expression_equals_true_condition(left_obj, right_obj, filter_unit);
  }
}

// 辅助函数：统一处理表达式转换为FilterObj
RC FilterStmt::convert_expression_to_filter_obj(Expression* expr, Table* default_table, 
                                                FilterObj& filter_obj, const char* side_name)
{
  if (expr == nullptr) {
    LOG_WARN("%s expression is null", side_name);
    return RC::INVALID_ARGUMENT;
  }

  // 智能处理：区分不同类型的表达式
  if (expr->type() == ExprType::UNBOUND_FIELD) {
    // 单独的字段表达式，直接绑定
    auto unbound_field = static_cast<UnboundFieldExpr*>(expr);
    const char* field_name = unbound_field->field_name();
    
    if (default_table != nullptr) {
      const FieldMeta* field_meta = default_table->table_meta().field(field_name);
      if (field_meta != nullptr) {
        Field field(default_table, field_meta);
        filter_obj.init_attr(field);
        return RC::SUCCESS;
      } else {
        LOG_WARN("field not found: %s", field_name);
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    } else {
      LOG_WARN("no default table for field: %s", field_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
  } else if (expr->type() == ExprType::VALUE) {
    // 常量表达式，直接求值
    Value result;
    RC rc = expr->try_get_value(result);
    if (rc == RC::SUCCESS) {
      // 检查NULL值：如果表达式结果是NULL，设置特殊标记
      if (result.is_null()) {
        LOG_INFO("WHERE condition contains NULL value, will return empty result");
      }
      filter_obj.init_value(result);
      return RC::SUCCESS;
    } else {
      LOG_WARN("failed to evaluate %s constant expression", side_name);
      return rc;
    }
  } else {
    // 复杂表达式，尝试静态求值，失败则存储表达式副本
    Value result;
    RC rc = expr->try_get_value(result);
    if (rc == RC::SUCCESS) {
      // 能静态求值的常量表达式
      // 检查NULL值：如果表达式结果是NULL，设置特殊标记
      if (result.is_null()) {
        LOG_INFO("WHERE condition contains NULL value, will return empty result");
      }
      filter_obj.init_value(result);
      return RC::SUCCESS;
    } else {
      // 包含字段引用的复杂表达式，创建副本
      try {
        auto copied_expr = expr->copy();
        if (copied_expr == nullptr) {
          LOG_WARN("failed to copy %s expression", side_name);
          return RC::INTERNAL;
        }
        filter_obj.init_expression(copied_expr.release());
        return RC::SUCCESS;
      } catch (const std::exception& e) {
        LOG_WARN("exception when copying %s expression: %s", side_name, e.what());
        return RC::INTERNAL;
      } catch (...) {
        LOG_WARN("unknown exception when copying %s expression", side_name);
        return RC::INTERNAL;
      }
    }
  }
}

RC FilterStmt::create_filter_unit(Db *db, Table *default_table, unordered_map<string, Table *> *tables,
    const ConditionSqlNode &condition, FilterUnit *&filter_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp > NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  filter_unit = new FilterUnit;

  // 统一架构：所有条件都是表达式条件
  if (condition.is_expression_condition) {
    FilterObj left_obj, right_obj;
    
    // 处理左侧表达式
    rc = convert_expression_to_filter_obj(condition.left_expression, default_table, left_obj, "left");
    if (rc != RC::SUCCESS) {
      delete filter_unit;
      return rc;
    }
    
    // 处理单独表达式条件（NO_OP）
    if (comp == NO_OP) {
      rc = handle_single_expression_condition(left_obj, right_obj, filter_unit);
      if (rc != RC::SUCCESS) {
        delete filter_unit;
        return rc;
      }
    } else if (comp == IS_NULL || comp == IS_NOT_NULL) {
      // 处理 IS NULL 和 IS NOT NULL 条件（没有右侧表达式）
      filter_unit->set_left(left_obj);
      // 对于 IS NULL 和 IS NOT NULL，不设置右侧对象
      filter_unit->set_comp(comp);
    } else {
      // 处理其他比较操作符的右侧表达式
      rc = convert_expression_to_filter_obj(condition.right_expression, default_table, right_obj, "right");
      if (rc != RC::SUCCESS) {
        delete filter_unit;
        return rc;
      }
      filter_unit->set_left(left_obj);
      filter_unit->set_right(right_obj);
      filter_unit->set_comp(comp);
    }
    
    // 清理原始表达式内存（已经复制到FilterObj中）
    delete condition.left_expression;
    if (condition.right_expression != nullptr) {
      delete condition.right_expression;
      const_cast<ConditionSqlNode&>(condition).right_expression = nullptr;
    }
    const_cast<ConditionSqlNode&>(condition).left_expression = nullptr;
    
    return RC::SUCCESS;
  }

  // 如果不是表达式条件，说明有问题（因为现在所有条件都应该是表达式）
  LOG_WARN("condition is not an expression condition, this should not happen with unified architecture");
  delete filter_unit;
  return RC::INVALID_ARGUMENT;

  // 检查两个类型是否能够比较
  return rc;
}
