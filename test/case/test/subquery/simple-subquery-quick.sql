-- 简单子查询快速验证测试
-- 仅包含最核心的测试点，用于快速验证功能是否正常

-- 准备简单数据
DROP TABLE IF EXISTS T1;
DROP TABLE IF EXISTS T2;

CREATE TABLE T1 (id INT, val INT);
CREATE TABLE T2 (id INT, ref INT);

INSERT INTO T1 VALUES (1, 10);
INSERT INTO T1 VALUES (2, 20);
INSERT INTO T1 VALUES (3, 30);

INSERT INTO T2 VALUES (1, 1);
INSERT INTO T2 VALUES (2, 1);
INSERT INTO T2 VALUES (3, 2);

-- ========================================
-- 快速测试 1: IN 子查询
-- 预期: 返回 id=1,2 的行
-- ========================================
SELECT * FROM T1 WHERE id IN (SELECT ref FROM T2);

-- ========================================
-- 快速测试 2: NOT IN 子查询
-- 预期: 返回 id=3 的行
-- ========================================
SELECT * FROM T1 WHERE id NOT IN (SELECT ref FROM T2);

-- ========================================
-- 快速测试 3: EXISTS 子查询
-- 预期: 返回所有行（因为 T2 不为空）
-- ========================================
SELECT * FROM T1 WHERE EXISTS (SELECT * FROM T2);

-- ========================================
-- 快速测试 4: NOT EXISTS 子查询
-- 预期: 返回空（因为 T2 不为空）
-- ========================================
SELECT * FROM T1 WHERE NOT EXISTS (SELECT * FROM T2);

-- ========================================
-- 快速测试 5: 比较运算符与子查询
-- 预期: 返回 val > 10 的行 (id=2,3)
-- ========================================
SELECT * FROM T1 WHERE val > (SELECT MIN(val) FROM T1);

-- ========================================
-- 快速测试 6: 聚合函数子查询
-- 预期: 返回 val=30 的行 (id=3)
-- ========================================
SELECT * FROM T1 WHERE val = (SELECT MAX(val) FROM T1);

-- ========================================
-- 快速测试 7: 嵌套子查询
-- 预期: 返回 id=2 的行
-- ========================================
SELECT * FROM T1 WHERE id IN (
    SELECT ref FROM T2 WHERE id > (SELECT MIN(id) FROM T2)
);

-- ========================================
-- 快速测试 8: NULL 处理 - NOT IN 关键测试
-- ========================================
INSERT INTO T2 VALUES (4, NULL);

-- 预期: 返回空集（NOT IN 遇到 NULL）
SELECT * FROM T1 WHERE id NOT IN (SELECT ref FROM T2);

-- 清理
DELETE FROM T2 WHERE id = 4;

-- ========================================
-- 快速测试 9: SELECT 子句中的子查询
-- 预期: 每行显示 id, val, 以及对应的 T2 中的计数
-- ========================================
SELECT id, val, (SELECT COUNT(*) FROM T2 WHERE T2.ref = T1.id) as cnt FROM T1;

-- ========================================
-- 快速测试 10: 组合条件
-- 预期: 返回 id=2 的行
-- ========================================
SELECT * FROM T1 WHERE id IN (SELECT ref FROM T2) AND val > 15;

-- 清理数据
DROP TABLE T1;
DROP TABLE T2;

-- ========================================
-- 成功信息
-- ========================================
-- 如果所有测试都执行成功，说明简单子查询核心功能正常


