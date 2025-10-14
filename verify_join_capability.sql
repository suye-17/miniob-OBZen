-- 验证JOIN核心能力的完整测试

-- 1. 设置hash join配置
set hash_join = 1;

-- 2. 查看原始数据
select * from join_table_1;
select * from join_table_2;

-- 3. 执行多表查询（实现JOIN效果）
select * from join_table_1, join_table_2;

-- 4. 验证Explain功能
explain select * from join_table_1, join_table_2;

-- 5. 测试不同的hash_join设置
set hash_join = 0;
explain select * from join_table_1, join_table_2;

exit;
