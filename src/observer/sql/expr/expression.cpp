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
// Created by Wangyunlai on 2022/07/05.
//

#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/arithmetic_operator.hpp"
#include "sql/stmt/select_stmt.h"
#include "sql/expr/subquery_executor.h"
#include "sql/parser/parse_defs.h"
#include "session/session.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "common/sys/rc.h"
#include "common/value.h"
#include "storage/common/column.h"
#include <cmath>

using namespace std;

// 辅助函数：绑定UnboundFieldExpr到FieldExpr
static RC bind_unbound_field_expr(unique_ptr<Expression> &expr, const vector<Table *> &tables)
{
  if (!expr || expr->type() != ExprType::UNBOUND_FIELD) {
    return RC::SUCCESS;
  }

  auto unbound = static_cast<UnboundFieldExpr *>(expr.get());
  const char *table_name = unbound->table_name();
  const char *field_name = unbound->field_name();

  if (strlen(table_name) > 0) {
    // 有表名前缀，直接查找
    Table *table = nullptr;
    for (Table *t : tables) {
      if (strcasecmp(t->name(), table_name) == 0) {
        table = t;
        break;
      }
    }
    if (!table) {
      LOG_WARN("Table not found: %s", table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    const FieldMeta *field_meta = table->table_meta().field(field_name);
    if (!field_meta) {
      LOG_WARN("Field not found: %s.%s", table_name, field_name);
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    expr = make_unique<FieldExpr>(table, field_meta);
  } else {
    // 没有表名前缀，在所有表中查找
    Table *found_table = nullptr;
    const FieldMeta *found_field = nullptr;
    for (Table *t : tables) {
      const FieldMeta *field_meta = t->table_meta().field(field_name);
      if (field_meta) {
        if (found_table) {
          LOG_WARN("Ambiguous field: %s", field_name);
          return RC::SCHEMA_FIELD_NOT_EXIST;
        }
        found_table = t;
        found_field = field_meta;
      }
    }
    if (!found_table) {
      LOG_WARN("Field not found in any table: %s", field_name);
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    expr = make_unique<FieldExpr>(found_table, found_field);
  }

  return RC::SUCCESS;
}

// LIKE模式匹配实现：%匹配零个或多个字符，_匹配单个字符
static bool match_like_pattern(const char *text, const char *pattern)
{
  // 添加空指针检查
  if (!text || !pattern) {
    return false;
  }
  
  const char *t = text;
  const char *p = pattern;
  
  while (*p) {
    if (*p == '%') {
      p++;
      if (*p == '\0') return true;
      
      while (*t) {
        if (match_like_pattern(t, p)) return true;
        t++;
      }
      return false;
    } 
    else if (*p == '_') {
      if (*t == '\0') return false;
      p++; t++;
    } 
    else {
      if (*t != *p) return false;
      p++; t++;
    }
  }
  
  return *t == '\0';
}

RC FieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  LOG_INFO("FieldExpr::get_value: table='%s', field='%s'", 
           table_name() ? table_name() : "NULL", 
           field_name() ? field_name() : "NULL");
           
  TupleCellSpec spec(table_name(), field_name());
  RC rc = tuple.find_cell(spec, value);
  
  LOG_INFO("FieldExpr::get_value: find_cell returned rc=%s", strrc(rc));
  
  return rc;
}

bool FieldExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != ExprType::FIELD) {
    return false;
  }
  const auto &other_field_expr = static_cast<const FieldExpr &>(other);
  return table_name() == other_field_expr.table_name() && field_name() == other_field_expr.field_name();
}

// TODO: 在进行表达式计算时，`chunk` 包含了所有列，因此可以通过 `field_id` 获取到对应列。
// 后续可以优化成在 `FieldExpr` 中存储 `chunk` 中某列的位置信息。
RC FieldExpr::get_column(Chunk &chunk, Column &column)
{
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
  } else {
    column.reference(chunk.column(field().meta()->field_id()));
  }
  return RC::SUCCESS;
}

bool ValueExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != ExprType::VALUE) {
    return false;
  }
  const auto &other_value_expr = static_cast<const ValueExpr &>(other);
  return value_.compare(other_value_expr.get_value()) == 0;
}

RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
  return RC::SUCCESS;
}

RC ValueExpr::get_column(Chunk &chunk, Column &column)
{
  column.init(value_);
  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
CastExpr::CastExpr(unique_ptr<Expression> child, AttrType cast_type) : child_(std::move(child)), cast_type_(cast_type)
{}

CastExpr::~CastExpr() {}

RC CastExpr::cast(const Value &value, Value &cast_value) const
{
  RC rc = RC::SUCCESS;
  if (this->value_type() == value.attr_type()) {
    cast_value = value;
    return rc;
  }
  rc = Value::cast_to(value, cast_type_, cast_value);
  return rc;
}

RC CastExpr::get_value(const Tuple &tuple, Value &result) const
{
  Value value;
  RC    rc = child_->get_value(tuple, value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, result);
}

RC CastExpr::try_get_value(Value &result) const
{
  Value value;
  RC    rc = child_->try_get_value(value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, result);
}

////////////////////////////////////////////////////////////////////////////////

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : comp_(comp), left_(std::move(left)), right_(std::move(right))
{}

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, const vector<Value> &right_values)
    : comp_(comp), left_(std::move(left)), right_values_(right_values), has_value_list_(true), has_subquery_(false)
{
}

// 新增：支持子查询的构造函数
ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<SelectSqlNode> subquery)
    : comp_(comp), left_(std::move(left)), right_(nullptr), right_values_(), has_value_list_(false), subquery_(std::move(subquery)), has_subquery_(true)
{
}

// 新增：支持 EXISTS 的构造函数（不需要左侧表达式）
ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<SelectSqlNode> subquery)
    : comp_(comp), left_(nullptr), right_(nullptr), right_values_(), has_value_list_(false), subquery_(std::move(subquery)), has_subquery_(true)
{
  // EXISTS 和 NOT EXISTS 不需要左侧表达式
  assert(comp == EXISTS_OP || comp == NOT_EXISTS_OP);
}

ComparisonExpr::~ComparisonExpr() {}

