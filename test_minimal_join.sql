-- 最小化INNER JOIN测试
create table a(id int);
create table b(id int);
insert into a values(1);
insert into b values(1);
select * from a inner join b on a.id=b.id;
exit;
