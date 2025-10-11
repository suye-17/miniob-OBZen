#include "sql/operator/update_physical_operator.h"
#include "common/log/log.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/expr/tuple.h"
#include "common/type/data_type.h"
#include "common/type/attr_type.h"
#include <limits>
#include <climits>

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];

  // 开启子算子，扫描和过滤数据
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  // 收集所有需要更新的记录
  while (OB_SUCC(rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record   &record    = row_tuple->record();
    records_.emplace_back(std::move(record));
  }

  if (rc != RC::RECORD_EOF) {
    LOG_WARN("failed to fetch records from child operator: %s", strrc(rc));
    return rc;
  }

  child->close();

  // 验证所有字段元数据
  std::vector<const FieldMeta *> field_metas;
  for (const std::string &field_name : field_names_) {
    const FieldMeta *field_meta = table_->table_meta().field(field_name.c_str());
    if (field_meta == nullptr) {
      LOG_WARN("no such field in table. table=%s, field=%s", table_->name(), field_name.c_str());
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    field_metas.push_back(field_meta);
  }

  // 批量更新所有记录
  for (Record &record : records_) {
    Record new_record = record;

    RowTuple row_tuple;
    row_tuple.set_record(&record);
    row_tuple.set_schema(table_, table_->table_meta().field_metas());

    // 逐个字段更新
    for (size_t i = 0; i < field_names_.size(); ++i) {
      const FieldMeta *field_meta = field_metas[i];
      Expression      *expression = expressions_[i];

      int offset = field_meta->offset();
      int len    = field_meta->len();

      // 计算表达式的值
      Value expression_value;
      rc = expression->get_value(row_tuple, expression_value);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to evaluate expression for field. table=%s, field=%s, rc=%s",
                 table_->name(), field_names_[i].c_str(), strrc(rc));
        return rc;
      }

      int         int_val;
      float       float_val;
      std::string string_val;

      // 类型转换
      Value converted_value;
      if (expression_value.attr_type() != field_meta->type()) {
        RC cast_rc = DataType::type_instance(expression_value.attr_type())
                         ->cast_to(expression_value, field_meta->type(), converted_value);
        if (cast_rc != RC::SUCCESS) {
          LOG_WARN("failed to cast expression result from %s to %s. table=%s, field=%s",
                   attr_type_to_string(expression_value.attr_type()), 
                   attr_type_to_string(field_meta->type()),
                   table_->name(), field_names_[i].c_str());
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      } else {
        converted_value = expression_value;
      }

      // 处理 NULL 值
      if (converted_value.is_null()) {
        if (!field_meta->nullable()) {
          LOG_WARN("field does not allow null values. table=%s, field=%s", 
                   table_->name(), field_names_[i].c_str());
          return RC::CONSTRAINT_VIOLATION;
        }
        memset(new_record.data() + offset, 0xFF, len);
      } else {
        // 根据字段类型更新值
        switch (field_meta->type()) {
          case AttrType::INTS: {
            int_val = converted_value.get_int();
            memcpy(new_record.data() + offset, &int_val, len);
          } break;
          case AttrType::FLOATS: {
            float_val = converted_value.get_float();
            memcpy(new_record.data() + offset, &float_val, len);
          } break;
          case AttrType::CHARS: {
            string_val = converted_value.get_string();
            memset(new_record.data() + offset, 0, len);
            size_t copy_len = std::min(static_cast<size_t>(len), string_val.length());
            if (copy_len > 0) {
              memcpy(new_record.data() + offset, string_val.c_str(), copy_len);
            }
          } break;
          case AttrType::DATES: {
            int_val = converted_value.get_int();
            memcpy(new_record.data() + offset, &int_val, len);
          } break;
          default: LOG_WARN("unsupported field type: %d", field_meta->type()); return RC::INTERNAL;
        }
      }
    }

    // 提交更新
    rc = trx_->update_record(table_, record, new_record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to update record: %s", strrc(rc));
      return rc;
    }

    update_count_++;
  }

  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next() { return RC::RECORD_EOF; }

RC UpdatePhysicalOperator::close()
{
  records_.clear();
  update_count_ = 0;
  trx_          = nullptr;
  return RC::SUCCESS;
}
