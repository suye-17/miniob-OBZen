-- 全面测试LIKE功能

-- 查看数据
select * from like_table;

-- 测试各种LIKE模式
SELECT * FROM like_table WHERE name LIKE 'a%';     -- 前缀匹配：apple
SELECT * FROM like_table WHERE name LIKE '%a%';    -- 包含匹配：apple, banana
SELECT * FROM like_table WHERE name LIKE '%t';     -- 后缀匹配：coconut
SELECT * FROM like_table WHERE name LIKE 'f__';    -- 单字符通配符：fig
SELECT * FROM like_table WHERE name LIKE 'b_n%';   -- 混合通配符：banana

-- 测试精确匹配
SELECT * FROM like_table WHERE name LIKE 'fig';    -- 精确匹配：fig

-- 测试空模式和特殊情况
SELECT * FROM like_table WHERE name LIKE '%';      -- 匹配所有
SELECT * FROM like_table WHERE name LIKE '';       -- 匹配空字符串

exit;
