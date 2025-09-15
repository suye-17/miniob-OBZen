#pragma once

#include "sql/operator/physical_operator.h"
#include "common/value.h"

class Trx;
class UpdateStmt;

/**
 * @brief UPDATE操作的物理算子
 * @ingroup PhysicalOperator
 * @details 
 * UpdatePhysicalOperator实现UPDATE语句的具体执行逻辑，采用火山模型(Volcano Model)。
 * 
 * 执行策略：
 * 1. 先收集策略：为了避免在遍历过程中修改数据导致的问题，采用先收集所有需要更新的记录，
 *    然后统一更新的策略。这与DELETE操作的策略相同。
 * 
 * 2. 事务集成：所有更新操作都通过事务接口执行，支持MVCC和事务回滚。
 * 
 * 3. 类型处理：支持不同数据类型(INTS, FLOATS, CHARS)的字段更新，
 *    在更新时进行类型转换和内存拷贝。
 * 
 * 算子执行流程：
 * - open(): 开启子算子，收集所有符合条件的记录，批量执行更新
 * - next(): 总是返回EOF，因为UPDATE不返回数据
 * - close(): 清理资源
 */
class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  /**
   * @brief 构造函数，初始化UPDATE物理算子
   * @param table 目标表对象
   * @param field_name 要更新的字段名
   * @param expression 更新表达式，支持复杂计算
   */
  UpdatePhysicalOperator(Table *table, const std::string &field_name, Expression *expression) 
    : table_(table), field_name_(field_name), expression_(expression) {}

  /**
   * @brief 虚析构函数
   */
  virtual ~UpdatePhysicalOperator() = default;

  /**
   * @brief 获取物理算子类型
   * @return UPDATE物理算子类型
   */
  PhysicalOperatorType type() const override { return PhysicalOperatorType::UPDATE; }

  /**
   * @brief 获取操作类型（用于级联优化器）
   * @return UPDATE操作类型
   */
  OpType get_op_type() const override { return OpType::UPDATE; }

  /**
   * @brief 开启UPDATE算子，执行核心更新逻辑
   * @details 这个方法实现了UPDATE的完整执行流程：
   * 1. 开启子算子（通常是表扫描+条件过滤）
   * 2. 收集所有需要更新的记录
   * 3. 批量执行更新操作
   * 4. 处理类型转换和事务集成
   * 
   * @param trx 事务对象，用于事务控制和MVCC
   * @return RC 操作结果码
   */
  RC open(Trx *trx) override;
  
  /**
   * @brief 获取下一个结果
   * @details UPDATE操作不返回数据，总是返回EOF
   * @return RC::RECORD_EOF 表示没有更多数据
   */
  RC next() override;
  
  /**
   * @brief 关闭算子，清理资源
   * @return RC 操作结果码
   */
  RC close() override;

  /**
   * @brief 获取当前元组
   * @details UPDATE操作不返回数据，总是返回nullptr
   * @return nullptr
   */
  Tuple *current_tuple() override { return nullptr; }

private:
  Table       *table_ = nullptr;     ///< 目标表对象
  std::string  field_name_;          ///< 要更新的字段名
  Expression  *expression_ = nullptr; ///< 更新表达式，支持复杂计算
  Trx         *trx_   = nullptr;     ///< 事务对象，用于事务控制
  std::vector<Record> records_;      ///< 收集的需要更新的记录列表
  int          update_count_ = 0;    ///< 更新的记录数量统计
};
