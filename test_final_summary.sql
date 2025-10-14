-- 最终功能验证
SELECT * FROM subq_main;
SELECT * FROM subq_main WHERE id IN (1, 2);
SELECT * FROM subq_main WHERE EXISTS (SELECT 1 FROM subq_ref WHERE ref_id = 1);
