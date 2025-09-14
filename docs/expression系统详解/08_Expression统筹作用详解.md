# Expression的统筹作用详解

## 1. Expression是什么？ - 从生活到数据库

### 1.1 生活中的表达式概念
```cpp
// 想象你在超市购物：
苹果价格 = 5元/斤          // 这是一个常量表达式
我买的苹果重量 = 3斤        // 这是一个变量表达式  
总价 = 苹果价格 * 重量      // 这是一个算术表达式
总价 > 10元               // 这是一个比较表达式

// 这些都是"表达式" - 能够计算出具体值的东西
```

### 1.2 数据库中的表达式无处不在
```sql
-- 一个简单的SQL查询包含多种表达式：
SELECT 
    name,                    -- 字段表达式：从数据行中获取name字段
    age + 1 AS next_age,     -- 算术表达式：年龄加1
    salary * 1.1 AS new_salary,  -- 算术表达式：工资乘以1.1
    'Active' AS status       -- 常量表达式：固定字符串
FROM employees 
WHERE age > 25               -- 比较表达式：年龄大于25
  AND department = 'IT'      -- 比较表达式：部门等于IT
  AND salary > 5000;         -- 比较表达式：工资大于5000
```

**核心理解：** Expression是数据库中"任何能计算出值的东西"的抽象！

## 2. Expression的统筹架构

### 2.1 统一接口设计
```cpp
// Expression就像一个"万能计算器"接口
class Expression {
public:
    // 核心方法：给我一行数据，我计算出一个结果
    virtual RC get_value(const Tuple& tuple, Value& result) const = 0;
    
    // 告诉你我能算出什么类型的结果
    virtual AttrType value_type() const = 0;
    
    // 告诉你我是什么类型的表达式
    virtual ExprType type() const = 0;
    
    // 其他辅助方法...
};
```

### 2.2 表达式家族体系
```cpp
// Expression家族就像一个公司的组织架构：

Expression (CEO - 制定统一标准)
│
├── ValueExpr (会计 - 管理常量值)
│   └── 职责：存储和返回固定值 (42, "hello", 3.14)
│
├── FieldExpr (信息员 - 从数据中提取信息)  
│   └── 职责：从数据行中获取指定字段的值
│
├── ArithmeticExpr (数学家 - 执行算术运算)
│   └── 职责：执行 +、-、*、/ 等运算
│
├── ComparisonExpr (判官 - 执行比较判断)
│   └── 职责：执行 >、<、=、!= 等比较
│
├── ConjunctionExpr (协调员 - 逻辑运算)
│   └── 职责：执行 AND、OR 逻辑运算
│
└── AggregateExpr (统计员 - 聚合计算)
    └── 职责：执行 COUNT、SUM、AVG 等聚合
```

## 3. Expression的核心价值

### 3.1 解决的核心问题

**问题1：代码重复**
```cpp
// 没有统一抽象的情况：
void process_select_list() {
    for (auto& item : select_items) {
        if (item.type == "field") {
            // 处理字段的代码
            Value result = get_field_value(item.field_name);
        } else if (item.type == "constant") {
            // 处理常量的代码  
            Value result = item.constant_value;
        } else if (item.type == "arithmetic") {
            // 处理算术的代码
            Value left = process_left_operand();
            Value right = process_right_operand();
            Value result = calculate(left, right, item.operator);
        }
        // ... 更多类型，代码越来越复杂
    }
}
```

**解决方案：统一接口**
```cpp
// 有了Expression抽象：
void process_select_list() {
    for (auto& expr : select_expressions) {
        Value result;
        RC rc = expr->get_value(tuple, result);  // 统一调用！
        if (rc == RC::SUCCESS) {
            output_results.push_back(result);
        }
    }
}
```

**问题2：难以扩展**
```cpp
// 添加新类型需要修改所有相关代码
// 有了Expression系统，只需要：
class NewExpressionType : public Expression {
    RC get_value(const Tuple& tuple, Value& result) const override {
        // 实现新的计算逻辑
        return RC::SUCCESS;
    }
    // 实现其他虚函数...
};
// 现有代码无需修改！
```

