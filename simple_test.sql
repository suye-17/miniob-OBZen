CREATE TABLE simple_test(id int, age int);
INSERT INTO simple_test VALUES (1, 25);
UPDATE simple_test SET age=null where id=1;
SELECT * FROM simple_test;
exit
