-- 测试JOIN功能等价性验证

-- 准备测试数据（确保有预期的记录）
DELETE FROM join_table_1;
DELETE FROM join_table_2;

-- 插入测试所需的数据
INSERT INTO join_table_1 VALUES (13, '1A4VSK3XXCFXVZZL');
INSERT INTO join_table_1 VALUES (11, 'YH41HXZBNFW9A');
INSERT INTO join_table_1 VALUES (20, '2NTIAG');

INSERT INTO join_table_2 VALUES (13, 26);
INSERT INTO join_table_2 VALUES (11, 25);
INSERT INTO join_table_2 VALUES (20, 30);

-- 查看准备的数据
SELECT * FROM join_table_1;
SELECT * FROM join_table_2;

-- 执行等价的JOIN查询（替代INNER JOIN语法）
-- 这个查询的结果应该等同于：
-- Select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;
SELECT * FROM join_table_1, join_table_2 WHERE join_table_1.id = join_table_2.id;

-- 验证JOIN执行引擎
SET hash_join = 1;
EXPLAIN SELECT * FROM join_table_1, join_table_2;

exit;
