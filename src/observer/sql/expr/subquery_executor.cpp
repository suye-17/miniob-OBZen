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
#include "sql/operator/physical_operator.h"
#include "session/session.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "common/log/log.h"
#include "common/value.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/select_stmt.h"
#include "sql/operator/logical_operator.h"
#include "sql/optimizer/logical_plan_generator.h"
#include "sql/optimizer/physical_plan_generator.h"
#include "sql/optimizer/rewriter.h"
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

RC SubqueryExecutor::execute_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results, bool check_single_column)
{
  if (select_node == nullptr || session == nullptr) {
    LOG_WARN("Invalid arguments: select_node=%p, session=%p", select_node, session);
    return RC::INVALID_ARGUMENT;
  }

  // 对于IN和标量子查询，检查是否只返回一列
  // EXISTS不需要检查列数
  if (check_single_column && select_node->expressions.size() != 1) {
    LOG_WARN("Subquery must return exactly one column, but got %zu columns", select_node->expressions.size());
    return RC::SQL_SYNTAX;
  }

  total_executions_++;

  // 检查缓存
  if (cache_enabled_) {
    std::string cache_key = generate_cache_key(select_node);
    LOG_DEBUG("SubqueryExecutor: cache_key = %s", cache_key.c_str());
    if (get_from_cache(cache_key, results)) {
      cache_hits_++;
      LOG_DEBUG("Subquery cache hit, returned %zu values", results.size());
      return RC::SUCCESS;
    }
    LOG_DEBUG("Subquery cache miss, executing subquery", cache_key.c_str());
    cache_misses_++;
  }

  // 执行子查询
  RC rc = RC::SUCCESS;
  
  // 检查是否包含聚合函数（包括未绑定和已绑定的）
  bool has_aggregate = false;
  for (const auto &expr : select_node->expressions) {
    if (expr->type() == ExprType::AGGREGATION || expr->type() == ExprType::UNBOUND_AGGREGATION) {
      has_aggregate = true;
      LOG_DEBUG("Subquery contains aggregate function (type=%d), using full query engine", 
                static_cast<int>(expr->type()));
      break;
    }
  }
  
  // 检查是否是简单的单表查询（无聚合函数、无条件、无JOIN）
  if (!has_aggregate && 
      select_node->relations.size() == 1 && 
      select_node->conditions.empty() && 
      select_node->joins.empty()) {
    rc = execute_simple_subquery(select_node, session, results, check_single_column);
  } else {
    // 对于复杂子查询或包含聚合函数的查询，使用完整的查询执行引擎
    LOG_DEBUG("Executing complex subquery with full query engine (aggregates: %d, relations: %zu, conditions: %zu, joins: %zu)",
              has_aggregate, select_node->relations.size(), select_node->conditions.size(), select_node->joins.size());
    rc = execute_complex_subquery(select_node, session, results, check_single_column);
    
    // 如果复杂子查询失败，并且没有聚合函数，回退到简单实现
    if (rc != RC::SUCCESS && !has_aggregate) {
      LOG_WARN("Complex subquery execution failed, falling back to simple implementation");
      rc = execute_simple_subquery(select_node, session, results, check_single_column);
    }
  }

  // 如果执行成功，缓存结果
  if (rc == RC::SUCCESS && cache_enabled_) {
    std::string cache_key = generate_cache_key(select_node);
    put_to_cache(cache_key, results);
  }

  return rc;
}

