# MiniOB INNER JOIN 实现文档

## 概述

这个文档记录了在MiniOB中实现INNER JOIN功能的过程。INNER JOIN是SQL中最常用的多表查询功能，只返回满足连接条件的记录。

## 实现功能

- 支持标准SQL语法：`SELECT * FROM table1 INNER JOIN table2 ON condition`
- 支持多种连接条件：等值连接、复合条件连接
- 支持WHERE子句与JOIN条件组合
- 实现嵌套循环连接算法
- 完整的优化器集成和执行计划生成

## 架构设计

INNER JOIN的实现涉及SQL执行的各个层次：

```
用户输入SQL
    ↓
词法/语法分析 (yacc_sql.y)
    ↓
语义分析 (select_stmt.cpp)
    ↓
查询优化 (logical_plan_generator.cpp)
    ↓
物理执行 (nested_loop_join_physical_operator.cpp)
    ↓
返回结果
```

## 核心实现

### 1. SQL解析器扩展

在 `yacc_sql.y` 中添加JOIN语法规则：

```yacc
join_clause:
    /* empty */ { $$ = nullptr; }
    | join_clause INNER JOIN relation ON condition
    {
        if ($1 == nullptr) {
            $$ = new vector<JoinSqlNode>;
        } else {
            $$ = $1;
        }
        JoinSqlNode join_node;
        join_node.type = JoinType::INNER_JOIN;
        join_node.relation = $4;
        join_node.condition = *$6;
        $$->push_back(join_node);
        delete $6;
    }
```

在 `parse_defs.h` 中定义数据结构：

```cpp
enum class JoinType {
    INNER_JOIN,
    LEFT_JOIN,
    RIGHT_JOIN
};

struct JoinSqlNode {
    JoinType           type;
    string             relation;
    ConditionSqlNode   condition;
};
```

### 2. 语义分析

扩展 `SelectStmt` 类支持多表：

```cpp
struct JoinTable {
    Table        *table;
    std::string   alias;
    JoinType      join_type;
    Expression   *condition;
};

class SelectStmt {
private:
    std::vector<Table *>       tables_;
    std::vector<JoinTable>     join_tables_;
    std::vector<Expression *>  query_expressions_;
    FilterStmt                *filter_stmt_;
};
```

关键是处理多表的表达式绑定，需要将所有参与JOIN的表都加入到绑定上下文中。

### 3. 逻辑算子

实现 `JoinLogicalOperator` 类：

```cpp
class JoinLogicalOperator : public LogicalOperator {
private:
    JoinType     join_type_;
    Expression  *condition_;
};
```

在逻辑计划生成时，构建算子链：`TableGet(left) + TableGet(right) + JoinOperator`

### 4. 物理执行

核心是 `NestedLoopJoinPhysicalOperator` 类，实现嵌套循环JOIN算法：

```cpp
RC NestedLoopJoinPhysicalOperator::next() {
    while (true) {
        if (!outer_tuple_fetched_) {
            // 获取外表下一条记录
            rc = fetch_next_outer_tuple();
            if (rc == RC::RECORD_EOF) return RC::RECORD_EOF;
            
            // 重置内表扫描
            rc = reset_inner_operator();
            outer_tuple_fetched_ = true;
        }

        // 获取内表下一条记录
        rc = right_child_->next();
        if (rc == RC::RECORD_EOF) {
            outer_tuple_fetched_ = false;
            continue;
        }

        // 检查JOIN条件
        bool join_condition_satisfied = false;
        rc = evaluate_join_condition(join_condition_satisfied);
        
        if (join_condition_satisfied) {
            joined_tuple_.set_left(left_tuple_);
            joined_tuple_.set_right(right_tuple_);
            return RC::SUCCESS;
        }
    }
}
```

### 5. 联合元组

实现 `JoinedTuple` 类来处理多表字段访问：

