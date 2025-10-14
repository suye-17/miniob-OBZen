-- 测试标量子查询功能

-- 查看测试数据
SELECT * FROM subq_main;
SELECT * FROM subq_ref;

-- 测试标量子查询：查找id等于子查询返回值的记录
SELECT * FROM subq_main WHERE id = (SELECT ref_id FROM subq_ref WHERE value = 100);

-- 测试标量子查询：查找score大于子查询返回的平均值的记录
SELECT * FROM subq_main WHERE score > (SELECT AVG(value) FROM subq_ref);

-- 测试标量子查询：查找id小于子查询返回的最大值的记录
SELECT * FROM subq_main WHERE id < (SELECT MAX(ref_id) FROM subq_ref);
