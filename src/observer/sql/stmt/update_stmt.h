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
// Created by Wangyunlai on 2022/5/22.
//

#pragma once

#include "common/sys/rc.h"
#include "sql/stmt/stmt.h"

class Table;

class FilterStmt;

/**
 * @brief UPDATE语句的语义表示类
 * @ingroup Statement
 * @details
 * UpdateStmt封装了一个完整的UPDATE语句的所有信息，包括：
 * - 目标表信息
 * - 要更新的字段名
 * - 新的字段值
 * - WHERE条件过滤器
 *
 * 这个类遵循了MiniOB的设计模式，类似于InsertStmt和DeleteStmt，
 * 将SQL语法树转换为内部的语义表示，便于后续的查询优化和执行。
 */
class UpdateStmt : public Stmt
{
public:
  UpdateStmt() = default;

  /**
   * @brief 构造函数，初始化UPDATE语句的所有组件
   * @param table 目标表对象
   * @param field_name 要更新的字段名
   * @param expression 更新的表达式（支持复杂计算）
   * @param filter_stmt WHERE条件过滤器，可以为nullptr（表示无条件更新）
   */
  UpdateStmt(Table *table, const std::vector<std::string> &field_names, std::vector<Expression *> &&expressions,
      FilterStmt *filter_stmt);

  /**
   * @brief 析构函数，释放FilterStmt资源
   */
  ~UpdateStmt() override;

  /**
   * @brief 获取语句类型
   * @return UPDATE语句类型
   */
  StmtType type() const override { return StmtType::UPDATE; }

public:
  /**
   * @brief 从语法解析结果创建UpdateStmt对象
   * @details 这是一个工厂方法，负责验证UPDATE语句的合法性并创建相应的对象
   *
   * @param db 数据库对象
   * @param update_sql 从语法分析得到的UPDATE节点
   * @param stmt 输出参数，返回创建的UpdateStmt对象
   * @return RC 操作结果码
   */
  static RC create(Db *db, const UpdateSqlNode &update_sql, Stmt *&stmt);

public:
  // 访问器方法，用于获取UPDATE语句的各个组件

  /**
   * @brief 获取目标表对象
   * @return 目标表指针
   */
  Table *table() const { return table_; }

  /**
   * @brief 获取要更新的字段名
   * @return 字段名字符串引用
   */
  const std::vector<std::string> &field_names() const { return field_names_; }

  /**
   * @brief 获取更新表达式
   * @return 表达式指针
   */
  const std::vector<Expression *> &expressions() const { return expressions_; }

  /**
   * @brief 获取WHERE条件过滤器
   * @return FilterStmt指针，可能为nullptr
   */
  FilterStmt *filter_stmt() const { return filter_stmt_; }

private:
  Table                    *table_ = nullptr;        ///< 目标表对象
  std::vector<std::string>  field_names_;            ///< 要更新的字段名
  std::vector<Expression *> expressions_;            ///< 更新的表达式，支持多个表达式
  FilterStmt               *filter_stmt_ = nullptr;  ///< WHERE条件过滤器，nullptr表示无条件更新
};
