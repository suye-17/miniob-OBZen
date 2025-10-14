-- 测试修复后的DELETE语句
-- 首先清理并重新插入数据
delete from t_basic;
insert into t_basic values(1, 20, 'Tom', 85.5);
insert into t_basic values(2, 22, 'Jack', 92.0);
insert into t_basic values(3, 21, 'Mary', 78.5);

-- 查看初始数据
select * from t_basic;

-- 测试您要求的DELETE语句
delete from t_basic where id=2;

-- 验证删除结果
select * from t_basic;

exit;
