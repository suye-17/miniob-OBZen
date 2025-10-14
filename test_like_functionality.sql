-- 测试LIKE功能
CREATE TABLE like_table(id int, name char(30));
insert into like_table VALUES (0, 'apple');
insert into like_table VALUES (1, 'banana');
insert into like_table VALUES (2, 'coconut');
insert into like_table VALUES (3, 'fig');

-- 查看数据
select * from like_table;

-- 测试LIKE语句
SELECT * FROM like_table WHERE name LIKE 'c%';

exit;