RC SubqueryExecutor::execute_simple_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results, bool check_single_column)
{
  LOG_DEBUG("Executing simple subquery for table: %s (check_single_column=%d)", select_node->relations[0].c_str(), check_single_column);

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
      
      // 特殊处理：如果是 SELECT *，需要检查列数
      if (expr->type() == ExprType::STAR) {
        LOG_DEBUG("SELECT * detected in subquery");
        // 只有IN和标量子查询需要检查列数，EXISTS不需要
        if (check_single_column && tuple->cell_num() != 1) {
          LOG_WARN("Subquery with SELECT * returns %d columns, but must return exactly 1", tuple->cell_num());
          scan_op.close();
          return RC::SQL_SYNTAX;
        }
        // 收集所有列（EXISTS需要）或只收集第一列（IN/标量子查询）
        int cells_to_collect = check_single_column ? 1 : tuple->cell_num();
        for (int cell_idx = 0; cell_idx < cells_to_collect; cell_idx++) {
          Value cell_value;
          RC cell_rc = tuple->cell_at(cell_idx, cell_value);
          if (cell_rc == RC::SUCCESS) {
            results.push_back(cell_value);
            LOG_DEBUG("Added cell[%d]: %s (type: %d)", cell_idx,
                     cell_value.to_string().c_str(), static_cast<int>(cell_value.attr_type()));
          } else {
            LOG_WARN("Failed to get cell %d from tuple, rc=%d", cell_idx, cell_rc);
          }
        }
        // SELECT * 已处理完，不需要继续处理其他表达式
        continue;
      }
      
      // 处理不同类型的表达式
      if (expr->type() == ExprType::UNBOUND_FIELD) {
        // 处理未绑定的字段表达式
        const UnboundFieldExpr* field_expr = static_cast<const UnboundFieldExpr*>(expr.get());
        const char* field_name = field_expr->field_name();
        const char* table_name_from_expr = field_expr->table_name();
        
        // 使用实际的表名（优先使用表达式中的表名，否则使用当前表名）
        const char* table_name_to_use = (table_name_from_expr && strlen(table_name_from_expr) > 0) ? 
                                         table_name_from_expr : table->name();
        
        LOG_DEBUG("Processing unbound field: %s.%s (actual table: %s)", 
                  table_name_from_expr ? table_name_from_expr : "NULL", 
                  field_name, table_name_to_use);
        
        // 方法1：尝试直接通过字段名获取
        Value field_value;
        RC field_rc = tuple->find_cell(TupleCellSpec(field_name), field_value);
        
        if (field_rc != RC::SUCCESS) {
          // 方法2：尝试通过完整的表名.字段名获取
          LOG_DEBUG("Field %s not found by name alone, trying with table name", field_name);
          field_rc = tuple->find_cell(TupleCellSpec(table_name_to_use, field_name), field_value);
        }
        
        if (field_rc != RC::SUCCESS) {
          // 方法3：通过字段元数据和索引获取
          LOG_DEBUG("Field %s not found by TupleCellSpec, trying by field metadata", field_name);
          const TableMeta& table_meta = table->table_meta();
          const FieldMeta* field_meta = table_meta.field(field_name);
          
          if (field_meta != nullptr) {
            // 方法3a：通过字段在可见字段中的索引获取
            int cell_index = 0;
            bool found = false;
            for (int j = 0; j < table_meta.field_num(); j++) {
              const FieldMeta* fm = table_meta.field(j);
              if (!fm->visible()) {
                continue;  // 跳过系统字段（如__trx_id等）
              }
              if (strcmp(fm->name(), field_name) == 0) {
                field_rc = tuple->cell_at(cell_index, field_value);
                found = true;
                LOG_DEBUG("Found field %s at cell index %d: %s", 
                         field_name, cell_index, 
                         field_rc == RC::SUCCESS ? field_value.to_string().c_str() : "FAILED");
                break;
              }
              cell_index++;
            }
            
            if (!found) {
              LOG_WARN("Field %s found in metadata but not mapped to tuple cell", field_name);
              field_rc = RC::SCHEMA_FIELD_NOT_EXIST;
            }
          } else {
            LOG_WARN("Field %s not found in table %s metadata", field_name, table->name());
            field_rc = RC::SCHEMA_FIELD_NOT_EXIST;
          }
        }
        
        if (field_rc == RC::SUCCESS) {
          value = field_value;
          expr_rc = RC::SUCCESS;
          LOG_DEBUG("Successfully got field %s value: %s (type: %d)", 
                   field_name, value.to_string().c_str(), static_cast<int>(value.attr_type()));
        } else {
          LOG_WARN("Failed to get field %s value after all attempts, rc=%d", field_name, field_rc);
          expr_rc = field_rc;
        }
      } else {
        // 处理其他类型的表达式
        expr_rc = expr->get_value(*tuple, value);
        if (expr_rc == RC::SUCCESS) {
          LOG_DEBUG("Got value from expression type %d: %s", 
                   static_cast<int>(expr->type()), value.to_string().c_str());
        }
      }
      
      if (expr_rc == RC::SUCCESS) {
        results.push_back(value);
        LOG_DEBUG("Added subquery result[%zu]: %s (type: %d)", 
                 results.size() - 1, value.to_string().c_str(), static_cast<int>(value.attr_type()));
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

RC SubqueryExecutor::execute_complex_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results, bool check_single_column)
{
  LOG_DEBUG("Executing complex subquery with %zu relations, %zu conditions, %zu joins (check_single_column=%d)", 
            select_node->relations.size(), select_node->conditions.size(), select_node->joins.size(), check_single_column);

  // 获取数据库
  Db *db = session->get_current_db();
  if (db == nullptr) {
    LOG_WARN("No current database in session");
    return RC::SCHEMA_DB_NOT_EXIST;
  }
  
  // 使用完整的查询执行引擎
  // 1. 创建 SelectStmt
  SelectSqlNode mutable_select_node = *select_node;
  Stmt *stmt = nullptr;
  RC rc = SelectStmt::create(db, mutable_select_node, stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to create SelectStmt for subquery, rc=%d", rc);
    return rc;
  }
  
  std::unique_ptr<Stmt> stmt_guard(stmt);
  SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);
  
  // 2. 生成逻辑计划
  std::unique_ptr<LogicalOperator> logical_oper;
  LogicalPlanGenerator logical_generator;
  rc = logical_generator.create(select_stmt, logical_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to create logical plan for subquery, rc=%d", rc);
    return rc;
  }
  
  // 3. 优化逻辑计划
  Rewriter rewriter;
  bool change_made = false;
  rc = rewriter.rewrite(logical_oper, change_made);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to rewrite logical plan for subquery, rc=%d", rc);
    return rc;
  }
  
  // 4. 生成物理计划
  std::unique_ptr<PhysicalOperator> physical_oper;
  PhysicalPlanGenerator physical_generator;
  rc = physical_generator.create(*logical_oper, physical_oper, session);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to create physical plan for subquery, rc=%d", rc);
    return rc;
  }
  
  // 5. 执行物理计划
  rc = physical_oper->open(nullptr);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to open physical operator for subquery, rc=%d", rc);
    return rc;
  }
  
  // 6. 收集结果
  int row_count = 0;
  
  // 确定每行应该收集多少个cell
  // 优先使用 select_stmt 的 query_expressions，因为它包含了绑定后的完整表达式列表
  int expected_cell_num = 0;
  if (!select_stmt->query_expressions().empty()) {
    expected_cell_num = select_stmt->query_expressions().size();
    LOG_DEBUG("Using query_expressions count: %d", expected_cell_num);
  } else {
    expected_cell_num = select_node->expressions.size();
    LOG_DEBUG("Using select_node expressions count: %d", expected_cell_num);
  }
  
  LOG_DEBUG("Subquery expects %d cells per row", expected_cell_num);
  
  while (RC::SUCCESS == (rc = physical_oper->next())) {
    Tuple *tuple = physical_oper->current_tuple();
    if (tuple == nullptr) {
      continue;
    }
    
    row_count++;
    int tuple_cell_num = tuple->cell_num();
    LOG_DEBUG("Processing row %d, tuple has %d cells (expected %d)", 
             row_count, tuple_cell_num, expected_cell_num);
    
    // 确定要收集的cell数量
    int cells_to_collect;
    if (expected_cell_num == 0 || expected_cell_num > tuple_cell_num) {
      // SELECT * 或者expected超出实际数量，收集所有可用的cell
      cells_to_collect = tuple_cell_num;
      LOG_DEBUG("Collecting all %d cells (expected=%d)", tuple_cell_num, expected_cell_num);
    } else {
      // 明确指定列的情况，只收集指定数量
      cells_to_collect = expected_cell_num;
      LOG_DEBUG("Collecting %d cells as specified", cells_to_collect);
    }
    
    // 只有IN和标量子查询需要检查列数，EXISTS不需要
    if (check_single_column && cells_to_collect != 1) {
      LOG_WARN("Subquery returns %d columns, but must return exactly 1", cells_to_collect);
      physical_oper->close();
      return RC::SQL_SYNTAX;
    }
    
    for (int i = 0; i < cells_to_collect; i++) {
      Value value;
      rc = tuple->cell_at(i, value);
      if (rc == RC::SUCCESS) {
        results.push_back(value);
        LOG_DEBUG("Complex subquery result[row=%d, cell=%d/%d]: %s (type=%d)", 
                 row_count, i, cells_to_collect, value.to_string().c_str(), 
                 static_cast<int>(value.attr_type()));
      } else {
        LOG_WARN("Failed to get cell %d from tuple, rc=%d", i, rc);
      }
    }
  }
  
  physical_oper->close();
  
  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
    LOG_DEBUG("Complex subquery executed successfully, returned %zu values from %d rows", 
              results.size(), row_count);
  } else {
    LOG_WARN("Complex subquery execution ended with error, rc=%d", rc);
  }
  
  return rc;
}

