# 查询优化功能需求对齐文档

## 文档概述
**任务**: 针对Join算子进行查询优化  
**创建时间**: 2025-10-16  
**状态**: 需求分析阶段

---

## 1. 原始需求

### 1.1 任务背景
实现MiniOB的查询优化器，分为两层：
1. **查询改写（基于规则）**: 逻辑算子到逻辑算子的转换
2. **物理优化（基于代价）**: 逻辑算子到物理算子的转换

### 1.2 具体任务

#### 任务A: 谓词下推（基于规则）
将WHERE子句中的条件下推到JOIN算子或其子节点：

**示例1**:
```sql
SELECT * FROM tbl1, tbl2 WHERE tbl1.col3 = tbl2.col4 AND tbl1.col5 > 100;
```
- `tbl1.col5 > 100` → 下推到tbl1的TableScan
- `tbl1.col3 = tbl2.col4` → 下推到Join算子

**示例2**:
```sql
SELECT * FROM tbl1, tbl2, tbl3 
WHERE tbl1.a = tbl2.a AND tbl2.a > tbl3.a AND tbl3.a > 100;
```
- `tbl3.a > 100` → 下推到tbl3的TableScan
- `tbl2.a > tbl3.a` → 下推到相应Join算子
- `tbl1.a = tbl2.a` → 下推到相应Join算子

#### 任务B: Join物理算子选择（基于代价）
根据代价选择HashJoin或NestedLoopJoin：

**代价公式**:

1. **NestedLoopJoin**:
   ```
   cost_nlj = left × right × CPU + output × CPU
   ```

2. **HashJoin**:
   ```
   cost_hashjoin = left × hash_cost + right × hash_probe + output × CPU
   ```

---

## 2. 现有代码分析

### 2.1 相关文件结构

#### 优化器相关
```
src/observer/sql/optimizer/
  ├── predicate_to_join_rule.h/.cpp  (❌ 空实现)
  ├── predicate_pushdown_rewriter.h/.cpp
  ├── rewrite_rule.h
  ├── rewriter.h/.cpp
  ├── cascade/  (Cascade优化器框架)
  │   ├── README.md
  │   ├── cost_model.h
  │   └── ...
```

#### 算子相关
```
src/observer/sql/operator/
  ├── join_logical_operator.h/.cpp
  ├── join_physical_operator.h/.cpp
  ├── hash_join_physical_operator.h/.cpp
  ├── table_scan_logical_operator.h/.cpp
  └── predicate_logical_operator.h/.cpp
```

### 2.2 现有接口

#### RewriteRule接口
```cpp
class RewriteRule {
public:
  virtual RC rewrite(unique_ptr<LogicalOperator> &oper, bool &change_made) = 0;
};
```

#### JoinLogicalOperator
```cpp
class JoinLogicalOperator : public LogicalOperator {
public:
  JoinType join_type() const;
  Expression *condition() const;
};
```

---

## 3. 实现方案

### 3.1 谓词下推实现

#### 核心思路
1. **表名追踪**: 为每个算子和表达式添加涉及的表集合
2. **条件分析**: 分析每个谓词条件涉及哪些表
3. **下推判断**: 根据表集合判断条件能否下推
4. **递归下推**: 条件下推后继续检查能否进一步下推

#### 数据结构扩展
```cpp
// 为LogicalOperator添加
class LogicalOperator {
  std::set<std::string> involved_tables_;  // 涉及的表
};

// 为Expression添加
class Expression {
  std::set<std::string> involved_tables();  // 获取涉及的表
};
```

#### PredicateToJoinRewriter结构
```cpp
class PredicateToJoinRewriter : public RewriteRule {
public:
  RC rewrite(unique_ptr<LogicalOperator> &oper, bool &change_made) override;

private:
  // 收集算子涉及的表
  void collect_tables(LogicalOperator *oper, std::set<std::string> &tables);
  
  // 分析谓词可下推位置
  RC analyze_predicate(Expression *pred, const std::set<std::string> &tables, 
                       PushdownTarget &target);
  
  // 执行谓词下推
  RC push_predicate(unique_ptr<LogicalOperator> &oper, Expression *pred);
};
```

### 3.2 Join物理算子选择

#### Cascade框架集成
在`src/observer/sql/optimizer/cascade/`中添加：

```cpp
// join_physical_rule.h
class JoinLogicalToPhysicalRule : public Rule {
public:
  RC apply(GroupExpression *group_expr, OptimizationContext &ctx) override;
  
private:
  // 生成HashJoin物理算子
  RC generate_hash_join(GroupExpression *group_expr, OptimizationContext &ctx);
  
  // 生成NestedLoopJoin物理算子
  RC generate_nested_loop_join(GroupExpression *group_expr, OptimizationContext &ctx);
};
```

#### 代价计算
```cpp
// cost_calculator.h
class JoinCostCalculator {
public:
  // 计算NLJ代价
  double calculate_nlj_cost(size_t left_card, size_t right_card, size_t output_card);
  
  // 计算HashJoin代价
  double calculate_hash_join_cost(size_t left_card, size_t right_card, size_t output_card);
};
```

---

## 4. 实现步骤

### 阶段1: 表追踪机制 (opt-3)
1. 为LogicalOperator添加表集合字段
2. 为Expression添加表集合计算方法
3. 在算子创建时收集涉及的表

### 阶段2: 谓词下推 (opt-2)
1. 实现PredicateToJoinRewriter类
2. 实现条件分析逻辑
3. 实现下推执行逻辑
4. 集成到Rewriter流程

### 阶段3: Join代价计算 (opt-5)
1. 实现JoinCostCalculator
2. 获取表的基数信息
3. 实现两种Join的代价公式

### 阶段4: 物理算子选择 (opt-4)
1. 创建JoinLogicalToPhysicalRule
2. 实现代价比较逻辑
3. 选择最优物理算子
4. 集成到Cascade框架

### 阶段5: 测试验证 (opt-6)
1. 运行dblab-optimizer.test
2. 验证谓词下推正确性
3. 验证Join算子选择正确性

---

## 5. 技术约束

### 必须遵守
- ❌ 不能修改`cost_model.h`
- ✅ 必须使用Cascade框架
- ✅ 必须支持多表Join
- ✅ 不需要考虑OR条件

### 已有基础
- ✅ HashJoin已实现（任务2）
- ✅ INNER JOIN语法已支持
- ✅ Cascade优化器框架已存在

---

## 6. 验收标准

### 功能正确性
1. WHERE条件正确下推到TableScan
2. JOIN条件正确下推到Join算子
3. 多表Join的条件正确处理
4. 基于代价选择正确的Join算子

### 性能提升
1. 下推后减少中间结果
2. 正确选择更优的Join算法

### 测试通过
- ✅ dblab-optimizer.test中所有相关用例通过

---

## 7. 参考资料

### 内部文档
- `src/observer/sql/optimizer/cascade/README.md`
- 现有的`predicate_pushdown_rewriter.h`

### 论文
- Cascade论文
- Columbia论文

### 测试
- `test/case/test/dblab-optimizer.test`

---

**下一步**: 开始实现阶段1 - 表追踪机制

