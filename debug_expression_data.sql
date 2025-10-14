-- 调试表达式数据问题

-- 查看完整的exp_table数据
SELECT * FROM exp_table;

-- 测试WHERE条件
SELECT * FROM exp_table WHERE id<>5/2;

-- 分步计算聚合值
SELECT min(col1) FROM exp_table WHERE id<>5/2;
SELECT avg(col2) FROM exp_table WHERE id<>5/2;
SELECT max(col3) FROM exp_table WHERE id<>5/2;
SELECT max(col4) FROM exp_table WHERE id<>5/2;
SELECT max(col4) - 4 FROM exp_table WHERE id<>5/2;

exit;
