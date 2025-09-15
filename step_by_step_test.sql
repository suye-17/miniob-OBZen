-- Step by step testing for LIKE functionality

-- Step 1: Create table first
CREATE TABLE test_like(id int, name char(20), description char(50));

-- Step 2: Insert data one by one (using English characters first)
INSERT INTO test_like VALUES (1, 'Alice', 'A student from Beijing');

-- Step 3: Test basic SELECT
SELECT * FROM test_like;

-- Step 4: Test simple LIKE
SELECT * FROM test_like WHERE name LIKE 'A%';

-- Step 5: Insert more test data
INSERT INTO test_like VALUES (2, 'Bob', 'A teacher from Shanghai');
INSERT INTO test_like VALUES (3, 'Charlie', 'A worker from Guangzhou');

-- Step 6: Test more LIKE patterns
SELECT * FROM test_like WHERE description LIKE '%teacher%';
SELECT * FROM test_like WHERE name LIKE '_ob';
SELECT * FROM test_like WHERE description LIKE '%from%';

-- Step 7: Test user table
CREATE TABLE users(id int, username char(30), email char(50));

-- Step 8: Insert user data
INSERT INTO users VALUES (1, 'alice123', 'alice@example.com');
INSERT INTO users VALUES (2, 'bob456', 'bob@test.com');
INSERT INTO users VALUES (3, 'charlie789', 'charlie@demo.org');

-- Step 9: Test LIKE on user table
SELECT * FROM users WHERE username LIKE 'alice%';
SELECT * FROM users WHERE email LIKE '%@example.%';
SELECT * FROM users WHERE email LIKE '%test%';

