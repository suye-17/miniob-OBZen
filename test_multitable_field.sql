-- 测试多表查询中的字段绑定

-- 测试1：选择只存在于一个表中的字段
SELECT age FROM join_table_1, join_table_2;

-- 测试2：选择存在于两个表中的字段（应该会有歧义）
SELECT id FROM join_table_1, join_table_2;

exit;
