# WHERE子句表达式支持设计文档

## 1. 功能概述

本文档描述如何在MiniOB的WHERE子句中支持算术表达式，使得可以执行类似以下的SQL查询：

```sql
SELECT * FROM exp_table WHERE 3 + col2 < 9;
SELECT * FROM table1 WHERE col1 * 2 > col2 + 10;
```

## 2. 实现目标

- 支持WHERE子句中的算术表达式（加、减、乘、除）
- 支持表达式与字段、常量的比较
- 正确绑定表达式中的字段引用
- 正确计算表达式的值类型

## 3. 实现方案

### 3.1 语法解析层（Parser）

#### 3.1.1 修改 parse_defs.h

在 `ConditionSqlNode` 结构中添加表达式字段：

```cpp
struct ConditionSqlNode
{
  // ... 原有字段 ...
  
  // 新增：表达式支持
  Expression *left_expr;   ///< 左侧表达式（如果不为nullptr，则使用表达式）
  Expression *right_expr;  ///< 右侧表达式（如果不为nullptr，则使用表达式）
  
  // 构造函数中初始化
  ConditionSqlNode() 
    : left_is_attr(0), comp(NO_OP), right_is_attr(0), 
      has_subquery(false), subquery(nullptr), 
      left_expr(nullptr), right_expr(nullptr) {}
  
  // 析构函数
  ~ConditionSqlNode();
  
  // 拷贝构造函数和赋值操作符
  ConditionSqlNode(const ConditionSqlNode& other);
  ConditionSqlNode& operator=(const ConditionSqlNode& other);
};
```

#### 3.1.2 修改 parse_defs.cpp

实现 `ConditionSqlNode` 的析构函数和拷贝函数，确保表达式的正确管理：

```cpp
ConditionSqlNode::~ConditionSqlNode() {
  if (left_expr) {
    delete left_expr;
    left_expr = nullptr;
  }
  if (right_expr) {
    delete right_expr;
    right_expr = nullptr;
  }
}

ConditionSqlNode::ConditionSqlNode(const ConditionSqlNode& other) {
  // 拷贝基本字段...
  
  // 深拷贝表达式
  if (other.left_expr) {
    left_expr = other.left_expr->copy().release();
  } else {
    left_expr = nullptr;
  }
  
  if (other.right_expr) {
    right_expr = other.right_expr->copy().release();
  } else {
    right_expr = nullptr;
  }
}
```

#### 3.1.3 修改 yacc_sql.y

在 `condition` 语法规则的**最前面**添加 `expression comp_op expression` 规则：

```yacc
condition:
    expression comp_op expression
    {
      $$ = new ConditionSqlNode;
      $$->left_expr = $1;
      $$->right_expr = $3;
      $$->comp = $2;
    }
    | rel_attr comp_op value
    {
      // ... 原有规则保持不变 ...
    }
    // ... 其他规则 ...
```

**重要提示**：必须将 `expression comp_op expression` 放在第一个，这样yacc会优先尝试匹配表达式规则。

### 3.2 语句层（Statement）

#### 3.2.1 修改 filter_stmt.h

在 `FilterObj` 结构中添加表达式字段：

```cpp
struct FilterObj
{
  bool  is_attr;
  Field field;
  Value value;
  vector<Value> value_list;
  bool has_value_list = false;
  bool has_subquery = false;
  unique_ptr<SelectSqlNode> subquery = nullptr;
  
  // 新增：表达式支持
  Expression *expr = nullptr;  ///< 表达式（如算术表达式等）
  
  // 构造函数、析构函数、拷贝构造等（实现在cpp中）
  FilterObj() = default;
  FilterObj(const FilterObj& other);
  FilterObj& operator=(const FilterObj& other);
  ~FilterObj();

  // 初始化表达式的方法
  void init_expr(Expression *expression)
  {
    is_attr = false;
    this->expr = expression;
  }
};
```

#### 3.2.2 修改 filter_stmt.cpp

在 `FilterObj` 的实现中添加表达式管理：

