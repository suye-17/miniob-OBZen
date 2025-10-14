-- 调试测试数据和计算过程

-- 清理并重新创建测试数据
delete from exp_table;

-- 插入您提到的测试数据
insert into exp_table VALUES (7, 1, 7, 5.76, 2.67);
insert into exp_table VALUES (9, 6, 8, 30, 6.79);
insert into exp_table VALUES (4, 2, 1, 4.59, 9.38);

-- 查看最终数据
select * from exp_table;

-- 逐步验证计算过程
-- 第一个查询：select count(id) from exp_table where 8/9*4 < 8+col3*col3/4;

-- 先测试左侧表达式：8/9*4
select 8/9*4;

-- 测试右侧表达式对每条记录：8+col3*col3/4
select id, col3, col3*col3, col3*col3/4, 8+col3*col3/4 from exp_table;

-- 测试完整的WHERE条件
select id, 8/9*4 as left_expr, 8+col3*col3/4 as right_expr from exp_table;

-- 第二个查询：select min(col1)+avg(col2)*max(col3)/(max(col4) - 5) from exp_table where id<>8/2;

-- 先测试WHERE条件：id<>8/2
select 8/2;
select * from exp_table where id<>4;

-- 测试各个聚合函数
select min(col1) from exp_table where id<>4;
select avg(col2) from exp_table where id<>4;
select max(col3) from exp_table where id<>4;
select max(col4) from exp_table where id<>4;

-- 测试分步计算
select avg(col2)*max(col3) from exp_table where id<>4;
select max(col4) - 5 from exp_table where id<>4;

exit;
