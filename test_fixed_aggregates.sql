-- 测试修复后的聚合函数

-- 确保DELETE功能正常
delete from exp_table where id=1;
select * from exp_table;

-- 测试基本聚合函数
select count(*) from exp_table;
select count(id) from exp_table;
select min(col1), max(col1), avg(col2) from exp_table;

-- 测试目标查询1：复杂WHERE条件
select count(id) from exp_table where 8/9*4 < 8+col3*col3/4;

-- 测试目标查询2：复杂聚合表达式
select min(col1)+avg(col2)*max(col3)/(max(col4) - 5) from exp_table where id<>8/2;

exit;
