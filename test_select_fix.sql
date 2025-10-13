-- 测试修复后的SELECT语句
create table t_basic(id int, age int, name char(4), score float);

-- 插入测试数据
insert into t_basic values(1, 20, 'Tom', 85.5);
insert into t_basic values(2, 22, 'Jack', 92.0);
insert into t_basic values(3, 21, 'Mary', 78.5);

-- 测试您的SELECT语句
select id, age, name, score from t_basic;

-- 测试其他SELECT变体
select * from t_basic;
select id from t_basic;
select id, name from t_basic where age > 20;

exit;
