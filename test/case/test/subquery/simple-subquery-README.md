# 简单子查询测试说明

## 测试文件说明

- **simple-subquery-test.sql**: 完整的测试用例（15个测试类别，100+测试点）
- **simple-subquery-basic.sql**: 精简的核心测试用例（36个关键测试点）

## 功能测试覆盖

### 1. IN 子查询 (Basic IN Subquery)

**测试目标**: 验证 IN 操作符与子查询的基本功能

```sql
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2);
```

**测试要点**:
- ✅ 基本 IN 子查询
- ✅ IN 子查询返回空集
- ✅ IN 子查询返回多行
- ✅ IN 子查询包含聚合函数

**预期行为**:
- 返回 t1 中 id 在子查询结果集中的所有行
- 空结果集应返回空
- 正确处理重复值

---

### 2. NOT IN 子查询 (NOT IN Subquery)

**测试目标**: 验证 NOT IN 操作符，特别是 NULL 值处理

```sql
SELECT * FROM t1 WHERE id NOT IN (SELECT ref_id FROM t2);
```

**测试要点**:
- ✅ 基本 NOT IN 子查询
- ✅ NOT IN 子查询返回空集（应返回全部）
- ⚠️ **NOT IN 包含 NULL 的特殊情况**（重要！）

**NULL 特殊性说明**:
```sql
-- 如果子查询结果包含 NULL
SELECT * FROM t1 WHERE id NOT IN (1, 2, NULL);
-- 等价于: WHERE id != 1 AND id != 2 AND id != NULL
-- 结果: 空集（因为任何值 != NULL 都是 UNKNOWN）
```

**预期行为**:
- 子查询无 NULL: 返回不在结果集中的行
- 子查询有 NULL: 通常返回空集（SQL标准行为）
- 空结果集: 返回所有行

---

### 3. EXISTS 子查询 (EXISTS Subquery)

**测试目标**: 验证 EXISTS 存在性测试

```sql
SELECT * FROM t1 WHERE EXISTS (SELECT * FROM t2 WHERE condition);
```

**测试要点**:
- ✅ 基本 EXISTS 检测
- ✅ EXISTS 返回非空（真）
- ✅ EXISTS 返回空（假）
- ✅ 关联子查询（虽然是简单子查询，但允许引用外层）

**预期行为**:
- 子查询有结果: 返回外层所有行
- 子查询无结果: 返回空
- EXISTS 只关心是否存在，不关心具体值

---

### 4. NOT EXISTS 子查询 (NOT EXISTS Subquery)

**测试目标**: 验证 NOT EXISTS 不存在测试

```sql
SELECT * FROM t1 WHERE NOT EXISTS (SELECT * FROM t2 WHERE condition);
```

**测试要点**:
- ✅ 基本 NOT EXISTS
- ✅ 子查询返回空（返回全部）
- ✅ 子查询返回非空（返回空）

**预期行为**:
- 与 EXISTS 相反的逻辑
- 不受 NULL 值影响（与 NOT IN 不同）

---

### 5. 比较运算符与子查询 (Comparison with Subquery)

**测试目标**: 验证 =, >, <, >=, <=, != 与子查询结合

```sql
SELECT * FROM t1 WHERE value > (SELECT AVG(score) FROM t2);
```

**测试要点**:
- ✅ 等于 (=)
- ✅ 大于 (>)
- ✅ 小于 (<)
- ✅ 大于等于 (>=)
- ✅ 小于等于 (<=)
- ✅ 不等于 (!=, <>)

**重要约束**:
- ⚠️ 子查询必须返回单行单列（标量子查询）
- ⚠️ 多行子查询应报错或使用 ANY/ALL

**预期行为**:
- 正常比较: 按运算符逻辑返回结果
- 子查询返回 NULL: 比较结果为 UNKNOWN，行不返回
- 子查询返回多行: 错误或特殊处理

---

### 6. 聚合函数子查询 (Aggregate in Subquery)

**测试目标**: 验证子查询中的聚合函数

```sql
SELECT * FROM t1 WHERE value > (SELECT AVG(score) FROM t2);
```

**支持的聚合函数**:
- ✅ MAX() - 最大值
- ✅ MIN() - 最小值
- ✅ AVG() - 平均值
- ✅ COUNT() - 计数
- ✅ SUM() - 求和

**测试要点**:
- 单个聚合函数
- 聚合函数与 WHERE 条件
- 聚合函数与 GROUP BY
- 聚合函数返回 NULL 的情况

---

### 7. 嵌套子查询 (Nested Subquery)

**测试目标**: 验证多层嵌套子查询

