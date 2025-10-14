-- 测试运算符优先级问题

-- 查看数据
SELECT * FROM exp_table;

-- 分步计算，检查优先级
-- 原表达式：min(col1)+avg(col2)*max(col3)/(max(col4) - 4)

-- 步骤1：计算各个聚合函数
SELECT min(col1) FROM exp_table WHERE id<>5/2;
SELECT avg(col2) FROM exp_table WHERE id<>5/2;
SELECT max(col3) FROM exp_table WHERE id<>5/2;
SELECT max(col4) FROM exp_table WHERE id<>5/2;

-- 步骤2：测试乘法和除法的优先级
-- avg(col2)*max(col3) 应该先算
SELECT avg(col2)*max(col3) FROM exp_table WHERE id<>5/2;

-- 步骤3：测试除法
-- (avg(col2)*max(col3))/(max(col4) - 4)
SELECT avg(col2)*max(col3)/(max(col4) - 4) FROM exp_table WHERE id<>5/2;

-- 步骤4：最终加法
SELECT min(col1)+avg(col2)*max(col3)/(max(col4) - 4) FROM exp_table WHERE id<>5/2;

-- 测试可能的错误理解：是否是 (min(col1)+avg(col2))*max(col3)/(max(col4) - 4)
SELECT (min(col1)+avg(col2))*max(col3)/(max(col4) - 4) FROM exp_table WHERE id<>5/2;

exit;
