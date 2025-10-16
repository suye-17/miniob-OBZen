# JOIN字段验证问题修复方案

## 问题描述

**问题SQL：**
```sql
Select * from join_table_1 inner join join_table_2 
on join_table_1.id=join_table_2.id and join_table_2.level>36;
```

**表结构：**
- `join_table_1(id int, name char(20))`
- `join_table_2(id int, age int)`  ← 没有level字段

**期望结果：** FAILURE
**实际结果：** 返回表头 `id | name | id | age`

---

## 根本原因分析

从代码分析和日志来看，字段验证逻辑是**正确**的，错误确实被检测到：

```
field not found in table: join_table_2.level
failed to bind child expression 1 in conjunction
failed to bind fields in join condition. rc=SCHEMA_FIELD_NOT_EXIST
```

但问题在于：**即使返回了错误，测试框架仍然看到了表头输出**。

可能的原因：

### 原因1：执行了不完整的operator

如果 `logical_plan_generator` 在bind字段之前已经构建了部分逻辑计划，可能会导致部分operator被生成。

### 原因2：错误传递链断裂

错误在某个阶段被检测到但没有正确传递给客户端。

### 原因3：测试时机问题

测试框架在错误返回前就捕获了部分输出。

---

## 解决方案

### 方案1：增强错误检查（推荐）

在 `select_stmt.cpp` 的JOIN条件创建阶段就进行字段验证，而不是等到逻辑计划生成阶段。

**修改位置：** `src/observer/sql/stmt/select_stmt.cpp` 第165行

**当前代码：**
```cpp
// 创建JOIN条件表达式
Expression *join_condition = nullptr;
RC rc = create_join_conditions_expression(join_sql.conditions, join_condition, table_map);
if (rc != RC::SUCCESS) {
  LOG_WARN("failed to create join condition expression");
  delete select_stmt;
  return rc;
}
```

**修改后：**
```cpp
// 创建JOIN条件表达式
Expression *join_condition = nullptr;
RC rc = create_join_conditions_expression(join_sql.conditions, join_condition, table_map);
if (rc != RC::SUCCESS) {
  LOG_WARN("failed to create join condition expression");
  delete select_stmt;
  return rc;
}

// ✅ 新增：立即验证JOIN条件中的字段
vector<Table *> current_tables = tables;
current_tables.push_back(table);
for (const auto& pair : table_map) {
  if (std::find(current_tables.begin(), current_tables.end(), pair.second) == current_tables.end()) {
    current_tables.push_back(pair.second);
  }
}

if (join_condition != nullptr) {
  unique_ptr<Expression> condition_copy(join_condition->copy().release());
  rc = validate_expression_fields(condition_copy, current_tables);
  if (rc != RC::SUCCESS) {
    LOG_WARN("JOIN condition contains invalid field. rc=%s", strrc(rc));
    delete join_condition;
    delete select_stmt;
    return rc;
  }
}
```

### 方案2：添加专门的字段验证函数

**新增函数：** `src/observer/sql/stmt/select_stmt.cpp`

```cpp
/**
 * @brief 验证表达式中的所有字段是否存在
 * @details 递归验证表达式树中的所有UnboundFieldExpr
 */
RC validate_expression_fields(unique_ptr<Expression> &expr, const vector<Table *> &tables)
{
  if (!expr) {
    return RC::SUCCESS;
  }

  switch (expr->type()) {
    case ExprType::UNBOUND_FIELD: {
      auto unbound_field = static_cast<UnboundFieldExpr *>(expr.get());
      const char *field_name = unbound_field->field_name();
      const char *table_name = unbound_field->table_name();

      // 查找目标表
      Table *target_table = nullptr;
      if (table_name && strlen(table_name) > 0) {
        // 指定了表名
        auto it = find_if(tables.begin(), tables.end(), 
                         [table_name](Table *table) { 
                           return strcmp(table->name(), table_name) == 0; 
                         });
        target_table = (it != tables.end()) ? *it : nullptr;
      } else {
        // 没有指定表名，在所有表中查找字段
        auto it = find_if(tables.begin(), tables.end(), 
                         [field_name](Table *table) {
                           return table->table_meta().field(field_name) != nullptr;
                         });
        target_table = (it != tables.end()) ? *it : nullptr;
      }

      if (!target_table) {
        LOG_WARN("field not found: %s", field_name);
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }

      const FieldMeta *field_meta = target_table->table_meta().field(field_name);
      if (!field_meta) {
        LOG_WARN("field not found in table: %s.%s", target_table->name(), field_name);
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }

      return RC::SUCCESS;
    }

    case ExprType::COMPARISON: {
      auto comp_expr = static_cast<ComparisonExpr *>(expr.get());
      
      if (comp_expr->left()) {
        unique_ptr<Expression> left = comp_expr->left()->copy();
        RC rc = validate_expression_fields(left, tables);
        if (rc != RC::SUCCESS) return rc;
      }
      
      if (comp_expr->right()) {
        unique_ptr<Expression> right = comp_expr->right()->copy();
        RC rc = validate_expression_fields(right, tables);
        if (rc != RC::SUCCESS) return rc;
      }
      
      return RC::SUCCESS;
    }

    case ExprType::CONJUNCTION: {
      auto conjunction_expr = static_cast<ConjunctionExpr *>(expr.get());
      for (size_t i = 0; i < conjunction_expr->children().size(); i++) {
        unique_ptr<Expression> child = conjunction_expr->children()[i]->copy();
        RC rc = validate_expression_fields(child, tables);
        if (rc != RC::SUCCESS) {
          LOG_WARN("failed to validate child expression %zu in conjunction", i);
          return rc;
        }
      }
      return RC::SUCCESS;
    }

    case ExprType::ARITHMETIC: {
      auto arith_expr = static_cast<ArithmeticExpr *>(expr.get());
      
      if (arith_expr->left()) {
        unique_ptr<Expression> left = arith_expr->left()->copy();
        RC rc = validate_expression_fields(left, tables);
        if (rc != RC::SUCCESS) return rc;
      }
      
      if (arith_expr->right()) {
        unique_ptr<Expression> right = arith_expr->right()->copy();
        RC rc = validate_expression_fields(right, tables);
        if (rc != RC::SUCCESS) return rc;
      }
      
      return RC::SUCCESS;
    }

    case ExprType::FIELD:
    case ExprType::VALUE:
    case ExprType::STAR:
      // 这些表达式不需要验证
      return RC::SUCCESS;

    default:
      // 其他类型暂时跳过
      return RC::SUCCESS;
  }
}
```

