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
// Created by WangYunlai on 2022/12/30.
//

#include "sql/operator/nested_loop_join_physical_operator.h"

NestedLoopJoinPhysicalOperator::NestedLoopJoinPhysicalOperator() {}

NestedLoopJoinPhysicalOperator::NestedLoopJoinPhysicalOperator(Expression *condition)
    : condition_(condition) {}

NestedLoopJoinPhysicalOperator::~NestedLoopJoinPhysicalOperator()
{
  if (condition_) {
    delete condition_;
    condition_ = nullptr;
  }
}

RC NestedLoopJoinPhysicalOperator::open(Trx *trx)
{
  if (children_.size() != 2) {
    LOG_WARN("nlj operator should have 2 children");
    return RC::INTERNAL;
  }

  RC rc         = RC::SUCCESS;
  left_         = children_[0].get();
  right_        = children_[1].get();
  right_closed_ = true;
  round_done_   = true;

  rc   = left_->open(trx);
  trx_ = trx;
  return rc;
}

RC NestedLoopJoinPhysicalOperator::next()
{
  RC rc = RC::SUCCESS;
  while (RC::SUCCESS == rc) {
    bool left_need_step = (left_tuple_ == nullptr);
    if (round_done_) {
      left_need_step = true;
    }

    if (left_need_step) {
      rc = left_next();
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }

    rc = right_next();
    if (rc != RC::SUCCESS) {
      if (rc == RC::RECORD_EOF) {
        rc          = RC::SUCCESS;
        round_done_ = true;
        continue;
      } else {
        return rc;
      }
    }

    // 评估JOIN条件
    bool join_condition_satisfied = true;  // 默认满足（无条件时为笛卡尔积）
    if (condition_ != nullptr) {
      rc = evaluate_join_condition(join_condition_satisfied);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }

    // 如果JOIN条件满足，返回当前组合
    if (join_condition_satisfied) {
      return RC::SUCCESS;
    }

    // JOIN条件不满足，继续寻找下一个组合
  }
  return rc;
}

RC NestedLoopJoinPhysicalOperator::close()
{
  RC rc = left_->close();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to close left oper. rc=%s", strrc(rc));
  }

  if (!right_closed_) {
    rc = right_->close();
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to close right oper. rc=%s", strrc(rc));
    } else {
      right_closed_ = true;
    }
  }
  return rc;
}

Tuple *NestedLoopJoinPhysicalOperator::current_tuple() { return &joined_tuple_; }

RC NestedLoopJoinPhysicalOperator::left_next()
{
  RC rc = RC::SUCCESS;
  rc    = left_->next();
  if (rc != RC::SUCCESS) {
    return rc;
  }

  left_tuple_ = left_->current_tuple();
  joined_tuple_.set_left(left_tuple_);
  return rc;
}

RC NestedLoopJoinPhysicalOperator::right_next()
{
  RC rc = RC::SUCCESS;
  if (round_done_) {
    if (!right_closed_) {
      rc = right_->close();

      right_closed_ = true;
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }

    rc = right_->open(trx_);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    right_closed_ = false;

    round_done_ = false;
  }

  rc = right_->next();
  if (rc != RC::SUCCESS) {
    if (rc == RC::RECORD_EOF) {
      round_done_ = true;
    }
    return rc;
  }

  right_tuple_ = right_->current_tuple();
  joined_tuple_.set_right(right_tuple_);
  return rc;
}

RC NestedLoopJoinPhysicalOperator::evaluate_join_condition(bool &result)
{
  if (condition_ == nullptr) {
    result = true;  // 无条件时相当于笛卡尔积
    return RC::SUCCESS;
  }

  // 确保joined_tuple_包含当前的左右元组
  joined_tuple_.set_left(left_tuple_);
  joined_tuple_.set_right(right_tuple_);

  // 计算JOIN条件表达式的值
  Value condition_value;
  RC rc = condition_->get_value(joined_tuple_, condition_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to evaluate join condition: %s", strrc(rc));
    return rc;
  }

  result = condition_value.get_boolean();
  return RC::SUCCESS;
}