RC ComparisonExpr::compare_value(const Value &left, const Value &right, bool &result) const
{
  RC  rc         = RC::SUCCESS;
  
  // 添加详细的比较日志
  LOG_DEBUG("compare_value: left=%s (type=%d), right=%s (type=%d)", 
           left.to_string().c_str(), static_cast<int>(left.attr_type()),
           right.to_string().c_str(), static_cast<int>(right.attr_type()));
  
  int cmp_result = left.compare(right);
  result         = false;
  LOG_INFO("COMPARE: left=%s(%d), right=%s(%d), cmp_result=%d", 
           left.to_string().c_str(), (int)left.attr_type(),
           right.to_string().c_str(), (int)right.attr_type(), cmp_result);
  switch (comp_) {
    case EQUAL_TO: {
      result = (0 == cmp_result);
      LOG_INFO("EQUAL_TO result: %s", result ? "true" : "false");
    } break;
    case LESS_EQUAL: {
      result = (cmp_result <= 0);
    } break;
    case NOT_EQUAL: {
      result = (cmp_result != 0);
    } break;
    case LESS_THAN: {
      result = (cmp_result < 0);
    } break;
    case GREAT_EQUAL: {
      result = (cmp_result >= 0);
    } break;
    case GREAT_THAN: {
      result = (cmp_result > 0);
    } break;
    case IS_NULL: {
      result = left.is_null();
    } break;
    case IS_NOT_NULL: {
      result = !left.is_null();
    } break;
    case LIKE_OP: {
      if (left.attr_type() != AttrType::CHARS || right.attr_type() != AttrType::CHARS) {
        LOG_WARN("LIKE operation requires string types, got left: %d, right: %d", 
                 left.attr_type(), right.attr_type());
        rc = RC::INVALID_ARGUMENT;
        break;
      }
      
      std::string text = left.get_string();
      std::string pattern = right.get_string();
      result = match_like_pattern(text.c_str(), pattern.c_str());
      LOG_INFO("LIKE result: '%s' LIKE '%s' = %s", text.c_str(), pattern.c_str(), result ? "true" : "false");
    } break;
    case NOT_LIKE_OP: {
      if (left.attr_type() != AttrType::CHARS || right.attr_type() != AttrType::CHARS) {
        LOG_WARN("NOT LIKE operation requires string types, got left: %d, right: %d", 
                 left.attr_type(), right.attr_type());
        rc = RC::INVALID_ARGUMENT;
        break;
      }
      
      std::string text = left.get_string();
      std::string pattern = right.get_string();
      result = !match_like_pattern(text.c_str(), pattern.c_str());
      LOG_INFO("NOT LIKE result: '%s' NOT LIKE '%s' = %s", text.c_str(), pattern.c_str(), result ? "true" : "false");
    } break;
    default: {
      LOG_WARN("unsupported comparison. %d", comp_);
      rc = RC::INTERNAL;
    } break;
  }

  return rc;
}

RC ComparisonExpr::compare_with_value_list(const Value &left, const vector<Value> &right_values, bool &result) const
{
  result = false;
  
  // 检查值列表是否为空
  if (right_values.empty()) {
    result = (comp_ == NOT_IN_OP);  // 空集合的情况
    return RC::SUCCESS;
  }
  
  // 遍历值列表进行比较
  for (const Value &right_value : right_values) {
    int cmp_result = left.compare(right_value);
    LOG_DEBUG("比较 %s (类型:%d) 和 %s (类型:%d), 结果: %d", 
              left.to_string().c_str(), static_cast<int>(left.attr_type()),
              right_value.to_string().c_str(), static_cast<int>(right_value.attr_type()),
              cmp_result);
    
    if (cmp_result == 0) {
      // 找到匹配项
      result = (comp_ == IN_OP);
      return RC::SUCCESS;
    }
  }
  
  // 没有找到匹配项
  result = (comp_ == NOT_IN_OP);
  
  return RC::SUCCESS;
}

RC ComparisonExpr::try_get_value(Value &cell) const
{
  // EXISTS/NOT EXISTS操作不能在编译时求值，因为需要执行子查询
  if (comp_ == EXISTS_OP || comp_ == NOT_EXISTS_OP) {
    return RC::INVALID_ARGUMENT;
  }

  // 尝试计算常量表达式的值
  Value left_value, right_value;

  RC rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    return RC::INVALID_ARGUMENT;
  }

  // IS NULL 和 IS NOT NULL 是一元操作，不需要右操作数
  if (comp_ == IS_NULL || comp_ == IS_NOT_NULL) {
    bool is_null = left_value.is_null();
    cell.set_boolean(comp_ == IS_NULL ? is_null : !is_null);
    return RC::SUCCESS;
  }

  // 对于其他比较操作，需要右操作数
  rc = right_->try_get_value(right_value);
  if (rc != RC::SUCCESS) {
    return RC::INVALID_ARGUMENT;
  }

  // SQL标准：NULL与任何值比较都返回NULL
  if (left_value.is_null() || right_value.is_null()) {
    cell.set_null();
    return RC::SUCCESS;
  }

  bool value = false;
  rc         = compare_value(left_value, right_value, value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to compare tuple cells. rc=%s", strrc(rc));
  } else {
    cell.set_boolean(value);
  }
  return rc;
}

