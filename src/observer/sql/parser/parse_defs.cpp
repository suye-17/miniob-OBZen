/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/parser/parse_defs.h"
#include "sql/expr/expression.h"

// SelectSqlNode 析构函数实现
SelectSqlNode::~SelectSqlNode() = default;

// SelectSqlNode 拷贝构造函数实现
SelectSqlNode::SelectSqlNode(const SelectSqlNode& other) {
  // 拷贝基本类型成员
  relations = other.relations;
  joins = other.joins;
  conditions = other.conditions;
  
  // 深拷贝expressions
  expressions.reserve(other.expressions.size());
  for (const auto& expr : other.expressions) {
    if (expr) {
      expressions.push_back(expr->copy());
    }
  }
  
  // 深拷贝group_by
  group_by.reserve(other.group_by.size());
  for (const auto& expr : other.group_by) {
    if (expr) {
      group_by.push_back(expr->copy());
    }
  }
}

// SelectSqlNode 拷贝赋值操作符实现
SelectSqlNode& SelectSqlNode::operator=(const SelectSqlNode& other) {
  if (this != &other) {
    // 清空当前内容
    expressions.clear();
    group_by.clear();
    
    // 拷贝基本类型成员
    relations = other.relations;
    joins = other.joins;
    conditions = other.conditions;
    
    // 深拷贝expressions
    expressions.reserve(other.expressions.size());
    for (const auto& expr : other.expressions) {
      if (expr) {
        expressions.push_back(expr->copy());
      }
    }
    
    // 深拷贝group_by
    group_by.reserve(other.group_by.size());
    for (const auto& expr : other.group_by) {
      if (expr) {
        group_by.push_back(expr->copy());
      }
    }
  }
  return *this;
}

// 创建深拷贝的静态方法
unique_ptr<SelectSqlNode> SelectSqlNode::create_copy(const SelectSqlNode* original) {
  if (original == nullptr) {
    return nullptr;
  }
  return make_unique<SelectSqlNode>(*original);
}

// ConditionSqlNode 拷贝构造函数实现
ConditionSqlNode::ConditionSqlNode(const ConditionSqlNode& other) {
  left_is_attr = other.left_is_attr;
  left_value = other.left_value;
  left_attr = other.left_attr;
  comp = other.comp;
  right_is_attr = other.right_is_attr;
  right_attr = other.right_attr;
  right_value = other.right_value;
  right_values = other.right_values;
  has_subquery = other.has_subquery;
  
  // 深拷贝子查询
  if (other.subquery) {
    subquery = SelectSqlNode::create_copy(other.subquery.get());
  } else {
    subquery = nullptr;
  }
}

// ConditionSqlNode 拷贝赋值操作符实现
ConditionSqlNode& ConditionSqlNode::operator=(const ConditionSqlNode& other) {
  if (this != &other) {
    left_is_attr = other.left_is_attr;
    left_value = other.left_value;
    left_attr = other.left_attr;
    comp = other.comp;
    right_is_attr = other.right_is_attr;
    right_attr = other.right_attr;
    right_value = other.right_value;
    right_values = other.right_values;
    has_subquery = other.has_subquery;
    
    // 深拷贝子查询
    if (other.subquery) {
      subquery = SelectSqlNode::create_copy(other.subquery.get());
    } else {
      subquery = nullptr;
    }
  }
  return *this;
}
