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

#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/expr/arithmetic_operator.hpp"
#include <cmath>

using namespace std;

RC FieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  LOG_INFO("FieldExpr::get_value: table='%s', field='%s'", 
           table_name() ? table_name() : "NULL", 
           field_name() ? field_name() : "NULL");
           
  TupleCellSpec spec(table_name(), field_name());
  RC rc = tuple.find_cell(spec, value);
  
  LOG_INFO("FieldExpr::get_value: find_cell returned rc=%s", strrc(rc));
  
  return rc;
}

bool FieldExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != ExprType::FIELD) {
    return false;
  }
  const auto &other_field_expr = static_cast<const FieldExpr &>(other);
  return table_name() == other_field_expr.table_name() && field_name() == other_field_expr.field_name();
}

// TODO: 在进行表达式计算时，`chunk` 包含了所有列，因此可以通过 `field_id` 获取到对应列。
// 后续可以优化成在 `FieldExpr` 中存储 `chunk` 中某列的位置信息。
RC FieldExpr::get_column(Chunk &chunk, Column &column)
{
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
  } else {
    column.reference(chunk.column(field().meta()->field_id()));
  }
  return RC::SUCCESS;
}

bool ValueExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != ExprType::VALUE) {
    return false;
  }
  const auto &other_value_expr = static_cast<const ValueExpr &>(other);
  return value_.compare(other_value_expr.get_value()) == 0;
}

RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
  return RC::SUCCESS;
}

RC ValueExpr::get_column(Chunk &chunk, Column &column)
{
  column.init(value_);
  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
CastExpr::CastExpr(unique_ptr<Expression> child, AttrType cast_type) : child_(std::move(child)), cast_type_(cast_type)
{}

CastExpr::~CastExpr() {}

RC CastExpr::cast(const Value &value, Value &cast_value) const
{
  RC rc = RC::SUCCESS;
  if (this->value_type() == value.attr_type()) {
    cast_value = value;
    return rc;
  }
  rc = Value::cast_to(value, cast_type_, cast_value);
  return rc;
}

RC CastExpr::get_value(const Tuple &tuple, Value &result) const
{
  Value value;
  RC rc = child_->get_value(tuple, value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, result);
}

RC CastExpr::try_get_value(Value &result) const
{
  Value value;
  RC rc = child_->try_get_value(value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, result);
}

////////////////////////////////////////////////////////////////////////////////

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : comp_(comp), left_(std::move(left)), right_(std::move(right))
{
}

ComparisonExpr::~ComparisonExpr() {}

RC ComparisonExpr::compare_value(const Value &left, const Value &right, bool &result) const
{
  RC  rc         = RC::SUCCESS;
  int cmp_result = left.compare(right);
  result         = false;
  switch (comp_) {
    case EQUAL_TO: {
      result = (0 == cmp_result);
    } break;
    case LESS_EQUAL: {
      result = (cmp_result <= 0);
    } break;
    case NOT_EQUAL: {
      result = (cmp_result != 0);
    } break;
    case LESS_THAN: {
      result = (cmp_result < 0);
    } break;
    case GREAT_EQUAL: {
      result = (cmp_result >= 0);
    } break;
    case GREAT_THAN: {
      result = (cmp_result > 0);
    } break;
    default: {
      LOG_WARN("unsupported comparison. %d", comp_);
      rc = RC::INTERNAL;
    } break;
  }

  return rc;
}

RC ComparisonExpr::try_get_value(Value &cell) const
{
  if (left_->type() == ExprType::VALUE && right_->type() == ExprType::VALUE) {
    ValueExpr *  left_value_expr  = static_cast<ValueExpr *>(left_.get());
    ValueExpr *  right_value_expr = static_cast<ValueExpr *>(right_.get());
    const Value &left_cell        = left_value_expr->get_value();
    const Value &right_cell       = right_value_expr->get_value();

    bool value = false;
    RC   rc    = compare_value(left_cell, right_cell, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to compare tuple cells. rc=%s", strrc(rc));
    } else {
      cell.set_boolean(value);
    }
    return rc;
  }

  return RC::INVALID_ARGUMENT;
}

RC ComparisonExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }

  bool bool_value = false;

  rc = compare_value(left_value, right_value, bool_value);
  if (rc == RC::SUCCESS) {
    value.set_boolean(bool_value);
  }
  return rc;
}