RC ComparisonExpr::get_value(const Tuple &tuple, Value &value) const
{
  LOG_DEBUG("ComparisonExpr::get_value - comp_=%d, has_value_list_=%d, has_subquery_=%d", 
           static_cast<int>(comp_), has_value_list_, has_subquery_);
  
  Value left_value;
  Value right_value;

  // EXISTS/NOT EXISTS操作不需要左侧表达式
  if (comp_ == EXISTS_OP || comp_ == NOT_EXISTS_OP) {
    // 直接跳到子查询处理逻辑
    bool bool_value = false;
    
    if (has_subquery_) {
      // 处理子查询
      vector<Value> subquery_results;
      
      // 实际执行子查询
      RC subquery_rc = execute_subquery(subquery_results);
      if (subquery_rc != RC::SUCCESS) {
        LOG_ERROR("子查询执行失败! rc=%s", strrc(subquery_rc));
        return subquery_rc;
      }
      
      // EXISTS/NOT_EXISTS 子查询：只检查结果集是否为空
      LOG_DEBUG("Processing EXISTS/NOT_EXISTS subquery, result count=%zu", subquery_results.size());
      bool exists = !subquery_results.empty();
      bool_value = (comp_ == EXISTS_OP) ? exists : !exists;
      LOG_DEBUG("EXISTS result: exists=%s, final_result=%s", 
               exists ? "TRUE" : "FALSE", bool_value ? "TRUE" : "FALSE");
    } else {
      LOG_WARN("EXISTS/NOT EXISTS operation without subquery");
      return RC::INVALID_ARGUMENT;
    }
    
    value.set_boolean(bool_value);
    return RC::SUCCESS;
  }

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  // IS NULL 和 IS NOT NULL 处理 - 只需要左操作数
  if (comp_ == IS_NULL || comp_ == IS_NOT_NULL) {
    bool is_null = left_value.is_null();
    value.set_boolean(comp_ == IS_NULL ? is_null : !is_null);
    return RC::SUCCESS;
  }

  bool bool_value = false;

  // 优先处理IN/NOT IN的值列表形式
  if (has_value_list_) {
    // 使用值列表进行比较（IN/NOT IN操作）
    LOG_DEBUG("Processing IN/NOT IN with value list, left_value=%s, value_list_size=%zu", 
             left_value.to_string().c_str(), right_values_.size());
    rc = compare_with_value_list(left_value, right_values_, bool_value);
    LOG_DEBUG("IN/NOT IN result: %s", bool_value ? "TRUE" : "FALSE");
  } else if (has_subquery_) {
    // 处理子查询
    vector<Value> subquery_results;
    
    // 实际执行子查询
    fprintf(stderr, "[DEBUG] About to execute subquery in ComparisonExpr\n");
    RC subquery_rc = execute_subquery(subquery_results);
    fprintf(stderr, "[DEBUG] Subquery executed, rc=%d, result_count=%zu\n", static_cast<int>(subquery_rc), subquery_results.size());
    if (subquery_rc != RC::SUCCESS) {
      LOG_ERROR("子查询执行失败! rc=%s", strrc(subquery_rc));
      return subquery_rc;
    }
    
    // 区分 IN/NOT_IN、EXISTS/NOT_EXISTS 和标量子查询
    if (comp_ == IN_OP || comp_ == NOT_IN_OP) {
      // IN/NOT_IN 子查询：使用值列表比较
      Value left_value;
      rc = left_->get_value(tuple, left_value);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
        return rc;
      }
      rc = compare_with_value_list(left_value, subquery_results, bool_value);
    } else if (comp_ == EXISTS_OP || comp_ == NOT_EXISTS_OP) {
      // EXISTS/NOT_EXISTS 子查询：只检查结果集是否为空
      LOG_DEBUG("Processing EXISTS/NOT_EXISTS subquery, result count=%zu", subquery_results.size());
      bool exists = !subquery_results.empty();
      bool_value = (comp_ == EXISTS_OP) ? exists : !exists;
      LOG_DEBUG("EXISTS result: exists=%s, final_result=%s", 
               exists ? "TRUE" : "FALSE", bool_value ? "TRUE" : "FALSE");
      rc = RC::SUCCESS;
    } else {
      // 标量子查询：只能返回一个值
      if (subquery_results.empty()) {
        LOG_WARN("标量子查询返回空结果集");
        // 空结果集返回 NULL，在布尔上下文中为 false
        bool_value = false;
        rc = RC::SUCCESS;
      } else {
        // ✅ SQL标准：标量子查询只能返回一个值
        if (subquery_results.size() > 1) {
          fprintf(stderr, "[ERROR] Scalar subquery returned %zu rows - INVALID!\n", subquery_results.size());
          LOG_WARN("标量子查询返回多个值 (%zu 个)，这是错误的", subquery_results.size());
          LOG_WARN("SQL Hint: 添加 WHERE 条件或 LIMIT 1 来确保返回单行");
          return RC::INVALID_ARGUMENT;  // 返回错误
        }
        
        // 获取子查询值（第一个）
        Value subquery_value = subquery_results[0];
        LOG_DEBUG("Scalar subquery returned value: %s (type=%d)", 
                 subquery_value.to_string().c_str(), static_cast<int>(subquery_value.attr_type()));
        
        // 判断子查询在左边还是右边
        if (left_) {
          // 子查询在右边，left_ 是要比较的值
          Value left_value;
          rc = left_->get_value(tuple, left_value);
          if (rc != RC::SUCCESS) {
            LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
            return rc;
          }
          LOG_DEBUG("Comparing: left_value=%s (type=%d) comp_op=%d subquery_value=%s (type=%d)", 
                   left_value.to_string().c_str(), static_cast<int>(left_value.attr_type()),
                   static_cast<int>(comp_), 
                   subquery_value.to_string().c_str(), static_cast<int>(subquery_value.attr_type()));
          rc = compare_value(left_value, subquery_value, bool_value);
          LOG_DEBUG("Comparison result: %s", bool_value ? "TRUE" : "FALSE");
        } else if (right_) {
          // 子查询在左边，right_ 是要比较的值
          Value right_value;
          rc = right_->get_value(tuple, right_value);
          if (rc != RC::SUCCESS) {
            LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
            return rc;
          }
          LOG_DEBUG("Comparing: subquery_value=%s (type=%d) comp_op=%d right_value=%s (type=%d)", 
                   subquery_value.to_string().c_str(), static_cast<int>(subquery_value.attr_type()),
                   static_cast<int>(comp_), 
                   right_value.to_string().c_str(), static_cast<int>(right_value.attr_type()));
          rc = compare_value(subquery_value, right_value, bool_value);
          LOG_DEBUG("Comparison result: %s", bool_value ? "TRUE" : "FALSE");
        } else {
          LOG_ERROR("标量子查询：左右表达式都为空");
          return RC::INTERNAL;
        }
      }
    }
  } else if (right_) {
    // 使用右侧表达式进行比较
    Value left_value;
    rc = left_->get_value(tuple, left_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
      return rc;
    }
    Value right_value;
    rc = right_->get_value(tuple, right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
    rc = compare_value(left_value, right_value, bool_value);
  } else {
    LOG_WARN("ComparisonExpr: both has_value_list_ is false and right_ is null");
    return RC::INTERNAL;
  }
  
  if (rc == RC::SUCCESS) {
    value.set_boolean(bool_value);
    // 降低日志级别，避免在大数据量查询时产生过多日志
    LOG_TRACE("ComparisonExpr final result: bool_value=%s", bool_value ? "TRUE" : "FALSE");
  } else {
    LOG_WARN("ComparisonExpr failed with rc=%d", rc);
  }
  return rc;
}

RC ComparisonExpr::eval(Chunk &chunk, vector<uint8_t> &select)
{
  RC     rc = RC::SUCCESS;
  Column left_column;
  Column right_column;

  rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  if (left_column.attr_type() != right_column.attr_type()) {
    LOG_WARN("cannot compare columns with different types");
    return RC::INTERNAL;
  }
  if (left_column.attr_type() == AttrType::INTS) {
    rc = compare_column<int>(left_column, right_column, select);
  } else if (left_column.attr_type() == AttrType::FLOATS) {
    rc = compare_column<float>(left_column, right_column, select);
  } else if (left_column.attr_type() == AttrType::CHARS) {
    // 对于字符串类型，特别是LIKE和NOT LIKE操作，需要逐行处理
    if (comp_ == LIKE_OP || comp_ == NOT_LIKE_OP) {
      select.clear();
      select.resize(chunk.rows(), 0);
      
      for (int i = 0; i < chunk.rows(); i++) {
        Value left_value = left_column.get_value(i);
        Value right_value = right_column.get_value(i);
        
        // 检查类型是否为字符串类型
        if (left_value.attr_type() != AttrType::CHARS || right_value.attr_type() != AttrType::CHARS) {
          LOG_WARN("LIKE operation requires string types, got left: %d, right: %d", 
                   left_value.attr_type(), right_value.attr_type());
          select[i] = 0;
          continue;
        }
        
        std::string text = left_value.get_string();
        std::string pattern = right_value.get_string();
        
        // 检查空字符串
        if (text.empty() || pattern.empty()) {
          select[i] = 0;
          continue;
        }
        
        bool match_result = match_like_pattern(text.c_str(), pattern.c_str());
        if (comp_ == LIKE_OP) {
          select[i] = match_result ? 1 : 0;
        } else { // NOT_LIKE_OP
          select[i] = match_result ? 0 : 1;
        }
      }
    } else {
      // 对于其他字符串比较操作，也需要逐行处理
      select.clear();
      select.resize(chunk.rows(), 0);
      
      for (int i = 0; i < chunk.rows(); i++) {
        Value left_value = left_column.get_value(i);
        Value right_value = right_column.get_value(i);
        
        // 检查类型是否匹配
        if (left_value.attr_type() != right_value.attr_type()) {
          LOG_WARN("Cannot compare values with different types: left=%d, right=%d", 
                   left_value.attr_type(), right_value.attr_type());
          select[i] = 0;
          continue;
        }
        
        bool result = false;
        rc = compare_value(left_value, right_value, result);
        if (rc != RC::SUCCESS) {
          LOG_WARN("Failed to compare values at row %d: %s", i, strrc(rc));
          select[i] = 0;
          continue;
        }
        select[i] = result ? 1 : 0;
      }
    }
  } else {
    LOG_WARN("unsupported data type %d", left_column.attr_type());
    return RC::INTERNAL;
  }
  return rc;
}

