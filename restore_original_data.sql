-- 恢复原始测试数据

-- 清空现有数据
DELETE FROM exp_table;

-- 插入您提到的原始数据
INSERT INTO exp_table VALUES (1, 7, 2, 6.52, 9.08);
INSERT INTO exp_table VALUES (1, 9, 7, 8.34, 5.0);
INSERT INTO exp_table VALUES (1, 6, 4, 5.77, 8.55);

-- 查看恢复的数据
SELECT * FROM exp_table;

-- 重新测试查询
select min(col1)+avg(col2)*max(col3)/(max(col4) - 4) from exp_table where id<>5/2;

exit;