```cpp
class JoinedTuple : public Tuple {
public:
    void set_left(Tuple *left) { left_ = left; }
    void set_right(Tuple *right) { right_ = right; }
    
    RC find_cell(const TupleCellSpec &spec, Value &value) const override {
        // 先在左表找，再在右表找
        if (left_ && left_->find_cell(spec, value) == RC::SUCCESS) {
            return RC::SUCCESS;
        }
        if (right_) {
            return right_->find_cell(spec, value);
        }
        return RC::NOTFOUND;
    }
};
```

## 类型兼容性问题修复

在测试过程中发现了一个严重bug：当JOIN条件中比较不同类型字段时（如字符串和整数），程序会崩溃。

### 问题原因

原代码在类型比较时使用了ASSERT：

```cpp
// char_type.cpp
int CharType::compare(const Value &left, const Value &right) const {
    ASSERT(left.attr_type() == AttrType::CHARS && right.attr_type() == AttrType::CHARS, "invalid type");
    // ...
}
```

当执行 `join_table_1.name < join_table_2.age` 时，由于name是字符串，age是整数，触发断言失败，程序直接退出。

### 解决方案

移除ASSERT，实现跨类型比较：

```cpp
int CharType::compare(const Value &left, const Value &right) const {
    if (left.attr_type() != AttrType::CHARS) {
        LOG_WARN("Left operand is not a string type");
        return INT32_MAX;
    }

    switch (right.attr_type()) {
        case AttrType::CHARS:
            return common::compare_string(/*...*/);
        case AttrType::INTS:
            // 字符串转整数比较
            int left_as_int = left.get_int();
            int right_int = right.get_int();
            return (left_as_int < right_int) ? -1 : 
                   (left_as_int > right_int) ? 1 : 0;
        // ...
    }
}
```

同样修改了 `IntegerType::compare` 和 `FloatType::compare`，支持与字符串的比较。

完善了 `CharType::cast_to` 方法，支持字符串到数字的转换：

```cpp
RC CharType::cast_to(const Value &val, AttrType type, Value &result) const {
    switch (type) {
        case AttrType::INTS:
            try {
                int int_value = std::stoi(val.value_.pointer_value_);
                result.set_int(int_value);
                return RC::SUCCESS;
            } catch (const std::exception& e) {
                result.set_int(0);  // 转换失败返回0，符合MySQL行为
                return RC::SUCCESS;
            }
        // ...
    }
}
```

### 效果

修复后，这样的查询可以正常执行：

```sql
INSERT INTO join_table_1 VALUES (4, '16a');
SELECT * FROM join_table_1 INNER JOIN join_table_2 
ON join_table_1.name < join_table_2.age AND join_table_1.id = join_table_2.id;
```

字符串 '16a' 会被转换为整数 16 进行比较，符合MySQL的行为。

## 测试

基本功能测试：

```sql
CREATE TABLE users(id int, name char(20));
CREATE TABLE orders(id int, user_id int, amount float);

INSERT INTO users VALUES (1, 'Alice');
INSERT INTO users VALUES (2, 'Bob');
INSERT INTO orders VALUES (101, 1, 100.5);
INSERT INTO orders VALUES (102, 2, 200.0);

-- 基本JOIN
SELECT * FROM users u INNER JOIN orders o ON u.id = o.user_id;

-- 带WHERE条件
SELECT u.name, o.amount FROM users u INNER JOIN orders o 
ON u.id = o.user_id WHERE o.amount > 150;
```

## 技术难点

1. **多表表达式绑定**：需要正确处理字段引用的歧义问题
2. **JOIN条件处理**：将SQL条件转换为可执行的表达式树
3. **嵌套循环效率**：内表需要重复扫描，性能是O(m*n)
4. **元组结构设计**：JOIN后的记录包含多表字段，需要统一访问接口

## 后续优化

1. 实现索引嵌套循环JOIN和哈希JOIN算法
2. 支持更多JOIN类型（LEFT JOIN、RIGHT JOIN等）
3. JOIN顺序优化
4. 谓词下推等查询优化

## 总结

INNER JOIN的实现验证了MiniOB架构的扩展性。通过在各个层次添加多表支持，成功实现了这个重要的SQL功能。类型兼容性的修复也提高了系统的稳定性和MySQL兼容性。

---