### 3.2 统筹管理的威力

**场景：复杂查询处理**
```sql
SELECT 
    name,
    age + 1,
    salary * 1.1,
    CASE WHEN age > 30 THEN 'Senior' ELSE 'Junior' END
FROM employees 
WHERE (age > 25 AND salary > 5000) OR department = 'IT'
ORDER BY salary * 1.1 DESC;
```

```cpp
// Expression系统统筹处理：
class QueryProcessor {
    vector<unique_ptr<Expression>> select_exprs_;     // SELECT列表
    unique_ptr<Expression> where_expr_;               // WHERE条件
    vector<unique_ptr<Expression>> order_exprs_;      // ORDER BY列表
    
public:
    void execute() {
        for (auto& tuple : all_tuples) {
            // 1. 检查WHERE条件
            Value where_result;
            where_expr_->get_value(tuple, where_result);
            if (!where_result.get_boolean()) continue;
            
            // 2. 计算SELECT列表
            RowTuple result_tuple;
            for (auto& expr : select_exprs_) {
                Value column_result;
                expr->get_value(tuple, column_result);
                result_tuple.add_cell(column_result);
            }
            
            // 3. 计算ORDER BY值（用于排序）
            for (auto& expr : order_exprs_) {
                Value order_value;
                expr->get_value(tuple, order_value);
                // 存储排序键...
            }
            
            results.push_back(result_tuple);
        }
        
        // 4. 排序结果
        sort_results();
    }
};
```

## 4. 实际工作流程演示

### 4.1 完整的SQL处理流程
```cpp
// SQL: SELECT name, age + 1, salary > 5000 FROM employees WHERE age > 25

// 步骤1：解析阶段 - 创建表达式树
vector<unique_ptr<Expression>> select_exprs;

// name
select_exprs.push_back(make_unique<FieldExpr>("name"));

// age + 1  
select_exprs.push_back(make_unique<ArithmeticExpr>(ADD,
    make_unique<FieldExpr>("age"),
    make_unique<ValueExpr>(1)
));

// salary > 5000
select_exprs.push_back(make_unique<ComparisonExpr>(GREAT_THAN,
    make_unique<FieldExpr>("salary"), 
    make_unique<ValueExpr>(5000)
));

// WHERE age > 25
auto where_expr = make_unique<ComparisonExpr>(GREAT_THAN,
    make_unique<FieldExpr>("age"),
    make_unique<ValueExpr>(25)
);

// 步骤2：执行阶段 - 统一处理
for (auto& employee_tuple : employee_data) {
    // 检查WHERE条件
    Value where_result;
    where_expr->get_value(employee_tuple, where_result);
    
    if (where_result.get_boolean()) {  // 满足条件
        cout << "员工数据: ";
        
        // 计算所有SELECT表达式
        for (size_t i = 0; i < select_exprs.size(); i++) {
            Value result;
            select_exprs[i]->get_value(employee_tuple, result);
            cout << result.to_string();
            if (i < select_exprs.size() - 1) cout << ", ";
        }
        cout << endl;
    }
}
```

### 4.2 表达式树的递归计算
```cpp
// 复杂表达式：(age + bonus) * tax_rate - base_deduction
// 表达式树结构：
//       -
//      / \
//     *   base_deduction(1000)
//    / \
//   +   tax_rate(0.3)
//  / \
// age bonus

// 执行过程（递归）：
// 1. 根节点(-)调用 get_value()
// 2. 计算左子树(*):
//    2.1 计算左子树(+):
//        2.1.1 计算age字段 -> 25
//        2.1.2 计算bonus字段 -> 2000  
//        2.1.3 执行 25 + 2000 = 2025
//    2.2 计算右子树(tax_rate) -> 0.3
//    2.3 执行 2025 * 0.3 = 607.5
// 3. 计算右子树(base_deduction) -> 1000
// 4. 执行 607.5 - 1000 = -392.5
```

