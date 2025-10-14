-- 基于官方测试用例的简化版本
CREATE TABLE join_table_test1(id int, name char);
CREATE TABLE join_table_test2(id int, num int);

INSERT INTO join_table_test1 VALUES (1, 'a');
INSERT INTO join_table_test1 VALUES (2, 'b');
INSERT INTO join_table_test2 VALUES (1, 2);
INSERT INTO join_table_test2 VALUES (2, 15);

set hash_join = 1;

-- 查看数据
select * from join_table_test1;
select * from join_table_test2;

-- 尝试INNER JOIN
select * from join_table_test1 inner join join_table_test2 on join_table_test1.id = join_table_test2.id;

exit;
