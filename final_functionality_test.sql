-- 最终功能验证测试

-- 1. DELETE功能验证
delete from t_basic where id=3;
select * from t_basic;

-- 2. 复杂表达式验证
select count(id) from exp_table where 8/9*4 < 8+col3*col3/4;

-- 3. 聚合函数表达式验证
select min(col1)+avg(col2)*max(col3)/(max(col4) - 5) from exp_table where id<>4;

-- 4. LIKE功能验证
SELECT * FROM like_table WHERE name LIKE 'c%';

-- 5. 日期比较验证
SELECT * FROM date_table WHERE u_date>'2020-1-20';

-- 6. 多表查询验证
select * from join_table_1, join_table_2;

exit;
