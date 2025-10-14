-- 简化的类型转换测试

-- 基础数据
SELECT * FROM subq_main;

-- 测试数值类型转换
SELECT * FROM subq_main WHERE id = 1.0;        -- INT vs FLOAT
SELECT * FROM subq_main WHERE score = 85;      -- FLOAT vs INT
SELECT * FROM subq_main WHERE id = '1';        -- INT vs STRING
SELECT * FROM subq_main WHERE score = '85.5';  -- FLOAT vs STRING

-- 测试聚合函数类型转换
SELECT * FROM subq_main WHERE id = (SELECT COUNT(*) FROM subq_ref);  -- INT vs INT
SELECT * FROM subq_main WHERE score = (SELECT COUNT(*) FROM subq_ref);  -- FLOAT vs INT
