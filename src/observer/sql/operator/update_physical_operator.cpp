#include "sql/operator/update_physical_operator.h"
#include "common/log/log.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/expr/tuple.h"
#include "common/type/data_type.h"
#include "common/type/attr_type.h"
#include <limits>
#include <climits>

/**
 * @brief 开启UPDATE物理算子，执行完整的更新操作
 * @details 
 * 这个方法是UPDATE操作的核心实现，采用"先收集后更新"的策略：
 * 
 * 执行步骤：
 * 1. 开启子算子（表扫描+过滤）来获取需要更新的记录
 * 2. 遍历并收集所有符合条件的记录到内存中
 * 3. 关闭子算子，避免在更新过程中继续扫描
 * 4. 对每条收集的记录执行更新操作
 * 5. 通过事务接口进行实际的数据更新，确保事务一致性
 * 
 * 为什么采用"先收集后更新"策略？
 * - 避免在遍历表的过程中同时修改数据，防止迭代器失效
 * - 确保事务的一致性，所有更新操作在同一个事务上下文中执行
 * - 与DELETE操作保持一致的执行模式
 * 
 * @param trx 事务对象，用于MVCC控制和事务管理
 * @return RC 操作结果码
 */
RC UpdatePhysicalOperator::open(Trx *trx)
{
  // 检查是否有子算子，如果没有子算子，说明是无条件更新，直接返回成功
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  // 获取第一个子算子，通常是表扫描算子或包含过滤条件的算子链
  std::unique_ptr<PhysicalOperator> &child = children_[0];

  // 第一步：开启子算子，开始扫描和过滤数据
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  // 保存事务对象，后续更新操作需要用到
  trx_ = trx;

  // 第二步：收集所有需要更新的记录
  // 使用火山模型，通过next()方法逐条获取符合条件的记录
  while (OB_SUCC(rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    // 将元组转换为记录对象，并存储到记录列表中
    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record   &record    = row_tuple->record();
    records_.emplace_back(std::move(record));
  }

  // 检查循环退出的原因：应该是RECORD_EOF，如果是其他错误则返回错误
  if (rc != RC::RECORD_EOF) {
    LOG_WARN("failed to fetch records from child operator: %s", strrc(rc));
    return rc;
  }

  // 第三步：关闭子算子，释放扫描相关的资源
  child->close();

  // 第四步：验证字段元数据
  // 获取要更新字段的元数据信息，包括类型、偏移量、长度等
  const FieldMeta *field_meta = table_->table_meta().field(field_name_.c_str());
  if (field_meta == nullptr) {
    LOG_WARN("no such field in table. table=%s, field=%s", table_->name(), field_name_.c_str());
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  // 第五步：批量更新所有收集到的记录
  for (Record &record : records_) {
    // 创建新记录的副本，只更新指定字段，其他字段保持不变
    Record new_record = record;
    int offset = field_meta->offset();  // 字段在记录中的偏移量
    int len = field_meta->len();        // 字段的长度
    
    // 为当前记录创建元组，用于表达式计算
    RowTuple row_tuple;
    row_tuple.set_record(&record);
    row_tuple.set_schema(table_, table_->table_meta().field_metas());
    
    // 计算表达式的值
    Value expression_value;
    rc = expression_->get_value(row_tuple, expression_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to evaluate expression for record. table=%s, rid=%s, rc=%s",
               table_->name(), record.rid().to_string().c_str(), strrc(rc));
      return rc;
    }
    
    // 根据字段类型进行相应的类型转换和内存拷贝
    // 提前声明变量，避免作用域问题
    int int_val;
    float float_val;
    std::string string_val;
    
    // 检查表达式结果类型是否与字段类型兼容，如需要则进行类型转换
    Value converted_value;
    if (expression_value.attr_type() != field_meta->type()) {
      // 尝试类型转换
      RC cast_rc = DataType::type_instance(expression_value.attr_type())->cast_to(expression_value, field_meta->type(), converted_value);
      if (cast_rc != RC::SUCCESS) {
        LOG_WARN("failed to cast expression result from %s to %s. table=%s, field=%s",
                 attr_type_to_string(expression_value.attr_type()), attr_type_to_string(field_meta->type()),
                 table_->name(), field_name_.c_str());
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    } else {
      converted_value = expression_value;
    }
    
    // 检查是否为NULL值，需要特殊处理
    if (converted_value.is_null()) {
      // 首先检查字段是否允许NULL值
      if (!field_meta->nullable()) {
        LOG_WARN("field does not allow null values. table=%s, field=%s", 
                 table_->name(), field_name_.c_str());
        return RC::CONSTRAINT_VIOLATION;
      }
      
      // 对于NULL值，使用系统标准的0xFF模式来标记
      // 这与现有的NULL检测逻辑保持一致，不会引入新的Bug
      memset(new_record.data() + offset, 0xFF, len);
    } else {
      // 非NULL值的正常处理
      switch (field_meta->type()) {
        case AttrType::INTS: {
          // 处理整型字段
          int_val = converted_value.get_int();
          memcpy(new_record.data() + offset, &int_val, len);
        } break;
        case AttrType::FLOATS: {
          // 处理浮点型字段
          float_val = converted_value.get_float();
          memcpy(new_record.data() + offset, &float_val, len);
        } break;
        case AttrType::CHARS: {
          // 处理字符型字段，确保内存安全
          string_val = converted_value.get_string();
          
          // 先清零字段内存区域
          memset(new_record.data() + offset, 0, len);
          
          // 计算实际复制长度，避免缓冲区溢出
          size_t copy_len = std::min(static_cast<size_t>(len), string_val.length());
          if (copy_len > 0) {
            memcpy(new_record.data() + offset, string_val.c_str(), copy_len);
          }
        } break;
        case AttrType::DATES: {
          // 处理日期型字段
          // 日期类型在内存中存储为int值
          int_val = converted_value.get_int();
          memcpy(new_record.data() + offset, &int_val, len);
        } break;
        default:
          LOG_WARN("unsupported field type: %d", field_meta->type());
          return RC::INTERNAL;
      }
    }

    // 通过事务接口执行实际的更新操作
    // 这里会处理MVCC版本控制、索引更新、事务日志记录等
    rc = trx_->update_record(table_, record, new_record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to update record: %s", strrc(rc));
      return rc;
    }
    
    // 更新成功，计数器加一
    update_count_++;
  }

  return RC::SUCCESS;
}

/**
 * @brief 获取下一条记录
 * @details 
 * UPDATE操作不返回数据给上层算子，因为UPDATE是DML操作，不像SELECT那样产生结果集。
 * 所有的更新工作都在open()方法中完成，next()总是返回EOF表示没有更多数据。
 * 
 * @return RC::RECORD_EOF 表示没有更多记录
 */
RC UpdatePhysicalOperator::next()
{
  return RC::RECORD_EOF;
}

/**
 * @brief 关闭UPDATE算子，清理资源
 * @details 
 * 清理UPDATE操作中使用的资源，包括：
 * - 清空收集的记录列表
 * - 重置更新计数器
 * - 释放事务引用
 * 
 * @return RC::SUCCESS 总是成功
 */
RC UpdatePhysicalOperator::close()
{
  // 清理收集的记录列表，释放内存
  records_.clear();
  
  // 重置状态变量
  update_count_ = 0;
  trx_ = nullptr;
  
  return RC::SUCCESS;
}