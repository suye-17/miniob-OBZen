CREATE TABLE test_null(id int, name char(10), age int);
INSERT INTO test_null VALUES (1, 'Alice', 25);
UPDATE test_null SET age=null where id=1;
SELECT * FROM test_null;
