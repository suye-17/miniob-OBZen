-- 简单的JOIN功能测试

-- 查看当前数据
SELECT * FROM join_table_1;
SELECT * FROM join_table_2;

-- 执行笛卡尔积查询（这个应该工作）
SELECT * FROM join_table_1, join_table_2;

exit;