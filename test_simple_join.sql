-- 测试简单的多表查询
select * from join_table_1, join_table_2;
select * from join_table_1, join_table_2 where join_table_1.id=join_table_2.id;
exit;