```sql
SELECT * FROM t1 WHERE id IN (
    SELECT ref_id FROM t2 WHERE score > (
        SELECT AVG(score) FROM t2
    )
);
```

**测试要点**:
- ✅ 2层嵌套
- ✅ 3层嵌套
- ✅ IN 嵌套 IN
- ✅ EXISTS 嵌套 IN
- ✅ 比较运算嵌套聚合

**预期行为**:
- 从最内层开始执行
- 逐层向外传递结果
- 正确处理作用域

---

### 8. NULL 值处理 (NULL Handling)

**测试目标**: 验证各种场景下的 NULL 处理

**关键测试场景**:

#### 8.1 IN 与 NULL
```sql
-- 子查询结果: (1, 2, NULL)
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2);
-- 行为: id = 1 返回, id = 2 返回, id = 3 不返回
```

#### 8.2 NOT IN 与 NULL（重要！）
```sql
-- 子查询结果: (1, 2, NULL)
SELECT * FROM t1 WHERE id NOT IN (SELECT ref_id FROM t2);
-- 行为: 返回空集（SQL标准）
-- 原因: id != NULL 总是 UNKNOWN
```

#### 8.3 比较运算与 NULL
```sql
SELECT * FROM t1 WHERE value > (SELECT value FROM t1 WHERE id = 4);
-- 如果子查询返回 NULL, 比较结果为 UNKNOWN, 不返回该行
```

#### 8.4 EXISTS/NOT EXISTS 与 NULL
```sql
-- EXISTS 不受 NULL 影响，只关心行是否存在
SELECT * FROM t1 WHERE EXISTS (SELECT NULL FROM t2);
```

**NULL 处理规则**:
- IN: 忽略 NULL，匹配非 NULL 值
- NOT IN: 有 NULL 则全部失败
- EXISTS: 不受影响
- 比较: NULL 导致 UNKNOWN

---

### 9. SELECT 子句中的子查询 (Subquery in SELECT)

**测试目标**: 验证 SELECT 列表中的标量子查询

```sql
SELECT id, 
       (SELECT COUNT(*) FROM t2 WHERE t2.ref_id = t1.id),
       (SELECT MAX(score) FROM t2 WHERE t2.ref_id = t1.id)
FROM t1;
```

**测试要点**:
- ✅ 单个标量子查询
- ✅ 多个标量子查询
- ✅ 关联子查询引用外层表
- ✅ 聚合函数子查询

**预期行为**:
- 为每一行外层数据执行子查询
- 子查询必须返回单值
- NULL 值正常显示

---

### 10. 组合条件 (Combined Conditions)

**测试目标**: 验证子查询与其他条件的组合

```sql
-- AND 组合
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2) AND value > 100;

-- OR 组合
SELECT * FROM t1 WHERE id IN (...) OR value < 100;

-- NOT 组合
SELECT * FROM t1 WHERE NOT (id IN (...));
```

**测试要点**:
- ✅ AND 逻辑
- ✅ OR 逻辑
- ✅ NOT 逻辑
- ✅ 复杂布尔表达式
- ✅ 优先级处理

---

### 11. 边界情况 (Edge Cases)

**测试目标**: 验证特殊场景

| 场景 | 测试SQL | 预期结果 |
|------|---------|----------|
| 空表子查询 (IN) | `WHERE id IN (SELECT id FROM empty)` | 返回空 |
| 空表子查询 (NOT IN) | `WHERE id NOT IN (SELECT id FROM empty)` | 返回全部 |
| 空表子查询 (EXISTS) | `WHERE EXISTS (SELECT * FROM empty)` | 返回空 |
| 空表子查询 (NOT EXISTS) | `WHERE NOT EXISTS (SELECT * FROM empty)` | 返回全部 |
| 单行单列 | `WHERE id = (SELECT ...)` | 正常比较 |
| 多行单列比较 | `WHERE id = (SELECT ... 多行)` | 错误 |
| DISTINCT 子查询 | `WHERE id IN (SELECT DISTINCT ...)` | 去重后匹配 |

---

## 测试执行建议

### 测试顺序

1. **第一阶段**: 基础功能（TEST 1-11）
   - IN / NOT IN
   - EXISTS / NOT EXISTS
   
2. **第二阶段**: 比较和聚合（TEST 12-22）
   - 比较运算符
   - 聚合函数
   
3. **第三阶段**: 复杂场景（TEST 23-30）
   - 嵌套子查询
   - NULL 处理
   - 组合条件
   
4. **第四阶段**: 高级功能（TEST 31-36）
   - SELECT 子句子查询
   - 边界情况

