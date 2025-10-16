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

#include "common/sys/rc.h"

class Expression;
class CostModel;

/**
 * @brief Join算子代价计算器
 * @details 根据左右表的基数和输出基数，计算NLJ和HashJoin的代价
 */
class JoinCostCalculator
{
public:
  /**
   * @brief 计算NestedLoopJoin的代价
   * @param left_card 左表基数
   * @param right_card 右表基数
   * @param output_card 输出基数
   * @param cost_model 代价模型
   * @return 代价值
   * @details cost = left_card * right_card * CPU_OP + output_card * CPU_OP
   */
  static double calculate_nlj_cost(
      double left_card, double right_card, double output_card, CostModel *cost_model);

  /**
   * @brief 计算HashJoin的代价
   * @param left_card 左表基数（用于构建哈希表）
   * @param right_card 右表基数（用于探测）
   * @param output_card 输出基数
   * @param cost_model 代价模型
   * @return 代价值
   * @details cost = left_card * HASH_COST + right_card * HASH_PROBE + output_card * CPU_OP
   */
  static double calculate_hash_join_cost(
      double left_card, double right_card, double output_card, CostModel *cost_model);

  /**
   * @brief 检查是否为等值JOIN条件
   * @param condition JOIN条件表达式
   * @return 如果是等值条件返回true，否则返回false
   * @details 只有等值JOIN才能使用HashJoin
   */
  static bool is_equi_join(Expression *condition);
};

