/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai.wyl on 2021/5/18.
//

#include "storage/index/index_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "storage/field/field_meta.h"
#include "storage/table/table_meta.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");

RC IndexMeta::init(const char *name, const FieldMeta &field)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_  = name;
  field_ = field.name();
  field_names_.clear();
  field_names_.push_back(field.name());
  return RC::SUCCESS;
}

RC IndexMeta::init(const char *name, const vector<const FieldMeta *> &fields)
{
  if(common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  if(fields.empty()) {
    LOG_ERROR("Failed to init index, fields is empty.");
    return RC::INVALID_ARGUMENT;
  }
  if(fields.size() > 5) {
    LOG_ERROR("Failed to init index, too many fields: %zu (max: 5)", fields.size());
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  field_names_.clear();
  field_names_.reserve(fields.size());
  for(const auto &field : fields) {
    field_names_.push_back(field->name());
  }
  
  // 保持向后兼容性
  field_ = field_names_[0];
  
  return RC::SUCCESS;
}

 
void IndexMeta::to_json(Json::Value &json_value) const
{
  json_value[FIELD_NAME] = name_;
  
  if (is_multi_field()) {
    // 多字段索引：存储字段列表
    Json::Value field_array(Json::arrayValue);
    for (const string &field_name : field_names_) {
      field_array.append(field_name);
    }
    json_value["field_names"] = field_array;
    json_value[FIELD_FIELD_NAME] = field_;  // 保持兼容性
  } else {
    // 单字段索引：使用原有格式
    json_value[FIELD_FIELD_NAME] = field_;
  }
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value  = json_value[FIELD_NAME];
  if(!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::INTERNAL;
  }
  if(json_value.isMember("field_names")) {
    const Json::Value &field_names_value = json_value["field_names"];
    if(!field_names_value.isArray()) {
      LOG_ERROR("Field names is not an array. json value=%s", field_names_value.toStyledString().c_str());
      return RC::INTERNAL;
    }
    vector<const FieldMeta *> fields;
    for(const auto &field_name : field_names_value) {
      if(!field_name.isString()) {
        LOG_ERROR("Field name is not a string. json value=%s", field_name.toStyledString().c_str());
        return RC::INTERNAL;
      }
      const FieldMeta *field = table.field(field_name.asCString());
      if(nullptr == field) {
        LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_name.asCString());
        return RC::SCHEMA_FIELD_MISSING;
      }
      fields.push_back(field);
    }
    return index.init(name_value.asCString(), fields);
  } else {
    const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
    if(!field_value.isString()) {
      LOG_ERROR("Field name is not a string. json value=%s", field_value.toStyledString().c_str());
      return RC::INTERNAL;
    }
    const FieldMeta *field = table.field(field_value.asCString());
    if(nullptr == field) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
    return index.init(name_value.asCString(), *field);
  }
}

const char *IndexMeta::name() const { return name_.c_str(); }

const char *IndexMeta::field() const { return field_.c_str(); }

const vector<string> &IndexMeta::fields() const { return field_names_; }

size_t IndexMeta::field_count() const { return field_names_.size(); }

bool IndexMeta::is_multi_field() const { return field_names_.size() > 1; }

void IndexMeta::desc(ostream &os) const 
{ 
  os << "index name=" << name_ << ", fields=[";
  for (size_t i = 0; i < field_names_.size(); ++i) {
    if (i > 0) os << ",";
    os << field_names_[i];
  }
  os << "]";
}