## 5. Expression系统的优势

### 5.1 可组合性
```cpp
// 简单表达式可以组合成复杂表达式
auto age = make_unique<FieldExpr>("age");
auto one = make_unique<ValueExpr>(1);
auto salary = make_unique<FieldExpr>("salary");
auto rate = make_unique<ValueExpr>(1.1f);

// 组合1：age + 1
auto next_age = make_unique<ArithmeticExpr>(ADD, move(age), move(one));

// 组合2：salary * 1.1  
auto new_salary = make_unique<ArithmeticExpr>(MUL, move(salary), move(rate));

// 组合3：(age + 1) > (salary * 1.1)
auto complex_condition = make_unique<ComparisonExpr>(GREAT_THAN,
    move(next_age), move(new_salary));
```

### 5.2 类型安全
```cpp
// 编译时类型检查
ArithmeticExpr expr(ADD,
    make_unique<FieldExpr>("name"),    // 字符串字段
    make_unique<ValueExpr>(10)         // 整数常量
);

// 运行时类型检查
Value result;
RC rc = expr.get_value(tuple, result);
if (rc != RC::SUCCESS) {
    cout << "类型不兼容：字符串不能与整数相加" << endl;
}
```

### 5.3 性能优化
```cpp
// 常量折叠优化
auto const_expr = make_unique<ArithmeticExpr>(ADD,
    make_unique<ValueExpr>(1),
    make_unique<ValueExpr>(2)
);

Value result;
// 可以在编译时计算：1 + 2 = 3
const_expr->try_get_value(result);  // 不需要运行时计算

// 向量化执行
Chunk chunk;  // 包含1000行数据
Column result_column;
expression->get_column(chunk, result_column);  // 批量计算1000个结果
```

## 6. Expression在整个系统中的地位

### 6.1 SQL执行管道中的位置
```
SQL文本 -> 词法分析 -> 语法分析 -> 表达式构建 -> 表达式绑定 -> 查询优化 -> 执行
                                    ↑              ↑           ↑        ↑
                                Expression    Expression   Expression Expression
                                 创建          解析         优化       执行
```

### 6.2 与其他组件的协作
```cpp
// 1. 与Parser协作：解析SQL生成表达式
class SQLParser {
    unique_ptr<Expression> parse_expression(const string& sql);
};

// 2. 与Optimizer协作：优化表达式
class ExpressionOptimizer {
    unique_ptr<Expression> optimize(unique_ptr<Expression> expr);
};

// 3. 与Executor协作：执行表达式
class QueryExecutor {
    RC execute_expression(const Expression& expr, const Tuple& tuple, Value& result);
};

// 4. 与Storage协作：访问数据
class FieldExpr : public Expression {
    RC get_value(const Tuple& tuple, Value& result) const override {
        return tuple.find_cell(field_spec_, result);  // 从存储中获取数据
    }
};
```

## 7. 总结：Expression的统筹价值

Expression系统是MiniOB的"计算大脑"，它的统筹作用体现在：

### 7.1 统一抽象
- **一个接口处理所有计算：** 不管多复杂的计算，都用 `get_value()` 
- **类型安全：** 通过 `value_type()` 确保类型正确
- **递归组合：** 复杂表达式由简单表达式组合而成

### 7.2 系统协调  
- **解耦各模块：** Parser、Optimizer、Executor都通过Expression接口协作
- **便于扩展：** 新增表达式类型不影响现有代码
- **性能优化：** 支持常量折叠、向量化等优化技术

### 7.3 实际价值
- **代码复用：** 避免重复编写计算逻辑
- **维护性好：** 统一的接口易于理解和维护  
- **功能强大：** 支持任意复杂的计算表达式

**形象比喻：** Expression就像城市的"交通系统" - 统一的规则（红绿灯、道路标准），不同的交通工具（汽车、自行车、公交），但都能在同一套系统中协调运行，实现复杂的交通需求。

这就是Expression系统的统筹价值：用统一的抽象管理复杂的计算需求！