RC ComparisonExpr::eval(Chunk &chunk, vector<uint8_t> &select)
{
  RC     rc = RC::SUCCESS;
  Column left_column;
  Column right_column;

  rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  if (left_column.attr_type() != right_column.attr_type()) {
    LOG_WARN("cannot compare columns with different types");
    return RC::INTERNAL;
  }
  if (left_column.attr_type() == AttrType::INTS) {
    rc = compare_column<int>(left_column, right_column, select);
  } else if (left_column.attr_type() == AttrType::FLOATS) {
    rc = compare_column<float>(left_column, right_column, select);
  } else {
    // TODO: support string compare
    LOG_WARN("unsupported data type %d", left_column.attr_type());
    return RC::INTERNAL;
  }
  return rc;
}

template <typename T>
RC ComparisonExpr::compare_column(const Column &left, const Column &right, vector<uint8_t> &result) const
{
  RC rc = RC::SUCCESS;

  bool left_const  = left.column_type() == Column::Type::CONSTANT_COLUMN;
  bool right_const = right.column_type() == Column::Type::CONSTANT_COLUMN;
  if (left_const && right_const) {
    compare_result<T, true, true>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  } else if (left_const && !right_const) {
    compare_result<T, true, false>((T *)left.data(), (T *)right.data(), right.count(), result, comp_);
  } else if (!left_const && right_const) {
    compare_result<T, false, true>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  } else {
    compare_result<T, false, false>((T *)left.data(), (T *)right.data(), left.count(), result, comp_);
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
ConjunctionExpr::ConjunctionExpr(Type type, vector<unique_ptr<Expression>> &children)
    : conjunction_type_(type), children_(std::move(children))
{}

RC ConjunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    value.set_boolean(true);
    return rc;
  }

  Value tmp_value;
  for (const unique_ptr<Expression> &expr : children_) {
    rc = expr->get_value(tuple, tmp_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value by child expression. rc=%s", strrc(rc));
      return rc;
    }
    bool bool_value = tmp_value.get_boolean();
    if ((conjunction_type_ == Type::AND && !bool_value) || (conjunction_type_ == Type::OR && bool_value)) {
      value.set_boolean(bool_value);
      return rc;
    }
  }

  bool default_value = (conjunction_type_ == Type::AND);
  value.set_boolean(default_value);
  return rc;
}

////////////////////////////////////////////////////////////////////////////////

ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, Expression *left, Expression *right)
    : arithmetic_type_(type), left_(left), right_(right)
{}
ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : arithmetic_type_(type), left_(std::move(left)), right_(std::move(right))
{}

bool ArithmeticExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (type() != other.type()) {
    return false;
  }
  auto &other_arith_expr = static_cast<const ArithmeticExpr &>(other);
  return arithmetic_type_ == other_arith_expr.arithmetic_type() && left_->equal(*other_arith_expr.left_) &&
         right_->equal(*other_arith_expr.right_);
}
AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  // 如果任一操作数是向量类型，结果也是向量类型
  if (left_->value_type() == AttrType::VECTORS || right_->value_type() == AttrType::VECTORS) {
    return AttrType::VECTORS;
  }

  if (left_->value_type() == AttrType::INTS && right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }

  return AttrType::FLOATS;
}

