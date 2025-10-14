-- 测试日期功能的完整脚本
-- 基于用户提供的测试用例

-- 创建日期表
CREATE TABLE date_table(id int, u_date date);

-- 创建日期字段上的索引
CREATE INDEX index_id on date_table(u_date);

-- 插入测试数据
INSERT INTO date_table VALUES (1,'2020-01-21');
INSERT INTO date_table VALUES (2,'2020-10-21');
INSERT INTO date_table VALUES (3,'2020-1-01');
INSERT INTO date_table VALUES (11,'2042-02-02');
INSERT INTO date_table VALUES (9,'2038-01-19');

-- 查看所有数据
SELECT * FROM date_table;

-- 测试用户要求的日期比较查询
SELECT * FROM date_table WHERE u_date>'2020-1-20';

-- 测试其他日期比较操作
SELECT * FROM date_table WHERE u_date<'2020-06-01';
SELECT * FROM date_table WHERE u_date='2020-01-21';
SELECT * FROM date_table WHERE u_date>='2020-01-01';

-- 测试日期范围查询
SELECT * FROM date_table WHERE u_date>'2020-01-01' AND u_date<'2030-01-01';

exit;
