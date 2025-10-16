# 测试文件目录结构

本目录按功能模块分类组织测试文件，便于查找和维护。

## 📁 目录结构

```
test/case/test/
├── subquery/          # 子查询测试 (14个文件: 10个.test + 3个.sql + 1个.md)
├── join/              # JOIN测试 (3个.test文件)
├── dblab/             # 数据库实验测试 (3个.test文件)
├── primary/           # 基础功能测试 (11个.test文件)
├── vectorized/        # 向量化测试 (2个.test文件)
└── basic/             # 综合基础测试 (3个文件: 2个.test + 1个.sql)
```

## 🔍 各目录详情

### 1. subquery/ - 子查询测试 (14个文件)
包含所有子查询相关的测试用例：

**测试文件 (.test):**
- `advanced-subquery.test` - 高级子查询测试
- `debug-in-subquery.test` - IN子查询调试
- `in-subquery-error-cases.test` - IN子查询错误案例
- `primary-complex-sub-query.test` - 复杂子查询
- `primary-simple-sub-query.test` - 简单子查询
- `scalar-subquery-multicolumn-check.test` - 标量子查询多列检查
- `simple-feature-check.test` - 简单功能检查
- `ssq-in-subquery.test` - SSQ IN子查询
- `subquery-null-edge-cases.test` - 子查询NULL边界测试
- `test-in-subquery.test` - IN子查询测试

**SQL文件:**
- `simple-subquery-basic.sql` - 基础子查询SQL
- `simple-subquery-quick.sql` - 快速子查询测试SQL
- `simple-subquery-test.sql` - 子查询完整测试SQL

**文档:**
- `simple-subquery-README.md` - 子查询功能说明

**功能覆盖:**
- IN/NOT IN 子查询
- EXISTS/NOT EXISTS 子查询
- 标量子查询
- 复杂子查询
- NULL值处理
- 边界条件测试

### 2. join/ - JOIN测试 (3个文件)
包含所有JOIN操作相关的测试用例：
- `inner-join-comprehensive.test` - INNER JOIN综合测试
- `join-field-validation.test` - JOIN字段验证
- `primary-join-tables.test` - 多表JOIN基础测试

**功能覆盖:**
- INNER JOIN
- 多表JOIN
- JOIN字段验证
- JOIN条件组合

### 3. dblab/ - 数据库实验测试 (3个文件)
包含数据库实验室相关的测试用例：
- `dblab-hash-join.test` - Hash Join性能测试
- `dblab-optimizer.test` - 查询优化器测试
- `dblab-sort.test` - 排序功能测试

**功能覆盖:**
- Hash Join算法
- 查询优化策略
- 排序性能

### 4. primary/ - 基础功能测试 (11个文件)
包含数据库核心功能的测试用例：
- `primary-aggregation-func.test` - 聚合函数
- `primary-date.test` - 日期类型
- `primary-drop-table.test` - 删除表
- `primary-expression.test` - 表达式
- `primary-group-by.test` - 分组
- `primary-multi-index.test` - 多索引
- `primary-null.test` - NULL值处理
- `primary-order-by.test` - 排序
- `primary-text.test` - 文本类型
- `primary-unique.test` - 唯一约束
- `primary-update.test` - 更新操作

**功能覆盖:**
- 聚合函数 (COUNT, SUM, AVG, MAX, MIN)
- 数据类型 (DATE, TEXT)
- DDL操作 (DROP TABLE)
- DML操作 (UPDATE)
- 约束 (UNIQUE, NULL)
- 查询功能 (GROUP BY, ORDER BY, Expression)
- 索引功能 (Multi-Index)

### 5. vectorized/ - 向量化测试 (2个文件)
包含向量化执行相关的测试用例：
- `vectorized-aggregation-and-group-by.test` - 向量化聚合和分组
- `vectorized-basic.test` - 向量化基础操作

**功能覆盖:**
- SIMD向量化
- 批处理优化
- 性能提升验证

### 6. basic/ - 综合基础测试 (3个文件)
包含综合性和基础功能的测试用例：
- `basic.test` - 基本SQL操作
- `comprehensive-feature-verification.test` - 功能综合验证
- `fix_delete_problem.sql` - DELETE语句修复测试

**功能覆盖:**
- 基本CRUD操作
- 多功能集成测试
- 问题修复验证

## 📊 测试统计

| 分类 | 文件数 | 测试文件(.test) | SQL文件(.sql) | 文档(.md) |
|------|--------|----------------|---------------|-----------|
| subquery | 14 | 10 | 3 | 1 |
| join | 3 | 3 | 0 | 0 |
| dblab | 3 | 3 | 0 | 0 |
| primary | 11 | 11 | 0 | 0 |
| vectorized | 2 | 2 | 0 | 0 |
| basic | 3 | 2 | 1 | 0 |
| **总计** | **36** | **31** | **4** | **1** |

## 🔗 相关文档

- 子查询实现文档: `/docs/简单子查询实现文档.md`
- 子查询测试文档: `/docs/简单子查询测试文档.md`
- JOIN实现文档: `/docs/INNER_JOIN实现文档.md`
- JOIN测试文档: `/docs/INNER_JOIN测试文档.md`
- 查询优化文档: `/docs/查询优化实现文档.md`
- 表达式系统文档: `/docs/表达式系统实现文档.md`

## 📝 使用说明

1. **运行单个测试:**
   ```bash
   ./build/bin/observer -f test/case/test/subquery/advanced-subquery.test
   ```

2. **运行整个目录的测试:**
   ```bash
   for test in test/case/test/subquery/*.test; do
     ./build/bin/observer -f "$test"
   done
   ```

3. **执行SQL文件:**
   ```bash
   ./build/bin/obclient < test/case/test/subquery/simple-subquery-test.sql
   ```

---

**整理时间**: 2025-10-16  
**维护者**: AI Assistant  
**版本**: v1.0