RC ArithmeticExpr::calc_value(const Value &left_value, const Value &right_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  const AttrType target_type = value_type();
  value.set_type(target_type);

  // 如果目标类型是向量，需要确保操作数也都是向量类型
  Value left_converted = left_value;
  Value right_converted = right_value;
  
  if (target_type == AttrType::VECTORS) {
    // 转换左操作数
    if (left_value.attr_type() != AttrType::VECTORS) {
      rc = DataType::type_instance(AttrType::VECTORS)->cast_to(left_value, AttrType::VECTORS, left_converted);
      if (rc != RC::SUCCESS) {
        LOG_WARN("Failed to convert left operand to vector type");
        return rc;
      }
    }
    
    // 转换右操作数
    if (right_value.attr_type() != AttrType::VECTORS) {
      rc = DataType::type_instance(AttrType::VECTORS)->cast_to(right_value, AttrType::VECTORS, right_converted);
      if (rc != RC::SUCCESS) {
        LOG_WARN("Failed to convert right operand to vector type");
        return rc;
      }
    }
  }

  switch (arithmetic_type_) {
    case Type::ADD: {
      Value::add(left_converted, right_converted, value);
    } break;

    case Type::SUB: {
      Value::subtract(left_converted, right_converted, value);
    } break;

    case Type::MUL: {
      Value::multiply(left_converted, right_converted, value);
    } break;

    case Type::DIV: {
      Value::divide(left_value, right_value, value);
    } break;

    case Type::NEGATIVE: {
      Value::negative(left_value, value);
    } break;

    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported arithmetic type. %d", arithmetic_type_);
    } break;
  }
  return rc;
}

template <bool LEFT_CONSTANT, bool RIGHT_CONSTANT>
RC ArithmeticExpr::execute_calc(
    const Column &left, const Column &right, Column &result, Type type, AttrType attr_type) const
{
  RC rc = RC::SUCCESS;
  switch (type) {
    case Type::ADD: {
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, AddOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, AddOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
    } break;
    case Type::SUB:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, SubtractOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, SubtractOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::MUL:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, MultiplyOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, MultiplyOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::DIV:
      if (attr_type == AttrType::INTS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, int, DivideOperator>(
            (int *)left.data(), (int *)right.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        binary_operator<LEFT_CONSTANT, RIGHT_CONSTANT, float, DivideOperator>(
            (float *)left.data(), (float *)right.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    case Type::NEGATIVE:
      if (attr_type == AttrType::INTS) {
        unary_operator<LEFT_CONSTANT, int, NegateOperator>((int *)left.data(), (int *)result.data(), result.capacity());
      } else if (attr_type == AttrType::FLOATS) {
        unary_operator<LEFT_CONSTANT, float, NegateOperator>(
            (float *)left.data(), (float *)result.data(), result.capacity());
      } else {
        rc = RC::UNIMPLEMENTED;
      }
      break;
    default: rc = RC::UNIMPLEMENTED; break;
  }
  if (rc == RC::SUCCESS) {
    result.set_count(result.capacity());
  }
  return rc;
}

RC ArithmeticExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  return calc_value(left_value, right_value, value);
}

RC ArithmeticExpr::get_column(Chunk &chunk, Column &column)
{
  RC rc = RC::SUCCESS;
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
    return rc;
  }
  Column left_column;
  Column right_column;

  rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get column of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get column of right expression. rc=%s", strrc(rc));
    return rc;
  }
  return calc_column(left_column, right_column, column);
}

RC ArithmeticExpr::calc_column(const Column &left_column, const Column &right_column, Column &column) const
{
  RC rc = RC::SUCCESS;

  const AttrType target_type = value_type();
  column.init(target_type, left_column.attr_len(), max(left_column.count(), right_column.count()));
  bool left_const  = left_column.column_type() == Column::Type::CONSTANT_COLUMN;
  bool right_const = right_column.column_type() == Column::Type::CONSTANT_COLUMN;
  if (left_const && right_const) {
    column.set_column_type(Column::Type::CONSTANT_COLUMN);
    rc = execute_calc<true, true>(left_column, right_column, column, arithmetic_type_, target_type);
  } else if (left_const && !right_const) {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<true, false>(left_column, right_column, column, arithmetic_type_, target_type);
  } else if (!left_const && right_const) {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<false, true>(left_column, right_column, column, arithmetic_type_, target_type);
  } else {
    column.set_column_type(Column::Type::NORMAL_COLUMN);
    rc = execute_calc<false, false>(left_column, right_column, column, arithmetic_type_, target_type);
  }
  return rc;
}

RC ArithmeticExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->try_get_value(right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  return calc_value(left_value, right_value, value);
}

////////////////////////////////////////////////////////////////////////////////

UnboundAggregateExpr::UnboundAggregateExpr(const char *aggregate_name, Expression *child)
    : aggregate_name_(aggregate_name), child_(child)
{}

