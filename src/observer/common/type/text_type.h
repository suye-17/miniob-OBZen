#pragma once

#include "common/type/data_type.h"

/**
 * @brief 文本类型
 * @ingroup DataType
 */
class TextType : public DataType
{
public:
  TextType() : DataType(AttrType::TEXTS) {}
  virtual ~TextType() = default;

  int compare(const Value &left, const Value &right) const override;

  RC cast_to(const Value &val, AttrType type, Value &result) const override;
  int cast_cost(AttrType type) override;

  RC set_value_from_str(Value &val, const string &data) const override;

  RC to_string(const Value &val, string &result) const override;
};