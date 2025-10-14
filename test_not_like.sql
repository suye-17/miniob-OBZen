-- 测试NOT LIKE功能

-- 重新创建like_table数据
delete from like_table;
insert into like_table VALUES (0, 'fig');
insert into like_table VALUES (1, 'apple');
insert into like_table VALUES (2, 'orange');
insert into like_table VALUES (3, 'pineapple');
insert into like_table VALUES (6, 'lemon');

-- 查看数据
select * from like_table;

-- 测试NOT LIKE功能
SELECT * FROM like_table WHERE name NOT LIKE '%a%';

exit;
