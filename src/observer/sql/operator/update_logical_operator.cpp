#include "sql/operator/update_logical_operator.h"

/**
 * @brief 构造UPDATE逻辑算子
 * @details 初始化UPDATE操作所需的核心信息：目标表、字段名和新值
 * 
 * @param table 目标表对象，不能为空
  * @param field_names 要更新的字段名集合，必须存在于表中
 * @param expressions 更新的表达式集合，支持复杂计算
 */
UpdateLogicalOperator::UpdateLogicalOperator(Table *table, const std::vector<std::string> &field_names, const std::vector<Expression*> &expressions)
  : table_(table), field_names_(field_names), expressions_(expressions)
{
  // 注意：这里不做验证，因为验证已经在UpdateStmt::create()中完成了
  // 逻辑算子只负责存储经过验证的信息
}
