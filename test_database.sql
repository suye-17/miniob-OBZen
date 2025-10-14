-- 测试数据库功能
create table test_new(id int, name char(10));
insert into test_new values(1, 'hello');
insert into test_new values(2, 'world');
select * from test_new;
select test_new.id, test_new.name from test_new;
exit;
