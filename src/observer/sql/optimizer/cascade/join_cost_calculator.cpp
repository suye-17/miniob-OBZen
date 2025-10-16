/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/optimizer/cascade/join_cost_calculator.h"
#include "sql/optimizer/cascade/cost_model.h"
#include "sql/expr/expression.h"
#include "sql/parser/parse_defs.h"
#include "common/log/log.h"

double JoinCostCalculator::calculate_nlj_cost(
    double left_card, double right_card, double output_card, CostModel *cost_model)
{
  if (!cost_model) {
    LOG_WARN("cost_model is null");
    return 0.0;
  }

  double cpu_cost = cost_model->cpu_op();

  // NLJ代价：左表每一行都要扫描整个右表
  // cost = left_card * right_card * CPU_OP + output_card * CPU_OP
  double cost = left_card * right_card * cpu_cost + output_card * cpu_cost;

  LOG_DEBUG("NLJ cost: left=%f, right=%f, output=%f, cost=%f", left_card, right_card, output_card, cost);

  return cost;
}

double JoinCostCalculator::calculate_hash_join_cost(
    double left_card, double right_card, double output_card, CostModel *cost_model)
{
  if (!cost_model) {
    LOG_WARN("cost_model is null");
    return 0.0;
  }

  double hash_cost  = cost_model->hash_cost();
  double hash_probe = cost_model->hash_probe();
  double cpu_cost   = cost_model->cpu_op();

  // HashJoin代价：构建哈希表 + 探测 + 输出
  // cost = left_card * HASH_COST + right_card * HASH_PROBE + output_card * CPU_OP
  double cost = left_card * hash_cost + right_card * hash_probe + output_card * cpu_cost;

  LOG_DEBUG("HashJoin cost: left=%f, right=%f, output=%f, cost=%f", left_card, right_card, output_card, cost);

  return cost;
}

bool JoinCostCalculator::is_equi_join(Expression *condition)
{
  if (condition == nullptr) {
    return false;
  }

  ExprType expr_type = condition->type();

  // 情况1：单个比较表达式
  if (expr_type == ExprType::COMPARISON) {
    auto comp_expr = static_cast<ComparisonExpr *>(condition);
    CompOp comp = comp_expr->comp();
    
    // 只有等值比较才能使用HashJoin
    bool is_equal = (comp == EQUAL_TO);
    
    LOG_DEBUG("Checking comparison: comp=%d, is_equal=%d", static_cast<int>(comp), is_equal);
    
    return is_equal;
  }

  // 情况2：AND连接的多个条件
  if (expr_type == ExprType::CONJUNCTION) {
    auto conj_expr = static_cast<ConjunctionExpr *>(condition);
    
    // 只支持AND
    if (conj_expr->conjunction_type() != ConjunctionExpr::Type::AND) {
      LOG_DEBUG("Conjunction is not AND, cannot use HashJoin");
      return false;
    }

    // 所有子条件都必须是等值条件
    auto &children = conj_expr->children();
    for (auto &child : children) {
      if (!is_equi_join(child.get())) {
        LOG_DEBUG("Child condition is not equi-join, cannot use HashJoin");
        return false;
      }
    }

    LOG_DEBUG("All conditions are equi-join, can use HashJoin");
    return true;
  }

  // 其他类型的表达式不支持HashJoin
  LOG_DEBUG("Expression type %d not supported for HashJoin", static_cast<int>(expr_type));
  return false;
}