RC ComparisonExpr::execute_subquery(vector<Value> &results) const
{
  if (!has_subquery_ || subquery_ == nullptr) {
    LOG_WARN("No subquery to execute");
    return RC::INVALID_ARGUMENT;
  }
  
  // 🔧 修复：移除 mutable 缓存检查，避免跨 Tuple 的状态污染
  // 每次都重新执行子查询，确保数据正确性
  // SubqueryExecutor 内部不再使用 static，避免了缓存污染
  
  LOG_DEBUG("Executing subquery (always fresh execution)");
  
  const SelectSqlNode *select_node = subquery_.get();
  LOG_DEBUG("Executing subquery with %zu relations, %zu expressions", 
           select_node->relations.size(), select_node->expressions.size());
  
  // 检查是否包含聚合函数（包括未绑定和已绑定的）
  bool has_aggregate = false;
  for (const auto &expr : select_node->expressions) {
    if (expr && (expr->type() == ExprType::UNBOUND_AGGREGATION || expr->type() == ExprType::AGGREGATION)) {
      has_aggregate = true;
      LOG_DEBUG("Detected aggregate expression in subquery (type=%d)", static_cast<int>(expr->type()));
      break;
    }
  }
  
  // 检查是否是简单的单表查询（不包含聚合函数、条件或JOIN）
  if (!has_aggregate && 
      select_node->relations.size() == 1 && 
      select_node->conditions.empty() && 
      select_node->joins.empty()) {
    // 简单单表查询，尝试直接执行
    RC rc = execute_simple_subquery(select_node, results);
    if (rc == RC::SUCCESS) {
      LOG_DEBUG("Simple subquery executed successfully, returned %zu values", results.size());
      return RC::SUCCESS;
    } else {
      LOG_WARN("Simple subquery execution failed, will try complex subquery execution");
    }
  }
  
  // 使用完整的SubqueryExecutor执行（支持聚合函数、JOIN、WHERE等复杂查询）
  LOG_DEBUG("Using SubqueryExecutor for complex subquery execution");
  // 🔧 修复：移除static避免缓存污染
  SubqueryExecutor executor;
  RC rc = executor.execute_subquery(select_node, session_, results);
  
  if (rc == RC::SUCCESS) {
    LOG_DEBUG("Complex subquery executed successfully, returned %zu values", results.size());
    return RC::SUCCESS;
  } else {
    LOG_ERROR("SubqueryExecutor execution failed, rc=%d", rc);
    return rc;
  }
}

RC ComparisonExpr::execute_simple_subquery(const SelectSqlNode *select_node, vector<Value> &results) const
{
  // 实现真正的子查询执行逻辑
  LOG_DEBUG("Attempting to execute simple subquery for table: %s", 
           select_node->relations[0].c_str());
  
  // 检查是否有Session上下文
  if (session_ == nullptr) {
    LOG_WARN("No session context available for subquery execution");
    return RC::INVALID_ARGUMENT;
  }
  
  // 使用SubqueryExecutor执行子查询
  // 🔧 修复：移除static避免缓存污染
  SubqueryExecutor executor;
  RC rc = executor.execute_subquery(select_node, session_, results);
  
  if (rc == RC::SUCCESS) {
    LOG_DEBUG("Subquery executed successfully using SubqueryExecutor, returned %zu values", results.size());
  } else {
    LOG_WARN("Subquery execution failed with SubqueryExecutor, rc=%d", rc);
  }
  
  return rc;
}

// 🔧 修复：移除缓存清理方法，已不再需要
// void ComparisonExpr::clear_subquery_cache() const { ... }

void ComparisonExpr::set_session_context(class Session *session)
{
  session_ = session;
}

void ComparisonExpr::set_session_context_recursive(class Session *session)
{
  // 设置当前表达式的session上下文
  set_session_context(session);
  
  // 递归设置子表达式的session上下文
  if (left_) {
    left_->set_session_context_recursive(session);
  }
  if (right_) {
    right_->set_session_context_recursive(session);
  }
}

// 🔧 修复：移除递归缓存清理方法，已不再需要
// void ComparisonExpr::clear_subquery_cache_recursive() { ... }

template <typename T>
RC ComparisonExpr::compare_column(const Column &left, const Column &right, vector<uint8_t> &result) const
{
  RC rc = RC::SUCCESS;

  bool left_const  = left.column_type() == Column::Type::CONSTANT_COLUMN;
  bool right_const = right.column_type() == Column::Type::CONSTANT_COLUMN;
  if (left_const && right_const) {
    compare_result<T, true, true>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  } else if (left_const && !right_const) {
    compare_result<T, true, false>((T *)left.data(), (T *)right.data(), right.count(), result, comp_);
  } else if (!left_const && right_const) {
    compare_result<T, false, true>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  } else {
    compare_result<T, false, false>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
ConjunctionExpr::ConjunctionExpr(Type type, vector<unique_ptr<Expression>> &children)
    : conjunction_type_(type), children_(std::move(children))
{}

RC ConjunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    value.set_boolean(true);
    return rc;
  }

  Value tmp_value;
  for (const unique_ptr<Expression> &expr : children_) {
    rc = expr->get_value(tuple, tmp_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value by child expression. rc=%s", strrc(rc));
      return rc;
    }
    bool bool_value = tmp_value.get_boolean();
    if ((conjunction_type_ == Type::AND && !bool_value) || (conjunction_type_ == Type::OR && bool_value)) {
      value.set_boolean(bool_value);
      return rc;
    }
  }

  bool default_value = (conjunction_type_ == Type::AND);
  value.set_boolean(default_value);
  return rc;
}

