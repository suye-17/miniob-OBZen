-- 全面测试NULL值在WHERE条件中的处理
-- 根据SQL标准，所有包含NULL的WHERE条件都应该返回空结果

-- 1. 直接NULL值
SELECT 1 WHERE NULL;                    -- 应该返回空结果

-- 2. 除零运算产生的NULL
SELECT 1 WHERE 10/0 = 1;               -- 应该返回空结果  
SELECT 1 WHERE 2/0 > 5;                -- 应该返回空结果
SELECT 1 WHERE 0/0 < 10;               -- 应该返回空结果

-- 3. 复杂表达式中的NULL
SELECT 1 WHERE (10 + 5)/0 = 15;        -- 应该返回空结果
SELECT 1 WHERE 10/(5-5) > 0;           -- 应该返回空结果

-- 4. 比较操作中的NULL (这些在语法上可能不支持，但逻辑上应该返回空结果)
-- SELECT 1 WHERE NULL = NULL;         -- 应该返回空结果
-- SELECT 1 WHERE NULL = 1;            -- 应该返回空结果  
-- SELECT 1 WHERE 1 = NULL;            -- 应该返回空结果

-- 5. 正常情况（不包含NULL，应该有结果）
SELECT 1 WHERE 1 = 1;                  -- 应该返回1
SELECT 1 WHERE 2 > 1;                  -- 应该返回1
SELECT 1 WHERE 10/2 = 5;               -- 应该返回1
SELECT 1 WHERE 1;                      -- 应该返回1

-- 6. 假条件（应该返回空结果，但不是因为NULL）
SELECT 1 WHERE 0;                      -- 应该返回空结果
SELECT 1 WHERE 1 = 2;                  -- 应该返回空结果


