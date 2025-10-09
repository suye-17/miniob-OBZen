#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/text_type.h"
#include "common/value.h"
#include "common/types.h"

int TextType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::TEXTS, "left value must be TEXTS type");
  if (right.attr_type() == AttrType::TEXTS || right.attr_type() == AttrType::CHARS) {
    return common::compare_string(
        (void *)left.value_.pointer_value_, left.length_, 
        (void *)right.value_.pointer_value_, right.length_);
  }
  LOG_WARN("TextType::compare: unsupported right type %d", static_cast<int>(right.attr_type()));
  return INT32_MAX; 
}

int TextType::cast_cost(AttrType type)
{
  if (type == AttrType::TEXTS) {
    return 0;          
  }
  if (type == AttrType::CHARS) {
    return 1;         
  }
  return INT32_MAX;    
}

RC TextType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
  case AttrType::TEXTS:      
    result = val;
    break;
  case AttrType::CHARS:
    result = val;
    break;
  default:
    return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

RC TextType::set_value_from_str(Value &val, const string &data) const
{
  if (data.length() > TEXT_MAX_LENGTH) {
    LOG_WARN("Text length %zu exceeds maximum %d bytes", data.length(), TEXT_MAX_LENGTH);
    return RC::INVALID_ARGUMENT;
  }
  val.set_text(data.c_str(), data.length());
  return RC::SUCCESS;
}

RC TextType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}