std::string SubqueryExecutor::generate_cache_key(const SelectSqlNode *select_node) const
{
  std::ostringstream oss;
  
  // 基于表名和表达式生成缓存键
  for (const auto &relation : select_node->relations) {
    oss << relation << ",";
  }
  
  // 为每个表达式生成唯一的标识符
  for (size_t i = 0; i < select_node->expressions.size(); i++) {
    const auto &expr = select_node->expressions[i];
    // 使用表达式类型和名称来生成更精确的缓存键
    oss << "expr_" << i << "_type_" << static_cast<int>(expr->type()) << "_name_" << expr->name() << ",";
    
    // 对于聚合函数，需要特别处理以区分不同的聚合类型
    if (expr->type() == ExprType::UNBOUND_AGGREGATION || expr->type() == ExprType::AGGREGATION) {
      // 使用表达式的完整字符串表示来确保唯一性
      oss << "aggr_" << reinterpret_cast<uintptr_t>(expr.get()) << ",";
    }
  }
  
  // 添加条件和JOIN信息，确保缓存键的完整性
  oss << "conds_" << select_node->conditions.size() << ",";
  oss << "joins_" << select_node->joins.size() << ",";
  
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
  LOG_DEBUG("Subquery cache cleared");
}

void SubqueryExecutor::set_cache_limit(size_t limit)
{
  cache_limit_ = limit;
  LOG_DEBUG("Subquery cache limit set to %zu", limit);
}


