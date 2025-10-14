-- 全面测试聚合函数子查询功能

-- 查看测试数据
SELECT * FROM subq_main;
SELECT * FROM subq_ref;

-- 测试所有聚合函数的基本功能
SELECT COUNT(*) FROM subq_ref;
SELECT SUM(value) FROM subq_ref;
SELECT AVG(value) FROM subq_ref;
SELECT MAX(ref_id) FROM subq_ref;
SELECT MIN(ref_id) FROM subq_ref;

-- 测试聚合函数子查询 - COUNT
SELECT * FROM subq_main WHERE id <= (SELECT COUNT(*) FROM subq_ref);

-- 测试聚合函数子查询 - SUM
SELECT * FROM subq_main WHERE score < (SELECT SUM(value) FROM subq_ref);

-- 测试聚合函数子查询 - AVG
SELECT * FROM subq_main WHERE score > (SELECT AVG(value) FROM subq_ref);

-- 测试聚合函数子查询 - MAX
SELECT * FROM subq_main WHERE id = (SELECT MAX(ref_id) FROM subq_ref);

-- 测试聚合函数子查询 - MIN
SELECT * FROM subq_main WHERE id >= (SELECT MIN(ref_id) FROM subq_ref);

-- 测试复杂聚合函数子查询
SELECT * FROM subq_main WHERE score BETWEEN (SELECT MIN(value) FROM subq_ref) AND (SELECT MAX(value) FROM subq_ref);
