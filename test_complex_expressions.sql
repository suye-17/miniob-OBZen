-- 测试复杂表达式查询功能
-- 基于用户提供的测试用例

-- 创建表达式测试表
create table exp_table(id int, col1 int, col2 int, col3 float, col4 float);
create table exp_table2(id int, col1 int);

-- 插入测试数据
insert into exp_table VALUES (1, 1, 4, 7.83, 9.42);
insert into exp_table VALUES (9, 1, 9, 3.1, 7.16);
insert into exp_table VALUES (7, 9, 4, 1.37, 8.77);

-- 查看所有数据
select * from exp_table;

-- 测试用户要求的第一个复杂查询
-- select count(id) from exp_table where 1/3*5 < 2+col3*col3/5;
select count(id) from exp_table where 1/3*5 < 2+col3*col3/5;

-- 测试用户要求的第二个复杂查询  
-- select min(col1)+avg(col2)*max(col3)/(max(col4) - 5) from exp_table where id<>6/9;
select min(col1)+avg(col2)*max(col3)/(max(col4) - 5) from exp_table where id<>6/9;

-- 测试其他复杂表达式
select col1*col2, col3+col4 from exp_table;
select id, col1+col2*col3 from exp_table where col4>8;

-- 测试简单聚合函数
select count(*) from exp_table;
select min(col1), max(col1), avg(col2) from exp_table;

exit;
