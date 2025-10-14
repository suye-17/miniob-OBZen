-- 综合测试所有子查询功能

-- 查看测试数据
SELECT * FROM subq_main;
SELECT * FROM subq_ref;

-- 1. IN操作（值列表形式）✅
SELECT * FROM subq_main WHERE id IN (1, 2, 3);

-- 2. NOT IN操作（值列表形式）✅
SELECT * FROM subq_main WHERE id NOT IN (4, 5);

-- 3. IN操作（子查询形式）✅
SELECT * FROM subq_main WHERE id IN (SELECT ref_id FROM subq_ref);

-- 4. NOT IN操作（子查询形式）✅
SELECT * FROM subq_main WHERE id NOT IN (SELECT ref_id FROM subq_ref WHERE ref_id > 2);

-- 5. EXISTS操作✅
SELECT * FROM subq_main WHERE EXISTS (SELECT 1 FROM subq_ref WHERE ref_id = 1);

-- 6. NOT EXISTS操作✅
SELECT * FROM subq_main WHERE NOT EXISTS (SELECT 1 FROM subq_ref WHERE ref_id = 999);