UnboundAggregateExpr::UnboundAggregateExpr(const char *aggregate_name, unique_ptr<Expression> child)
    : aggregate_name_(aggregate_name), child_(std::move(child))
{}

////////////////////////////////////////////////////////////////////////////////
AggregateExpr::AggregateExpr(Type type, Expression *child) : aggregate_type_(type), child_(child) {}

AggregateExpr::AggregateExpr(Type type, unique_ptr<Expression> child) : aggregate_type_(type), child_(std::move(child))
{}

RC AggregateExpr::get_column(Chunk &chunk, Column &column)
{
  RC rc = RC::SUCCESS;
  if (pos_ != -1) {
    column.reference(chunk.column(pos_));
  } else {
    rc = RC::INTERNAL;
  }
  return rc;
}

bool AggregateExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != type()) {
    return false;
  }
  const AggregateExpr &other_aggr_expr = static_cast<const AggregateExpr &>(other);
  return aggregate_type_ == other_aggr_expr.aggregate_type() && child_->equal(*other_aggr_expr.child());
}

unique_ptr<Aggregator> AggregateExpr::create_aggregator() const
{
  unique_ptr<Aggregator> aggregator;
  switch (aggregate_type_) {
    case Type::SUM: {
      aggregator = make_unique<SumAggregator>();
      break;
    }
    default: {
      ASSERT(false, "unsupported aggregate type");
      break;
    }
  }
  return aggregator;
}

RC AggregateExpr::get_value(const Tuple &tuple, Value &value) const
{
  return tuple.find_cell(TupleCellSpec(name()), value);
}

RC AggregateExpr::type_from_string(const char *type_str, AggregateExpr::Type &type)
{
  RC rc = RC::SUCCESS;
  if (0 == strcasecmp(type_str, "count")) {
    type = Type::COUNT;
  } else if (0 == strcasecmp(type_str, "sum")) {
    type = Type::SUM;
  } else if (0 == strcasecmp(type_str, "avg")) {
    type = Type::AVG;
  } else if (0 == strcasecmp(type_str, "max")) {
    type = Type::MAX;
  } else if (0 == strcasecmp(type_str, "min")) {
    type = Type::MIN;
  } else {
    rc = RC::INVALID_ARGUMENT;
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
// DistanceFunctionExpr

DistanceFunctionExpr::DistanceFunctionExpr(Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : distance_type_(type), left_(std::move(left)), right_(std::move(right))
{}

RC DistanceFunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;

  LOG_INFO("DistanceFunctionExpr::get_value: getting left operand (should be embedding field)");
  
  // 检查left_是什么类型的表达式
  if (left_->type() == ExprType::FIELD) {
    const FieldExpr* field_expr = static_cast<const FieldExpr*>(left_.get());
    LOG_INFO("DistanceFunctionExpr: left_ is FieldExpr, table='%s', field='%s'",
             field_expr->table_name() ? field_expr->table_name() : "NULL",
             field_expr->field_name() ? field_expr->field_name() : "NULL");
  } else {
    LOG_INFO("DistanceFunctionExpr: left_ is not FieldExpr, type=%d", static_cast<int>(left_->type()));
  }

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get left value. rc=%s", strrc(rc));
    return rc;
  }
  
  LOG_INFO("DistanceFunctionExpr: left value obtained successfully, type=%d", 
           static_cast<int>(left_value.attr_type()));

  LOG_INFO("DistanceFunctionExpr::get_value: getting right operand");
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get right value. rc=%s", strrc(rc));
    return rc;
  }
  
  LOG_INFO("DistanceFunctionExpr: right value obtained successfully, type=%d", 
           static_cast<int>(right_value.attr_type()));

  return calculate_distance(left_value, right_value, value);
}

