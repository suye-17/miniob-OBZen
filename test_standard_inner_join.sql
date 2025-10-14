-- 测试标准INNER JOIN语法
-- 设置使用hash join
set hash_join = 1;

-- 测试用户要求的INNER JOIN查询
select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

exit;
