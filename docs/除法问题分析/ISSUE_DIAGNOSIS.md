# WHERE条件中除法表达式问题诊断

## 问题总结
`select * from test_expr where 2 = 2/3;` 返回了所有行，应该返回空结果。

## 测试结果对比

### ✅ 正常工作的情况
```sql
select 2/3;  -- 输出: DEBUG: Creating DIV expression, 结果: 0.67
```

### ❌ 异常的情况
```sql
select * from test_expr where 2 = 2/3;  -- 没有 DEBUG 输出，右边值为 2
```

### ❌ 更严重的问题
```sql
select * from test_expr where 2 = (2/3);  -- 语法解析失败
```

## 关键发现

1. **单独的除法表达式工作正常**：`select 2/3;` 能正确输出 0.67
2. **WHERE条件中的除法表达式被错误处理**：没有调用 `create_arithmetic_expression`
3. **带括号的表达式完全无法解析**：语法解析器直接失败

## 调试日志分析

```
COMPARISON try_get_value: getting right value, right_expr_type=?
COMPARISON try_get_value: right=2  // 应该是 0.6667
```

- 右边表达式 `2/3` 被解析为常量 `2`
- 没有出现 `ARITHMETIC try_get_value` 调试信息
- 没有出现 `DEBUG: Creating DIV expression` 信息

## 根本原因

**WHERE条件中的表达式解析与SELECT中的表达式解析使用了不同的逻辑，导致除法运算被错误处理。**

在WHERE条件解析时，可能存在：
1. 不同的表达式解析路径
2. 错误的常量折叠优化
3. 运算符优先级处理错误

## 下一步

需要查找WHERE条件表达式解析的具体实现，找到为什么 `2/3` 没有被解析为 `ArithmeticExpr`。
