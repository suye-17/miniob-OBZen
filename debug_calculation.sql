-- 调试完整计算过程

-- 查看过滤后的数据
select * from exp_table where id<>4;

-- 手动计算预期结果
-- min(col1) = 1
-- avg(col2) = (7+8)/2 = 7.5  
-- max(col3) = 30
-- max(col4) = 6.79
-- max(col4) - 5 = 1.79
-- avg(col2)*max(col3) = 7.5*30 = 225
-- avg(col2)*max(col3)/(max(col4) - 5) = 225/1.79 = 125.70
-- min(col1) + 125.70 = 1 + 125.70 = 126.70

-- 测试完整表达式
select min(col1)+avg(col2)*max(col3)/(max(col4) - 5) from exp_table where id<>4;

-- 分步验证计算
select 225/1.79;
select 1 + 225/1.79;

exit;
