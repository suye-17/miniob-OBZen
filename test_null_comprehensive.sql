-- 全面测试NULL值在WHERE条件中的处理
-- 所有包含NULL的WHERE条件都应该返回空结果

-- 1. 除零运算（返回NULL）
SELECT 1 WHERE 10/0 = 1;      -- 应该返回空结果
SELECT 1 WHERE 2/0 > 5;       -- 应该返回空结果  
SELECT 1 WHERE 0/0 < 10;      -- 应该返回空结果

-- 2. 正常情况（不包含NULL）
SELECT 1 WHERE 1 = 1;         -- 应该返回1
SELECT 1 WHERE 2 > 1;         -- 应该返回1
SELECT 1 WHERE 10/2 = 5;      -- 应该返回1

-- 3. 复杂表达式中的除零
SELECT 1 WHERE (10 + 5)/0 = 15;   -- 应该返回空结果
SELECT 1 WHERE 10/(5-5) > 0;      -- 应该返回空结果


