-- 测试SELECT字段绑定修复
-- 创建测试表
create table t_test(id int, name char(10), age int);

-- 插入测试数据
insert into t_test values(1, 'Alice', 25);
insert into t_test values(2, 'Bob', 30);
insert into t_test values(3, 'Charlie', 35);

-- 测试SELECT * (之前能工作)
select * from t_test;

-- 测试SELECT 具体字段名 (之前失败，现在应该成功)
select id, name, age from t_test;

-- 测试部分字段
select id, name from t_test;

-- 测试单个字段
select name from t_test;

-- 测试带WHERE条件的字段查询
select id, name from t_test where age > 25;
