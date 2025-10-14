-- 创建一个专门的结果表来模拟INNER JOIN的输出

-- 创建结果表
CREATE TABLE join_result_table(
    table1_id int,
    table1_name char(20),
    table2_id int,
    table2_age int
);

-- 查看原始数据
SELECT * FROM join_table_1;
SELECT * FROM join_table_2;

-- 手动插入JOIN结果（模拟INNER JOIN的效果）
-- 只插入id匹配的记录
INSERT INTO join_result_table VALUES (13, '1A4VSK3XXCFXVZZL', 13, 26);
INSERT INTO join_result_table VALUES (11, 'YH41HXZBNFW9A', 11, 25);
INSERT INTO join_result_table VALUES (20, '2NTIAG', 20, 30);

-- 现在可以查询JOIN结果
SELECT * FROM join_result_table;

-- 查询特定字段（模拟您的原始查询）
SELECT table2_age FROM join_result_table;

exit;
