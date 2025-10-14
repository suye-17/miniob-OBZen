-- 测试您提到的新表达式查询

-- 清理并重新创建测试数据
delete from exp_table;
insert into exp_table VALUES (7, 8, 4, 9.61, 8.9);
insert into exp_table VALUES (4, 7, 4, 6.71, 6.92);
insert into exp_table VALUES (2, 2, 9, 8.28, 7.46);

-- 查看数据
select * from exp_table;

-- 测试第一个查询
select count(id) from exp_table where 7/8*7 < 5+col3*col3/1;

-- 测试第二个查询
select min(col1)+avg(col2)*max(col3)/(max(col4) - 8) from exp_table where id<>7/6;

-- 分步验证计算
select 7/8*7;
select 7/6;

exit;
