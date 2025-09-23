# MiniOB DROP TABLE 功能实现文档

## 概述

这个文档记录了在MiniOB中实现DROP TABLE功能的过程。DROP TABLE用于删除表及其相关的所有资源，包括数据文件、索引文件、元数据文件等。

## 功能要求

- 支持标准SQL语法：`DROP TABLE table_name`
- 删除所有相关文件：.data、.table、.index文件
- 清理内存资源：表对象、索引对象、缓冲池数据
- 错误处理：表不存在、文件删除失败等情况

## 现状分析

MiniOB已经有了基本的框架：
- SQL解析层支持DROP TABLE语法
- DropTableExecutor有基本结构
- Db::drop_table()方法已存在

需要完善的部分：
- DropTableStmt类实现不完整
- Table::drop()方法需要重新实现

## 实现过程

### 1. 修复DropTableStmt类

编译时发现问题：
```
error: invalid new-expression of abstract class type 'DropTableStmt'
note: 'virtual StmtType Stmt::type() const'
```

**解决方案**：在`drop_table_stmt.h`中添加缺失的虚函数：

```cpp
class DropTableStmt : public Stmt
{
public:
  DropTableStmt(const string &table_name);
  ~DropTableStmt() override;

  // 添加缺失的type()方法
  StmtType type() const override { return StmtType::DROP_TABLE; }

  const string &table_name() const { return table_name_; }
  static RC create(Db *db, const DropTableSqlNode &drop_table, Stmt *&stmt);

private:
  string table_name_;
};
```

实现`create()`方法：

```cpp
RC DropTableStmt::create(Db *db, const DropTableSqlNode &drop_table, Stmt *&stmt)
{
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
```

### 2. 实现Table::drop()方法

这是核心部分，需要删除表的所有相关资源。

**遇到的问题**：
1. `TableEngine::close()`方法不存在 → 改用`engine_.reset()`
2. `RC::IOERR_UNLINK`错误码不存在 → 改用`RC::IOERR_ACCESS`

**完整实现**：

```cpp
RC Table::drop()
{
  RC rc = RC::SUCCESS;
  
  // 1. 删除所有索引文件
  for (int i = 0; i < table_meta_.index_num(); i++) {
    const IndexMeta *index_meta = table_meta_.index(i);
    if (index_meta != nullptr) {
      string index_file_path = table_index_file(db_->path().c_str(), 
                                               table_meta_.name(), 
                                               index_meta->name());
      int ret = ::unlink(index_file_path.c_str());
      if (ret != 0 && errno != ENOENT) {
        LOG_WARN("Failed to remove index file: %s, error: %s", 
                 index_file_path.c_str(), strerror(errno));
      } else {
        LOG_INFO("Successfully removed index file: %s", index_file_path.c_str());
      }
    }
  }

  // 2. 重置表引擎资源
  if (engine_ != nullptr) {
    engine_.reset();
  }

  // 3. 删除表数据文件
  string table_data_path = table_data_file(db_->path().c_str(), table_meta_.name());
  int ret = ::unlink(table_data_path.c_str());
  if (ret != 0 && errno != ENOENT) {
    LOG_WARN("Failed to remove table data file: %s, error: %s", 
             table_data_path.c_str(), strerror(errno));
  } else {
    LOG_INFO("Successfully removed table data file: %s", table_data_path.c_str());
  }

  // 4. 删除表元数据文件
  string table_meta_path = table_meta_file(db_->path().c_str(), table_meta_.name());
  ret = ::unlink(table_meta_path.c_str());
  if (ret != 0 && errno != ENOENT) {
    LOG_WARN("Failed to remove table meta file: %s, error: %s", 
             table_meta_path.c_str(), strerror(errno));
    rc = RC::IOERR_ACCESS;
  } else {
    LOG_INFO("Successfully removed table meta file: %s", table_meta_path.c_str());
  }

  return rc;
}
```

## 关键技术点

### 文件路径生成

MiniOB使用工具函数生成文件路径：

```cpp
// 在meta_util.h中定义
string table_meta_file(const char *base_dir, const char *table_name);    // {table_name}.table
string table_data_file(const char *base_dir, const char *table_name);    // {table_name}.data
string table_index_file(const char *base_dir, const char *table_name, const char *index_name);  // {table_name}-{index_name}.index
```

### 错误处理

**表不存在**：在语句创建阶段就检查，避免后续操作
```cpp
Table *table = db->find_table(table_name);
if (table == nullptr) {
  return RC::SCHEMA_TABLE_NOT_EXIST;
}
```

**文件删除失败**：使用ENOENT检查，文件不存在不算错误
```cpp
int ret = ::unlink(file_path.c_str());
if (ret != 0 && errno != ENOENT) {
  // 真正的删除错误
  LOG_WARN("Failed to remove file: %s", file_path.c_str());
}
```

### 资源清理顺序

删除顺序很重要：
1. **索引文件** → 避免数据不一致
2. **表引擎** → 清理内存对象
3. **数据文件** → 删除实际数据
4. **元数据文件** → 最后删除，保证原子性

### 数据库层面的清理

```cpp
RC Db::drop_table(const char *table_name) {
  Table *table = find_table(table_name);
  
  // 调用表的drop方法
  RC rc = table->drop();
  
  // 从内存中移除
  opened_tables_.erase(table_name);
  delete table;
  
  return rc;
}
```

## 测试验证

### 基本功能测试

```sql
-- 测试1：删除空表
CREATE TABLE test_empty(id int, name char(10));
DROP TABLE test_empty;

-- 测试2：删除有数据的表
CREATE TABLE test_data(id int, name char(10));
INSERT INTO test_data VALUES(1, 'Alice');
DROP TABLE test_data;

-- 测试3：删除有索引的表
CREATE TABLE test_index(id int, name char(10));
CREATE INDEX idx_id ON test_index(id);
DROP TABLE test_index;

-- 测试4：删除不存在的表（应该报错）
DROP TABLE non_exist_table;
```

### 文件系统验证

```bash
# 检查文件是否被删除
ls -la /path/to/miniob/db/sys/
# 应该看不到被删除表的.table、.data、.index文件
```

## 可能的扩展

### DROP TABLE IF EXISTS
```sql
DROP TABLE IF EXISTS table_name;
```
需要修改语法解析器，在表不存在时返回成功而不是错误。

### CASCADE删除
```sql
DROP TABLE table_name CASCADE;
```
删除表时同时删除依赖的视图和外键约束。

## 总结

实现DROP TABLE功能主要涉及：

1. **语句层面**：修复DropTableStmt类的虚函数实现
2. **存储层面**：实现Table::drop()方法，按正确顺序删除所有相关文件
3. **错误处理**：处理表不存在、文件删除失败等情况
4. **资源管理**：确保内存和文件资源都被正确清理

整个实现过程中最重要的是理解MiniOB的文件组织结构和资源管理机制，确保删除操作的完整性和安全性。

---