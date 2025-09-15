-- 测试WHERE条件中的NULL值处理
-- 所有包含NULL的WHERE条件都应该返回空结果

-- 直接NULL值比较
SELECT 1 WHERE NULL = NULL;    -- 应该返回空结果
SELECT 1 WHERE NULL = 1;       -- 应该返回空结果  
SELECT 1 WHERE 1 = NULL;       -- 应该返回空结果
SELECT 1 WHERE NULL > 0;       -- 应该返回空结果

-- 表达式结果为NULL的情况
SELECT 1 WHERE 10/0 = 1;       -- 应该返回空结果(10/0=NULL)
SELECT 1 WHERE NULL + 1 = 2;   -- 应该返回空结果(NULL+1=NULL)
SELECT 1 WHERE 1 * NULL = 0;   -- 应该返回空结果(1*NULL=NULL)

-- 正常情况（不包含NULL）
SELECT 1 WHERE 1 = 1;          -- 应该返回1
SELECT 1 WHERE 2 > 1;          -- 应该返回1
SELECT 1 WHERE 5/2 = 2.5;      -- 应该返回1


