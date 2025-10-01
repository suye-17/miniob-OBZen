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

#pragma once

#include "common/sys/rc.h"
#include "sql/parser/parse_defs.h"
#include "common/value.h"
#include <vector>
#include <unordered_map>

class Session;
class Db;
class Table;

/**
 * @brief 子查询执行器
 * @details 专门用于执行子查询的类，支持缓存和性能优化
 */
class SubqueryExecutor
{
public:
  SubqueryExecutor();
  ~SubqueryExecutor();

  /**
   * @brief 执行子查询
   * @param select_node 子查询节点
   * @param session 会话上下文
   * @param results 输出结果
   */
  RC execute_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results);

  /**
   * @brief 清除缓存
   */
  void clear_cache();

  /**
   * @brief 设置缓存大小限制
   */
  void set_cache_limit(size_t limit);

private:
  /**
   * @brief 执行简单单表子查询
   */
  RC execute_simple_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results);

  /**
   * @brief 执行复杂子查询（使用完整的查询执行引擎）
   */
  RC execute_complex_subquery(const SelectSqlNode *select_node, Session *session, std::vector<Value> &results);

  /**
   * @brief 生成缓存键
   */
  std::string generate_cache_key(const SelectSqlNode *select_node) const;

  /**
   * @brief 从缓存获取结果
   */
  bool get_from_cache(const std::string &cache_key, std::vector<Value> &results);

  /**
   * @brief 将结果存入缓存
   */
  void put_to_cache(const std::string &cache_key, const std::vector<Value> &results);

private:
  // 缓存相关
  std::unordered_map<std::string, std::vector<Value>> cache_;
  size_t cache_limit_;
  bool cache_enabled_;

  // 统计信息
  size_t cache_hits_;
  size_t cache_misses_;
  size_t total_executions_;
};