### 测试命令

```bash
# 运行基础测试
./bin/observer -f test/case/test/simple-subquery-basic.sql

# 运行完整测试
./bin/observer -f test/case/test/simple-subquery-test.sql

# 或使用测试框架
cd test
python3 run_test.py simple-subquery-basic
```

---

## 已知难点和注意事项

### 🔴 高优先级

1. **NOT IN 与 NULL**
   - 这是最容易出错的地方
   - 需要严格按照 SQL 标准处理
   - 建议单独测试

2. **子查询返回多行时的比较**
   - 必须检测并报错
   - 或者实现 ANY/ALL 支持

3. **关联子查询作用域**
   - 虽然是"简单"子查询
   - 但 EXISTS 等需要引用外层

### 🟡 中优先级

4. **性能考虑**
   - 子查询可能多次执行
   - 考虑缓存优化
   - 大结果集测试

5. **类型转换**
   - 不同类型值比较
   - 隐式类型转换
   - 错误提示

### 🟢 低优先级

6. **ORDER BY 在子查询中**
   - 通常无意义但应支持
   - 不影响结果正确性

7. **LIMIT 在子查询中**
   - 可能影响结果
   - 需要正确处理

---

## 测试检查清单

使用此清单确保所有功能都已测试：

- [ ] IN 基本功能
- [ ] IN 空集
- [ ] IN 多行结果
- [ ] NOT IN 基本功能
- [ ] NOT IN 空集
- [ ] **NOT IN 包含 NULL（关键）**
- [ ] EXISTS 基本功能
- [ ] EXISTS 真/假两种情况
- [ ] NOT EXISTS 基本功能
- [ ] 比较运算符 (=, >, <, >=, <=, !=)
- [ ] **比较运算子查询返回多行（应报错）**
- [ ] 聚合函数 (MAX, MIN, AVG, COUNT, SUM)
- [ ] 2层嵌套子查询
- [ ] 3层嵌套子查询
- [ ] NULL 在 IN 中的处理
- [ ] NULL 在 NOT IN 中的处理
- [ ] NULL 在比较中的处理
- [ ] AND/OR/NOT 组合
- [ ] SELECT 子句中的子查询
- [ ] 空表边界情况
- [ ] DISTINCT 子查询

---

## 预期输出示例

### TEST 1: 基本 IN
```
输入: SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2);
输出:
id | name | value
----|------|-------
1  | a    | 100
2  | b    | 200
3  | c    | 300
```

### TEST 6: NOT IN 包含 NULL
```
输入: SELECT * FROM t1 WHERE id NOT IN (SELECT score FROM t2);
输出: (空集)
说明: 因为 t2.score 包含 NULL
```

### TEST 12: 比较与聚合
```
输入: SELECT * FROM t1 WHERE value > (SELECT AVG(score) FROM t2);
子查询结果: AVG = 86.25
输出:
id | name | value
----|------|-------
2  | b    | 200
3  | c    | 300
```

---

## 错误处理测试

还应该测试错误情况：

```sql
-- 错误1: 比较运算符用于多行子查询
SELECT * FROM t1 WHERE id = (SELECT id FROM t2);
-- 预期: ERROR - Subquery returns more than 1 row

-- 错误2: 类型不匹配（如果严格类型检查）
SELECT * FROM t1 WHERE name = (SELECT score FROM t2 WHERE id = 1);
-- 预期: ERROR - Type mismatch 或 隐式转换

-- 错误3: 语法错误
SELECT * FROM t1 WHERE id IN;
-- 预期: SYNTAX ERROR
```

---

## 性能测试建议

```sql
-- 大数据量测试
INSERT INTO t1 VALUES ... (1000 rows)
INSERT INTO t2 VALUES ... (10000 rows)

-- 测试执行时间
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2);

-- 测试计划
EXPLAIN SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2);
```

---

## 总结

本测试套件覆盖了简单子查询的所有核心功能：

✅ **完整覆盖**: IN, NOT IN, EXISTS, NOT EXISTS, 比较运算符
✅ **聚合支持**: MAX, MIN, AVG, COUNT, SUM
✅ **NULL 处理**: 各种 NULL 场景
✅ **嵌套支持**: 多层嵌套子查询
✅ **边界测试**: 空集、单值、多值
✅ **组合逻辑**: AND, OR, NOT

🎯 **关键测试点**: NOT IN 与 NULL 的处理
🎯 **难点**: 多行子查询错误检测
🎯 **优化**: 子查询执行性能

---

祝测试顺利！如有问题，请参考 SQL 标准或主流数据库行为（MySQL, PostgreSQL）。


