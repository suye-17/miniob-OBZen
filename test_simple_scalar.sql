-- 测试简单的标量子查询
SELECT * FROM subq_main;
SELECT * FROM subq_main WHERE id = (SELECT 1);
