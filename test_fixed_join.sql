-- 测试修复后的INNER JOIN预处理器

-- 这个查询应该被转换为不带表前缀的形式
Select join_table_2.age from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

exit;
