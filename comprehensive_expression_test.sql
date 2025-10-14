-- 全面的表达式功能测试

-- 1. 测试基本算术运算优先级
select 2+3*4, (2+3)*4, 2*3+4, 2*(3+4);

-- 2. 测试浮点数精度
select 1/3, 2/3, 1.0/3.0, 10/3;

-- 3. 测试复杂嵌套表达式
select ((1+2)*3)+4, 1+(2*(3+4)), (1+2)*(3+4);

-- 4. 测试聚合函数的不同参数
select count(*), count(id), count(col1) from exp_table;

-- 5. 测试WHERE条件中的复杂表达式
select * from exp_table where col1*2 > col2;
select * from exp_table where col3/col4 > 1;

-- 6. 测试聚合函数中的表达式
select sum(col1*col2), avg(col3+col4), max(col1+col2) from exp_table;

-- 7. 测试运算符优先级在WHERE中的表现
select * from exp_table where 2+3*4 > 10;
select * from exp_table where (2+3)*4 > 10;

-- 8. 测试除零和NULL处理
select 10/0, 5/0.0, NULL+1, NULL*2;

-- 9. 测试不同数据类型的计算
select id+col3, col1*col4, col2-col3 from exp_table;

-- 10. 测试复杂的聚合表达式组合
select min(col1)*max(col2), avg(col3)/avg(col4), sum(col1)/count(*) from exp_table;

exit;
