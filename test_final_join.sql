-- 最终测试INNER JOIN

-- 测试您的原始查询
Select join_table_1.age from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

-- 测试正确的查询
Select join_table_2.age from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

-- 测试完整查询
Select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

exit;
