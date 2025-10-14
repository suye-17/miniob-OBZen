-- 测试DELETE功能是否正常工作

-- 创建测试表
CREATE TABLE test_delete_check(id int, name char(20));

-- 插入测试数据
INSERT INTO test_delete_check VALUES (1, 'test1');
INSERT INTO test_delete_check VALUES (2, 'test2');
INSERT INTO test_delete_check VALUES (3, 'test3');

-- 查看插入的数据
SELECT * FROM test_delete_check;

-- 测试DELETE功能
DELETE FROM test_delete_check WHERE id = 2;

-- 查看删除后的结果
SELECT * FROM test_delete_check;

-- 测试更复杂的DELETE条件
DELETE FROM test_delete_check WHERE name = 'test3';

-- 查看最终结果
SELECT * FROM test_delete_check;

exit;
