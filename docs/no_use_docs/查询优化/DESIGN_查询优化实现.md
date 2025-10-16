# 查询优化功能详细设计文档

## 文档概述
**任务**: Join算子查询优化  
**创建时间**: 2025-10-16  
**最后更新**: 2025-10-16  
**当前状态**: 表追踪机制已完成✅，谓词下推和物理算子选择待实现

---

## 1. 已完成工作 ✅

### 1.1 表追踪机制（opt-3）✅

#### 修改的文件
1. **src/observer/sql/operator/logical_operator.h**
   - 添加虚函数: `virtual std::unordered_set<std::string> get_involved_tables() const;`

2. **src/observer/sql/operator/logical_operator.cpp**
   - 实现默认行为：递归收集所有子算子的表

3. **src/observer/sql/operator/table_get_logical_operator.h**
   - 重写方法：直接返回`table_->name()`

4. **src/observer/sql/expr/expression.h**
   - 为Expression基类添加: `virtual std::unordered_set<std::string> get_involved_tables() const;`
   - FieldExpr: 返回`field_.table_name()`
   - ComparisonExpr: 合并左右表达式的表
   - ConjunctionExpr: 合并所有子表达式的表

#### 使用示例
```cpp
// 获取算子涉及的表
auto table_scan = make_unique<TableGetLogicalOperator>(table, READ);
auto tables = table_scan->get_involved_tables();  // 返回 {"table_name"}

// 获取表达式涉及的表
FieldExpr field(table, field_meta);
auto tables = field.get_involved_tables();  // 返回 {"table_name"}
```

---

## 2. 待实现：谓词下推规则（opt-2）

### 2.1 核心算法

```
对于每个PredicateLogicalOperator：
1. 收集所有谓词条件（AND连接的多个条件）
2. 对于每个谓词：
   a. 获取涉及的表集合: predicate_tables = pred->get_involved_tables()
   b. 如果 predicate_tables.size() == 1:
      → 单表条件，尝试下推到TableScan
   c. 如果 predicate_tables.size() > 1:
      → 多表条件，尝试下推到Join
3. 下推后如果Predicate没有条件了，删除该算子
4. 递归处理子算子
```

### 2.2 实现代码框架

**src/observer/sql/optimizer/predicate_to_join_rule.h**
```cpp
#pragma once

#include "common/lang/vector.h"
#include "sql/optimizer/rewrite_rule.h"

class PredicateToJoinRewriter : public RewriteRule
{
public:
  PredicateToJoinRewriter() = default;
  virtual ~PredicateToJoinRewriter() = default;

  RC rewrite(unique_ptr<LogicalOperator> &oper, bool &change_made) override;

private:
  // 尝试将谓词下推到子节点
  RC try_push_down_predicate(
      unique_ptr<LogicalOperator> &oper,
      Expression *predicate,
      bool &pushed);
  
  // 检查条件能否下推到指定算子
  bool can_push_to_operator(
      LogicalOperator *target_oper,
      Expression *predicate);
  
  // 将谓词添加到JOIN条件
  RC push_to_join(
      JoinLogicalOperator *join_oper,
      Expression *predicate);
  
  // 将谓词添加到TableScan
  RC push_to_table_scan(
      TableGetLogicalOperator *scan_oper,
      Expression *predicate);
};
```