RC DistanceFunctionExpr::get_column(Chunk &chunk, Column &column)
{
  // 获取左右操作数的列数据
  Column left_column;
  Column right_column;
  
  RC rc = left_->get_column(chunk, left_column);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  
  rc = right_->get_column(chunk, right_column);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  
  // 计算距离结果
  int row_num = chunk.rows();
  column.init(AttrType::FLOATS, sizeof(float), row_num);
  
  for (int i = 0; i < row_num; i++) {
    Value left_value = left_column.get_value(i);
    Value right_value = right_column.get_value(i);
    Value result;
    
    rc = calculate_distance(left_value, right_value, result);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    
    float distance_val = result.get_float();
    rc = column.append_one(reinterpret_cast<char*>(&distance_val));
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  
  return RC::SUCCESS;
}

bool DistanceFunctionExpr::equal(const Expression &other) const
{
  if (this == &other) {
    return true;
  }
  if (other.type() != type()) {
    return false;
  }
  
  const DistanceFunctionExpr &other_distance_expr = static_cast<const DistanceFunctionExpr &>(other);
  return distance_type_ == other_distance_expr.distance_type_ &&
         left_->equal(*other_distance_expr.left_) &&
         right_->equal(*other_distance_expr.right_);
}

unique_ptr<Expression> DistanceFunctionExpr::copy() const
{
  return make_unique<DistanceFunctionExpr>(distance_type_, left_->copy(), right_->copy());
}

RC DistanceFunctionExpr::calculate_distance(const Value &left_val, const Value &right_val, Value &result) const
{
  // 将操作数转换为向量类型
  Value left_vector = left_val;
  Value right_vector = right_val;
  
  // 转换左操作数
  if (left_val.attr_type() != AttrType::VECTORS) {
    RC rc = DataType::type_instance(AttrType::VECTORS)->cast_to(left_val, AttrType::VECTORS, left_vector);
    if (rc != RC::SUCCESS) {
      LOG_WARN("Failed to convert left operand to vector type: %d", static_cast<int>(left_val.attr_type()));
      return rc;
    }
  }
  
  // 转换右操作数
  if (right_val.attr_type() != AttrType::VECTORS) {
    RC rc = DataType::type_instance(AttrType::VECTORS)->cast_to(right_val, AttrType::VECTORS, right_vector);
    if (rc != RC::SUCCESS) {
      LOG_WARN("Failed to convert right operand to vector type: %d", static_cast<int>(right_val.attr_type()));
      return rc;
    }
  }
  
  const vector<float> &left_vec = left_vector.get_vector();
  const vector<float> &right_vec = right_vector.get_vector();
  
  // 检查维度是否相同
  if (left_vec.size() != right_vec.size()) {
    LOG_WARN("Vector dimensions mismatch for distance calculation: %zu vs %zu", 
             left_vec.size(), right_vec.size());
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
  
  float distance = 0.0f;
  
  switch (distance_type_) {
    case Type::L2_DISTANCE: {
      // 欧几里得距离：sqrt(sum((a_i - b_i)^2))
      float sum_squared = 0.0f;
      for (size_t i = 0; i < left_vec.size(); i++) {
        float diff = left_vec[i] - right_vec[i];
        sum_squared += diff * diff;
      }
      distance = sqrt(sum_squared);
    } break;
    
    case Type::COSINE_DISTANCE: {
      // 余弦距离：1 - (a·b)/(|a||b|)
      float dot_product = 0.0f;
      float left_magnitude = 0.0f;
      float right_magnitude = 0.0f;
      
      for (size_t i = 0; i < left_vec.size(); i++) {
        dot_product += left_vec[i] * right_vec[i];
        left_magnitude += left_vec[i] * left_vec[i];
        right_magnitude += right_vec[i] * right_vec[i];
      }
      
      left_magnitude = sqrt(left_magnitude);
      right_magnitude = sqrt(right_magnitude);
      
      if (left_magnitude == 0.0f || right_magnitude == 0.0f) {
        distance = 1.0f;  // 如果有零向量，余弦距离为1
      } else {
        float cosine_similarity = dot_product / (left_magnitude * right_magnitude);
        distance = 1.0f - cosine_similarity;
      }
    } break;
    
    case Type::INNER_PRODUCT: {
      // 内积：sum(a_i * b_i)
      for (size_t i = 0; i < left_vec.size(); i++) {
        distance += left_vec[i] * right_vec[i];
      }
    } break;
    
    default: {
      LOG_WARN("Unknown distance function type: %d", static_cast<int>(distance_type_));
      return RC::INTERNAL;
    }
  }
  
  result.set_float(distance);
  return RC::SUCCESS;
}
