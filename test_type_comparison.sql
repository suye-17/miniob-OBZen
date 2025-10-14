-- 测试不同类型值的比较

-- 查看测试数据和类型
SELECT * FROM subq_main;
SELECT * FROM subq_ref;

-- 测试整数与浮点数比较
SELECT * FROM subq_main WHERE id = 1.0;
SELECT * FROM subq_main WHERE score = 85;

-- 测试聚合函数返回类型与字段类型比较
SELECT * FROM subq_main WHERE id = (SELECT COUNT(*) FROM subq_ref);  -- INT vs INT
SELECT * FROM subq_main WHERE score = (SELECT AVG(value) FROM subq_ref);  -- FLOAT vs FLOAT
SELECT * FROM subq_main WHERE id = (SELECT AVG(value) FROM subq_ref);  -- INT vs FLOAT
SELECT * FROM subq_main WHERE score = (SELECT COUNT(*) FROM subq_ref);  -- FLOAT vs INT

-- 测试字符串与数值比较（如果支持）
SELECT * FROM subq_main WHERE name = '1';
SELECT * FROM subq_main WHERE id = '1';
