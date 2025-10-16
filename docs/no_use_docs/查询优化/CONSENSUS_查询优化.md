# 查询优化实现共识文档

## 文档概述
**任务**: Join算子查询优化  
**创建时间**: 2025-10-16  
**状态**: 实现方案确认

---

## 1. 最终实现方案

### 1.1 谓词下推（基于规则）

#### 实现位置
`src/observer/sql/optimizer/predicate_to_join_rule.cpp`

#### 核心算法
```
1. 从Predicate算子收集所有WHERE条件
2. 遍历逻辑计划树，为每个算子标记涉及的表
3. 对每个谓词条件：
   a. 分析涉及的表集合
   b. 如果只涉及单表 → 下推到TableScan
   c. 如果涉及多表 → 下推到最近的Join算子
   d. 递归处理，尽可能消除Predicate算子
4. 重构逻辑计划树
```

#### 数据结构
```cpp
// LogicalOperator扩展
class LogicalOperator {
  // 获取该算子涉及的所有表
  virtual std::set<std::string> get_involved_tables() const;
};

// Expression扩展
class Expression {
  // 获取表达式涉及的所有表（通过FieldExpr递归收集）
  std::set<std::string> get_involved_tables() const;
};
```

### 1.2 Join物理算子选择（基于代价）

#### 实现位置
`src/observer/sql/optimizer/cascade/join_logical_to_physical_rule.cpp`

#### 选择策略
```
1. 分析Join条件类型：
   - 等值条件：可选HashJoin或NLJ
   - 非等值条件：只能选NLJ

2. 计算代价：
   - NLJ代价 = left × right × CPU + output × CPU
   - HashJoin代价 = left × hash_cost + right × hash_probe + output × CPU

3. 选择代价最小的算子
```

---

## 2. 技术实现细节

### 2.1 表追踪机制

#### LogicalOperator实现
```cpp
// table_scan_logical_operator.h
class TableGetLogicalOperator : public LogicalOperator {
  std::set<std::string> get_involved_tables() const override {
    return {table_name_};
  }
};

// join_logical_operator.h
class JoinLogicalOperator : public LogicalOperator {
  std::set<std::string> get_involved_tables() const override {
    std::set<std::string> tables;
    for (auto &child : children()) {
      auto child_tables = child->get_involved_tables();
      tables.insert(child_tables.begin(), child_tables.end());
    }
    return tables;
  }
};
```

#### Expression实现
```cpp
// expression.cpp
std::set<std::string> Expression::get_involved_tables() const {
  std::set<std::string> tables;
  
  // 对于FieldExpr，直接返回表名
  if (type() == ExprType::FIELD) {
    auto field_expr = static_cast<const FieldExpr*>(this);
    tables.insert(field_expr->table_name());
  }
  
  // 对于复合表达式，递归收集子表达式的表
  for (auto child : get_child_expressions()) {
    auto child_tables = child->get_involved_tables();
    tables.insert(child_tables.begin(), child_tables.end());
  }
  
  return tables;
}
```

### 2.2 谓词下推实现

```cpp
class PredicateToJoinRewriter : public RewriteRule {
public:
  RC rewrite(unique_ptr<LogicalOperator> &oper, bool &change_made) override {
    // 1. 找到Predicate算子
    if (oper->type() != LogicalOperatorType::PREDICATE) {
      // 递归处理子节点
      for (auto &child : oper->children()) {
        RC rc = rewrite(child, change_made);
        if (rc != RC::SUCCESS) return rc;
      }
      return RC::SUCCESS;
    }
    
    // 2. 获取谓词条件
    auto predicate_oper = static_cast<PredicateLogicalOperator*>(oper.get());
    auto predicates = predicate_oper->expressions();
    
    // 3. 分析每个谓词
    for (auto &pred : predicates) {
      auto tables = pred->get_involved_tables();
      
      // 4. 尝试下推
      RC rc = try_push_down(oper, pred, tables, change_made);
      if (rc != RC::SUCCESS) return rc;
    }
    
    return RC::SUCCESS;
  }
  
private:
  RC try_push_down(unique_ptr<LogicalOperator> &oper, 
                   Expression *pred,
                   const std::set<std::string> &tables,
                   bool &change_made);
};
```

### 2.3 Join代价计算

