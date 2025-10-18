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

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_set>
#include "common/sys/rc.h"
#include "common/value.h"
#include "common/type/attr_type.h"
#include "storage/field/field.h"
#include "storage/common/column.h"
#include "storage/common/chunk.h"
#include "sql/expr/aggregator.h"

using std::string;
using std::vector;
using std::unique_ptr;
using std::unordered_set;

class Tuple;
struct SelectSqlNode;
class Session;

/**
 * @defgroup Expression
 * @brief 表达式
 */

/**
 * @brief 表达式类型
 * @ingroup Expression
 */
enum class ExprType
{
  NONE,
  STAR,                 ///< 星号，表示所有字段
  UNBOUND_FIELD,        ///< 未绑定的字段，需要在resolver阶段解析为FieldExpr
  UNBOUND_AGGREGATION,  ///< 未绑定的聚合函数，需要在resolver阶段解析为AggregateExpr

  FIELD,        ///< 字段。在实际执行时，根据行数据内容提取对应字段的值
  VALUE,        ///< 常量值
  CAST,         ///< 需要做类型转换的表达式
  COMPARISON,   ///< 需要做比较的表达式
  CONJUNCTION,  ///< 多个表达式使用同一种关系(AND或OR)来联结
  ARITHMETIC,   ///< 算术运算
  AGGREGATION,  ///< 聚合运算
  SUBQUERY,     ///< 子查询表达式
  FUNCTION,     ///< 函数表达式（如距离函数）
};

/**
 * @brief 表达式的抽象描述
 * @ingroup Expression
 * @details 在SQL的元素中，任何需要得出值的元素都可以使用表达式来描述
 * 比如获取某个字段的值、比较运算、类型转换
 * 当然还有一些当前没有实现的表达式，比如算术运算。
 *
 * 通常表达式的值，是在真实的算子运算过程中，拿到具体的tuple后
 * 才能计算出来真实的值。但是有些表达式可能就表示某一个固定的
 * 值，比如ValueExpr。
 *
 * TODO 区分unbound和bound的表达式
 */
class Expression
{
public:
  Expression() = default;

  virtual ~Expression() = default;

  /**
   * @brief 复制表达式
   */
  virtual unique_ptr<Expression> copy() const = 0;

  /**
   * @brief 判断两个表达式是否相等
   */
  virtual bool equal(const Expression &other) const { return false; }
  /**
   * @brief 根据具体的tuple，来计算当前表达式的值。tuple有可能是一个具体某个表的行数据
   */
  virtual RC get_value(const Tuple &tuple, Value &value) const = 0;

  /**
   * @brief 在没有实际运行的情况下，也就是无法获取tuple的情况下，尝试获取表达式的值
   * @details 有些表达式的值是固定的，比如ValueExpr，这种情况下可以直接获取值
   */
  virtual RC try_get_value(Value &value) const { return RC::UNIMPLEMENTED; }

  /**
   * @brief 从 `chunk` 中获取表达式的计算结果 `column`
   */
  virtual RC get_column(Chunk &chunk, Column &column) { return RC::UNIMPLEMENTED; }

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  virtual ExprType type() const = 0;

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  virtual AttrType value_type() const = 0;

  /**
   * @brief 表达式值的长度
   */
  virtual int value_length() const { return -1; }

  /**
   * @brief 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
   */
  virtual const char *name() const { return name_.c_str(); }
  virtual void        set_name(string name) { name_ = name; }

  /**
   * @brief 表达式在下层算子返回的 chunk 中的位置
   */
  virtual int  pos() const { return pos_; }
  virtual void set_pos(int pos) { pos_ = pos; }

  /**
   * @brief 用于 ComparisonExpr 获得比较结果 `select`。
   */
  virtual RC eval(Chunk &chunk, vector<uint8_t> &select) { return RC::UNIMPLEMENTED; }

  /**
   * @brief 设置session上下文（用于子查询执行）
   */
  virtual void set_session_context(class Session *session) { /* 默认实现为空 */ }

  /**
   * @brief 遍历表达式树，为所有子表达式设置session上下文
   */
  virtual void set_session_context_recursive(class Session *session) { 
    set_session_context(session); 
  }

  /**
   * @brief 递归清理子查询缓存（针对所有子表达式）
   */
  virtual void clear_subquery_cache_recursive() { 
    /* 默认实现为空 - 只有包含子查询的表达式需要实现 */ 
  }

