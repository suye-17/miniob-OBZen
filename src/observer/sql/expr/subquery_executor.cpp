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
// Created by Assistant on 2025/01/26.
//

#include "sql/expr/subquery_executor.h"
#include "sql/operator/table_scan_physical_operator.h"
#include "session/session.h"
#include "storage/db/db.h"
#include "common/log/log.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include <sstream>

using namespace common;

SubqueryExecutor::SubqueryExecutor() 
    : cache_limit_(1000), cache_enabled_(true), cache_hits_(0), cache_misses_(0), total_executions_(0)
{
}

SubqueryExecutor::~SubqueryExecutor()
{
  LOG_INFO("SubqueryExecutor destroyed. Cache hits: %zu, misses: %zu, total: %zu", 
           cache_hits_, cache_misses_, total_executions_);
}

RC SubqueryExecutor::execute_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results)
{
  if (select_node == nullptr || session == nullptr) {
    LOG_WARN("Invalid arguments: select_node=%p, session=%p", select_node, session);
    return RC::INVALID_ARGUMENT;
  }

  total_executions_++;

  // 检查缓存
  if (cache_enabled_) {
    std::string cache_key = generate_cache_key(select_node);
    if (get_from_cache(cache_key, results)) {
      cache_hits_++;
      LOG_DEBUG("Subquery cache hit, returned %zu values", results.size());
      return RC::SUCCESS;
    }
    cache_misses_++;
  }

  // 执行子查询
  RC rc = RC::SUCCESS;
  
  // 检查是否是简单的单表查询
  if (select_node->relations.size() == 1 && 
      select_node->conditions.empty() && 
      select_node->joins.empty()) {
    rc = execute_simple_subquery(select_node, session, results);
  } else {
    // 对于复杂子查询，尝试使用完整的查询执行引擎
    LOG_INFO("Executing complex subquery with full query engine");
    rc = execute_complex_subquery(select_node, session, results);
    
    // 如果复杂子查询失败，回退到简单实现
    if (rc != RC::SUCCESS) {
      LOG_WARN("Complex subquery execution failed, falling back to simple implementation");
      rc = execute_simple_subquery(select_node, session, results);
    }
  }

  // 如果执行成功，缓存结果
  if (rc == RC::SUCCESS && cache_enabled_) {
    std::string cache_key = generate_cache_key(select_node);
    put_to_cache(cache_key, results);
  }

  return rc;
}

