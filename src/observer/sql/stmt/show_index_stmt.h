#pragma once
#include <string>
#include <vector>
#include "sql/stmt/stmt.h"

struct ShowIndexSqlNode;
class Table;
class ShowIndexStmt : public Stmt
{
public:
  ShowIndexStmt(Table *table) : table_(table) {}

  virtual ~ShowIndexStmt() = default;

  StmtType type() const override { return StmtType::SHOW_INDEX; }

  Table *table() const { return table_; }

  static RC create(Db *db, const ShowIndexSqlNode &show_index, Stmt *&stmt);

private:
  Table *table_ = nullptr;
};