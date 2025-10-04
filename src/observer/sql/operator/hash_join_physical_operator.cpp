/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/operator/hash_join_physical_operator.h"
#include "common/log/log.h"
#include "sql/expr/tuple.h"
#include <functional>

HashJoinPhysicalOperator::HashJoinPhysicalOperator(Expression *join_condition)
    : join_condition_(join_condition)
{
}

HashJoinPhysicalOperator::~HashJoinPhysicalOperator()
{
  if (join_condition_ != nullptr) {
    delete join_condition_;
    join_condition_ = nullptr;
  }
}

RC HashJoinPhysicalOperator::open(Trx *trx)
{
  if (children_.size() != 2) {
    LOG_WARN("hash join operator should have 2 children, but have %d", children_.size());
    return RC::INTERNAL;
  }

  trx_ = trx;
  left_ = children_[0].get();
  right_ = children_[1].get();

  // 提取连接键
  RC rc = extract_join_keys();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to extract join keys. rc=%s", strrc(rc));
    return rc;
  }

  // 打开左表并执行构建阶段
  rc = left_->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open left child. rc=%s", strrc(rc));
    return rc;
  }

  rc = build_phase();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to build hash table. rc=%s", strrc(rc));
    return rc;
  }

  // 打开右表准备探测
  rc = right_->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open right child. rc=%s", strrc(rc));
    return rc;
  }

  build_done_ = true;
  return RC::SUCCESS;
}

RC HashJoinPhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  // 如果当前有匹配的记录，继续返回
  if (current_match_idx_ < current_matches_.size()) {
    left_tuple_ = current_matches_[current_match_idx_];
    joined_tuple_.set_left(left_tuple_);
    joined_tuple_.set_right(right_tuple_);
    current_match_idx_++;
    return RC::SUCCESS;
  }

  // 需要从右表读取下一条记录
  while (true) {
    rc = right_->next();
    if (rc != RC::SUCCESS) {
      return rc;  // 可能是 RECORD_EOF 或其他错误
    }

    right_tuple_ = right_->current_tuple();
    if (right_tuple_ == nullptr) {
      continue;
    }

    // 计算右表连接字段的哈希值
    Value right_value;
    if (right_join_expr_ != nullptr) {
      rc = right_join_expr_->get_value(*right_tuple_, right_value);
      if (rc != RC::SUCCESS) {
        LOG_DEBUG("failed to get right join value, skipping. rc=%s", strrc(rc));
        continue;
      }
    } else {
      LOG_WARN("right join expression is null");
      return RC::INTERNAL;
    }

    size_t hash_key = compute_hash(right_value);

    // 在哈希表中查找匹配的记录
    auto it = hash_table_.find(hash_key);
    if (it != hash_table_.end() && !it->second.empty()) {
      current_matches_ = it->second;
      current_match_idx_ = 0;

      // 验证是否真正匹配（处理哈希冲突）
      std::vector<Tuple *> valid_matches;
      for (Tuple *left_tuple : current_matches_) {
        Value left_value;
        if (left_join_expr_ != nullptr) {
          rc = left_join_expr_->get_value(*left_tuple, left_value);
          if (rc != RC::SUCCESS) {
            continue;
          }

          // 比较值是否相等
          if (left_value.compare(right_value) == 0) {
            valid_matches.push_back(left_tuple);
          }
        }
      }

      if (!valid_matches.empty()) {
        current_matches_ = valid_matches;
        left_tuple_ = current_matches_[current_match_idx_];
        joined_tuple_.set_left(left_tuple_);
        joined_tuple_.set_right(right_tuple_);
        current_match_idx_++;
        return RC::SUCCESS;
      }
    }
  }

  return RC::RECORD_EOF;
}

RC HashJoinPhysicalOperator::close()
{
  RC rc = RC::SUCCESS;

  if (left_ != nullptr) {
    rc = left_->close();
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to close left child. rc=%s", strrc(rc));
    }
  }

  if (right_ != nullptr) {
    rc = right_->close();
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to close right child. rc=%s", strrc(rc));
    }
  }

  // 清理哈希表
  hash_table_.clear();
  left_tuples_.clear();
  current_matches_.clear();

  return rc;
}

Tuple *HashJoinPhysicalOperator::current_tuple()
{
  return &joined_tuple_;
}

RC HashJoinPhysicalOperator::build_phase()
{
  RC rc = RC::SUCCESS;

  LOG_DEBUG("Building hash table from left table");

  // 扫描左表，构建哈希表
  while (true) {
    rc = left_->next();
    if (rc == RC::RECORD_EOF) {
      break;
    }
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get next tuple from left child. rc=%s", strrc(rc));
      return rc;
    }

    Tuple *tuple = left_->current_tuple();
    if (tuple == nullptr) {
      continue;
    }

    // 获取连接字段的值
    Value join_value;
    if (left_join_expr_ != nullptr) {
      rc = left_join_expr_->get_value(*tuple, join_value);
      if (rc != RC::SUCCESS) {
        LOG_DEBUG("failed to get left join value, skipping. rc=%s", strrc(rc));
        continue;
      }
    } else {
      LOG_WARN("left join expression is null");
      return RC::INTERNAL;
    }

    // 复制 tuple 并存储
    std::unique_ptr<Tuple> tuple_copy(new RowTuple(*static_cast<RowTuple*>(tuple)));
    Tuple *tuple_ptr = tuple_copy.get();
    left_tuples_.push_back(std::move(tuple_copy));

    // 计算哈希值并插入哈希表
    size_t hash_key = compute_hash(join_value);
    hash_table_[hash_key].push_back(tuple_ptr);
  }

  LOG_DEBUG("Hash table built with %zu unique keys and %zu total tuples", 
            hash_table_.size(), left_tuples_.size());

  return RC::SUCCESS;
}

RC HashJoinPhysicalOperator::extract_join_keys()
{
  if (join_condition_ == nullptr) {
    LOG_WARN("join condition is null");
    return RC::INVALID_ARGUMENT;
  }

  // 假设连接条件是一个比较表达式 (left_field = right_field)
  if (join_condition_->type() == ExprType::COMPARISON) {
    ComparisonExpr *comp_expr = static_cast<ComparisonExpr *>(join_condition_);
    
    // 获取左右表达式（从 unique_ptr 中获取原始指针）
    left_join_expr_ = comp_expr->left().get();
    right_join_expr_ = comp_expr->right().get();

    if (left_join_expr_ == nullptr || right_join_expr_ == nullptr) {
      LOG_WARN("join condition does not have left or right expression");
      return RC::INVALID_ARGUMENT;
    }

    LOG_DEBUG("Extracted join keys from comparison expression");
    return RC::SUCCESS;
  }

  LOG_WARN("join condition is not a comparison expression, type=%d", static_cast<int>(join_condition_->type()));
  return RC::INVALID_ARGUMENT;
}

size_t HashJoinPhysicalOperator::compute_hash(const Value &value) const
{
  // 根据值的类型计算哈希
  switch (value.attr_type()) {
    case AttrType::INTS: {
      return std::hash<int>{}(value.get_int());
    }
    case AttrType::FLOATS: {
      return std::hash<float>{}(value.get_float());
    }
    case AttrType::CHARS: {
      return std::hash<std::string>{}(value.get_string());
    }
    case AttrType::BOOLEANS: {
      return std::hash<bool>{}(value.get_boolean());
    }
    default: {
      // 对于其他类型，使用字符串表示的哈希
      return std::hash<std::string>{}(value.to_string());
    }
  }
}