RC SubqueryExecutor::execute_simple_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results)
{
  LOG_DEBUG("Executing simple subquery for table: %s", select_node->relations[0].c_str());

  // 获取数据库
  Db *db = session->get_current_db();
  if (db == nullptr) {
    LOG_WARN("No current database in session");
    return RC::SCHEMA_DB_NOT_EXIST;
  }

  // 获取表
  Table *table = db->find_table(select_node->relations[0].c_str());
  if (table == nullptr) {
    LOG_WARN("Table %s not found", select_node->relations[0].c_str());
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  LOG_DEBUG("Found table: %s", table->name());

  // 创建表扫描操作符
  TableScanPhysicalOperator scan_op(table, ReadWriteMode::READ_ONLY);

  // 打开扫描器
  RC rc = scan_op.open(nullptr);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to open table scan operator: %d", rc);
    return rc;
  }

  LOG_DEBUG("Table scan operator opened successfully");

  // 扫描数据
  int row_count = 0;
  LOG_DEBUG("Starting table scan for subquery execution");
  
  while (RC::SUCCESS == (rc = scan_op.next())) {
    Tuple *tuple = scan_op.current_tuple();
    if (tuple == nullptr) {
      LOG_DEBUG("Got null tuple, continuing...");
      continue;
    }

    row_count++;
    LOG_DEBUG("Processing row %d", row_count);

    // 处理查询表达式
    for (size_t i = 0; i < select_node->expressions.size(); i++) {
      const auto &expr = select_node->expressions[i];
      LOG_DEBUG("Processing expression %zu of type %d", i, static_cast<int>(expr->type()));
      
      Value value;
      RC expr_rc = RC::SUCCESS;
      
      // 处理不同类型的表达式
      if (expr->type() == ExprType::UNBOUND_FIELD) {
        // 处理未绑定的字段表达式
        const UnboundFieldExpr* field_expr = static_cast<const UnboundFieldExpr*>(expr.get());
        const char* field_name = field_expr->field_name();
        const char* table_name = field_expr->table_name();
        
        LOG_DEBUG("Processing unbound field: %s.%s", table_name ? table_name : "NULL", field_name);
        
        // 尝试从tuple中获取字段值
        Value field_value;
        RC field_rc = tuple->find_cell(TupleCellSpec(field_name), field_value);
        if (field_rc == RC::SUCCESS) {
          value = field_value;
          expr_rc = RC::SUCCESS;
          LOG_DEBUG("Found field %s: %s", field_name, value.to_string().c_str());
        } else {
          // 如果通过字段名找不到，尝试通过索引获取
          LOG_DEBUG("Field %s not found by name, trying by index", field_name);
          
          // 获取表的字段信息
          const TableMeta& table_meta = table->table_meta();
          const FieldMeta* field_meta = table_meta.field(field_name);
          if (field_meta != nullptr) {
            field_rc = tuple->find_cell(TupleCellSpec(table_name, field_name), field_value);
            if (field_rc == RC::SUCCESS) {
              value = field_value;
              expr_rc = RC::SUCCESS;
              LOG_DEBUG("Found field %s by full name: %s", field_name, value.to_string().c_str());
            } else {
              // 最后尝试通过字段ID获取
              int field_id = field_meta->field_id();
              field_rc = tuple->cell_at(field_id, field_value);
              if (field_rc == RC::SUCCESS) {
                value = field_value;
                expr_rc = RC::SUCCESS;
                LOG_DEBUG("Found field %s by ID %d: %s", field_name, field_id, value.to_string().c_str());
              } else {
                LOG_WARN("Failed to get field %s by ID %d, rc=%d", field_name, field_id, field_rc);
              }
            }
          } else {
            LOG_WARN("Field %s not found in table %s", field_name, table->name());
          }
        }
      } else {
        // 处理其他类型的表达式
        expr_rc = expr->get_value(*tuple, value);
      }
      
      if (expr_rc == RC::SUCCESS) {
        results.push_back(value);
        LOG_DEBUG("Subquery result: %s", value.to_string().c_str());
      } else {
        LOG_WARN("Failed to get value from expression %zu, rc=%d", i, expr_rc);
        LOG_WARN("Expression type: %d, name: %s", static_cast<int>(expr->type()), expr->name());
      }
    }
  }

  scan_op.close();

  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;  // 正常结束
    LOG_DEBUG("Reached end of table, processed %d rows", row_count);
  } else {
    LOG_WARN("Table scan ended with error: %d", rc);
  }

  LOG_DEBUG("Simple subquery executed successfully, returned %zu values", results.size());
  
  // 如果没有返回任何值，记录详细信息用于调试
  if (results.empty()) {
    LOG_WARN("Subquery returned no values - this may indicate:");
    LOG_WARN("1. Table %s has no data", select_node->relations[0].c_str());
    LOG_WARN("2. Expression evaluation failed");
    LOG_WARN("3. Table scan operator failed");
    LOG_WARN("Processed %d rows but got 0 values", row_count);
  }
  
  return rc;
}

