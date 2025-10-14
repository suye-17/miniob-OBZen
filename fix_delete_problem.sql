-- 解决DELETE和数据重复问题的完整方案

-- 1. 查看当前数据状态
select * from t_basic;

-- 2. 清理所有数据（如果需要）
delete from t_basic;

-- 3. 重新插入干净的数据
insert into t_basic values(1, 20, 'Tom', 85.5);
insert into t_basic values(2, 22, 'Jack', 92.0);
insert into t_basic values(3, 21, 'Mary', 78.5);

-- 4. 验证数据
select * from t_basic;

-- 5. 测试带WHERE的DELETE（使用表前缀）
delete from t_basic where t_basic.id=2;

-- 6. 验证删除结果
select * from t_basic;

exit;