**src/observer/sql/optimizer/predicate_to_join_rule.cpp**
```cpp
#include "sql/optimizer/predicate_to_join_rule.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"

RC PredicateToJoinRewriter::rewrite(unique_ptr<LogicalOperator> &oper, bool &change_made)
{
  RC rc = RC::SUCCESS;
  
  // 递归处理子节点
  for (auto &child : oper->children()) {
    rc = rewrite(child, change_made);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  
  // 处理Predicate算子
  if (oper->type() != LogicalOperatorType::PREDICATE) {
    return RC::SUCCESS;
  }
  
  // 获取谓词表达式
  auto &predicates = oper->expressions();
  if (predicates.empty()) {
    return RC::SUCCESS;
  }
  
  // 对每个谓词尝试下推
  vector<unique_ptr<Expression>> remaining_predicates;
  
  for (auto &pred : predicates) {
    bool pushed = false;
    
    // 尝试下推到子节点
    rc = try_push_down_predicate(oper, pred.get(), pushed);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    
    if (!pushed) {
      // 无法下推，保留在当前位置
      remaining_predicates.push_back(std::move(pred));
    } else {
      change_made = true;
    }
  }
  
  // 如果所有谓词都下推了，删除Predicate算子
  if (remaining_predicates.empty() && !oper->children().empty()) {
    // 用子节点替换当前节点
    oper = std::move(oper->children()[0]);
    change_made = true;
  }
  
  return RC::SUCCESS;
}

RC PredicateToJoinRewriter::try_push_down_predicate(
    unique_ptr<LogicalOperator> &oper,
    Expression *predicate,
    bool &pushed)
{
  // 获取谓词涉及的表
  auto pred_tables = predicate->get_involved_tables();
  
  if (pred_tables.empty()) {
    return RC::SUCCESS;  // 常量条件，不下推
  }
  
  // 检查子节点
  if (oper->children().empty()) {
    return RC::SUCCESS;
  }
  
  auto &child = oper->children()[0];
  
  // 情况1：下推到TableScan
  if (child->type() == LogicalOperatorType::TABLE_GET && pred_tables.size() == 1) {
    auto table_scan = dynamic_cast<TableGetLogicalOperator*>(child.get());
    auto scan_tables = table_scan->get_involved_tables();
    
    // 检查谓词涉及的表是否匹配
    if (scan_tables == pred_tables) {
      return push_to_table_scan(table_scan, predicate);
    }
  }
  
  // 情况2：下推到Join
  if (child->type() == LogicalOperatorType::JOIN) {
    auto join_oper = dynamic_cast<JoinLogicalOperator*>(child.get());
    auto join_tables = join_oper->get_involved_tables();
    
    // 检查JOIN涉及的表是否包含谓词的所有表
    bool all_tables_in_join = true;
    for (const auto &pred_table : pred_tables) {
      if (join_tables.find(pred_table) == join_tables.end()) {
        all_tables_in_join = false;
        break;
      }
    }
    
    if (all_tables_in_join) {
      return push_to_join(join_oper, predicate);
    }
  }
  
  pushed = false;
  return RC::SUCCESS;
}

RC PredicateToJoinRewriter::push_to_join(
    JoinLogicalOperator *join_oper,
    Expression *predicate)
{
  // 将谓词添加到JOIN条件
  // 需要与现有JOIN条件使用AND连接
  
  auto existing_condition = join_oper->condition();
  
  if (existing_condition == nullptr) {
    // 直接设置为JOIN条件
    // join_oper->set_condition(predicate->copy());
    // 注意：需要JoinLogicalOperator有set_condition方法
  } else {
    // 使用ConjunctionExpr连接
    vector<unique_ptr<Expression>> children;
    children.push_back(existing_condition->copy());
    children.push_back(predicate->copy());
    auto conj = make_unique<ConjunctionExpr>(ConjunctionExpr::Type::AND, children);
    // join_oper->set_condition(std::move(conj));
  }
  
  return RC::SUCCESS;
}

RC PredicateToJoinRewriter::push_to_table_scan(
    TableGetLogicalOperator *scan_oper,
    Expression *predicate)
{
  // 将谓词添加到TableScan的predicates
  vector<unique_ptr<Expression>> preds;
  preds.push_back(predicate->copy());
  scan_oper->set_predicates(std::move(preds));
  
  return RC::SUCCESS;
}
```

### 2.3 需要修改JoinLogicalOperator

需要为JoinLogicalOperator添加设置条件的方法：

```cpp
// join_logical_operator.h
class JoinLogicalOperator : public LogicalOperator {
public:
  // 添加方法
  void set_condition(unique_ptr<Expression> condition) {
    condition_ = condition;
  }
  
  void add_condition(unique_ptr<Expression> additional_cond) {
    if (condition_ == nullptr) {
      condition_ = std::move(additional_cond);
    } else {
      // 使用AND连接
      vector<unique_ptr<Expression>> children;
      children.push_back(std::move(condition_));
      children.push_back(std::move(additional_cond));
      condition_ = make_unique<ConjunctionExpr>(
          ConjunctionExpr::Type::AND, children);
    }
  }
};
```

### 2.4 集成到Rewriter

在`src/observer/sql/optimizer/rewriter.cpp`中注册规则：

```cpp
// 在optimize方法中添加
PredicateToJoinRewriter pred_to_join_rewriter;
rc = pred_to_join_rewriter.rewrite(logical_operator, change_made);
if (rc != RC::SUCCESS) {
  return rc;
}
```

---

## 3. 待实现：Join物理算子选择（opt-4, opt-5）

### 3.1 代价计算