```cpp
// FilterObj 拷贝构造函数实现
FilterObj::FilterObj(const FilterObj& other) 
  : is_attr(other.is_attr), field(other.field), value(other.value),
    value_list(other.value_list), has_value_list(other.has_value_list),
    has_subquery(other.has_subquery)
{
  if (other.subquery) {
    subquery = SelectSqlNode::create_copy(other.subquery.get());
  }
  if (other.expr) {
    expr = other.expr->copy().release();
  }
}

// FilterObj 析构函数实现
FilterObj::~FilterObj()
{
  if (expr) {
    delete expr;
    expr = nullptr;
  }
}
```

在 `create_filter_unit` 函数中处理表达式：

```cpp
RC FilterStmt::create_filter_unit(Db *db, Table *default_table, 
    unordered_map<string, Table *> *tables,
    const ConditionSqlNode &condition, FilterUnit *&filter_unit)
{
  // 创建绑定上下文，用于绑定表达式
  BinderContext binder_context;
  if (default_table != nullptr) {
    binder_context.add_table(default_table);
  }
  if (tables != nullptr) {
    for (auto &pair : *tables) {
      binder_context.add_table(pair.second);
    }
  }
  ExpressionBinder expression_binder(binder_context);

  // 处理左侧表达式
  if (condition.left_expr != nullptr) {
    // 左侧是表达式（如算术表达式）
    unique_ptr<Expression> left_expr(condition.left_expr->copy().release());
    vector<unique_ptr<Expression>> bound_expressions;
    rc = expression_binder.bind_expression(left_expr, bound_expressions);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to bind left expression");
      delete filter_unit;
      return rc;
    }
    FilterObj filter_obj;
    filter_obj.init_expr(bound_expressions[0].release());
    filter_unit->set_left(filter_obj);
  } else if (condition.left_is_attr) {
    // ... 原有逻辑 ...
  }
  
  // 处理右侧表达式（类似左侧）
  // ...
}
```

#### 3.2.3 修改 select_stmt.cpp

在 `create_condition_expression` 函数中优先使用表达式字段：

```cpp
RC create_condition_expression(const ConditionSqlNode &condition, 
                              Expression *&expr, 
                              const unordered_map<string, Table *> &table_map)
{
  unique_ptr<Expression> left_expr;
  
  // 处理左侧表达式 - 优先使用表达式字段
  if (condition.left_expr != nullptr) {
    left_expr.reset(condition.left_expr->copy().release());
  } else if (condition.left_is_attr) {
    const RelAttrSqlNode &attr = condition.left_attr;
    left_expr = make_unique<UnboundFieldExpr>(attr.relation_name, attr.attribute_name);
  } else {
    left_expr = make_unique<ValueExpr>(condition.left_value);
  }
  
  // 处理右侧表达式（类似）
  unique_ptr<Expression> right_expr;
  if (condition.right_expr != nullptr) {
    right_expr.reset(condition.right_expr->copy().release());
  } else if (condition.right_is_attr) {
    // ...
  } else {
    // ...
  }
  
  expr = new ComparisonExpr(condition.comp, std::move(left_expr), std::move(right_expr));
  return RC::SUCCESS;
}
```

### 3.3 优化器层（Optimizer）

#### 3.3.1 修改 logical_plan_generator.cpp

在 `create_plan(FilterStmt*)` 函数中处理 `FilterObj` 的表达式字段：

```cpp
RC LogicalPlanGenerator::create_plan(FilterStmt *filter_stmt, 
                                    unique_ptr<LogicalOperator> &logical_operator)
{
  for (const FilterUnit *filter_unit : filter_units) {
    const FilterObj &filter_obj_left  = filter_unit->left();
    const FilterObj &filter_obj_right = filter_unit->right();

    // 处理左侧表达式（优先使用expr字段）
    unique_ptr<Expression> left;
    if (filter_obj_left.expr != nullptr) {
      // 左侧是表达式（已绑定）
      left.reset(filter_obj_left.expr->copy().release());
    } else if (filter_obj_left.has_subquery && filter_obj_left.subquery) {
      // 左侧是子查询
      // ...
    } else if (filter_obj_left.is_attr) {
      // 左侧是属性
      left = make_unique<FieldExpr>(filter_obj_left.field);
    } else {
      // 左侧是常量值
      left = make_unique<ValueExpr>(filter_obj_left.value);
    }

    // 处理右侧表达式（类似左侧）
    // ...
  }
}
```

## 4. 关键技术点

