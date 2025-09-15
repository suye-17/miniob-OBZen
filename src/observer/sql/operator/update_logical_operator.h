#pragma once

#include "sql/operator/logical_operator.h"
#include "storage/field/field_meta.h"
#include "common/value.h"

/**
 * @brief UPDATE操作的逻辑算子
 * @ingroup LogicalOperator
 * @details 
 * UpdateLogicalOperator是UPDATE语句在逻辑计划阶段的表示。它存储了执行UPDATE
 * 操作所需的核心信息：目标表、字段名和新值。
 * 
 * 在查询优化阶段，这个逻辑算子会被转换为物理算子(UpdatePhysicalOperator)。
 * 逻辑算子通常会有子算子，如：
 * - TableGetLogicalOperator：用于扫描表中的记录
 * - PredicateLogicalOperator：用于过滤符合WHERE条件的记录
 * 
 * 算子树结构通常为：UpdateLogicalOperator -> PredicateLogicalOperator -> TableGetLogicalOperator
 */
class UpdateLogicalOperator : public LogicalOperator
{
public:
  /**
   * @brief 构造函数，初始化UPDATE逻辑算子
   * @param table 目标表对象
   * @param field_name 要更新的字段名
   * @param new_value 字段的新值
   */
  UpdateLogicalOperator(Table *table, const std::string &field_name, Expression *expression);
  
  /**
   * @brief 虚析构函数
   */
  virtual ~UpdateLogicalOperator() = default;

  /**
   * @brief 获取逻辑算子类型
   * @return UPDATE逻辑算子类型
   */
  LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }

  /**
   * @brief 获取操作类型
   * @return UPDATE逻辑操作类型
   */
  OpType get_op_type() const override { return OpType::LOGICALUPDATE; }

  // 访问器方法，用于获取UPDATE操作的相关信息
  
  /**
   * @brief 获取目标表对象
   * @return 目标表指针
   */
  Table              *table() const { return table_; }
  
  /**
   * @brief 获取要更新的字段名
   * @return 字段名字符串引用
   */
  const std::string  &field_name() const { return field_name_; }
  
  /**
   * @brief 获取更新表达式
   * @return 表达式指针
   */
  Expression         *expression() const { return expression_; }

private:
  Table       *table_;       ///< 目标表对象
  std::string  field_name_;  ///< 要更新的字段名
  Expression  *expression_;  ///< 更新的表达式，支持复杂计算
};