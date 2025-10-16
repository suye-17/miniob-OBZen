# 查询优化实现进度报告

## 概述
**任务**: Join算子查询优化（谓词下推+物理算子选择）  
**开始时间**: 2025-10-16  
**当前状态**: 表追踪机制已完成✅，等待继续实现谓词下推和物理算子选择

---

## ✅ 已完成工作

### 1. 需求分析和方案设计 ✅
- ✅ 创建 `ALIGNMENT_查询优化.md` - 需求对齐文档
- ✅ 创建 `CONSENSUS_查询优化.md` - 实现共识文档
- ✅ 创建 `DESIGN_查询优化实现.md` - 详细设计文档
- ✅ 研究测试用例 `dblab-optimizer.test`
- ✅ 研究Cascade优化器框架

### 2. 表追踪机制实现 ✅ (opt-3)

#### 修改的文件
1. **src/observer/sql/operator/logical_operator.h** ✅
   ```cpp
   // 添加虚函数
   virtual std::unordered_set<std::string> get_involved_tables() const;
   ```

2. **src/observer/sql/operator/logical_operator.cpp** ✅
   ```cpp
   // 默认实现：递归收集子算子的表
   std::unordered_set<std::string> LogicalOperator::get_involved_tables() const {
     std::unordered_set<std::string> tables;
     for (const auto &child : children_) {
       if (child) {
         auto child_tables = child->get_involved_tables();
         tables.insert(child_tables.begin(), child_tables.end());
       }
     }
     return tables;
   }
   ```

3. **src/observer/sql/operator/table_get_logical_operator.h** ✅
   ```cpp
   // 重写：返回表名
   std::unordered_set<std::string> get_involved_tables() const override {
     std::unordered_set<std::string> tables;
     if (table_) {
       tables.insert(table_->name());
     }
     return tables;
   }
   ```

4. **src/observer/sql/expr/expression.h** ✅
   ```cpp
   // 添加include
   #include "common/lang/unordered_set.h"
   
   // Expression基类：默认返回空集合
   virtual std::unordered_set<std::string> get_involved_tables() const { 
     return {}; 
   }
   
   // FieldExpr：返回字段所属表名
   std::unordered_set<std::string> get_involved_tables() const override {
     std::unordered_set<std::string> tables;
     if (field_.table_name() && field_.table_name()[0] != '\0') {
       tables.insert(field_.table_name());
     }
     return tables;
   }
   
   // ComparisonExpr：合并左右表达式的表
   std::unordered_set<std::string> get_involved_tables() const override {
     std::unordered_set<std::string> tables;
     if (left_) {
       auto left_tables = left_->get_involved_tables();
       tables.insert(left_tables.begin(), left_tables.end());
     }
     if (right_) {
       auto right_tables = right_->get_involved_tables();
       tables.insert(right_tables.begin(), right_tables.end());
     }
     return tables;
   }
   
   // ConjunctionExpr：合并所有子表达式的表
   std::unordered_set<std::string> get_involved_tables() const override {
     std::unordered_set<std::string> tables;
     for (const auto &child : children_) {
       if (child) {
         auto child_tables = child->get_involved_tables();
         tables.insert(child_tables.begin(), child_tables.end());
       }
     }
     return tables;
   }
   ```

#### 编译测试 ✅
- ✅ 编译通过
- ✅ 无linter错误
- ✅ 基础功能可用

---

## ⏳ 待实现工作

### 1. 谓词下推规则 (opt-2)

#### 需要创建的文件
- `src/observer/sql/optimizer/predicate_to_join_rule.cpp`

#### 需要修改的文件
- `src/observer/sql/optimizer/predicate_to_join_rule.h` (当前为空)
- `src/observer/sql/operator/join_logical_operator.h` (添加set_condition方法)
- `src/observer/sql/optimizer/rewriter.cpp` (注册新规则)

#### 核心逻辑
```
1. 遍历逻辑计划树，找到PredicateLogicalOperator
2. 对每个谓词条件：
   - 使用get_involved_tables()获取涉及的表
   - 如果只涉及1个表 → 下推到TableScan
   - 如果涉及多个表 → 下推到Join
3. 所有谓词下推后，删除Predicate算子
```

#### 参考代码
详见 `docs/查询优化/DESIGN_查询优化实现.md` 第2节

### 2. Join物理算子选择 (opt-4)