  /**
   * @brief 获取表达式涉及的所有表名
   * @details 用于谓词下推优化，递归收集所有FieldExpr中的表名
   */
  virtual std::unordered_set<std::string> get_involved_tables() const { 
    return {}; 
  }

protected:
  /**
   * @brief 表达式在下层算子返回的 chunk 中的位置
   * @details 当 pos_ = -1 时表示下层算子没有在返回的 chunk 中计算出该表达式的计算结果，
   * 当 pos_ >= 0时表示在下层算子中已经计算出该表达式的值（比如聚合表达式），且该表达式对应的结果位于
   * chunk 中 下标为 pos_ 的列中。
   */
  int pos_ = -1;

private:
  string name_;
};

class StarExpr : public Expression
{
public:
  StarExpr() : table_name_() {}
  StarExpr(const char *table_name) : table_name_(table_name) {}
  virtual ~StarExpr() = default;

  unique_ptr<Expression> copy() const override { return make_unique<StarExpr>(table_name_.c_str()); }

  ExprType type() const override { return ExprType::STAR; }
  AttrType value_type() const override { return AttrType::UNDEFINED; }

  RC get_value(const Tuple &tuple, Value &value) const override { return RC::UNIMPLEMENTED; }  // 不需要实现

  const char *table_name() const { return table_name_.c_str(); }

private:
  string table_name_;
};

class UnboundFieldExpr : public Expression
{
public:
  UnboundFieldExpr(const string &table_name, const string &field_name)
      : table_name_(table_name), field_name_(field_name)
  {}

  virtual ~UnboundFieldExpr() = default;

  unique_ptr<Expression> copy() const override { return make_unique<UnboundFieldExpr>(table_name_, field_name_); }

  ExprType type() const override { return ExprType::UNBOUND_FIELD; }
  AttrType value_type() const override { return AttrType::UNDEFINED; }

  RC get_value(const Tuple &tuple, Value &value) const override { return RC::INTERNAL; }

  const char *table_name() const { return table_name_.c_str(); }
  const char *field_name() const { return field_name_.c_str(); }

  std::unordered_set<std::string> get_involved_tables() const override
  {
    std::unordered_set<std::string> tables;
    if (!table_name_.empty()) {
      tables.insert(table_name_);
    }
    return tables;
  }

private:
  string table_name_;
  string field_name_;
};

/**
 * @brief 字段表达式
 * @ingroup Expression
 */
class FieldExpr : public Expression
{
public:
  FieldExpr() = default;
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field) {}
  FieldExpr(const Field &field) : field_(field) {}

  virtual ~FieldExpr() = default;

  bool equal(const Expression &other) const override;

  unique_ptr<Expression> copy() const override { return make_unique<FieldExpr>(field_); }

  ExprType type() const override { return ExprType::FIELD; }
  AttrType value_type() const override { return field_.attr_type(); }
  int      value_length() const override { return field_.meta()->len(); }

  Field &field() { return field_; }

  const Field &field() const { return field_; }

  const char *table_name() const { return field_.table_name(); }
  const char *field_name() const { return field_.field_name(); }

  RC get_column(Chunk &chunk, Column &column) override;

  RC get_value(const Tuple &tuple, Value &value) const override;

  // 重写：返回字段所属的表名
  std::unordered_set<std::string> get_involved_tables() const override
  {
    std::unordered_set<std::string> tables;
    if (field_.table_name() && field_.table_name()[0] != '\0') {
      tables.insert(field_.table_name());
    }
    return tables;
  }

private:
  Field field_;
};

/**
 * @brief 常量值表达式
 * @ingroup Expression
 */
class ValueExpr : public Expression
{
public:
  ValueExpr() = default;
  explicit ValueExpr(const Value &value) : value_(value) {}

  virtual ~ValueExpr() = default;

  bool equal(const Expression &other) const override;

  unique_ptr<Expression> copy() const override { return make_unique<ValueExpr>(value_); }

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC get_column(Chunk &chunk, Column &column) override;
  RC try_get_value(Value &value) const override
  {
    value = value_;
    return RC::SUCCESS;
  }

  ExprType type() const override { return ExprType::VALUE; }
  AttrType value_type() const override { return value_.attr_type(); }
  int      value_length() const override { return value_.length(); }

  void         get_value(Value &value) const { value = value_; }
  const Value &get_value() const { return value_; }

private:
  Value value_;
};

