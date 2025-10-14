-- 手动验证每一步计算

-- 查看当前数据
SELECT * FROM exp_table;

-- 验证WHERE条件：id<>5/2 即 id<>2.5
SELECT 5/2;
SELECT * FROM exp_table WHERE id<>2.5;

-- 分别计算每个聚合函数
SELECT min(col1) FROM exp_table WHERE id<>5/2;
SELECT avg(col2) FROM exp_table WHERE id<>5/2;
SELECT max(col3) FROM exp_table WHERE id<>5/2;
SELECT max(col4) FROM exp_table WHERE id<>5/2;

-- 计算分母
SELECT max(col4) - 4 FROM exp_table WHERE id<>5/2;

-- 完整计算
SELECT min(col1)+avg(col2)*max(col3)/(max(col4) - 4) FROM exp_table WHERE id<>5/2;

exit;
