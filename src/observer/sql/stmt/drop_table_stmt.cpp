#include "sql/stmt/drop_table_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"

DropTableStmt::DropTableStmt(const string &table_name) : table_name_(table_name)
{}

DropTableStmt::~DropTableStmt() = default;

RC DropTableStmt::create(Db *db, const DropTableSqlNode &drop_table, Stmt *&stmt)
{
  // 检查表名是否有效
  const char *table_name = drop_table.relation_name.c_str();
  if (common::is_blank(table_name)) {
    LOG_WARN("invalid table name. table name is blank");
    return RC::INVALID_ARGUMENT;
  }

  // 检查表是否存在
  Table *table = db->find_table(table_name);
  if (table == nullptr) {
    LOG_WARN("table does not exist. table name=%s", table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  stmt = new DropTableStmt(table_name);
  return RC::SUCCESS;
}