RC SubqueryExecutor::execute_complex_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results)
{
  LOG_INFO("Executing complex subquery with %zu relations, %zu conditions, %zu joins", 
           select_node->relations.size(), select_node->conditions.size(), select_node->joins.size());

  // 获取数据库
  Db *db = session->get_current_db();
  if (db == nullptr) {
    LOG_WARN("No current database in session");
    return RC::SCHEMA_DB_NOT_EXIST;
  }

  // 创建表映射
  std::unordered_map<std::string, Table*> table_map;
  for (const auto &relation : select_node->relations) {
    Table *table = db->find_table(relation.c_str());
    if (table == nullptr) {
      LOG_WARN("Table %s not found", relation.c_str());
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    table_map[relation] = table;
  }

  // 处理JOIN表
  for (const auto &join : select_node->joins) {
    Table *table = db->find_table(join.relation.c_str());
    if (table == nullptr) {
      LOG_WARN("JOIN table %s not found", join.relation.c_str());
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    table_map[join.relation] = table;
  }

  // 创建表扫描操作符（暂时只处理第一个表）
  Table *main_table = table_map[select_node->relations[0]];
  TableScanPhysicalOperator scan_op(main_table, ReadWriteMode::READ_ONLY);

  // 打开扫描器
  RC rc = scan_op.open(nullptr);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to open table scan operator: %d", rc);
    return rc;
  }

  LOG_DEBUG("Complex subquery scan operator opened successfully");

  // 扫描数据
  int row_count = 0;
  LOG_DEBUG("Starting complex subquery execution");
  
  while (RC::SUCCESS == (rc = scan_op.next())) {
    Tuple *tuple = scan_op.current_tuple();
    if (tuple == nullptr) {
      LOG_DEBUG("Got null tuple, continuing...");
      continue;
    }

    row_count++;
    LOG_DEBUG("Processing row %d in complex subquery", row_count);

    // 检查WHERE条件
    bool condition_passed = true;
    for (const auto &condition : select_node->conditions) {
      // 实现条件检查逻辑
      Value left_value;
      Value right_value;
      
      // 获取左侧值
      if (condition.left_is_attr) {
        // 左边是属性
        const char* field_name = condition.left_attr.attribute_name.c_str();
        const char* table_name = condition.left_attr.relation_name.empty() ? main_table->name() : condition.left_attr.relation_name.c_str();
        
        // 先尝试通过字段名直接获取
        RC field_rc = tuple->find_cell(TupleCellSpec(field_name), left_value);
        if (field_rc != RC::SUCCESS) {
          // 尝试使用完整表名
          field_rc = tuple->find_cell(TupleCellSpec(table_name, field_name), left_value);
          if (field_rc != RC::SUCCESS) {
            // 尝试通过字段元数据获取
            const TableMeta& table_meta = main_table->table_meta();
            const FieldMeta* field_meta = table_meta.field(field_name);
            if (field_meta != nullptr) {
              // 通过字段索引获取（跳过前面的系统字段）
              int cell_index = 0;
              for (int i = 0; i < table_meta.field_num(); i++) {
                const FieldMeta* fm = table_meta.field(i);
                if (!fm->visible()) continue;  // 跳过不可见字段
                if (strcmp(fm->name(), field_name) == 0) {
                  field_rc = tuple->cell_at(cell_index, left_value);
                  break;
                }
                cell_index++;
              }
            }
            
            if (field_rc != RC::SUCCESS) {
              LOG_WARN("Failed to get left field value: %s from table %s", field_name, table_name);
              condition_passed = false;
              break;
            }
          }
        }
        LOG_DEBUG("Left field %s value: %s", field_name, left_value.to_string().c_str());
      } else {
        // 左边是常量值
        left_value = condition.left_value;
        LOG_DEBUG("Left constant value: %s", left_value.to_string().c_str());
      }
      
      // 获取右侧值
      if (condition.right_is_attr) {
        // 右边是属性
        const char* field_name = condition.right_attr.attribute_name.c_str();
        const char* table_name = condition.right_attr.relation_name.empty() ? main_table->name() : condition.right_attr.relation_name.c_str();
        
        // 先尝试通过字段名直接获取
        RC field_rc = tuple->find_cell(TupleCellSpec(field_name), right_value);
        if (field_rc != RC::SUCCESS) {
          // 尝试使用完整表名
          field_rc = tuple->find_cell(TupleCellSpec(table_name, field_name), right_value);
          if (field_rc != RC::SUCCESS) {
            // 尝试通过字段元数据获取
            const TableMeta& table_meta = main_table->table_meta();
            const FieldMeta* field_meta = table_meta.field(field_name);
            if (field_meta != nullptr) {
              // 通过字段索引获取（跳过前面的系统字段）
              int cell_index = 0;
              for (int i = 0; i < table_meta.field_num(); i++) {
                const FieldMeta* fm = table_meta.field(i);
                if (!fm->visible()) continue;  // 跳过不可见字段
                if (strcmp(fm->name(), field_name) == 0) {
                  field_rc = tuple->cell_at(cell_index, right_value);
                  break;
                }
                cell_index++;
              }
            }
            
            if (field_rc != RC::SUCCESS) {
              LOG_WARN("Failed to get right field value: %s from table %s", field_name, table_name);
              condition_passed = false;
              break;
            }
          }
        }
        LOG_DEBUG("Right field %s value: %s", field_name, right_value.to_string().c_str());
      } else {
        // 右边是常量值
        right_value = condition.right_value;
        LOG_DEBUG("Right constant value: %s", right_value.to_string().c_str());
      }
      
      // 执行比较操作
      int cmp_result = left_value.compare(right_value);
      bool condition_result = false;
      
      switch (condition.comp) {
        case EQUAL_TO:
          condition_result = (cmp_result == 0);
          break;
        case LESS_THAN:
          condition_result = (cmp_result < 0);
          break;
        case GREAT_THAN:
          condition_result = (cmp_result > 0);
          break;
        case LESS_EQUAL:
          condition_result = (cmp_result <= 0);
          break;
        case GREAT_EQUAL:
          condition_result = (cmp_result >= 0);
          break;
        case NOT_EQUAL:
          condition_result = (cmp_result != 0);
          break;
        default:
          break;
      }
      
      LOG_DEBUG("Condition result: %s (cmp=%d, op=%d)", condition_result ? "true" : "false", cmp_result, condition.comp);
      
      if (!condition_result) {
        condition_passed = false;
        break;
      }
    }

    if (!condition_passed) {
      LOG_DEBUG("Row %d did not pass conditions, skipping", row_count);
      continue;
    }

    // 处理查询表达式
    for (size_t i = 0; i < select_node->expressions.size(); i++) {
      const auto &expr = select_node->expressions[i];
      LOG_DEBUG("Processing expression %zu of type %d", i, static_cast<int>(expr->type()));
      
      Value value;
      RC expr_rc = RC::SUCCESS;
      
      // 处理不同类型的表达式
      if (expr->type() == ExprType::UNBOUND_FIELD) {
        // 处理未绑定的字段表达式
        const UnboundFieldExpr* field_expr = static_cast<const UnboundFieldExpr*>(expr.get());
        const char* field_name = field_expr->field_name();
        const char* table_name = field_expr->table_name();
        
        LOG_DEBUG("Processing unbound field: %s.%s", table_name ? table_name : "NULL", field_name);
        
        // 尝试从tuple中获取字段值
        Value field_value;
        RC field_rc = tuple->find_cell(TupleCellSpec(field_name), field_value);
        if (field_rc == RC::SUCCESS) {
          value = field_value;
          expr_rc = RC::SUCCESS;
          LOG_DEBUG("Found field %s: %s", field_name, value.to_string().c_str());
        } else {
          // 如果通过字段名找不到，尝试通过索引获取
          LOG_DEBUG("Field %s not found by name, trying by index", field_name);
          
          // 获取表的字段信息
          const TableMeta& table_meta = main_table->table_meta();
          const FieldMeta* field_meta = table_meta.field(field_name);
          if (field_meta != nullptr) {
            field_rc = tuple->find_cell(TupleCellSpec(table_name, field_name), field_value);
            if (field_rc == RC::SUCCESS) {
              value = field_value;
              expr_rc = RC::SUCCESS;
              LOG_DEBUG("Found field %s by full name: %s", field_name, value.to_string().c_str());
            } else {
              // 最后尝试通过字段ID获取
              int field_id = field_meta->field_id();
              field_rc = tuple->cell_at(field_id, field_value);
              if (field_rc == RC::SUCCESS) {
                value = field_value;
                expr_rc = RC::SUCCESS;
                LOG_DEBUG("Found field %s by ID %d: %s", field_name, field_id, value.to_string().c_str());
              } else {
                LOG_WARN("Failed to get field %s by ID %d, rc=%d", field_name, field_id, field_rc);
              }
            }
          } else {
            LOG_WARN("Field %s not found in table %s", field_name, main_table->name());
          }
        }
      } else {
        // 处理其他类型的表达式
        expr_rc = expr->get_value(*tuple, value);
      }
      
      if (expr_rc == RC::SUCCESS) {
        results.push_back(value);
        LOG_DEBUG("Complex subquery result: %s", value.to_string().c_str());
      } else {
        LOG_WARN("Failed to get value from expression %zu, rc=%d", i, expr_rc);
        LOG_WARN("Expression type: %d, name: %s", static_cast<int>(expr->type()), expr->name());
      }
    }
  }

  scan_op.close();

  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;  // 正常结束
    LOG_DEBUG("Reached end of table in complex subquery, processed %d rows", row_count);
  } else {
    LOG_WARN("Complex subquery table scan ended with error: %d", rc);
  }

  LOG_DEBUG("Complex subquery executed successfully, returned %zu values", results.size());
  
  return rc;
}

std::string SubqueryExecutor::generate_cache_key(const SelectSqlNode *select_node) const
{
  std::ostringstream oss;
  
  // 基于表名和表达式生成缓存键
  for (const auto &relation : select_node->relations) {
    oss << relation << ",";
  }
  
  for (const auto &expr : select_node->expressions) {
    oss << expr->name() << ",";
  }
  
  return oss.str();
}

bool SubqueryExecutor::get_from_cache(const std::string &cache_key, std::vector<Value> &results)
{
  auto it = cache_.find(cache_key);
  if (it != cache_.end()) {
    results = it->second;
    return true;
  }
  return false;
}

void SubqueryExecutor::put_to_cache(const std::string &cache_key, const std::vector<Value> &results)
{
  // 检查缓存大小限制
  if (cache_.size() >= cache_limit_) {
    // 简单的LRU策略：清除第一个条目
    cache_.erase(cache_.begin());
  }
  
  cache_[cache_key] = results;
}

void SubqueryExecutor::clear_cache()
{
  cache_.clear();
  LOG_INFO("Subquery cache cleared");
}

void SubqueryExecutor::set_cache_limit(size_t limit)
{
  cache_limit_ = limit;
  LOG_INFO("Subquery cache limit set to %zu", limit);
}


