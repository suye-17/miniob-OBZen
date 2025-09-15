-- MiniOB兼容的测试SQL语句

-- 1. 创建简单测试表
CREATE TABLE test_like(id int, name char(20), description char(50));

-- 2. 插入测试数据
INSERT INTO test_like VALUES (1, 'Alice', 'A student from Beijing');
INSERT INTO test_like VALUES (2, 'Bob', 'A teacher from Shanghai');
INSERT INTO test_like VALUES (3, 'Charlie', 'A worker from Guangzhou');
INSERT INTO test_like VALUES (4, 'David', 'An engineer from Shenzhen');
INSERT INTO test_like VALUES (5, 'Eva', 'A doctor from Beijing');

-- 3. 测试LIKE功能
SELECT * FROM test_like WHERE name LIKE 'A%';
SELECT * FROM test_like WHERE description LIKE '%Beijing%';
SELECT * FROM test_like WHERE name LIKE '_ob';
SELECT * FROM test_like WHERE description LIKE '%teacher%';

-- 4. 创建用户表(简化版)
CREATE TABLE users(
  id int,
  username char(30),
  email char(50),
  phone char(20)
);

-- 5. 插入用户数据
INSERT INTO users VALUES (1, 'alice123', 'alice@example.com', '13800138001');
INSERT INTO users VALUES (2, 'bob456', 'bob@test.com', '13900139002');
INSERT INTO users VALUES (3, 'charlie789', 'charlie@demo.org', '13700137003');

-- 6. 测试用户表的LIKE查询
SELECT * FROM users WHERE username LIKE 'alice%';
SELECT * FROM users WHERE email LIKE '%@example.%';
SELECT * FROM users WHERE phone LIKE '138%';

