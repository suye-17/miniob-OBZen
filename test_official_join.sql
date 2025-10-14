-- 测试官方的INNER JOIN用例

-- 创建官方测试表结构
CREATE TABLE test_join_1(id int, name char);
CREATE TABLE test_join_2(id int, num int);

-- 插入官方测试数据
INSERT INTO test_join_1 VALUES (1, 'a');
INSERT INTO test_join_1 VALUES (2, 'b');
INSERT INTO test_join_1 VALUES (3, 'c');
INSERT INTO test_join_2 VALUES (1, 2);
INSERT INTO test_join_2 VALUES (2, 15);

-- 测试官方的INNER JOIN语法
Select * from test_join_1 inner join test_join_2 on test_join_1.id=test_join_2.id;

exit;