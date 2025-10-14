-- 测试用户要求的目标表达式查询
-- 基于EXPRESSION终极实现文档

-- 首先确保DELETE功能正常
delete from t_basic where id=1;
select * from t_basic;

-- 创建测试数据（如果表不存在）
create table exp_table(id int, col1 int, col2 int, col3 float, col4 float);
insert into exp_table VALUES (7, 1, 7, 5.76, 2.67);
insert into exp_table VALUES (9, 6, 8, 30, 6.79);
insert into exp_table VALUES (4, 2, 1, 4.59, 9.38);

-- 查看数据
select * from exp_table;

-- 测试目标查询1：复杂WHERE条件
select count(id) from exp_table where 8/9*4 < 8+col3*col3/4;

-- 测试目标查询2：复杂聚合表达式
select min(col1)+avg(col2)*max(col3)/(max(col4) - 5) from exp_table where id<>8/2;

-- 测试基本表达式计算
select 8/9*4, 8+5.76*5.76/4;
select 8/2;

exit;
