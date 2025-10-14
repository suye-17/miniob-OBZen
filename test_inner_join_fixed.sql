-- 测试修复后的INNER JOIN功能

-- 查看测试数据
select * from join_table_1;
select * from join_table_2;

-- 测试用户要求的INNER JOIN查询
Select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

exit;