/**
 * @brief 类型转换表达式
 * @ingroup Expression
 */
class CastExpr : public Expression
{
public:
  CastExpr(unique_ptr<Expression> child, AttrType cast_type);
  virtual ~CastExpr();

  unique_ptr<Expression> copy() const override { return make_unique<CastExpr>(child_->copy(), cast_type_); }

  ExprType type() const override { return ExprType::CAST; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  RC try_get_value(Value &value) const override;

  AttrType value_type() const override { return cast_type_; }

  unique_ptr<Expression> &child() { return child_; }

private:
  RC cast(const Value &value, Value &cast_value) const;

private:
  unique_ptr<Expression> child_;      ///< 从这个表达式转换
  AttrType               cast_type_;  ///< 想要转换成这个类型
};

/**
 * @brief 比较表达式
 * @ingroup Expression
 */
class ComparisonExpr : public Expression
{
public:
  ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right);
  ComparisonExpr(CompOp comp, unique_ptr<Expression> left, const vector<Value> &right_values);
  // 新增：支持子查询的构造函数
  ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<SelectSqlNode> subquery);
  // 新增：支持 EXISTS 的构造函数（不需要左侧表达式）
  ComparisonExpr(CompOp comp, unique_ptr<SelectSqlNode> subquery);
  virtual ~ComparisonExpr();

  ExprType type() const override { return ExprType::COMPARISON; }
  RC       get_value(const Tuple &tuple, Value &value) const override;
  AttrType value_type() const override { return AttrType::BOOLEANS; }
  CompOp   comp() const { return comp_; }

  unique_ptr<Expression> copy() const override
  {
    if (has_subquery_) {
      // 需要深拷贝子查询节点
      auto subquery_copy = make_unique<SelectSqlNode>(*subquery_);
      // EXISTS/NOT_EXISTS 不需要左侧表达式
      if (comp_ == EXISTS_OP || comp_ == NOT_EXISTS_OP) {
        return make_unique<ComparisonExpr>(comp_, std::move(subquery_copy));
      } else if (left_) {
        return make_unique<ComparisonExpr>(comp_, left_->copy(), std::move(subquery_copy));
      } else {
        LOG_WARN("Subquery comparison without left expression (comp=%d)", comp_);
        return nullptr;
      }
    } else if (has_value_list_) {
      return make_unique<ComparisonExpr>(comp_, left_->copy(), right_values_);
    } else if (right_) {
      return make_unique<ComparisonExpr>(comp_, left_->copy(), right_->copy());
    } else {
      // 这种情况不应该发生，但为了安全起见
      LOG_WARN("ComparisonExpr copy: has_subquery_, has_value_list_ are false and right_ is null");
      return nullptr;
    }
  }

  /**
   * @brief 根据 ComparisonExpr 获得 `select` 结果。
   * select 的长度与chunk 的行数相同，表示每一行在ComparisonExpr 计算后是否会被输出。
   */
  RC eval(Chunk &chunk, vector<uint8_t> &select) override;

  unique_ptr<Expression> &left() { return left_; }
  unique_ptr<Expression> &right() { return right_; }

  /**
   * 尝试在没有tuple的情况下获取当前表达式的值
   * 在优化的时候，可能会使用到
   */
  RC try_get_value(Value &value) const override;

  /**
   * compare the two tuple cells
   * @param value the result of comparison
   */
  RC compare_value(const Value &left, const Value &right, bool &value) const;
  RC compare_with_value_list(const Value &left, const vector<Value> &right_values, bool &result) const;
  
  // 子查询执行方法
  RC execute_subquery(vector<Value> &results) const;
  RC execute_simple_subquery(const SelectSqlNode *select_node, vector<Value> &results) const;
  
  // 🔧 修复：移除缓存管理方法声明
  // void clear_subquery_cache() const;
  
  // 设置会话上下文（用于子查询执行）
  void set_session_context(class Session *session) override;
  
  // 遍历表达式树设置session上下文
  void set_session_context_recursive(class Session *session) override;
  // 🔧 修复：移除缓存清理方法声明
  // void clear_subquery_cache_recursive() override;

  // 重写：收集左右表达式涉及的表
  std::unordered_set<std::string> get_involved_tables() const override
  {
    std::unordered_set<std::string> tables;
    if (left_) {
      auto left_tables = left_->get_involved_tables();
      tables.insert(left_tables.begin(), left_tables.end());
    }
    if (right_) {
      auto right_tables = right_->get_involved_tables();
      tables.insert(right_tables.begin(), right_tables.end());
    }
    return tables;
  }

  template <typename T>
  RC compare_column(const Column &left, const Column &right, vector<uint8_t> &result) const;

