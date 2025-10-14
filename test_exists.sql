-- 测试EXISTS和NOT EXISTS功能

-- 查看测试数据
SELECT * FROM subq_main;
SELECT * FROM subq_ref;

-- 测试EXISTS：查找在subq_ref中有对应记录的subq_main记录
SELECT * FROM subq_main WHERE EXISTS (SELECT 1 FROM subq_ref WHERE subq_ref.ref_id = subq_main.id);

-- 测试NOT EXISTS：查找在subq_ref中没有对应记录的subq_main记录  
SELECT * FROM subq_main WHERE NOT EXISTS (SELECT 1 FROM subq_ref WHERE subq_ref.ref_id = subq_main.id);

-- 测试EXISTS with 常量子查询（应该返回所有记录）
SELECT * FROM subq_main WHERE EXISTS (SELECT 1 FROM subq_ref WHERE ref_id = 1);

-- 测试NOT EXISTS with 空结果子查询（应该返回所有记录）
SELECT * FROM subq_main WHERE NOT EXISTS (SELECT 1 FROM subq_ref WHERE ref_id = 999);
