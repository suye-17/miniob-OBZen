#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/vector_type.h"
#include "common/value.h"
#include "observer/common/utils.h"
#include <sstream>
#include <iomanip>

int VectorType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::VECTORS, "left type is not vector");
  ASSERT(right.attr_type() == AttrType::VECTORS, "right type is not vector");
  
  const vector<float> &left_vec = left.get_vector();
  const vector<float> &right_vec = right.get_vector();
  
  // 检查维度是否相同
  if (left_vec.size() != right_vec.size()) {
    LOG_WARN("Vector dimensions mismatch for comparison: %zu vs %zu", left_vec.size(), right_vec.size());
    // 维度不同时，按维度大小比较
    if (left_vec.size() < right_vec.size()) return -1;
    if (left_vec.size() > right_vec.size()) return 1;
    return 0;
  }
  
  // 字典序比较：逐个元素从左到右比较
  for (size_t i = 0; i < left_vec.size(); i++) {
    if (left_vec[i] < right_vec[i]) {
      return -1;  // left < right
    } else if (left_vec[i] > right_vec[i]) {
      return 1;   // left > right
    }
    // 如果 left_vec[i] == right_vec[i]，继续比较下一个元素
  }
  
  // 所有元素都相等
  return 0;
}

RC VectorType::add(const Value &left, const Value &right, Value &result) const
{
  ASSERT(left.attr_type() == AttrType::VECTORS, "left type is not vector");
  ASSERT(right.attr_type() == AttrType::VECTORS, "right type is not vector");
  
  const vector<float> &left_vec = left.get_vector();
  const vector<float> &right_vec = right.get_vector();
  
  // 检查维度是否相同
  if (left_vec.size() != right_vec.size()) {
    LOG_WARN("Vector dimensions mismatch: %zu vs %zu", left_vec.size(), right_vec.size());
    return RC::INVALID_ARGUMENT;
  }
  
  // 执行逐元素加法
  vector<float> result_vec;
  result_vec.reserve(left_vec.size());
  
  for (size_t i = 0; i < left_vec.size(); ++i) {
    result_vec.push_back(left_vec[i] + right_vec[i]);
  }
  
  result.set_vector(result_vec);
  return RC::SUCCESS;
}

RC VectorType::subtract(const Value &left, const Value &right, Value &result) const
{
  ASSERT(left.attr_type() == AttrType::VECTORS, "left type is not vector");
  ASSERT(right.attr_type() == AttrType::VECTORS, "right type is not vector");
  
  const vector<float> &left_vec = left.get_vector();
  const vector<float> &right_vec = right.get_vector();
  
  // 检查维度是否相同
  if (left_vec.size() != right_vec.size()) {
    LOG_WARN("Vector dimensions mismatch: %zu vs %zu", left_vec.size(), right_vec.size());
    return RC::INVALID_ARGUMENT;
  }
  
  // 执行逐元素减法
  vector<float> result_vec;
  result_vec.reserve(left_vec.size());
  
  for (size_t i = 0; i < left_vec.size(); ++i) {
    result_vec.push_back(left_vec[i] - right_vec[i]);
  }
  
  result.set_vector(result_vec);
  return RC::SUCCESS;
}

RC VectorType::multiply(const Value &left, const Value &right, Value &result) const
{
  ASSERT(left.attr_type() == AttrType::VECTORS, "left type is not vector");
  ASSERT(right.attr_type() == AttrType::VECTORS, "right type is not vector");
  
  const vector<float> &left_vec = left.get_vector();
  const vector<float> &right_vec = right.get_vector();
  
  // 检查维度是否相同
  if (left_vec.size() != right_vec.size()) {
    LOG_WARN("Vector dimensions mismatch: %zu vs %zu", left_vec.size(), right_vec.size());
    return RC::INVALID_ARGUMENT;
  }
  
  // 执行逐元素乘法
  vector<float> result_vec;
  result_vec.reserve(left_vec.size());
  
  for (size_t i = 0; i < left_vec.size(); ++i) {
    result_vec.push_back(left_vec[i] * right_vec[i]);
  }
  
  result.set_vector(result_vec);
  return RC::SUCCESS;
}

RC VectorType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::VECTORS: {
      // 从其他类型转换为向量类型
      if (val.attr_type() == AttrType::CHARS) {
        // 从字符串解析向量："[1,2,3]" -> vector<float>{1.0, 2.0, 3.0}
        std::vector<float> elements;
        RC rc = parse_vector_literal(val.get_string().c_str(), elements);
        if (rc != RC::SUCCESS) {
          LOG_WARN("Failed to parse vector from string: %s", val.get_string().c_str());
          return rc;
        }
        result.set_vector(elements);
        return RC::SUCCESS;
      } else {
        LOG_WARN("Unsupported cast from type %d to VECTORS", static_cast<int>(val.attr_type()));
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
    case AttrType::CHARS: {
      // 从向量类型转换为字符串类型
      if (val.attr_type() == AttrType::VECTORS) {
        std::string str_result;
        RC rc = to_string(val, str_result);
        if (rc != RC::SUCCESS) {
          LOG_WARN("Failed to convert vector to string");
          return rc;
        }
        result.set_string(str_result.c_str());
        return RC::SUCCESS;
      } else {
        LOG_WARN("Unsupported cast from type %d to CHARS", static_cast<int>(val.attr_type()));
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
    default:
      LOG_WARN("Unsupported cast from VECTORS to type %d", static_cast<int>(type));
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }
}

RC VectorType::to_string(const Value &val, std::string &result) const
{
  ASSERT(val.attr_type() == AttrType::VECTORS, "val type is not vector");
  
  const vector<float> &vec = val.get_vector();
  
  if (vec.empty()) {
    result = "[]";
    return RC::SUCCESS;
  }
  
  std::ostringstream oss;
  oss << "[";
  
  for (size_t i = 0; i < vec.size(); ++i) {
    if (i > 0) {
      oss << ",";
    }
    
    // 简单格式化：去掉末尾的0和小数点
    float val_f = vec[i];
    if (val_f == static_cast<int>(val_f)) {
      // 整数，直接输出
      oss << static_cast<int>(val_f);
    } else {
      // 浮点数，保留2位小数
      oss << std::fixed << std::setprecision(2) << val_f;
    }
  }
  
  oss << "]";
  result = oss.str();
  
  return RC::SUCCESS;
}

int VectorType::cast_cost(AttrType type)
{
  if (type == AttrType::VECTORS) {
    return 0;  
  }
  if (type == AttrType::CHARS) {
    return 1;  
  }
  return INT32_MAX;  
}
