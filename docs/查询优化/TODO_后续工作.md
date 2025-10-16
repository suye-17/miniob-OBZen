# 查询优化功能 - 后续工作清单

## 已完成✓

- [x] 表追踪机制(`get_involved_tables()`)
- [x] 谓词下推规则(`PredicateToJoinRewriter`)
- [x] Join代价计算器(`JoinCostCalculator`)
- [x] 基于代价的Join物理算子选择
  - [x] `LogicalJoinToNestedLoopJoin`
  - [x] `LogicalJoinToHashJoin`
- [x] 代码编译通过
- [x] 编写完整技术文档

## 待完成事项

### 1. 测试验证 【优先级：高】

#### 1.1 功能测试
- [ ] 创建测试用例验证谓词下推
  - SQL示例：`SELECT * FROM t1, t2 WHERE t1.id = t2.id AND t1.col1 > 10;`
  - 验证点：`t1.col1 > 10` 是否下推到TableScan
  
- [ ] 创建测试用例验证Join算子选择
  - 小表Join: 验证是否选择Hash Join
  - 大表Join: 验证是否选择Hash Join
  - 非等值Join: 验证是否只有NLJ可用

#### 1.2 性能测试
- [ ] 对比优化前后的查询执行时间
- [ ] 测试不同数据规模下的性能表现
- [ ] 生成性能测试报告

**操作指引**:
```bash
cd /home/simpur/miniob-OBZen
# 运行测试（具体命令依赖项目测试框架）
./build_debug/bin/observer -f ./etc/observer.ini ...
```

### 2. 代码Review 【优先级：中】

#### 2.1 代码质量检查
- [ ] 检查是否有内存泄漏
- [ ] 检查是否有未处理的错误码
- [ ] 确认所有public方法都有注释

#### 2.2 性能优化
- [ ] 检查`get_involved_tables()`的性能（是否需要缓存结果）
- [ ] 检查谓词下推的递归深度限制
- [ ] 优化代价计算的效率

### 3. 功能增强 【优先级：中】

#### 3.1 谓词下推扩展
- [ ] 支持`OR`条件的谓词下推
- [ ] 支持子查询的谓词下推
- [ ] 支持更复杂的表达式（如CASE WHEN）

#### 3.2 代价模型完善
- [ ] 引入真实的表统计信息（行数、基数）
- [ ] 考虑索引扫描的代价
- [ ] 支持更精确的选择率估算

#### 3.3 更多Join算法
- [ ] 实现`IndexNestedLoopJoin`
- [ ] 实现`SortMergeJoin`
- [ ] 支持Join重排序优化

### 4. 文档完善 【优先级：低】

- [ ] 添加用户使用手册
- [ ] 添加优化器调优指南
- [ ] 添加性能测试报告模板

### 5. 已知问题

#### 5.1 Linter警告
**文件**: `src/observer/sql/operator/logical_operator.h`  
**问题**: `'common/lang/unordered_set.h' is not used`  
**说明**: 这是误报，该头文件是`get_involved_tables()`返回类型所必需的。

**解决方案**: 可以忽略，或添加注释：
```cpp
// NOLINTNEXTLINE - unordered_set is required for return type
#include "common/lang/unordered_set.h"
```

#### 5.2 其他注意事项
- 当前代价模型使用简化的估算，需要引入统计信息以提高准确性
- Hash Join只支持等值Join，非等值Join会fallback到NLJ
- 谓词下推只处理`Predicate + Join`模式，其他模式暂不支持

---

## 测试建议

### 建议1: 手动验证谓词下推

**步骤**:
1. 启动observer并创建测试表
2. 执行带WHERE条件的JOIN查询
3. 使用`EXPLAIN`查看执行计划（如果支持）
4. 验证谓词是否下推到正确位置

**示例SQL**:
```sql
CREATE TABLE t1(id INT, col1 INT, col2 VARCHAR(20));
CREATE TABLE t2(id INT, col3 INT);
INSERT INTO t1 VALUES (1, 10, 'a'), (2, 20, 'b'), (3, 30, 'c');
INSERT INTO t2 VALUES (1, 100), (2, 200), (4, 400);

-- 测试查询
SELECT * FROM t1, t2 WHERE t1.id = t2.id AND t1.col1 > 15;
-- 预期: t1.col1 > 15 应该在Join之前过滤t1
```

### 建议2: 对比不同Join算法的性能

**步骤**:
1. 创建不同规模的测试表（小表100行，大表10000行）
2. 执行等值Join和非等值Join
3. 记录执行时间
4. 分析算法选择是否合理

**示例测试脚本**:
```bash
# 小表Join（预期：Hash Join更快）
time ./obclient -s ... < test_small_join.sql

# 大表Join（预期：Hash Join更快）
time ./obclient -s ... < test_large_join.sql

# 非等值Join（预期：只能用NLJ）
time ./obclient -s ... < test_non_equi_join.sql
```

---

## 配置说明

### 环境要求
- 编译器：GCC 7+ 或 Clang 6+
- CMake：3.10+
- 操作系统：Linux

### 编译选项
```bash
cd /home/simpur/miniob-OBZen
mkdir -p build_debug && cd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make observer -j$(nproc)
```

### 运行配置
- 配置文件：`./etc/observer.ini`
- 日志级别：可在配置文件中调整
- 优化器开关：目前自动启用，未来可考虑添加开关

---

## 联系与支持

如有问题或建议，请：
1. 查阅 `docs/查询优化/FINAL_查询优化.md`
2. 检查代码注释和测试用例
3. 提交Issue或与团队讨论

---

**更新时间**: 2025-10-16  
**维护者**: 待定

