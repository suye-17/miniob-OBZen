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
// Created by wangyunlai.wyl on 2021/5/19.
//

#include "storage/index/index.h"

RC Index::init(const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  index_meta_ = index_meta;
  field_meta_ = field_meta;
  field_metas_.clear();
  field_metas_.push_back(field_meta);
  return RC::SUCCESS;
}

RC Index::init(const IndexMeta &index_meta, const vector<const FieldMeta *> &field_metas)
{
  index_meta_ = index_meta;
  field_metas_.clear();
  field_metas_.reserve(field_metas.size());
  for (const auto *field_meta : field_metas) {
    field_metas_.push_back(*field_meta);
  }
  if (!field_metas_.empty()) {
    field_meta_ = field_metas_[0];  // 保持向后兼容
  }
  return RC::SUCCESS;
}
