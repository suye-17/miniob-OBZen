-- 测试DELETE语句
create table test_delete(id int, name char(10));
insert into test_delete values(1, 'test1');
insert into test_delete values(2, 'test2');
select * from test_delete;
delete from test_delete where id=2;
select * from test_delete;
exit;