### 4.1 表达式绑定

表达式绑定是将语法解析阶段的 `UnboundFieldExpr` 转换为 `FieldExpr` 的过程：

```cpp
ExpressionBinder expression_binder(binder_context);
rc = expression_binder.bind_expression(left_expr, bound_expressions);
```

绑定过程会：
1. 遍历表达式树中的所有 `UnboundFieldExpr`
2. 查找对应的表和字段
3. 替换为 `FieldExpr`（包含实际的字段元数据）

### 4.2 表达式求值类型

算术表达式的求值类型在 `ArithmeticExpr::value_type()` 中确定：

```cpp
AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  if (left_->value_type() == AttrType::INTS && 
      right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }

  return AttrType::FLOATS;
}
```

### 4.3 内存管理

关键的内存管理点：
1. `ConditionSqlNode` 拥有 `left_expr` 和 `right_expr`，需要在析构时删除
2. `FilterObj` 拥有 `expr`，需要在析构时删除
3. 拷贝时使用 `copy()` 方法进行深拷贝

## 5. 测试用例

### 5.1 基本算术表达式

```sql
-- 测试加法
SELECT * FROM exp_table WHERE 3 + col2 < 9;

-- 预期结果：col2 < 6 的所有行
-- 4 | 1 | 1 | 1.87 | 3.39
-- 6 | 3 | 4 | 5.81 | 9.72
-- 8 | 4 | 4 | 5.28 | 1.56
```

### 5.2 复杂表达式

```sql
-- 测试乘法和加法
SELECT * FROM table1 WHERE col1 * 2 + 5 > 20;

-- 测试括号
SELECT * FROM table1 WHERE (col1 + col2) * 2 < 100;
```

### 5.3 表达式与字段比较

```sql
SELECT * FROM table1 WHERE col1 + 10 < col2;
SELECT * FROM table1 WHERE col1 * 2 = col2;
```

## 6. 已知问题和限制

### 6.1 当前状态

实现已完成：
- ✅ 语法解析支持
- ✅ 数据结构扩展
- ✅ 表达式绑定
- ✅ 逻辑计划生成

### 6.2 问题诊断

如果出现 "Type comparison failed between 0 and 0" 错误，可能的原因：
1. 表达式未正确绑定（`UnboundFieldExpr` 未转换为 `FieldExpr`）
2. 表达式的 `value_type()` 返回 `AttrType::UNDEFINED`

解决方法：
- 确保在 `filter_stmt.cpp` 中正确调用了 `expression_binder.bind_expression()`
- 检查绑定上下文是否包含所有相关的表

## 7. 扩展方向

### 7.1 短期扩展
- 支持更多运算符（模运算、位运算等）
- 支持函数调用（如 `ABS(col1)`, `UPPER(col2)`）
- 改进类型转换逻辑

### 7.2 长期扩展
- 表达式索引优化
- 常量折叠优化
- 表达式下推优化

## 8. 参考资料

- MiniOB SQL 表达式设计文档：`docs/docs/design/miniob-sql-expression.md`
- MiniOB 如何新增SQL语句：`docs/docs/design/miniob-how-to-add-new-sql.md`

## 9. 附录：完整的代码修改清单

### 9.1 修改的文件列表

1. `src/observer/sql/parser/parse_defs.h` - 添加表达式字段
2. `src/observer/sql/parser/parse_defs.cpp` - 实现拷贝和析构
3. `src/observer/sql/parser/yacc_sql.y` - 添加语法规则
4. `src/observer/sql/stmt/filter_stmt.h` - 扩展FilterObj
5. `src/observer/sql/stmt/filter_stmt.cpp` - 实现表达式绑定
6. `src/observer/sql/stmt/select_stmt.cpp` - 处理表达式条件
7. `src/observer/sql/optimizer/logical_plan_generator.cpp` - 生成逻辑计划

### 9.2 关键代码片段位置

- 语法规则：`yacc_sql.y:660` (condition规则的第一个分支)
- 表达式绑定：`filter_stmt.cpp:109-130` (左侧表达式处理)
- 逻辑计划：`logical_plan_generator.cpp:188-201` (处理expr字段)

---

**文档版本**: 1.0  
**创建日期**: 2025-10-06  
**最后更新**: 2025-10-06

