# MiniOB 子查询功能 - 待办事项

## 🎯 已完成的功能 ✅

1. **Session上下文传递** ✅
   - 解决了子查询执行时的Session上下文问题
   - 确保物理操作符正确设置Session上下文

2. **EXISTS/NOT EXISTS语句** ✅
   - 完整实现EXISTS和NOT EXISTS语法支持
   - 正确处理子查询结果集为空的情况
   - 测试验证通过

3. **IN/NOT IN操作** ✅
   - 值列表形式：`WHERE field IN (1, 2, 3)`
   - 子查询形式：`WHERE field IN (SELECT ...)`
   - 正确的内存管理和深拷贝

4. **标量子查询基础架构** ✅
   - 添加了SubqueryExpr类
   - 语法解析支持：`(SELECT ...)`
   - 表达式系统集成

5. **综合测试** ✅
   - 验证了所有已实现功能的正确性
   - 确认内存管理和性能表现

## 🔄 需要完善的功能

### 1. 聚合函数子查询 🔄
**优先级**: 高
**描述**: 确保子查询中的聚合函数正常工作
**示例**: 
```sql
SELECT * FROM subq_main WHERE score > (SELECT AVG(value) FROM subq_ref);
SELECT * FROM subq_main WHERE id = (SELECT MAX(ref_id) FROM subq_ref);
```
**技术要点**:
- 验证聚合函数在子查询中的解析
- 确保聚合结果正确传递
- 处理聚合函数的类型推导

### 2. NOT IN的NULL值特殊处理 🔄
**优先级**: 中
**描述**: 正确处理NOT IN操作中包含NULL值的情况
**SQL标准**: 如果子查询结果包含NULL，NOT IN应该返回NULL（在布尔上下文中为false）
**示例**:
```sql
-- 如果subq_ref中有NULL值，这个查询应该返回空结果
SELECT * FROM subq_main WHERE id NOT IN (SELECT ref_id FROM subq_ref);
```
**技术要点**:
- 检测子查询结果中的NULL值
- 实现三值逻辑（TRUE/FALSE/NULL）
- 确保NULL值的正确传播

### 3. 标量子查询比较运算完善 🔄
**优先级**: 中
**描述**: 完善标量子查询的比较运算支持
**当前状态**: 语法支持已添加，但执行层面需要完善
**示例**:
```sql
SELECT * FROM subq_main WHERE id = (SELECT ref_id FROM subq_ref WHERE value = 100);
SELECT * FROM subq_main WHERE score > (SELECT value FROM subq_ref WHERE ref_id = 1);
```
**技术要点**:
- 修复语法解析问题（条件vs表达式）
- 处理多行结果的错误情况
- 支持所有比较操作符（=, >, <, >=, <=, !=）

### 4. 类型转换系统 🔄
**优先级**: 低
**描述**: 处理表达式中不同数据类型之间的比较和转换
**示例**:
```sql
-- 整数与浮点数比较
SELECT * FROM subq_main WHERE id > (SELECT AVG(value) FROM subq_ref);
-- 字符串与数字比较（如果支持）
SELECT * FROM subq_main WHERE name = (SELECT CAST(ref_id AS CHAR) FROM subq_ref WHERE ref_id = 1);
```
**技术要点**:
- 实现隐式类型转换规则
- 处理类型不兼容的错误情况
- 确保转换的正确性和性能

## 🚀 建议的实现顺序

1. **聚合函数子查询** - 基础功能，使用频率高
2. **标量子查询比较运算完善** - 完善现有功能
3. **NOT IN的NULL值处理** - SQL标准合规性
4. **类型转换系统** - 增强功能，提升用户体验

## 🛠️ 技术债务

1. **内存泄漏**: 子查询拷贝时存在少量内存泄漏，需要优化SelectSqlNode的拷贝机制
2. **性能优化**: 子查询缓存机制可以进一步优化
3. **错误处理**: 需要更详细的错误信息和更好的错误恢复机制

## 📝 配置和部署注意事项

1. **编译要求**: 确保使用支持C++17的编译器
2. **内存配置**: 子查询功能可能增加内存使用，建议适当调整buffer pool大小
3. **测试覆盖**: 建议在生产环境部署前进行全面的子查询功能测试

## 🔍 已知限制

1. **相关子查询**: 当前只支持非相关子查询，不支持相关子查询
2. **嵌套深度**: 理论上支持任意嵌套深度，但实际受栈空间限制
3. **性能**: 复杂子查询可能影响查询性能，建议合理使用索引

## 📞 联系和支持

如需技术支持或有问题反馈，请通过以下方式联系：
- 项目文档: `/docs/子查询功能最终报告.md`
- 测试用例: `/test_*.sql` 文件
- 核心代码: `/src/observer/sql/expr/expression.cpp`