private:
  CompOp                 comp_;
  unique_ptr<Expression> left_;
  unique_ptr<Expression> right_;
  vector<Value>          right_values_;  ///< 用于IN操作的值列表
  bool                   has_value_list_ = false;  ///< 是否使用值列表
  
  // 新增：子查询支持
  unique_ptr<SelectSqlNode> subquery_ = nullptr;   ///< 子查询节点（拥有所有权）
  bool                     has_subquery_ = false; ///< 是否使用子查询
  
  // 🔧 修复：移除 mutable 缓存变量，避免跨查询的状态污染
  // mutable vector<Value>    subquery_cache_;
  // mutable bool             cache_valid_ = false;
  
  // 会话上下文（用于子查询执行）
  mutable class Session   *session_ = nullptr;    ///< 会话上下文
};

/**
 * @brief 联结表达式
 * @ingroup Expression
 * 多个表达式使用同一种关系(AND或OR)来联结
 * 当前miniob仅有AND操作
 */
class ConjunctionExpr : public Expression
{
public:
  enum class Type
  {
    AND,
    OR
  };

public:
  ConjunctionExpr(Type type, vector<unique_ptr<Expression>> &children);
  virtual ~ConjunctionExpr() = default;

  unique_ptr<Expression> copy() const override
  {
    vector<unique_ptr<Expression>> children;
    for (auto &child : children_) {
      children.emplace_back(child->copy());
    }
    return make_unique<ConjunctionExpr>(conjunction_type_, children);
  }

  ExprType type() const override { return ExprType::CONJUNCTION; }
  AttrType value_type() const override { return AttrType::BOOLEANS; }
  RC       get_value(const Tuple &tuple, Value &value) const override;

  Type conjunction_type() const { return conjunction_type_; }

  vector<unique_ptr<Expression>> &children() { return children_; }

  // 遍历表达式树设置session上下文
  void set_session_context_recursive(class Session *session) override;

  // 重写：收集所有子表达式涉及的表
  std::unordered_set<std::string> get_involved_tables() const override
  {
    std::unordered_set<std::string> tables;
    for (const auto &child : children_) {
      if (child) {
        auto child_tables = child->get_involved_tables();
        tables.insert(child_tables.begin(), child_tables.end());
      }
    }
    return tables;
  }

private:
  Type                           conjunction_type_;
  vector<unique_ptr<Expression>> children_;
};

/**
 * @brief 算术表达式
 * @ingroup Expression
 */
class ArithmeticExpr : public Expression
{
public:
  enum class Type
  {
    ADD,
    SUB,
    MUL,
    DIV,
    NEGATIVE,
  };

public:
  ArithmeticExpr(Type type, Expression *left, Expression *right);
  ArithmeticExpr(Type type, unique_ptr<Expression> left, unique_ptr<Expression> right);
  virtual ~ArithmeticExpr() = default;

  unique_ptr<Expression> copy() const override
  {
    if (right_) {
      return make_unique<ArithmeticExpr>(arithmetic_type_, left_->copy(), right_->copy());
    } else {
      return make_unique<ArithmeticExpr>(arithmetic_type_, left_->copy(), nullptr);
    }
  }

  bool     equal(const Expression &other) const override;
  ExprType type() const override { return ExprType::ARITHMETIC; }

  AttrType value_type() const override;
  int      value_length() const override
  {
    if (!right_) {
      return left_->value_length();
    }
    return 4;  // sizeof(float) or sizeof(int)
  };

  RC get_value(const Tuple &tuple, Value &value) const override;

  RC get_column(Chunk &chunk, Column &column) override;

  RC try_get_value(Value &value) const override;

  Type arithmetic_type() const { return arithmetic_type_; }

  unique_ptr<Expression> &left() { return left_; }
  unique_ptr<Expression> &right() { return right_; }

  // 遍历表达式树设置session上下文
  void set_session_context_recursive(class Session *session) override;

private:
  RC calc_value(const Value &left_value, const Value &right_value, Value &value) const;