#### 需要创建的文件
- `src/observer/sql/optimizer/cascade/join_logical_to_physical_rule.h`
- `src/observer/sql/optimizer/cascade/join_logical_to_physical_rule.cpp`

#### 需要修改的文件
- `src/observer/sql/optimizer/cascade/rules.h` (注册新规则)

#### 核心逻辑
```
1. 判断JOIN条件类型：
   - 等值条件 → 可选HashJoin或NLJ
   - 非等值条件 → 只能选NLJ
2. 获取统计信息（基数）
3. 计算两种算子的代价
4. 选择代价最小的
```

### 3. 代价计算 (opt-5)

#### 需要创建的文件
- `src/observer/sql/optimizer/cascade/join_cost_calculator.h`
- `src/observer/sql/optimizer/cascade/join_cost_calculator.cpp`

#### 代价公式
```cpp
// NLJ代价
cost_nlj = left_card * right_card * CPU_COST + output_card * CPU_COST

// HashJoin代价
cost_hash = left_card * HASH_COST + right_card * HASH_PROBE_COST + output_card * CPU_COST
```

#### 参考
`src/observer/sql/optimizer/cascade/cost_model.h` 中的常量

### 4. 测试验证 (opt-6)

#### 测试用例
```bash
cd /home/simpur/miniob-OBZen/test
./run-test.sh case/test/dblab-optimizer.test
```

#### 预期结果
- ✅ 行21-50: 谓词下推验证（use_cascade=0）
- ✅ 行56-100: 物理算子选择验证（use_cascade=1）

---

## 📋 实现检查清单

### 谓词下推
- [ ] 实现PredicateToJoinRewriter类
- [ ] 实现try_push_down_predicate方法
- [ ] 实现push_to_join方法
- [ ] 实现push_to_table_scan方法
- [ ] 为JoinLogicalOperator添加set_condition/add_condition方法
- [ ] 在rewriter.cpp中注册规则
- [ ] 编译测试
- [ ] 运行dblab-optimizer.test (行21-50)

### 物理算子选择
- [ ] 实现JoinCostCalculator类
- [ ] 实现calculate_nlj_cost方法
- [ ] 实现calculate_hash_join_cost方法
- [ ] 实现is_equi_join方法
- [ ] 实现JoinLogicalToPhysicalRule类
- [ ] 在rules.h中注册规则
- [ ] 编译测试
- [ ] 运行dblab-optimizer.test (行56-100)

---

## 🔍 下一步行动

### 选项1: 继续实现谓词下推
1. 打开 `src/observer/sql/optimizer/predicate_to_join_rule.h`
2. 实现PredicateToJoinRewriter类（参考DESIGN文档第2.2节）
3. 在rewriter.cpp中注册
4. 编译并运行部分测试

### 选项2: 跳到物理算子选择
1. 先实现JoinCostCalculator
2. 实现JoinLogicalToPhysicalRule
3. 在cascade框架中注册
4. 测试代价计算是否正确

### 选项3: 分阶段完整实现
1. 完成谓词下推 + 测试
2. 完成物理算子选择 + 测试
3. 整体测试
4. 编写文档

---

## 📚 参考文档

### 已创建文档
- `docs/查询优化/ALIGNMENT_查询优化.md` - 需求分析
- `docs/查询优化/CONSENSUS_查询优化.md` - 实现方案
- `docs/查询优化/DESIGN_查询优化实现.md` - 详细设计（含代码框架）
- `docs/查询优化/PROGRESS_当前进度.md` - 本文档

### 参考资料
- `docs/docs/design/miniob-cascade.md` - Cascade优化器
- `src/observer/sql/optimizer/predicate_pushdown_rewriter.cpp` - 参考实现
- `test/case/test/dblab-optimizer.test` - 测试用例

---

## 💡 提示

**如果继续实现**:
- 谓词下推相对简单，建议先完成
- 物理算子选择涉及Cascade框架，需仔细阅读文档
- 每完成一个TODO就编译测试
- 使用EXPLAIN查看执行计划验证

**如果遇到问题**:
- 参考`predicate_pushdown_rewriter.cpp`的实现模式
- 查看`docs/docs/design/miniob-cascade.md`理解框架
- 使用LOG_INFO添加调试日志
- 运行单个测试用例而非全部

---

**最后更新**: 2025-10-16  
**编译状态**: ✅ 通过  
**测试状态**: ⏳ 待运行

