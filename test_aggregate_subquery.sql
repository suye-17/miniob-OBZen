-- 测试聚合函数子查询功能

-- 查看测试数据
SELECT * FROM subq_main;
SELECT * FROM subq_ref;

-- 测试基本聚合函数
SELECT AVG(value) FROM subq_ref;
SELECT MAX(ref_id) FROM subq_ref;
SELECT MIN(ref_id) FROM subq_ref;
SELECT SUM(value) FROM subq_ref;
SELECT COUNT(*) FROM subq_ref;

-- 测试聚合函数子查询
SELECT * FROM subq_main WHERE score > (SELECT AVG(value) FROM subq_ref);
SELECT * FROM subq_main WHERE id = (SELECT MAX(ref_id) FROM subq_ref);
SELECT * FROM subq_main WHERE id < (SELECT MIN(ref_id) FROM subq_ref);
SELECT * FROM subq_main WHERE score < (SELECT SUM(value) FROM subq_ref);
SELECT * FROM subq_main WHERE id <= (SELECT COUNT(*) FROM subq_ref);