---

## 修复步骤

### 步骤1：备份当前代码

```bash
cd /home/simpur/miniob-OBZen
cp src/observer/sql/stmt/select_stmt.cpp src/observer/sql/stmt/select_stmt.cpp.backup
```

### 步骤2：添加验证函数

在 `select_stmt.cpp` 文件开头添加 `validate_expression_fields` 函数（在 `create_join_conditions_expression` 函数之后）。

### 步骤3：在JOIN处理中调用验证

在 `SelectStmt::create` 函数的JOIN表处理部分，创建JOIN条件后立即调用验证函数。

### 步骤4：编译测试

```bash
cd /home/simpur/miniob-OBZen
./build.sh
```

### 步骤5：运行测试

```bash
# 使用我们创建的测试文件
./build_debug/bin/obclient < test_join_field_error.sql
```

---

## 预期效果

修复后，执行以下SQL：

```sql
Select * from join_table_1 inner join join_table_2 
on join_table_1.id=join_table_2.id and join_table_2.level>36;
```

应该返回：
```
FAILURE
```

而不是：
```
id | name | id | age
```

---

## 测试验证

创建测试脚本验证修复：

```sql
-- test_fix_verification.sql

CREATE TABLE join_table_1(id int, name char(20));
CREATE TABLE join_table_2(id int, age int);

INSERT INTO join_table_1 VALUES (1, 'test');
INSERT INTO join_table_2 VALUES (1, 25);

-- 应该返回 FAILURE
Select * from join_table_1 inner join join_table_2 
on join_table_1.id=join_table_2.id and join_table_2.level>36;

-- 应该返回数据
Select * from join_table_1 inner join join_table_2 
on join_table_1.id=join_table_2.id and join_table_2.age>20;

DROP TABLE join_table_1;
DROP TABLE join_table_2;
```

---

##备选方案

如果方案1修改较大，可以采用更简单的方案：

### 简化方案：在语义层直接检查

在 `create_condition_expression` 函数中，创建UnboundFieldExpr时就检查字段是否存在：

```cpp
RC create_condition_expression(const ConditionSqlNode &condition, Expression *&expr, 
                              const unordered_map<string, Table *> &table_map)
{
  unique_ptr<Expression> left_expr;
  
  if (condition.left_expression != nullptr) {
    left_expr.reset(condition.left_expression->copy().release());
  } else if (condition.left_is_attr) {
    const RelAttrSqlNode &attr = condition.left_attr;
    
    // ✅ 新增：检查字段是否存在
    if (attr.relation_name.length() > 0) {
      auto it = table_map.find(attr.relation_name);
      if (it == table_map.end()) {
        LOG_WARN("table not found: %s", attr.relation_name.c_str());
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      Table *table = it->second;
      const FieldMeta *field_meta = table->table_meta().field(attr.attribute_name.c_str());
      if (!field_meta) {
        LOG_WARN("field not found: %s.%s", attr.relation_name.c_str(), attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    }
    
    left_expr = make_unique<UnboundFieldExpr>(attr.relation_name, attr.attribute_name);
  } else {
    left_expr = make_unique<ValueExpr>(condition.left_value);
  }
  
  // 同样处理右侧...
}
```

---

**修复优先级：** 🔴 高
**修复难度：** ⭐⭐⭐ (中等)
**影响范围：** JOIN查询的错误处理
**向后兼容：** ✅ 完全兼容

---

**文档创建时间：** 2025年10月15日
**问题状态：** 待修复