void ConjunctionExpr::set_session_context_recursive(class Session *session)
{
  // 设置当前表达式的session上下文
  set_session_context(session);
  
  // 递归设置子表达式的session上下文
  for (auto &child : children_) {
    if (child) {
      child->set_session_context_recursive(session);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, Expression *left, Expression *right)
    : arithmetic_type_(type), left_(left), right_(right)
{}
ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : arithmetic_type_(type), left_(std::move(left)), right_(std::move(right))
{}

bool ArithmeticExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (type() != other.type()) {
    return false;
  }
  auto &other_arith_expr = static_cast<const ArithmeticExpr &>(other);
  return arithmetic_type_ == other_arith_expr.arithmetic_type() && left_->equal(*other_arith_expr.left_) &&
         right_->equal(*other_arith_expr.right_);
}
AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  // 如果任一操作数是向量类型，结果也是向量类型
  if (left_->value_type() == AttrType::VECTORS || right_->value_type() == AttrType::VECTORS) {
    return AttrType::VECTORS;
  }

  if (left_->value_type() == AttrType::INTS && right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }

  return AttrType::FLOATS;
}

RC ArithmeticExpr::calc_value(const Value &left_value, const Value &right_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  // NULL值传播：任何NULL参与的运算都返回NULL
  if (left_value.is_null()) {
    value.set_null();
    return RC::SUCCESS;
  }

  // 对于二元运算，检查右操作数
  if (arithmetic_type_ != Type::NEGATIVE && right_value.is_null()) {
    value.set_null();
    return RC::SUCCESS;
  }

  const AttrType target_type = value_type();
  value.set_type(target_type);

  // 如果目标类型是向量，需要确保操作数也都是向量类型
  Value left_converted = left_value;
  Value right_converted = right_value;
  
  if (target_type == AttrType::VECTORS) {
    // 转换左操作数
    if (left_value.attr_type() != AttrType::VECTORS) {
      rc = DataType::type_instance(AttrType::VECTORS)->cast_to(left_value, AttrType::VECTORS, left_converted);
      if (rc != RC::SUCCESS) {
        LOG_WARN("Failed to convert left operand to vector type");
        return rc;
      }
    }
    
    // 转换右操作数
    if (right_value.attr_type() != AttrType::VECTORS) {
      rc = DataType::type_instance(AttrType::VECTORS)->cast_to(right_value, AttrType::VECTORS, right_converted);
      if (rc != RC::SUCCESS) {
        LOG_WARN("Failed to convert right operand to vector type");
        return rc;
      }
    }
  }

  switch (arithmetic_type_) {
    case Type::ADD: {
      Value::add(left_converted, right_converted, value);
    } break;

    case Type::SUB: {
      Value::subtract(left_converted, right_converted, value);
    } break;

    case Type::MUL: {
      Value::multiply(left_converted, right_converted, value);
    } break;

    case Type::DIV: {
      Value::divide(left_value, right_value, value);
    } break;

    case Type::NEGATIVE: {
      Value::negative(left_value, value);
    } break;

    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported arithmetic type. %d", arithmetic_type_);
    } break;
  }
  return rc;
}

template <bool LEFT_CONSTANT, bool RIGHT_CONSTANT>
RC ArithmeticExpr::execute_calc(
    const Column &left, const Column &right, Column &result, Type type, AttrType attr_type) const
{
  RC rc = RC::SUCCESS;
  switch (type) {
    case Type::ADD: {
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, AddOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, AddOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
    } break;
    case Type::SUB:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, SubtractOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, SubtractOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::MUL:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, MultiplyOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, MultiplyOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::DIV:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, DivideOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, DivideOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::NEGATIVE:
      if (attr_type == AttrType::INTS) {
        unary_operator<LEFT_CONSTANT, int, NegateOperator>((int *)left.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        unary_operator<LEFT_CONSTANT, float, NegateOperator>(
            (float *)left.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    default: rc = RC::UNIMPLEMENTED; break;
  }
  if (rc == RC::SUCCESS) {
    result.set_count(result.capacity());
  }
  return rc;
}

RC ArithmeticExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  LOG_INFO("ArithmeticExpr::get_value - op=%d, left_type=%d, right_type=%d", 
           static_cast<int>(arithmetic_type_),
           left_ ? static_cast<int>(left_->type()) : -1,
           right_ ? static_cast<int>(right_->type()) : -1);

  rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  
  LOG_INFO("ArithmeticExpr: left_value=%s(type=%d)", 
           left_value.to_string().c_str(), static_cast<int>(left_value.attr_type()));

  // 处理一元运算符（如负号）
  if (arithmetic_type_ == Type::NEGATIVE) {
    // 对于负号运算，right_是nullptr，直接对left_value取负
    return calc_value(left_value, Value(), value);
  }

  // 处理二元运算符
  if (right_ == nullptr) {
    LOG_WARN("right operand is null for binary arithmetic operation");
    return RC::INVALID_ARGUMENT;
  }

  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  
  LOG_INFO("ArithmeticExpr: right_value=%s(type=%d), will calc", 
           right_value.to_string().c_str(), static_cast<int>(right_value.attr_type()));
  
  rc = calc_value(left_value, right_value, value);
  
  LOG_INFO("ArithmeticExpr: result=%s(type=%d)", 
           value.to_string().c_str(), static_cast<int>(value.attr_type()));
  
  return rc;
}

RC ArithmeticExpr::get_column(Chunk &chunk, Column &column)
{
  RC rc = RC::SUCCESS;
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
    return rc;
  }
  Column left_column;
  Column right_column;

  rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get column of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get column of right expression. rc=%s", strrc(rc));
    return rc;
  }
  return calc_column(left_column, right_column, column);
}

RC ArithmeticExpr::calc_column(const Column &left_column, const Column &right_column, Column &column) const
{
  RC rc = RC::SUCCESS;

  const AttrType target_type = value_type();
  column.init(target_type, left_column.attr_len(), max(left_column.count(), right_column.count()));
  bool left_const  = left_column.column_type() == Column::Type::CONSTANT_COLUMN;
  bool right_const = right_column.column_type() == Column::Type::CONSTANT_COLUMN;
  if (left_const && right_const) {
    column.set_column_type(Column::Type::CONSTANT_COLUMN);
    rc = execute_calc<true, true>(left_column, right_column, column, arithmetic_type_, target_type);
  } else if (left_const && !right_const) {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<true, false>(left_column, right_column, column, arithmetic_type_, target_type);
  } else if (!left_const && right_const) {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<false, true>(left_column, right_column, column, arithmetic_type_, target_type);
  } else {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<false, false>(left_column, right_column, column, arithmetic_type_, target_type);
  }
  return rc;
}

RC ArithmeticExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

// 减少冗余日志输出 - 只在需要时输出
#ifdef DEBUG_EXPRESSION_EVAL
  LOG_INFO("ARITHMETIC try_get_value: type=%d", (int)arithmetic_type_);
#endif

  rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    // try_get_value失败是正常的（表达式包含字段时），不需要警告
    return rc;
  }

  if (right_) {
    rc = right_->try_get_value(right_value);
    if (rc != RC::SUCCESS) {
      // try_get_value失败是正常的（表达式包含字段时），不需要警告
      return rc;
    }
  }

#ifdef DEBUG_EXPRESSION_EVAL
  LOG_INFO("ARITHMETIC try_get_value calling calc_value: left=%s, right=%s, op=%d",
           left_value.to_string().c_str(), 
           right_value.to_string().c_str(), 
           (int)arithmetic_type_);
