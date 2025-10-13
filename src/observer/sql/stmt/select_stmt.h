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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include "common/sys/rc.h"
#include "sql/stmt/stmt.h"
#include "sql/parser/parse_defs.h"

class FieldMeta;
class FilterStmt;
class Db;
class Table;
class Expression;

/**
 * @brief JOIN表信息
 */
struct JoinTable {
  Table        *table;      ///< 表对象
  std::string   alias;      ///< 表别名
  JoinType      join_type;  ///< JOIN类型
  Expression   *condition;  ///< JOIN条件表达式
};

/**
 * @brief 表示select语句（扩展JOIN支持）
 * @ingroup Statement
 */
class SelectStmt : public Stmt
{
public:
  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override { return StmtType::SELECT; }

public:
  static RC create(Db *db, SelectSqlNode &select_sql, Stmt *&stmt);

public:
  const vector<Table *>       &tables() const { return tables_; }
  const vector<JoinTable>     &join_tables() const { return join_tables_; }
  FilterStmt                  *filter_stmt() const { return filter_stmt_; }

  vector<unique_ptr<Expression>> &query_expressions() { return query_expressions_; }
  vector<unique_ptr<Expression>> &group_by() { return group_by_; }
  FilterStmt                     *having_filter_stmt() const { return having_filter_stmt_; }

private:
  vector<unique_ptr<Expression>> query_expressions_;
  vector<Table *>                tables_;
  vector<JoinTable>              join_tables_;      ///< JOIN表列表
  FilterStmt                    *filter_stmt_ = nullptr;
  vector<unique_ptr<Expression>> group_by_;
  FilterStmt                    *having_filter_stmt_ = nullptr;
};
