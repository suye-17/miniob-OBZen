-- 验证INNER JOIN的预期结果
-- 通过多表查询实现相同功能

-- 查看原始数据
select * from join_table_1;
select * from join_table_2;

-- 执行笛卡尔积查询
select * from join_table_1, join_table_2;

-- 手动验证：从笛卡尔积结果中找到id匹配的记录
-- 预期结果：13 | 1A4VSK3XXCFXVZZL | 13 | 26

exit;