#endif

  return calc_value(left_value, right_value, value);
}

void ArithmeticExpr::set_session_context_recursive(class Session *session)
{
  // 设置当前表达式的session上下文
  set_session_context(session);
  
  // 递归设置子表达式的session上下文
  if (left_) {
    left_->set_session_context_recursive(session);
  }
  if (right_) {
    right_->set_session_context_recursive(session);
  }
}

////////////////////////////////////////////////////////////////////////////////

UnboundAggregateExpr::UnboundAggregateExpr(const char *aggregate_name, Expression *child)
    : aggregate_name_(aggregate_name), child_(child)
{}

UnboundAggregateExpr::UnboundAggregateExpr(const char *aggregate_name, unique_ptr<Expression> child)
    : aggregate_name_(aggregate_name), child_(std::move(child))
{}

UnboundAggregateExpr::UnboundAggregateExpr(const char *aggregate_name, vector<unique_ptr<Expression>> children)
    : aggregate_name_(aggregate_name), children_(std::move(children))
{}

////////////////////////////////////////////////////////////////////////////////
AggregateExpr::AggregateExpr(Type type, Expression *child) : aggregate_type_(type), child_(child) {}

AggregateExpr::AggregateExpr(Type type, unique_ptr<Expression> child) : aggregate_type_(type), child_(std::move(child))
{}

RC AggregateExpr::get_column(Chunk &chunk, Column &column)
{
  RC rc = RC::SUCCESS;
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
  } else {
    rc = RC::INTERNAL;
  }
  return rc;
}

bool AggregateExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != type()) {
    return false;
  }
  const AggregateExpr &other_aggr_expr = static_cast<const AggregateExpr &>(other);
  return aggregate_type_ == other_aggr_expr.aggregate_type() && child_->equal(*other_aggr_expr.child());
}

unique_ptr<Aggregator> AggregateExpr::create_aggregator() const
{
  unique_ptr<Aggregator> aggregator;
  switch (aggregate_type_) {
    case Type::COUNT: {
      aggregator = make_unique<CountAggregator>();
      break;
    }
    case Type::SUM: {
      aggregator = make_unique<SumAggregator>();
      break;
    }
    case Type::AVG: {
      aggregator = make_unique<AvgAggregator>();
      break;
    }
    case Type::MAX: {
      aggregator = make_unique<MaxAggregator>();
      break;
    }
    case Type::MIN: {
      aggregator = make_unique<MinAggregator>();
      break;
    }
    default: {
      ASSERT(false, "unsupported aggregate type");
      break;
    }
  }
  return aggregator;
}

RC AggregateExpr::get_value(const Tuple &tuple, Value &value) const
{
  return tuple.find_cell(TupleCellSpec(name()), value);
}

