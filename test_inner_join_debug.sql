-- 调试INNER JOIN解析问题

-- 首先确保DELETE功能正常
delete from exp_table where id=7;
select * from exp_table;

-- 查看JOIN表数据
select * from join_table_1;
select * from join_table_2;

-- 测试INNER JOIN语句
Select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

exit;
