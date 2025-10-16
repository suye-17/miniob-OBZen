-- 简单子查询基础测试用例
-- 这是一个精简版本，包含核心必测功能

-- 准备数据
DROP TABLE IF EXISTS t1;
DROP TABLE IF EXISTS t2;
CREATE TABLE t1 (id INT, name CHAR(10), value INT);
CREATE TABLE t2 (id INT, ref_id INT, score INT);

INSERT INTO t1 VALUES (1, 'a', 100);
INSERT INTO t1 VALUES (2, 'b', 200);
INSERT INTO t1 VALUES (3, 'c', 300);
INSERT INTO t1 VALUES (4, 'd', NULL);

INSERT INTO t2 VALUES (1, 1, 85);
INSERT INTO t2 VALUES (2, 1, 90);
INSERT INTO t2 VALUES (3, 2, 75);
INSERT INTO t2 VALUES (4, 3, 95);
INSERT INTO t2 VALUES (5, 5, 80);
INSERT INTO t2 VALUES (6, 2, NULL);

-- ========== IN 子查询测试 ==========
-- TEST 1: 基本 IN
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2);

-- TEST 2: IN 返回空集
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2 WHERE score > 100);

-- TEST 3: IN 子查询包含聚合
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2 WHERE score > 80);

-- ========== NOT IN 子查询测试 ==========
-- TEST 4: 基本 NOT IN
SELECT * FROM t1 WHERE id NOT IN (SELECT ref_id FROM t2 WHERE ref_id IS NOT NULL);

-- TEST 5: NOT IN 返回空集（应返回全部）
SELECT * FROM t1 WHERE id NOT IN (SELECT ref_id FROM t2 WHERE score > 100);

-- TEST 6: NOT IN 包含 NULL（关键测试）
-- 如果子查询结果中有NULL，NOT IN应返回空集或正确处理
SELECT * FROM t1 WHERE id NOT IN (SELECT score FROM t2);

-- ========== EXISTS 子查询测试 ==========
-- TEST 7: 基本 EXISTS
SELECT * FROM t1 WHERE EXISTS (SELECT * FROM t2 WHERE t2.ref_id = t1.id);

-- TEST 8: EXISTS 返回非空
SELECT * FROM t1 WHERE EXISTS (SELECT * FROM t2 WHERE score > 80);

-- TEST 9: EXISTS 返回空
SELECT * FROM t1 WHERE EXISTS (SELECT * FROM t2 WHERE score > 100);

-- ========== NOT EXISTS 子查询测试 ==========
-- TEST 10: 基本 NOT EXISTS
SELECT * FROM t1 WHERE NOT EXISTS (SELECT * FROM t2 WHERE t2.ref_id = t1.id);

-- TEST 11: NOT EXISTS 返回空（应返回全部）
SELECT * FROM t1 WHERE NOT EXISTS (SELECT * FROM t2 WHERE score > 100);

-- ========== 比较运算符测试 ==========
-- TEST 12: = 子查询
SELECT * FROM t1 WHERE value = (SELECT MAX(score) FROM t2);

-- TEST 13: > 子查询
SELECT * FROM t1 WHERE value > (SELECT AVG(score) FROM t2 WHERE score IS NOT NULL);

-- TEST 14: < 子查询
SELECT * FROM t1 WHERE value < (SELECT SUM(score) FROM t2 WHERE score IS NOT NULL);

-- TEST 15: >= 子查询
SELECT * FROM t2 WHERE score >= (SELECT MIN(value) FROM t1 WHERE value IS NOT NULL);

-- TEST 16: <= 子查询
SELECT * FROM t2 WHERE score <= (SELECT MAX(value) FROM t1 WHERE value IS NOT NULL);

-- TEST 17: != 子查询
SELECT * FROM t1 WHERE value != (SELECT MIN(score) FROM t2 WHERE score IS NOT NULL);

-- ========== 聚合函数子查询测试 ==========
-- TEST 18: MAX
SELECT * FROM t2 WHERE score = (SELECT MAX(score) FROM t2);

-- TEST 19: MIN
SELECT * FROM t2 WHERE score = (SELECT MIN(score) FROM t2 WHERE score IS NOT NULL);

-- TEST 20: AVG
SELECT * FROM t1 WHERE value > (SELECT AVG(value) FROM t1 WHERE value IS NOT NULL);

-- TEST 21: COUNT
SELECT * FROM t1 WHERE id <= (SELECT COUNT(*) FROM t2);

-- TEST 22: SUM
SELECT * FROM t1 WHERE value < (SELECT SUM(score) FROM t2 WHERE score IS NOT NULL);

-- ========== 嵌套子查询测试 ==========
-- TEST 23: 2层嵌套
SELECT * FROM t1 WHERE id IN (
    SELECT ref_id FROM t2 WHERE score > (SELECT AVG(score) FROM t2 WHERE score IS NOT NULL)
);

-- TEST 24: IN 中嵌套 IN
SELECT * FROM t1 WHERE id IN (
    SELECT ref_id FROM t2 WHERE id IN (SELECT id FROM t2 WHERE score > 80)
);

-- ========== NULL 处理测试 ==========
-- TEST 25: 子查询结果包含 NULL（IN）
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2);

-- TEST 26: 子查询结果包含 NULL（NOT IN）
SELECT * FROM t2 WHERE id NOT IN (SELECT id FROM t1 WHERE value IS NOT NULL);

-- TEST 27: 比较运算中子查询可能返回 NULL
SELECT * FROM t1 WHERE value > (SELECT value FROM t1 WHERE id = 4);

-- ========== 组合条件测试 ==========
-- TEST 28: AND 组合
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2) AND value > 150;

-- TEST 29: OR 组合
SELECT * FROM t1 WHERE id IN (SELECT ref_id FROM t2 WHERE score > 90) OR value < 150;

-- TEST 30: NOT 组合
SELECT * FROM t1 WHERE NOT (id IN (SELECT ref_id FROM t2 WHERE score < 80));

-- ========== SELECT 子句中的子查询 ==========
-- TEST 31: SELECT 中的标量子查询
SELECT id, name, (SELECT COUNT(*) FROM t2 WHERE t2.ref_id = t1.id) FROM t1;

-- TEST 32: SELECT 中的聚合子查询
SELECT id, name, (SELECT MAX(score) FROM t2 WHERE t2.ref_id = t1.id) FROM t1;

-- TEST 33: SELECT 中多个子查询
SELECT id, 
       (SELECT COUNT(*) FROM t2 WHERE t2.ref_id = t1.id),
       (SELECT AVG(score) FROM t2 WHERE t2.ref_id = t1.id)
FROM t1;

-- ========== 边界情况测试 ==========
-- TEST 34: 空结果集
DROP TABLE IF EXISTS empty;
CREATE TABLE empty (id INT);
SELECT * FROM t1 WHERE id IN (SELECT id FROM empty);
SELECT * FROM t1 WHERE id NOT IN (SELECT id FROM empty);

-- TEST 35: 单行单列
SELECT * FROM t1 WHERE id = (SELECT ref_id FROM t2 WHERE id = 1);

-- TEST 36: DISTINCT 子查询
SELECT * FROM t1 WHERE id IN (SELECT DISTINCT ref_id FROM t2 WHERE ref_id IS NOT NULL);

-- ========== 清理 ==========
DROP TABLE t1;
DROP TABLE t2;
DROP TABLE empty;