  RC calc_column(const Column &left_column, const Column &right_column, Column &column) const;

  template <bool LEFT_CONSTANT, bool RIGHT_CONSTANT>
  RC execute_calc(const Column &left, const Column &right, Column &result, Type type, AttrType attr_type) const;

private:
  Type                   arithmetic_type_;
  unique_ptr<Expression> left_;
  unique_ptr<Expression> right_;
};

class UnboundAggregateExpr : public Expression
{
public:
  UnboundAggregateExpr(const char *aggregate_name, Expression *child);
  UnboundAggregateExpr(const char *aggregate_name, unique_ptr<Expression> child);
  // 新增：多参数构造函数
  UnboundAggregateExpr(const char *aggregate_name, vector<unique_ptr<Expression>> children);
  virtual ~UnboundAggregateExpr() = default;

  ExprType type() const override { return ExprType::UNBOUND_AGGREGATION; }

  unique_ptr<Expression> copy() const override
  {
    if (is_multi_param()) {
      vector<unique_ptr<Expression>> copied_children;
      for (const auto &child : children_) {
        copied_children.push_back(child->copy());
      }
      return make_unique<UnboundAggregateExpr>(aggregate_name_.c_str(), std::move(copied_children));
    } else {
      return make_unique<UnboundAggregateExpr>(aggregate_name_.c_str(), child_->copy());
    }
  }

  const char *aggregate_name() const { return aggregate_name_.c_str(); }

  unique_ptr<Expression> &child() { return child_; }

  // 新增：多参数相关方法
  bool                                  is_multi_param() const { return !children_.empty(); }
  const vector<unique_ptr<Expression>> &children() const { return children_; }

  RC       get_value(const Tuple &tuple, Value &value) const override { return RC::INTERNAL; }
  AttrType value_type() const override
  {
    if (is_multi_param()) {
      return children_.empty() ? AttrType::UNDEFINED : children_[0]->value_type();
    } else {
      return child_->value_type();
    }
  }

private:
  string                         aggregate_name_;
  unique_ptr<Expression>         child_;     // 保持单参数兼容性
  vector<unique_ptr<Expression>> children_;  // 新增：多参数存储
};

class AggregateExpr : public Expression
{
public:
  enum class Type
  {
    COUNT,
    SUM,
    AVG,
    MAX,
    MIN,
  };

public:
  AggregateExpr(Type type, Expression *child);
  AggregateExpr(Type type, unique_ptr<Expression> child);
  virtual ~AggregateExpr() = default;

  bool equal(const Expression &other) const override;

  unique_ptr<Expression> copy() const override { return make_unique<AggregateExpr>(aggregate_type_, child_->copy()); }

  ExprType type() const override { return ExprType::AGGREGATION; }

  AttrType value_type() const override { return child_->value_type(); }
  int      value_length() const override { return child_->value_length(); }

  RC get_value(const Tuple &tuple, Value &value) const override;

  RC get_column(Chunk &chunk, Column &column) override;

  Type aggregate_type() const { return aggregate_type_; }

  unique_ptr<Expression> &child() { return child_; }

  const unique_ptr<Expression> &child() const { return child_; }

  unique_ptr<Aggregator> create_aggregator() const;

public:
  static RC type_from_string(const char *type_str, Type &type);

private:
  Type                   aggregate_type_;
  unique_ptr<Expression> child_;
};

/**
 * @brief 子查询表达式
 * @ingroup Expression
 * 用于处理标量子查询，如: SELECT * FROM t1 WHERE (SELECT id FROM t2) = t1.id
 */
class SubqueryExpr : public Expression
{
public:
  SubqueryExpr(unique_ptr<SelectSqlNode> subquery);
  virtual ~SubqueryExpr() = default;

  unique_ptr<Expression> copy() const override;

  ExprType type() const override { return ExprType::SUBQUERY; }
  
  // 子查询的值类型需要通过执行子查询或分析其SELECT列表来确定
  // 对于标量子查询，返回第一个SELECT表达式的类型
  AttrType value_type() const override;
  int      value_length() const override;

  RC get_value(const Tuple &tuple, Value &value) const override;
  
  // 设置会话上下文，用于执行子查询
  void set_session_context_recursive(Session *session) override;
  
  const SelectSqlNode* subquery() const { return subquery_.get(); }

private:
  unique_ptr<SelectSqlNode> subquery_;
  mutable Session *session_ = nullptr;
  
