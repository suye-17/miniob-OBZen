#pragma once

#include "sql/stmt/stmt.h"
#include "sql/parser/parse_defs.h"


/**
 * @brief DropTable 语句
 * @ingroup Statement
 */
class DropTableStmt : public Stmt
{
public:
  DropTableStmt(const string &table_name);
  ~DropTableStmt() override;

  StmtType type() const override { return StmtType::DROP_TABLE; }

  const string &table_name() const { return table_name_; }

  static RC create(Db *db, const DropTableSqlNode &drop_table, Stmt *&stmt);

private:
  string table_name_;
};