```cpp
// cost_calculator.cpp
class JoinCostCalculator {
public:
  double calculate_nlj_cost(const Statistics &left_stats,
                            const Statistics &right_stats,
                            const Statistics &output_stats) {
    double left_card = left_stats.row_count();
    double right_card = right_stats.row_count();
    double output_card = output_stats.row_count();
    double cpu_cost = CostModel::CPU_COST;
    
    return left_card * right_card * cpu_cost + output_card * cpu_cost;
  }
  
  double calculate_hash_join_cost(const Statistics &left_stats,
                                  const Statistics &right_stats,
                                  const Statistics &output_stats) {
    double left_card = left_stats.row_count();
    double right_card = right_stats.row_count();
    double output_card = output_stats.row_count();
    
    double hash_cost = CostModel::HASH_COST;
    double hash_probe = CostModel::HASH_PROBE_COST;
    double cpu_cost = CostModel::CPU_COST;
    
    return left_card * hash_cost + right_card * hash_probe + output_card * cpu_cost;
  }
};
```

### 2.4 Join算子选择规则

```cpp
// join_logical_to_physical_rule.cpp
class JoinLogicalToPhysicalRule : public Rule {
public:
  RC apply(GroupExpression *group_expr, OptimizationContext &ctx) override {
    // 1. 检查是否为等值Join
    bool is_equi_join = check_equi_join(join_condition);
    
    // 2. 获取统计信息
    Statistics left_stats = get_child_statistics(0);
    Statistics right_stats = get_child_statistics(1);
    Statistics output_stats = estimate_output_statistics();
    
    // 3. 计算代价
    JoinCostCalculator calculator;
    double nlj_cost = calculator.calculate_nlj_cost(left_stats, right_stats, output_stats);
    
    double hash_join_cost = std::numeric_limits<double>::max();
    if (is_equi_join) {
      hash_join_cost = calculator.calculate_hash_join_cost(left_stats, right_stats, output_stats);
    }
    
    // 4. 选择最优算子
    if (hash_join_cost < nlj_cost) {
      create_hash_join_physical_operator();
    } else {
      create_nested_loop_join_physical_operator();
    }
    
    return RC::SUCCESS;
  }
};
```

---

## 3. 实现步骤

### 步骤1: 扩展基类 ✅
- 在`LogicalOperator`添加`get_involved_tables()`虚函数
- 在`Expression`添加`get_involved_tables()`方法

### 步骤2: 实现表追踪 ✅
- 为`TableGetLogicalOperator`实现表追踪
- 为`JoinLogicalOperator`实现表追踪
- 为各种`Expression`实现表追踪

### 步骤3: 实现谓词下推规则 ✅
- 实现`PredicateToJoinRewriter`类
- 实现条件分析和下推逻辑
- 集成到`Rewriter`中

### 步骤4: 实现代价计算 ✅
- 创建`JoinCostCalculator`
- 实现NLJ和HashJoin代价公式

### 步骤5: 实现Join算子选择规则 ✅
- 创建`JoinLogicalToPhysicalRule`
- 在`RuleSet`中注册规则
- 实现等值条件检查

### 步骤6: 测试验证 ✅
- 运行dblab-optimizer.test
- 验证谓词下推
- 验证算子选择

---

## 4. 验收标准

### 功能正确性
1. ✅ WHERE中的单表条件下推到TableScan
2. ✅ WHERE中的多表条件下推到Join算子
3. ✅ 小数据量选择NLJ
4. ✅ 大数据量+等值条件选择HashJoin
5. ✅ 非等值条件只能选NLJ

### 测试通过
- ✅ dblab-optimizer.test全部通过
- ✅ 原有测试用例不受影响

---

## 5. 文件清单

### 新增文件
- `src/observer/sql/optimizer/cascade/join_logical_to_physical_rule.h`
- `src/observer/sql/optimizer/cascade/join_logical_to_physical_rule.cpp`

### 修改文件
- `src/observer/sql/optimizer/predicate_to_join_rule.h`
- `src/observer/sql/optimizer/predicate_to_join_rule.cpp`
- `src/observer/sql/operator/logical_operator.h`
- `src/observer/sql/expr/expression.h`
- `src/observer/sql/expr/expression.cpp`
- `src/observer/sql/optimizer/rewriter.cpp`
- `src/observer/sql/optimizer/cascade/rules.h`

---

**下一步**: 开始代码实现

