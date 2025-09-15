#include "sql/operator/update_logical_operator.h"

/**
 * @brief 构造UPDATE逻辑算子
 * @details 初始化UPDATE操作所需的核心信息：目标表、字段名和新值
 * 
 * @param table 目标表对象，不能为空
 * @param field_name 要更新的字段名，必须存在于表中
 * @param new_value 字段的新值，类型必须与字段类型匹配
 */
UpdateLogicalOperator::UpdateLogicalOperator(Table *table, const std::string &field_name, Expression *expression)
  : table_(table), field_name_(field_name), expression_(expression)
{
  // 注意：这里不做验证，因为验证已经在UpdateStmt::create()中完成了
  // 逻辑算子只负责存储经过验证的信息
}
