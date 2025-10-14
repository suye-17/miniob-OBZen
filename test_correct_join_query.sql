-- 测试正确的JOIN查询

-- 查看表结构
SELECT * FROM join_table_1;
SELECT * FROM join_table_2;

-- 正确的查询：从join_table_2获取age字段
Select join_table_2.age from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

exit;
