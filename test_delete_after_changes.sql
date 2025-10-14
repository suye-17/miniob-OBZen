-- 测试修改后DELETE语句是否还能工作
select * from t_basic;
delete from t_basic where id=2;
select * from t_basic;
exit;
