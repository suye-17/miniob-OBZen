# Expression基类详解 - 表达式系统的核心

## 1. 文件位置和作用
- **文件路径：** `src/observer/sql/expr/expression.h`
- **作用：** 定义了所有表达式的基类和具体实现
- **重要性：** 这是整个表达式系统的核心，所有表达式都继承自这个基类

## 2. 头文件包含分析
```cpp
#include "common/lang/string.h"      // 字符串相关
#include "common/lang/memory.h"      // 智能指针相关  
#include "common/lang/unordered_set.h"  // 无序集合
#include "common/value.h"            // Value类定义
#include "storage/field/field.h"     // 字段定义
#include "sql/expr/aggregator.h"     // 聚合器
#include "storage/common/chunk.h"    // 数据块（向量化执行）
```

**为什么需要这些头文件？**
- 表达式需要操作字符串（表达式名字）
- 需要智能指针管理内存
- 需要Value类存储计算结果
- 需要Field类表示数据库字段
- 需要Chunk支持向量化计算

## 3. ExprType枚举详解

```cpp
enum class ExprType
{
  NONE,                 // 空类型
  STAR,                 // 星号，表示所有字段 (SELECT *)
  UNBOUND_FIELD,        // 未绑定的字段，需要在resolver阶段解析为FieldExpr
  UNBOUND_AGGREGATION,  // 未绑定的聚合函数，需要在resolver阶段解析为AggregateExpr

  FIELD,        // 字段。在实际执行时，根据行数据内容提取对应字段的值
  VALUE,        // 常量值
  CAST,         // 需要做类型转换的表达式
  COMPARISON,   // 需要做比较的表达式
  CONJUNCTION,  // 多个表达式使用同一种关系(AND或OR)来联结
  ARITHMETIC,   // 算术运算
  AGGREGATION,  // 聚合运算
};
```

### 3.1 为什么要用enum class？
```cpp
// 老式枚举的问题
enum Color { RED, GREEN, BLUE };
enum Status { RED, ACTIVE };  // 错误！RED重复定义

// 现代强类型枚举解决了这个问题
enum class ExprType { FIELD, VALUE };
enum class NodeType { FIELD, TABLE };  // OK！不会冲突

// 使用时必须加前缀
ExprType type = ExprType::FIELD;  // 必须写ExprType::
```

### 3.2 UNBOUND vs BOUND 的概念
```cpp
// SQL解析阶段：字段名还是字符串，不知道具体是哪个表的哪个字段
"SELECT name, age FROM students WHERE age > 18"
//      ^^^^  ^^^                   ^^^
//   这些都是UNBOUND_FIELD

// 绑定阶段：确定了具体的表和字段
FieldExpr(students_table, name_field)
FieldExpr(students_table, age_field)
//   现在变成了BOUND的FieldExpr
```

## 4. Expression基类详解

### 4.1 类的基本结构
```cpp
class Expression
{
public:
  Expression() = default;           // 默认构造函数
  virtual ~Expression() = default;  // 虚析构函数（重要！）

  // 纯虚函数 - 子类必须实现
  virtual unique_ptr<Expression> copy() const = 0;
  virtual RC get_value(const Tuple &tuple, Value &value) const = 0;
  virtual ExprType type() const = 0;
  virtual AttrType value_type() const = 0;

  // 虚函数 - 子类可以选择重写
  virtual bool equal(const Expression &other) const { return false; }
  virtual RC try_get_value(Value &value) const { return RC::UNIMPLEMENTED; }
  virtual RC get_column(Chunk &chunk, Column &column) { return RC::UNIMPLEMENTED; }
  virtual int value_length() const { return -1; }
  virtual RC eval(Chunk &chunk, vector<uint8_t> &select) { return RC::UNIMPLEMENTED; }

  // 普通成员函数
  virtual const char *name() const { return name_.c_str(); }
  virtual void set_name(string name) { name_ = name; }
  virtual int pos() const { return pos_; }
  virtual void set_pos(int pos) { pos_ = pos; }

protected:
  int pos_ = -1;      // 在chunk中的位置

private:
  string name_;       // 表达式的名字
};
```

### 4.2 为什么需要虚析构函数？
```cpp
// 没有虚析构函数的问题
class Base {
public:
    ~Base() { cout << "Base destructor" << endl; }  // 不是virtual
};

class Derived : public Base {
    int* data;
public:
    Derived() { data = new int[100]; }
    ~Derived() { 
        delete[] data;  // 释放内存
        cout << "Derived destructor" << endl; 
    }
};

// 问题代码
Base* ptr = new Derived();  // 多态
delete ptr;  // 只调用Base的析构函数！Derived的析构函数不会被调用！
            // 内存泄漏！data没有被释放！

// 正确做法：虚析构函数
class Base {
public:
    virtual ~Base() { cout << "Base destructor" << endl; }  // virtual！
};
// 现在delete ptr会正确调用Derived的析构函数
```

### 4.3 核心虚函数详解

#### get_value() - 最重要的方法
```cpp
virtual RC get_value(const Tuple &tuple, Value &value) const = 0;
```

