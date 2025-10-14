-- 测试子查询形式的IN操作

-- 查看数据
SELECT * FROM subq_main;
SELECT * FROM subq_ref;

-- 测试IN子查询
SELECT * FROM subq_main WHERE id IN (SELECT ref_id FROM subq_ref);

exit;
