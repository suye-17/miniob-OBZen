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
// Created by Wangyunlai on 2022/5/22.
//

#pragma once

#include "common/lang/unordered_map.h"
#include "common/lang/vector.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include "storage/field/field.h"

class Db;
class Table;
class FieldMeta;

/**
 * @brief 过滤对象 - 表示WHERE条件中的一个操作数
 * @details 支持三种类型：字段引用、常量值、复杂表达式
 * 使用现代C++设计模式，具有完整的内存管理和类型安全
 */
struct FilterObj
{
  enum class Type
  {
    FIELD,      // 字段引用 (如: id, name)
    VALUE,      // 常量值 (如: 123, 'hello')
    EXPRESSION  // 复杂表达式 (如: id+1, (col1*2)/3)
  };

  Type        type_;
  Field       field;
  Value       value;
  Expression *expression;

  FilterObj() : type_(Type::VALUE), expression(nullptr) {}

  // 析构函数：释放表达式内存
  ~FilterObj()
  {
    if (expression != nullptr) {
      delete expression;
      expression = nullptr;
    }
  }

  // 拷贝构造函数：深拷贝表达式
  FilterObj(const FilterObj &other) : type_(other.type_), field(other.field), value(other.value), expression(nullptr)
  {
    if (other.expression != nullptr) {
      expression = other.expression->copy().release();
    }
  }

  // 拷贝赋值运算符：深拷贝表达式
  FilterObj &operator=(const FilterObj &other)
  {
    if (this != &other) {
      // 先释放当前的表达式
      delete expression;

      // 拷贝数据
      type_ = other.type_;
      field = other.field;
      value = other.value;

      // 深拷贝表达式
      if (other.expression != nullptr) {
        expression = other.expression->copy().release();
      } else {
        expression = nullptr;
      }
    }
    return *this;
  }

  // 移动构造函数
  FilterObj(FilterObj &&other) noexcept
      : type_(other.type_), field(std::move(other.field)), value(std::move(other.value)), expression(other.expression)
  {
    other.expression = nullptr;
  }

  // 移动赋值运算符
  FilterObj &operator=(FilterObj &&other) noexcept
  {
    if (this != &other) {
      // 先释放当前的表达式
      delete expression;

      // 移动数据
      type_      = other.type_;
      field      = std::move(other.field);
      value      = std::move(other.value);
      expression = other.expression;

      // 清空源对象
      other.expression = nullptr;
    }
    return *this;
  }

  void init_attr(const Field &field)
  {
    clear_expression();
    type_       = Type::FIELD;
    this->field = field;
  }

  void init_value(const Value &value)
  {
    clear_expression();
    type_       = Type::VALUE;
    this->value = value;
  }

  void init_expression(Expression *expr)
  {
    clear_expression();
    type_      = Type::EXPRESSION;
    expression = expr;
  }

  // 查询方法
  bool is_attr() const { return type_ == Type::FIELD; }
  bool is_value() const { return type_ == Type::VALUE; }
  bool is_expression() const { return type_ == Type::EXPRESSION; }
  Type get_type() const { return type_; }

private:
  void clear_expression()
  {
    if (expression != nullptr) {
      delete expression;
      expression = nullptr;
    }
  }
};

class FilterUnit
{
public:
  FilterUnit() = default;
  ~FilterUnit() {}

  void set_comp(CompOp comp) { comp_ = comp; }

  CompOp comp() const { return comp_; }

  void set_left(const FilterObj &obj) { left_ = obj; }
  void set_right(const FilterObj &obj) { right_ = obj; }

  const FilterObj &left() const { return left_; }
  const FilterObj &right() const { return right_; }

private:
  CompOp    comp_ = NO_OP;
  FilterObj left_;
  FilterObj right_;
};

/**
 * @brief Filter/谓词/过滤语句
 * @ingroup Statement
 */
class FilterStmt
{
public:
  FilterStmt() = default;
  virtual ~FilterStmt();

public:
  const vector<FilterUnit *> &filter_units() const { return filter_units_; }

public:
  static RC create(Db *db, Table *default_table, unordered_map<string, Table *> *tables,
      const ConditionSqlNode *conditions, int condition_num, FilterStmt *&stmt);

  static RC create_filter_unit(Db *db, Table *default_table, unordered_map<string, Table *> *tables,
      const ConditionSqlNode &condition, FilterUnit *&filter_unit);

private:
  /**
   * @brief 表达式到FilterObj的统一转换函数
   * @param expr 待转换的表达式指针
   * @param default_table 默认表上下文
   * @param filter_obj 输出的FilterObj对象
   * @param side_name 调试用的边名称("left"/"right")
   * @return RC 转换结果
   * @details 智能处理不同类型的表达式：
   *   - UNBOUND_FIELD: 直接绑定到表字段
   *   - VALUE: 静态求值为常量
   *   - 其他: 尝试静态求值，失败则保存表达式副本
   */
  static RC convert_expression_to_filter_obj(Expression *expr, Table *default_table,
      unordered_map<string, Table *> *tables, FilterObj &filter_obj, const char *side_name);

  /**
   * @brief 处理单独表达式条件（WHERE expression）
   * @param left_obj 左侧FilterObj（表达式结果）
   * @param right_obj 右侧FilterObj（将被设置）
   * @param filter_unit 要配置的FilterUnit
   * @return RC 处理结果
   * @details 对于单独表达式：
   *   - 如果是NULL值，创建恒假条件（1=0）
   *   - 否则创建 expression=true 的条件
   */
  static RC handle_single_expression_condition(FilterObj &left_obj, FilterObj &right_obj, FilterUnit *filter_unit);

  /**
   * @brief 创建恒假条件（用于NULL值处理）
   */
  static RC create_always_false_condition(FilterObj &left_obj, FilterObj &right_obj, FilterUnit *filter_unit);

  /**
   * @brief 创建表达式等于真值的条件
   */
  static RC create_expression_equals_true_condition(FilterObj &left_obj, FilterObj &right_obj, FilterUnit *filter_unit);

  vector<FilterUnit *> filter_units_;  ///< 过滤单元列表，默认AND关系
};
