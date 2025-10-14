-- 测试子查询表达式解析
SELECT * FROM subq_main WHERE id = (SELECT 1);
