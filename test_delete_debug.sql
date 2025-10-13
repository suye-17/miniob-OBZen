-- 测试DELETE语句解析
-- 先创建表和数据
create table test_del(id int, name char(10));
insert into test_del values(1, 'test1');
insert into test_del values(2, 'test2');

-- 测试简单的DELETE语句
delete from test_del where id=1;