  // 🔧 修复：移除 mutable 缓存变量，避免跨查询的状态污染
  // mutable AttrType cached_value_type_ = AttrType::UNDEFINED;
  // mutable bool     type_cached_ = false;
};

/**
 * @brief IN/NOT IN表达式
 * 支持两种形式:
 * 1. expr IN (SELECT ...) 或 expr NOT IN (SELECT ...)
 * 2. expr IN (value1, value2, ...) 或 expr NOT IN (value1, value2, ...)
 */
class InExpr : public Expression
{
public:
  // IN (subquery) 构造函数 - 保留原有功能
  InExpr(bool is_not, std::unique_ptr<Expression> left, std::unique_ptr<Expression> subquery);
  
  // IN (value_list) 构造函数 - 新增功能
  InExpr(bool is_not, std::unique_ptr<Expression> left, std::vector<std::unique_ptr<Expression>> &&value_list);
  
  virtual ~InExpr() = default;

  std::unique_ptr<Expression> copy() const override;

  ExprType type() const override { return ExprType::COMPARISON; }
  AttrType value_type() const override { return AttrType::BOOLEANS; }
  int      value_length() const override { return 4; }

  RC get_value(const Tuple &tuple, Value &value) const override;
  
  void set_session_context_recursive(Session *session) override;

  std::unordered_set<std::string> get_involved_tables() const override;

  /**
   * @brief 绑定内部的UnboundFieldExpr到具体的FieldExpr
   * @param tables 可用的表列表
   */
  RC bind_fields(const std::vector<Table *> &tables);

private:
  bool                                     is_not_;      // true表示NOT IN
  std::unique_ptr<Expression>              left_;        // 左侧表达式
  std::unique_ptr<Expression>              subquery_;    // 子查询（子查询形式）
  std::vector<std::unique_ptr<Expression>> value_list_;  // 值列表（值列表形式）
  bool                                     is_subquery_; // 是否为子查询形式
  mutable Session                         *session_ = nullptr;
};

/**
 * @brief EXISTS/NOT EXISTS表达式
 * 支持: EXISTS (SELECT ...) 或 NOT EXISTS (SELECT ...)
 */
class ExistsExpr : public Expression
{
public:
  ExistsExpr(bool is_not, std::unique_ptr<Expression> subquery);
  virtual ~ExistsExpr() = default;

  std::unique_ptr<Expression> copy() const override;

  ExprType type() const override { return ExprType::COMPARISON; }
  AttrType value_type() const override { return AttrType::BOOLEANS; }
  int      value_length() const override { return 4; }

  RC get_value(const Tuple &tuple, Value &value) const override;
  
  void set_session_context_recursive(Session *session) override;

  std::unordered_set<std::string> get_involved_tables() const override;

private:
  bool                        is_not_;    // true表示NOT EXISTS
  std::unique_ptr<Expression> subquery_;  // 子查询
  mutable Session            *session_ = nullptr;
};

/**
 * @brief 距离函数表达式
 * @ingroup Expression
 */
class DistanceFunctionExpr : public Expression
{
public:
  enum class Type
  {
    L2_DISTANCE,      ///< 欧几里得距离：sqrt(sum((a_i - b_i)^2))
    COSINE_DISTANCE,  ///< 余弦距离：1 - (a·b)/(|a||b|)
    INNER_PRODUCT,    ///< 内积：a·b = sum(a_i * b_i)
  };

public:
  DistanceFunctionExpr(Type type, unique_ptr<Expression> left, unique_ptr<Expression> right);
  virtual ~DistanceFunctionExpr() = default;

  ExprType type() const override { return ExprType::FUNCTION; }
  AttrType value_type() const override { return AttrType::FLOATS; }

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC get_column(Chunk &chunk, Column &column) override;
  
  bool equal(const Expression &other) const override;
  unique_ptr<Expression> copy() const override;

  Type distance_type() const { return distance_type_; }
  const unique_ptr<Expression> &left() const { return left_; }
  const unique_ptr<Expression> &right() const { return right_; }
  unique_ptr<Expression> &left() { return left_; }
  unique_ptr<Expression> &right() { return right_; }

private:
  RC calculate_distance(const Value &left_val, const Value &right_val, Value &result) const;

private:
  Type                   distance_type_;
  unique_ptr<Expression> left_;
  unique_ptr<Expression> right_;
};
