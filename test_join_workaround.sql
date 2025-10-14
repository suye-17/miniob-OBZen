-- JOIN功能的替代解决方案
-- 使用多表查询 + WHERE条件实现INNER JOIN效果

-- 查看各表数据
select * from join_table_1;
select * from join_table_2;

-- 方案1：使用表前缀的多表查询（推荐）
select join_table_1.id, join_table_1.name, join_table_2.age from join_table_1, join_table_2 where join_table_1.id=join_table_2.id;

-- 方案2：如果上面失败，使用子查询方式
select * from join_table_1 where id in (select id from join_table_2);

exit;