**参数解释：**
- `const Tuple &tuple` - 输入的数据行（只读引用，不复制，效率高）
- `Value &value` - 输出参数，存储计算结果（引用传递，直接修改）
- `const` - 这个方法不会修改Expression对象本身
- 返回`RC` - 操作结果码（成功/失败/错误类型）

**实际例子：**
```cpp
// 假设有一行数据：{"张三", 20, 85.5}
RowTuple tuple;
tuple.add_cell(TupleCell("张三"));
tuple.add_cell(TupleCell(20));
tuple.add_cell(TupleCell(85.5f));

// 不同表达式的get_value行为：

// 1. 常量表达式
ValueExpr const_expr(42);
Value result1;
const_expr.get_value(tuple, result1);  // result1 = 42（忽略tuple）

// 2. 字段表达式
FieldExpr field_expr(table, age_field);  // 假设age是第2个字段
Value result2;
field_expr.get_value(tuple, result2);   // result2 = 20（从tuple取第2个值）

// 3. 算术表达式：age + 10
auto age_expr = make_unique<FieldExpr>(table, age_field);
auto const_expr = make_unique<ValueExpr>(10);
ArithmeticExpr add_expr(ADD, move(age_expr), move(const_expr));
Value result3;
add_expr.get_value(tuple, result3);     // result3 = 30（20 + 10）
```

#### copy() - 深拷贝表达式
```cpp
virtual unique_ptr<Expression> copy() const = 0;
```

**为什么需要copy？**
- 优化器需要复制表达式树
- 不同的执行计划可能需要同一个表达式的多份拷贝
- 必须是深拷贝，不能共享内部状态

```cpp
// 例子：复制一个算术表达式
ArithmeticExpr original(ADD, 
    make_unique<FieldExpr>(table, field1),
    make_unique<ValueExpr>(10)
);

auto copied = original.copy();  // 深拷贝，包括子表达式
// original和copied完全独立，修改一个不影响另一个
```

#### type() - 表达式类型识别
```cpp
virtual ExprType type() const = 0;
```

**用途：**
- 类型判断和转换
- 优化器根据类型做不同优化
- 调试和错误处理

```cpp
// 使用例子
unique_ptr<Expression> expr = parse_expression("age + 10");

if (expr->type() == ExprType::ARITHMETIC) {
    // 安全转换为ArithmeticExpr
    auto* arith_expr = static_cast<ArithmeticExpr*>(expr.get());
    if (arith_expr->arithmetic_type() == ArithmeticExpr::Type::ADD) {
        cout << "这是加法表达式" << endl;
    }
}
```

### 4.4 可选虚函数详解

#### try_get_value() - 编译时求值
```cpp
virtual RC try_get_value(Value &value) const { return RC::UNIMPLEMENTED; }
```

**目的：** 在没有实际数据的情况下尝试计算表达式值

```cpp
// 能在编译时计算的表达式
ValueExpr const_expr(42);
Value result;
RC rc = const_expr.try_get_value(result);  // 成功！result = 42

ArithmeticExpr add_expr(ADD, 
    make_unique<ValueExpr>(1), 
    make_unique<ValueExpr>(2)
);
RC rc2 = add_expr.try_get_value(result);   // 成功！result = 3

// 不能在编译时计算的表达式
FieldExpr field_expr(table, field);
RC rc3 = field_expr.try_get_value(result); // 失败！需要实际数据
```

#### get_column() - 向量化执行
```cpp
virtual RC get_column(Chunk &chunk, Column &column) { return RC::UNIMPLEMENTED; }
```

**向量化 vs 逐行处理：**
```cpp
// 传统逐行处理（慢）
for (int i = 0; i < 1000; i++) {
    Value result;
    expr->get_value(tuples[i], result);  // 1000次函数调用
    results[i] = result;
}

// 向量化处理（快）
Chunk chunk;  // 包含1000行数据
Column result_column;
expr->get_column(chunk, result_column);  // 1次函数调用处理1000行！
```

## 5. 成员变量详解

### 5.1 pos_ - 位置优化
```cpp
protected:
  int pos_ = -1;  // 表达式在下层算子返回的chunk中的位置
```

**作用：** 避免重复计算

```cpp
// 场景：SELECT age, age+1, age*2 FROM students
// 如果没有位置优化，age会被计算3次

// 有了位置优化：
// 1. 下层算子计算出age，放在chunk的第0列，设置pos_=0
// 2. age+1表达式直接从chunk第0列取age值，不需要重新计算
// 3. age*2表达式也直接从chunk第0列取age值
```

### 5.2 name_ - 表达式名字
```cpp
private:
  string name_;  // 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
```

**用途：**
- 查询结果的列名显示
- 错误信息提示
- 调试信息

```cpp
// SQL: SELECT age+1 AS new_age FROM students
// name_ = "new_age"

// SQL: SELECT age+1 FROM students  
// name_ = "age+1"  (从原始SQL提取)
```

## 6. 设计亮点总结

1. **多态设计：** 统一接口，不同实现
2. **内存安全：** 智能指针自动管理内存
3. **性能优化：** 支持向量化执行和位置缓存
4. **类型安全：** 强类型枚举避免错误
5. **扩展性好：** 容易添加新的表达式类型

下一步我们来看具体的表达式实现！
