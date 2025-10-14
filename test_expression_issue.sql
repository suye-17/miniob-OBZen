-- 测试表达式功能是否正常

-- 查看表数据
SELECT * FROM exp_table;

-- 测试第一个查询
select count(id) from exp_table where 1/5*8 < 4+col3*col3/8;

-- 测试第二个查询
select min(col1)+avg(col2)*max(col3)/(max(col4) - 4) from exp_table where id<>5/2;

exit;
