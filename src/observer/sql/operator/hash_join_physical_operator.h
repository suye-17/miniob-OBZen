/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "sql/operator/physical_operator.h"
#include "sql/expr/expression.h"
#include <unordered_map>
#include <vector>

/**
 * @brief Hash Join 算子
 * @details 实现基于哈希的连接算法，分为构建阶段和探测阶段
 * 构建阶段：遍历左表（构建表），建立哈希表
 * 探测阶段：遍历右表（探测表），在哈希表中查找匹配的记录
 * @ingroup PhysicalOperator
 */
class HashJoinPhysicalOperator : public PhysicalOperator
{
public:
  HashJoinPhysicalOperator(Expression *join_condition);
  virtual ~HashJoinPhysicalOperator();

  PhysicalOperatorType type() const override { return PhysicalOperatorType::HASH_JOIN; }

  RC     open(Trx *trx) override;
  RC     next() override;
  RC     close() override;
  Tuple *current_tuple() override;

private:
  /**
   * @brief 构建阶段：扫描左表并建立哈希表
   */
  RC build_phase();

  /**
   * @brief 探测阶段：扫描右表并在哈希表中查找匹配
   */
  RC probe_phase();

  /**
   * @brief 从条件表达式中提取左右表的连接字段
   */
  RC extract_join_keys();

  /**
   * @brief 计算哈希键值
   */
  size_t compute_hash(const Value &value) const;

private:
  Trx *trx_ = nullptr;

  // 左表（构建表）和右表（探测表）
  PhysicalOperator *left_  = nullptr;
  PhysicalOperator *right_ = nullptr;

  // 连接条件
  Expression *join_condition_ = nullptr;

  // 当前的左右表 tuple
  Tuple *left_tuple_  = nullptr;
  Tuple *right_tuple_ = nullptr;

  // 连接后的 tuple
  JoinedTuple joined_tuple_;

  // 哈希表：key 是哈希值，value 是左表的 tuple 列表
  std::unordered_map<size_t, std::vector<Tuple *>> hash_table_;

  // 存储左表的所有 tuple（用于内存管理）
  std::vector<std::unique_ptr<Tuple>> left_tuples_;

  // 当前探测阶段的状态
  bool   build_done_       = false;
  size_t current_hash_key_ = 0;
  size_t current_match_idx_ = 0;
  std::vector<Tuple *> current_matches_;

  // 用于提取连接字段的表达式
  Expression *left_join_expr_  = nullptr;
  Expression *right_join_expr_ = nullptr;
};