**src/observer/sql/optimizer/cascade/join_cost_calculator.h**
```cpp
#pragma once

#include "common/sys/rc.h"

class Statistics;

class JoinCostCalculator {
public:
  // 计算NestedLoopJoin代价
  // cost = left_card * right_card * CPU + output_card * CPU
  static double calculate_nlj_cost(
      double left_card,
      double right_card,
      double output_card);
  
  // 计算HashJoin代价
  // cost = left_card * HASH_COST + right_card * HASH_PROBE + output_card * CPU
  static double calculate_hash_join_cost(
      double left_card,
      double right_card,
      double output_card);
  
  // 检查是否为等值JOIN条件
  static bool is_equi_join(Expression *condition);
};
```

**src/observer/sql/optimizer/cascade/join_cost_calculator.cpp**
```cpp
#include "sql/optimizer/cascade/join_cost_calculator.h"
#include "sql/optimizer/cascade/cost_model.h"
#include "sql/expr/expression.h"

double JoinCostCalculator::calculate_nlj_cost(
    double left_card,
    double right_card,
    double output_card)
{
  double cpu_cost = CostModel::CPU_COST;
  return left_card * right_card * cpu_cost + output_card * cpu_cost;
}

double JoinCostCalculator::calculate_hash_join_cost(
    double left_card,
    double right_card,
    double output_card)
{
  double hash_cost = CostModel::HASH_COST;
  double hash_probe = CostModel::HASH_PROBE_COST;
  double cpu_cost = CostModel::CPU_COST;
  
  return left_card * hash_cost + right_card * hash_probe + output_card * cpu_cost;
}

bool JoinCostCalculator::is_equi_join(Expression *condition)
{
  if (condition == nullptr) {
    return false;
  }
  
  if (condition->type() == ExprType::COMPARISON) {
    auto comp_expr = static_cast<ComparisonExpr*>(condition);
    return comp_expr->comp() == EQUAL_TO;
  }
  
  if (condition->type() == ExprType::CONJUNCTION) {
    auto conj_expr = static_cast<ConjunctionExpr*>(condition);
    // 所有子条件都必须是等值条件
    for (auto &child : conj_expr->children()) {
      if (!is_equi_join(child.get())) {
        return false;
      }
    }
    return true;
  }
  
  return false;
}
```

### 3.2 Join物理算子选择规则

**src/observer/sql/optimizer/cascade/join_logical_to_physical_rule.h**
```cpp
#pragma once

#include "sql/optimizer/cascade/rule.h"

class JoinLogicalToPhysicalRule : public Rule {
public:
  RC apply(GroupExpression *group_expr, OptimizationContext &ctx) override;
  
private:
  // 生成HashJoin物理算子
  RC generate_hash_join(GroupExpression *group_expr, OptimizationContext &ctx);
  
  // 生成NestedLoopJoin物理算子
  RC generate_nlj(GroupExpression *group_expr, OptimizationContext &ctx);
};
```

### 3.3 注册规则

在`src/observer/sql/optimizer/cascade/rules.h`的`RuleSet`中注册：

```cpp
// 在init_logical_rules()或init_implementation_rules()中
rules_.push_back(make_unique<JoinLogicalToPhysicalRule>());
```

---

## 4. 测试验证（opt-6）

### 4.1 运行测试
```bash
cd test
./run-test.sh case/test/dblab-optimizer.test
```

### 4.2 验证点
1. ✅ WHERE中的单表条件下推到TableScan
2. ✅ WHERE中的多表条件下推到Join
3. ✅ 小数据量选择NLJ
4. ✅ 大数据量+等值条件选择HashJoin
5. ✅ 非等值条件只能选NLJ

---

## 5. 编译和调试

### 5.1 编译
```bash
cd build_debug
make observer -j4
```

### 5.2 查看执行计划
```sql
EXPLAIN select * from t1, t2 where t1.id = t2.id and t1.col > 100;
```

### 5.3 开启Cascade优化器
```sql
SET use_cascade=1;
```

---

## 6. 参考资料

### 内部文档
- `docs/docs/design/miniob-cascade.md` - Cascade优化器设计
- `src/observer/sql/optimizer/cascade/README.md`

### 测试用例
- `test/case/test/dblab-optimizer.test`

### 相关代码
- `src/observer/sql/optimizer/predicate_pushdown_rewriter.cpp` - 参考实现
- `src/observer/sql/optimizer/cascade/rules.h` - 规则注册

---

**下一步**: 实现PredicateToJoinRewriter的完整代码

