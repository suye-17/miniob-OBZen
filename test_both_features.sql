-- 测试INNER JOIN和DELETE功能

-- 1. 首先确保DELETE功能正常
delete from exp_table where id=9;
select * from exp_table;

-- 2. 测试INNER JOIN语句
Select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

exit;
