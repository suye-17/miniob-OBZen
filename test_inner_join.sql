-- 测试INNER JOIN功能
-- 基于用户提供的测试用例

-- 创建连接测试表
CREATE TABLE join_table_1(id int, name char(20));
CREATE TABLE join_table_2(id int, age int);
CREATE TABLE join_table_3(id int, level int);
create table join_table_empty_1(id int, num_empty_1 int);
create table join_table_empty_2(id int, num_empty_2 int);

-- 插入测试数据到join_table_1
insert into join_table_1 values(11, 'YH41HXZBNFW9A');
insert into join_table_1 values(20, '2NTIAG');
insert into join_table_1 values(4, '3ZES94O46T5WZOOC');
insert into join_table_1 values(1, 'TEST1');
insert into join_table_1 values(2, 'TEST2');

-- 插入测试数据到join_table_2
insert into join_table_2 values(11, 25);
insert into join_table_2 values(20, 30);
insert into join_table_2 values(4, 22);
insert into join_table_2 values(3, 28);

-- 查看各表数据
select * from join_table_1;
select * from join_table_2;

-- 测试用户要求的INNER JOIN查询
Select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

-- 测试其他JOIN变体
select join_table_1.id, join_table_1.name, join_table_2.age from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id;

-- 测试空表的JOIN
select * from join_table_empty_1 inner join join_table_empty_2 on join_table_empty_1.id=join_table_empty_2.id;

exit;
