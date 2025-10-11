/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/type/char_type.h"
#include "common/type/float_type.h"
#include "common/type/integer_type.h"
#include "common/type/date_type.h"
#include "common/type/data_type.h"
#include "common/type/vector_type.h"
#include "common/type/boolean_type.h"
#include "common/value.h"

array<unique_ptr<DataType>, static_cast<int>(AttrType::MAXTYPE)> DataType::type_instances_ = {
    make_unique<DataType>(AttrType::UNDEFINED),
    make_unique<CharType>(),
    make_unique<IntegerType>(),
    make_unique<FloatType>(),
    make_unique<DateType>(),
    make_unique<VectorType>(),
    make_unique<BooleanType>(),
};

/**
 * @brief 基础DataType类的cast_to实现，专门处理UNDEFINED类型的转换
 * @details 当源值是UNDEFINED类型（表示NULL）时，将其转换为目标类型的NULL值
 * 这解决了UPDATE语句中 SET field=null 时的类型转换问题
 */
RC DataType::cast_to(const Value &val, AttrType type, Value &result) const
{
  // 只有当前DataType实例是UNDEFINED类型时才处理NULL值转换
  if (attr_type_ == AttrType::UNDEFINED) {
    // 创建目标类型的NULL值
    result.set_null();
    result.set_type(type);  // 设置目标类型
    return RC::SUCCESS;
  }

  // 对于其他类型，返回不支持，让具体的子类处理
  return RC::UNSUPPORTED;
}