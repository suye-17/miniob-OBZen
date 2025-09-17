-- 测试 LIKE 功能的 SQL 脚本
-- 基于用户提供的测试用例

-- 创建测试表
CREATE TABLE like_table(id int, name char(30));

-- 插入测试数据
insert into like_table VALUES (0, 'orange');
insert into like_table VALUES (1, 'coconut');
insert into like_table VALUES (2, 'strawberry');
insert into like_table VALUES (3, 'cherry');
insert into like_table VALUES (4, 'cherry');
insert into like_table VALUES (5, 'cherry');
insert into like_table VALUES (11, 'cherry');
insert into like_table VALUES (12, 'coconut');

-- 测试 LIKE 查询：查找以 'c' 开头的名称
-- 预期结果应该包含：coconut, cherry 等
SELECT * FROM like_table WHERE name LIKE 'c%';

-- 额外的 LIKE 测试用例
-- 测试精确匹配
SELECT * FROM like_table WHERE name LIKE 'cherry';

-- 测试包含匹配
SELECT * FROM like_table WHERE name LIKE '%erry%';

-- 测试单字符通配符
SELECT * FROM like_table WHERE name LIKE '_herry';

-- 测试结尾匹配
SELECT * FROM like_table WHERE name LIKE '%ut';

-- 显示所有数据以供对比
SELECT * FROM like_table;
