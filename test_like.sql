-- 测试LIKE功能的SQL文件

-- 创建测试表
CREATE TABLE test_like(id int, name char(20), description char(50));

-- 插入测试数据
INSERT INTO test_like VALUES (1, 'Alice', 'A student from Beijing');
INSERT INTO test_like VALUES (2, 'Bob', 'A teacher from Shanghai');
INSERT INTO test_like VALUES (3, 'Charlie', 'A worker from Guangzhou');
INSERT INTO test_like VALUES (4, 'David', 'An engineer from Shenzhen');
INSERT INTO test_like VALUES (5, 'Eva', 'A doctor from Beijing');

-- 测试基本LIKE操作
-- 1. 精确匹配
SELECT * FROM test_like WHERE name LIKE 'Alice';

-- 2. %匹配多个字符
SELECT * FROM test_like WHERE description LIKE '%Beijing%';

-- 3. _匹配单个字符  
SELECT * FROM test_like WHERE name LIKE '_ob';

-- 4. %在开头
SELECT * FROM test_like WHERE description LIKE '%teacher%';

-- 5. %在结尾
SELECT * FROM test_like WHERE name LIKE 'A%';

-- 6. 组合使用%和_
SELECT * FROM test_like WHERE description LIKE 'A% from %';

-- 显示所有数据进行对比
SELECT * FROM test_like;
