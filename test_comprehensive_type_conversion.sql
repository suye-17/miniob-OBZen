-- 全面测试类型转换功能

-- 查看测试数据和字段类型
SELECT * FROM subq_main;  -- id(INT), name(CHARS), score(FLOAT)
SELECT * FROM subq_ref;   -- ref_id(INT), value(INT)

-- 1. 数值类型之间的转换测试
-- INT vs FLOAT
SELECT * FROM subq_main WHERE id = 1.0;        -- INT字段 vs FLOAT常量
SELECT * FROM subq_main WHERE score = 85;      -- FLOAT字段 vs INT常量
SELECT * FROM subq_main WHERE id = 1.5;        -- INT字段 vs FLOAT常量（应该无匹配）

-- 2. 字符串与数值类型转换测试
SELECT * FROM subq_main WHERE id = '1';        -- INT字段 vs STRING常量
SELECT * FROM subq_main WHERE score = '85.5';  -- FLOAT字段 vs STRING常量
SELECT * FROM subq_main WHERE name = '1';      -- STRING字段 vs STRING常量

-- 3. 聚合函数结果类型转换测试
-- COUNT(INT) vs INT字段
SELECT * FROM subq_main WHERE id = (SELECT COUNT(*) FROM subq_ref);
-- AVG(FLOAT) vs FLOAT字段  
SELECT * FROM subq_main WHERE score = (SELECT AVG(value) FROM subq_ref);
-- AVG(FLOAT) vs INT字段
SELECT * FROM subq_main WHERE id = (SELECT AVG(value) FROM subq_ref);
-- COUNT(INT) vs FLOAT字段
SELECT * FROM subq_main WHERE score = (SELECT COUNT(*) FROM subq_ref);

-- 4. 复杂类型转换测试
-- 字符串字段与数值子查询比较
SELECT * FROM subq_main WHERE name = (SELECT CAST(ref_id AS CHAR) FROM subq_ref WHERE ref_id = 1);

-- 5. 边界情况测试
SELECT * FROM subq_main WHERE id > 0.5;        -- INT vs 小数
SELECT * FROM subq_main WHERE score < 100;     -- FLOAT vs INT
