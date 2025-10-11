#include "common/type/date_type.h"
#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/value.h"
#include "common/log/log.h"
#include <iomanip>

int DateType::compare(const Value &left, const Value &right) const
{
  return common::compare_int((void *)&left.value_.int_value_, (void *)&right.value_.int_value_);
}

RC DateType::to_string(const Value &val, string &result) const
{
  std::ostringstream oss;
  int                year  = val.value_.int_value_ / 10000;
  int                month = (val.value_.int_value_ % 10000) / 100;
  int                day   = val.value_.int_value_ % 100;
  oss << std::setfill('0') << std::setw(4) << year << '-' << std::setw(2) << month << '-' << std::setw(2) << day;
  result = oss.str();
  return RC::SUCCESS;
}

int DateType::cast_cost(AttrType type)
{
  if (type == AttrType::DATES || type == AttrType::INTS) {
    return 0;
  }
  return INT32_MAX;
}