RC AggregateExpr::type_from_string(const char *type_str, AggregateExpr::Type &type)
{
  RC rc = RC::SUCCESS;
  if (0 == strcasecmp(type_str, "count")) {
    type = Type::COUNT;
  } else if (0 == strcasecmp(type_str, "sum")) {
    type = Type::SUM;
  } else if (0 == strcasecmp(type_str, "avg")) {
    type = Type::AVG;
  } else if (0 == strcasecmp(type_str, "max")) {
    type = Type::MAX;
  } else if (0 == strcasecmp(type_str, "min")) {
    type = Type::MIN;
  } else {
    rc = RC::INVALID_ARGUMENT;
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
// SubqueryExpr

SubqueryExpr::SubqueryExpr(unique_ptr<SelectSqlNode> subquery) : subquery_(std::move(subquery)) {}

unique_ptr<Expression> SubqueryExpr::copy() const
{
  return make_unique<SubqueryExpr>(SelectSqlNode::create_copy(subquery_.get()));
}

AttrType SubqueryExpr::value_type() const
{
  // 🔧 修复：移除缓存逻辑，每次重新计算类型
  // 需要分析子查询的SELECT列表来确定类型
  // 对于标量子查询，应该只有一个SELECT表达式
  if (subquery_ && !subquery_->expressions.empty()) {
    const auto& first_expr = subquery_->expressions[0];
    
    // 处理聚合函数表达式
    if (first_expr->type() == ExprType::UNBOUND_AGGREGATION) {
      const UnboundAggregateExpr* agg_expr = static_cast<const UnboundAggregateExpr*>(first_expr.get());
      
      // 根据聚合函数类型确定返回类型
      string agg_name = agg_expr->aggregate_name();
      if (agg_name == "count") {
        return AttrType::INTS;
      } else if (agg_name == "avg") {
        return AttrType::FLOATS;
      } else if (agg_name == "sum") {
        return AttrType::FLOATS;  // 默认为FLOATS以支持更广泛的数值
      } else if (agg_name == "max" || agg_name == "min") {
        return AttrType::FLOATS;  // 暂时返回FLOATS作为通用数值类型
      } else {
        return AttrType::FLOATS;  // 默认数值类型
      }
    }
    // 如果是UnboundFieldExpr，需要绑定后才能确定类型
    else if (first_expr->type() == ExprType::UNBOUND_FIELD) {
      const UnboundFieldExpr* field_expr = static_cast<const UnboundFieldExpr*>(first_expr.get());
      
      // 如果有session上下文，尝试获取实际类型
      if (session_ != nullptr) {
        Db *db = session_->get_current_db();
        if (db != nullptr && !subquery_->relations.empty()) {
          Table *table = db->find_table(subquery_->relations[0].c_str());
          if (table != nullptr) {
            const TableMeta& table_meta = table->table_meta();
            const FieldMeta* field_meta = table_meta.field(field_expr->field_name());
            if (field_meta != nullptr) {
              return field_meta->type();
            }
          }
        }
      }
      // 如果无法确定具体类型，返回通用数值类型
      return AttrType::FLOATS;
    } else {
      // 对于其他类型的表达式，直接获取其类型
      AttrType expr_type = first_expr->value_type();
      if (expr_type != AttrType::UNDEFINED) {
        return expr_type;
      }
    }
  }
  
  // 如果无法确定类型，返回通用数值类型而不是UNDEFINED
  return AttrType::FLOATS;
}

int SubqueryExpr::value_length() const
{
  // 暂时返回固定长度，实际应该根据子查询结果确定
  return 4;
}

RC SubqueryExpr::get_value(const Tuple &tuple, Value &value) const
{
  if (subquery_ == nullptr) {
    LOG_WARN("Subquery is null");
    return RC::INVALID_ARGUMENT;
  }
  
  if (session_ == nullptr) {
    LOG_WARN("No session context available for subquery execution");
    return RC::INVALID_ARGUMENT;
  }
  
  // 执行子查询
  std::vector<Value> results;
  // 🔧 修复：移除static避免缓存污染
  SubqueryExecutor executor;
  RC rc = executor.execute_subquery(subquery_.get(), session_, results);
  
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to execute subquery, rc=%d", rc);
    return rc;
  }
  
  // 标量子查询应该返回单个值
  if (results.empty()) {
    // 空结果集，返回NULL值
    value.set_null();
    value.set_type(value_type());
    LOG_DEBUG("Subquery returned empty result set, returning NULL");
    return RC::SUCCESS;
  }
  
  if (results.size() > 1) {
    // ✅ SQL标准：标量子查询返回多行是错误
    LOG_WARN("Scalar subquery returned more than one row (%zu rows)", results.size());
    LOG_WARN("SQL Hint: Add LIMIT 1 or use IN/EXISTS instead");
    return RC::INVALID_ARGUMENT;  // 返回错误而不是继续执行
  }
  
  value = results[0];
  LOG_DEBUG("Subquery returned value: %s", value.to_string().c_str());
  
  return RC::SUCCESS;
}

void SubqueryExpr::set_session_context_recursive(class Session *session)
{
  session_ = session;
  // 🔧 修复：移除缓存清理逻辑
}

////////////////////////////////////////////////////////////////////////////////
// InExpr

// IN (subquery) 构造函数 - 保留原有功能
InExpr::InExpr(bool is_not, std::unique_ptr<Expression> left, std::unique_ptr<Expression> subquery)
  : is_not_(is_not), left_(std::move(left)), subquery_(std::move(subquery)), is_subquery_(true)
{}

// IN (value_list) 构造函数 - 新增功能
InExpr::InExpr(bool is_not, std::unique_ptr<Expression> left, std::vector<std::unique_ptr<Expression>> &&value_list)
  : is_not_(is_not), left_(std::move(left)), value_list_(std::move(value_list)), is_subquery_(false)
{}

std::unique_ptr<Expression> InExpr::copy() const
{
  if (is_subquery_) {
    // 子查询形式
    return std::make_unique<InExpr>(is_not_, left_->copy(), subquery_->copy());
  } else {
    // 值列表形式
    std::vector<std::unique_ptr<Expression>> copied_list;
    copied_list.reserve(value_list_.size());
    for (const auto &expr : value_list_) {
      copied_list.push_back(expr->copy());
    }
    return std::make_unique<InExpr>(is_not_, left_->copy(), std::move(copied_list));
  }
}

RC InExpr::get_value(const Tuple &tuple, Value &value) const
{
  // 计算左侧表达式的值
  Value left_value;
  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  bool found = false;
  bool has_null = false;

  if (is_subquery_) {
    // 子查询形式 - 保留原有逻辑
    if (!session_) {
      LOG_WARN("Session not set for IN subquery");
      return RC::INTERNAL;
    }

    // 执行子查询获取所有结果
    std::vector<Value> results;
    SubqueryExpr *subquery_expr = dynamic_cast<SubqueryExpr*>(subquery_.get());
    if (!subquery_expr || !subquery_expr->subquery()) {
      LOG_WARN("Invalid subquery in IN expression");
      return RC::INTERNAL;
    }
    // 🔧 修复：移除static避免缓存污染，每次创建新实例
    SubqueryExecutor executor;
    rc = executor.execute_subquery(subquery_expr->subquery(), session_, results);
    if (rc != RC::SUCCESS) {
      LOG_WARN("Failed to execute subquery in IN expression");
      return rc;
    }

    // 检查左值是否在结果集中
    for (const Value &result : results) {
      if (result.is_null()) {
        has_null = true;
        continue;
      }
      
      if (left_value.is_null()) {
        has_null = true;
        break;
      }

      int cmp = left_value.compare(result);
      if (cmp == 0) {
        found = true;
        break;
      }
    }
  } else {
    // 值列表形式 - 新增逻辑
    for (const auto &expr : value_list_) {
      Value list_value;
      rc = expr->get_value(tuple, list_value);
      if (rc != RC::SUCCESS) {
        return rc;
      }

      if (list_value.is_null()) {
        has_null = true;
        continue;
      }
      
      if (left_value.is_null()) {
        has_null = true;
        break;
      }

      int cmp = left_value.compare(list_value);
      if (cmp == 0) {
        found = true;
        break;
      }
    }
  }

  // 处理NOT IN遇到NULL的特殊情况
  // NOT IN: 如果找到了值，返回false；如果没找到但有NULL，返回NULL；否则返回true
  // IN: 如果找到了值，返回true；如果没找到但有NULL，返回NULL；否则返回false
  if (is_not_) {
    if (found) {
      value.set_boolean(false);
    } else if (has_null) {
      value.set_null();
      value.set_type(AttrType::BOOLEANS);
    } else {
      value.set_boolean(true);
    }
  } else {
    if (found) {
      value.set_boolean(true);
    } else if (has_null) {
      value.set_null();
      value.set_type(AttrType::BOOLEANS);
    } else {
      value.set_boolean(false);
    }
  }

  return RC::SUCCESS;
}

void InExpr::set_session_context_recursive(Session *session)
{
  session_ = session;
  if (left_) {
    left_->set_session_context_recursive(session);
  }
  if (is_subquery_ && subquery_) {
    subquery_->set_session_context_recursive(session);
  }
  if (!is_subquery_) {
    for (auto &expr : value_list_) {
      if (expr) {
        expr->set_session_context_recursive(session);
      }
    }
  }
}

std::unordered_set<std::string> InExpr::get_involved_tables() const
{
  std::unordered_set<std::string> tables;
  if (left_) {
    auto left_tables = left_->get_involved_tables();
    tables.insert(left_tables.begin(), left_tables.end());
  }
  if (is_subquery_ && subquery_) {
    auto subquery_tables = subquery_->get_involved_tables();
    tables.insert(subquery_tables.begin(), subquery_tables.end());
  }
  if (!is_subquery_) {
    for (const auto &expr : value_list_) {
      if (expr) {
        auto expr_tables = expr->get_involved_tables();
        tables.insert(expr_tables.begin(), expr_tables.end());
      }
    }
  }
  return tables;
}

RC InExpr::bind_fields(const std::vector<Table *> &tables)
{
  // 绑定left_表达式中的UnboundFieldExpr
  if (left_) {
    RC rc = bind_unbound_field_expr(left_, tables);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  
  // subquery不需要在这里绑定，它会在执行时处理
  
  // 值列表需要绑定其中的字段
  if (!is_subquery_) {
    for (auto &expr : value_list_) {
      if (expr) {
        RC rc = bind_unbound_field_expr(expr, tables);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
    }
  }
  
  return RC::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// ExistsExpr

ExistsExpr::ExistsExpr(bool is_not, std::unique_ptr<Expression> subquery)
  : is_not_(is_not), subquery_(std::move(subquery))
{}

std::unique_ptr<Expression> ExistsExpr::copy() const
{
  return std::make_unique<ExistsExpr>(is_not_, subquery_->copy());
}

RC ExistsExpr::get_value(const Tuple &tuple, Value &value) const
{
  if (!session_) {
    LOG_WARN("Session not set for EXISTS expression");
    return RC::INTERNAL;
  }

  // 执行子查询，检查是否有结果
  std::vector<Value> results;
  SubqueryExpr *subquery_expr = dynamic_cast<SubqueryExpr*>(subquery_.get());
  if (!subquery_expr || !subquery_expr->subquery()) {
    LOG_WARN("Invalid subquery in EXISTS expression");
    return RC::INTERNAL;
  }
  // 🔧 修复：移除static避免缓存污染
  SubqueryExecutor executor;
  // EXISTS不需要检查列数，传false
  RC rc = executor.execute_subquery(subquery_expr->subquery(), session_, results, false);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to execute subquery in EXISTS expression");
    return rc;
  }

  // EXISTS: 如果有结果返回true，否则返回false
  // NOT EXISTS: 如果有结果返回false，否则返回true
  bool exists = !results.empty();
  if (is_not_) {
    value.set_boolean(!exists);
  } else {
    value.set_boolean(exists);
  }

  return RC::SUCCESS;
}

void ExistsExpr::set_session_context_recursive(Session *session)
{
  session_ = session;
  if (subquery_) {
    subquery_->set_session_context_recursive(session);
  }
}

std::unordered_set<std::string> ExistsExpr::get_involved_tables() const
{
  if (subquery_) {
    return subquery_->get_involved_tables();
  }
  return {};
}

////////////////////////////////////////////////////////////////////////////////
// DistanceFunctionExpr

DistanceFunctionExpr::DistanceFunctionExpr(Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : distance_type_(type), left_(std::move(left)), right_(std::move(right))
{}

RC DistanceFunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;

  LOG_INFO("DistanceFunctionExpr::get_value: getting left operand (should be embedding field)");
  
  // 检查left_是什么类型的表达式
  if (left_->type() == ExprType::FIELD) {
    const FieldExpr* field_expr = static_cast<const FieldExpr*>(left_.get());
    LOG_INFO("DistanceFunctionExpr: left_ is FieldExpr, table='%s', field='%s'",
             field_expr->table_name() ? field_expr->table_name() : "NULL",
             field_expr->field_name() ? field_expr->field_name() : "NULL");
  } else {
    LOG_INFO("DistanceFunctionExpr: left_ is not FieldExpr, type=%d", static_cast<int>(left_->type()));
  }

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get left value. rc=%s", strrc(rc));
    return rc;
  }
  
  LOG_INFO("DistanceFunctionExpr: left value obtained successfully, type=%d", 
           static_cast<int>(left_value.attr_type()));

  LOG_INFO("DistanceFunctionExpr::get_value: getting right operand");
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get right value. rc=%s", strrc(rc));
    return rc;
  }
  
  LOG_INFO("DistanceFunctionExpr: right value obtained successfully, type=%d", 
           static_cast<int>(right_value.attr_type()));

  return calculate_distance(left_value, right_value, value);
}

RC DistanceFunctionExpr::get_column(Chunk &chunk, Column &column)
{
  // 获取左右操作数的列数据
  Column left_column;
  Column right_column;
  
  RC rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  
  // 计算距离结果
  int row_num = chunk.rows();
  column.init(AttrType::FLOATS, sizeof(float), row_num);
  
  for (int i = 0; i < row_num; i++) {
    Value left_value = left_column.get_value(i);
    Value right_value = right_column.get_value(i);
    Value result;
    
    rc = calculate_distance(left_value, right_value, result);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    
    float distance_val = result.get_float();
    rc = column.append_one(reinterpret_cast<char*>(&distance_val));
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  
  return RC::SUCCESS;
}

bool DistanceFunctionExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != type()) {
    return false;
  }
  
  const DistanceFunctionExpr &other_distance_expr = static_cast<const DistanceFunctionExpr &>(other);
  return distance_type_ == other_distance_expr.distance_type_ &&
         left_->equal(*other_distance_expr.left_) &&
         right_->equal(*other_distance_expr.right_);
}

