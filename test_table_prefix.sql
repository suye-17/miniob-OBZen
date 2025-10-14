-- 测试表前缀字段解析

-- 测试简单的字段选择（不带表前缀）
SELECT age FROM join_table_2;

-- 测试带表前缀的字段选择（单表）
SELECT join_table_2.age FROM join_table_2;

exit;
