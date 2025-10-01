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
// Created by Wangyunlai on 2021/5/12.
//

#pragma once

#include "common/sys/rc.h"
#include "common/lang/string.h"
#include <vector>
class TableMeta;
class FieldMeta;

namespace Json {
class Value;
}  // namespace Json

/**
 * @brief 描述一个索引
 * @ingroup Index
 * @details 一个索引包含了表的哪些字段，索引的名称等。
 * 如果以后实现了多种类型的索引，还需要记录索引的类型，对应类型的一些元数据等
 */
class IndexMeta
{
public:
  IndexMeta() = default;

  RC init(const char *name, const FieldMeta &field, bool is_unique = false);
  RC init(const char *name, const vector<const FieldMeta *> &fields, bool is_unique = false);

public:
  const char *name() const;
  const char *field() const;                    // 向后兼容，返回第一个字段
  const vector<string> &fields() const;         // 返回所有字段名
  size_t field_count() const;                   // 字段数量
  bool is_multi_field() const;                  // 是否多字段索引
  bool is_unique() const;                       // 是否为唯一索引
  void set_unique(bool unique);              // 设置是否为唯一索引

  void desc(ostream &os) const;

public:
  void      to_json(Json::Value &json_value) const;
  static RC from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index);

protected:
  string name_;                    // index's name
  string field_;                   // field's name (保持兼容性，存储第一个字段名)
  vector<string> field_names_;     // 多字段名列表
  bool is_unique_ = false;        // 是是否为唯一索引
};