unique_ptr<Expression> DistanceFunctionExpr::copy() const
{
  return make_unique<DistanceFunctionExpr>(distance_type_, left_->copy(), right_->copy());
}

RC DistanceFunctionExpr::calculate_distance(const Value &left_val, const Value &right_val, Value &result) const
{
  // 将操作数转换为向量类型
  Value left_vector = left_val;
  Value right_vector = right_val;
  
  // 转换左操作数
  if (left_val.attr_type() != AttrType::VECTORS) {
    RC rc = DataType::type_instance(AttrType::VECTORS)->cast_to(left_val, AttrType::VECTORS, left_vector);
    if (rc != RC::SUCCESS) {
      LOG_WARN("Failed to convert left operand to vector type: %d", static_cast<int>(left_val.attr_type()));
      return rc;
    }
  }
  
  // 转换右操作数
  if (right_val.attr_type() != AttrType::VECTORS) {
    RC rc = DataType::type_instance(AttrType::VECTORS)->cast_to(right_val, AttrType::VECTORS, right_vector);
    if (rc != RC::SUCCESS) {
      LOG_WARN("Failed to convert right operand to vector type: %d", static_cast<int>(right_val.attr_type()));
      return rc;
    }
  }
  
  const vector<float> &left_vec = left_vector.get_vector();
  const vector<float> &right_vec = right_vector.get_vector();
  
  // 检查维度是否相同
  if (left_vec.size() != right_vec.size()) {
    LOG_WARN("Vector dimensions mismatch for distance calculation: %zu vs %zu", 
             left_vec.size(), right_vec.size());
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
  
  float distance = 0.0f;
  
  switch (distance_type_) {
    case Type::L2_DISTANCE: {
      // 欧几里得距离：sqrt(sum((a_i - b_i)^2))
      float sum_squared = 0.0f;
      for (size_t i = 0; i < left_vec.size(); i++) {
        float diff = left_vec[i] - right_vec[i];
        sum_squared += diff * diff;
      }
      distance = sqrt(sum_squared);
    } break;
    
    case Type::COSINE_DISTANCE: {
      // 余弦距离：1 - (a·b)/(|a||b|)
      float dot_product = 0.0f;
      float left_magnitude = 0.0f;
      float right_magnitude = 0.0f;
      
      for (size_t i = 0; i < left_vec.size(); i++) {
        dot_product += left_vec[i] * right_vec[i];
        left_magnitude += left_vec[i] * left_vec[i];
        right_magnitude += right_vec[i] * right_vec[i];
      }
      
      left_magnitude = sqrt(left_magnitude);
      right_magnitude = sqrt(right_magnitude);
      
      if (left_magnitude == 0.0f || right_magnitude == 0.0f) {
        distance = 1.0f;  // 如果有零向量，余弦距离为1
      } else {
        float cosine_similarity = dot_product / (left_magnitude * right_magnitude);
        distance = 1.0f - cosine_similarity;
      }
    } break;
    
    case Type::INNER_PRODUCT: {
      // 内积：sum(a_i * b_i)
      for (size_t i = 0; i < left_vec.size(); i++) {
        distance += left_vec[i] * right_vec[i];
      }
    } break;
    
    default: {
      LOG_WARN("Unknown distance function type: %d", static_cast<int>(distance_type_));
      return RC::INTERNAL;
    }
  }
  
  result.set_float(distance);
  return RC::SUCCESS;
}
