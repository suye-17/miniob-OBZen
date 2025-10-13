#include "sql/executor/show_index_executor.h"
#include "sql/stmt/show_index_stmt.h"
#include "storage/table/table_meta.h"
#include "storage/table/table.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "common/log/log.h"
#include "sql/operator/string_list_physical_operator.h"

using namespace std;
using namespace common;

RC ShowIndexExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  ASSERT(stmt->type() == StmtType::SHOW_INDEX, 
                "show index executor can not run this command: %d", static_cast<int>(stmt->type()));
  ShowIndexStmt *show_index_stmt = static_cast<ShowIndexStmt *>(stmt);
  SqlResult     *sql_result      = sql_event->session_event()->sql_result();
  TupleSchema    tuple_schema;
  tuple_schema.append_cell(TupleCellSpec("", "Table", "Table"));
  tuple_schema.append_cell(TupleCellSpec("", "Non_unique", "Non_unique"));
  tuple_schema.append_cell(TupleCellSpec("", "Key_name", "Key_name"));
  tuple_schema.append_cell(TupleCellSpec("", "Seq_in_index", "Seq_in_index"));
  tuple_schema.append_cell(TupleCellSpec("", "Column_name", "Column_name"));

  sql_result->set_tuple_schema(tuple_schema);

  Table           *table      = show_index_stmt->table();
  const TableMeta &table_meta = table->table_meta();

  // 创建 StringListPhysicalOperator
  auto oper = new StringListPhysicalOperator;

  // 获取所有索引
  const vector<IndexMeta> *index_metas = table_meta.index_metas();

  for (const IndexMeta &index_meta : *index_metas) {
    const vector<string> &field_names = index_meta.fields();

    // 对于多字段索引，每个字段都显示一行
    for (size_t i = 0; i < field_names.size(); i++) {
      // 构造一行数据：Table | Non_unique | Key_name | Seq_in_index | Column_name
      string row =
          string(table_meta.name()) + "|1|" + index_meta.name() + "|" + to_string(i + 1) + "|" + field_names[i];
      oper->append(row);
    }
  }

  sql_result->set_operator(unique_ptr<PhysicalOperator>(oper));

  return RC::SUCCESS;
}
