-- ========================================
-- 简单子查询完整功能测试
-- ========================================

-- 准备测试数据
CREATE TABLE subq_main(id int, name char(20), score float);
CREATE TABLE subq_ref(ref_id int, value int);

INSERT INTO subq_main VALUES (1, 'Alice', 85.5);
INSERT INTO subq_main VALUES (2, 'Bob', 92.0);
INSERT INTO subq_main VALUES (3, 'Charlie', 78.5);
INSERT INTO subq_main VALUES (4, 'David', 88.0);
INSERT INTO subq_main VALUES (5, 'Eve', 95.5);

INSERT INTO subq_ref VALUES (1, 100);
INSERT INTO subq_ref VALUES (2, 200);
INSERT INTO subq_ref VALUES (3, 300);

-- ========================================
-- 功能1: 测试IN操作（值列表形式）
-- ========================================
SELECT * FROM subq_main WHERE id IN (1, 3, 5);

-- ========================================
-- 功能2: 测试NOT IN操作（值列表形式）
-- ========================================
SELECT * FROM subq_main WHERE id NOT IN (2, 4);

-- ========================================
-- 功能3: 测试IN操作（子查询形式）
-- ========================================
SELECT * FROM subq_main WHERE id IN (SELECT ref_id FROM subq_ref);

-- ========================================
-- 功能4: 测试NOT IN操作（子查询形式）
-- ========================================
SELECT * FROM subq_main WHERE id NOT IN (SELECT ref_id FROM subq_ref);

-- ========================================
-- 功能5: 测试标量子查询比较运算
-- ========================================
SELECT * FROM subq_main WHERE score > (SELECT 80.0);

-- ========================================
-- 功能6: 测试子查询中的聚合函数
-- ========================================
SELECT * FROM subq_main WHERE score > (SELECT MIN(score) FROM subq_main);

-- ========================================
-- 功能7: 测试子查询返回单个值的比较
-- ========================================  
SELECT * FROM subq_main WHERE id = (SELECT ref_id FROM subq_ref WHERE value = 200);

exit;
