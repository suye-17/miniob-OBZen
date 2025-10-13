#include "sql/stmt/show_index_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "common/log/log.h"
#include "common/lang/string.h"

using namespace std;
using namespace common;

RC ShowIndexStmt::create(Db *db, const ShowIndexSqlNode &show_index, Stmt *&stmt)
{
  stmt = nullptr;

  const char *table_name = show_index.relation_name.c_str();
  if (is_blank(table_name)) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    return RC::INVALID_ARGUMENT;
  }
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("table not found. db=%p, table_name=%s", db, table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  stmt = new ShowIndexStmt(table);
  if (nullptr == stmt) {
    return RC::NOMEM;
  }
  return RC::SUCCESS;
}