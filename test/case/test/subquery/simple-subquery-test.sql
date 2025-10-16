-- 简单子查询功能测试
-- 测试包括：IN/NOT IN, EXISTS/NOT EXISTS, 比较运算, 聚合函数, NULL处理

-- 准备测试数据
DROP TABLE IF EXISTS students;
DROP TABLE IF EXISTS courses;
DROP TABLE IF EXISTS enrollments;
DROP TABLE IF EXISTS grades;

-- 创建学生表
CREATE TABLE students (
    id INT,
    name CHAR(20),
    age INT,
    class_id INT
);

-- 创建课程表
CREATE TABLE courses (
    id INT,
    course_name CHAR(30),
    credit INT
);

-- 创建选课表
CREATE TABLE enrollments (
    student_id INT,
    course_id INT,
    score INT
);

-- 创建成绩等级表
CREATE TABLE grades (
    grade_name CHAR(10),
    min_score INT,
    max_score INT
);

-- 插入学生数据
INSERT INTO students VALUES (1, 'Alice', 20, 1);
INSERT INTO students VALUES (2, 'Bob', 21, 1);
INSERT INTO students VALUES (3, 'Charlie', 19, 2);
INSERT INTO students VALUES (4, 'David', 22, 2);
INSERT INTO students VALUES (5, 'Eve', 20, 3);
INSERT INTO students VALUES (6, 'Frank', 23, NULL);

-- 插入课程数据
INSERT INTO courses VALUES (101, 'Math', 4);
INSERT INTO courses VALUES (102, 'Physics', 3);
INSERT INTO courses VALUES (103, 'Chemistry', 3);
INSERT INTO courses VALUES (104, 'English', 2);
INSERT INTO courses VALUES (105, 'History', 2);

-- 插入选课数据
INSERT INTO enrollments VALUES (1, 101, 90);
INSERT INTO enrollments VALUES (1, 102, 85);
INSERT INTO enrollments VALUES (2, 101, 78);
INSERT INTO enrollments VALUES (2, 103, 82);
INSERT INTO enrollments VALUES (3, 102, 95);
INSERT INTO enrollments VALUES (3, 103, 88);
INSERT INTO enrollments VALUES (4, 101, 92);
INSERT INTO enrollments VALUES (5, 104, NULL);

-- 插入成绩等级数据
INSERT INTO grades VALUES ('A', 90, 100);
INSERT INTO grades VALUES ('B', 80, 89);
INSERT INTO grades VALUES ('C', 70, 79);
INSERT INTO grades VALUES ('D', 60, 69);

-- =====================================================
-- 测试1: 基本 IN 子查询
-- =====================================================
-- 1.1 查询选修了课程的学生
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments);

-- 1.2 查询选修了Math课程的学生
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments WHERE course_id = 101);

-- 1.3 查询成绩在90分以上的学生
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments WHERE score > 90);

-- 1.4 IN子查询返回空集
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments WHERE score > 100);

-- =====================================================
-- 测试2: NOT IN 子查询
-- =====================================================
-- 2.1 查询没有选课的学生
SELECT * FROM students WHERE id NOT IN (SELECT student_id FROM enrollments);

-- 2.2 查询没有选修Math课程的学生
SELECT * FROM students WHERE id NOT IN (SELECT student_id FROM enrollments WHERE course_id = 101);

-- 2.3 NOT IN 子查询包含 NULL 值的特殊情况
-- 这应该返回空集或正确处理NULL
SELECT * FROM students WHERE id NOT IN (SELECT course_id FROM enrollments WHERE score IS NULL OR score > 80);

-- 2.4 NOT IN 子查询返回空集（应返回所有记录）
SELECT * FROM students WHERE id NOT IN (SELECT student_id FROM enrollments WHERE score > 100);

-- =====================================================
-- 测试3: EXISTS 子查询
-- =====================================================
-- 3.1 查询有选课记录的学生
SELECT * FROM students s WHERE EXISTS (SELECT * FROM enrollments e WHERE e.student_id = s.id);

-- 3.2 查询是否存在分数大于90的记录
SELECT * FROM students WHERE EXISTS (SELECT * FROM enrollments WHERE score > 90);

-- 3.3 EXISTS 子查询返回空集
SELECT * FROM students WHERE EXISTS (SELECT * FROM enrollments WHERE score > 100);

-- =====================================================
-- 测试4: NOT EXISTS 子查询
-- =====================================================
-- 4.1 查询没有选课记录的学生
SELECT * FROM students s WHERE NOT EXISTS (SELECT * FROM enrollments e WHERE e.student_id = s.id);

-- 4.2 查询不存在分数小于60的情况下的所有学生
SELECT * FROM students WHERE NOT EXISTS (SELECT * FROM enrollments WHERE score < 60);

-- =====================================================
-- 测试5: 比较运算符与子查询（单行子查询）
-- =====================================================
-- 5.1 查询年龄大于某个班级平均年龄的学生
SELECT * FROM students WHERE age > (SELECT AVG(age) FROM students WHERE class_id = 1);

-- 5.2 查询分数等于最高分的选课记录
SELECT * FROM enrollments WHERE score = (SELECT MAX(score) FROM enrollments);

-- 5.3 查询学分大于平均学分的课程
SELECT * FROM courses WHERE credit > (SELECT AVG(credit) FROM courses);

-- 5.4 比较运算 >= 
SELECT * FROM students WHERE age >= (SELECT MIN(age) FROM students);

-- 5.5 比较运算 <
SELECT * FROM students WHERE age < (SELECT MAX(age) FROM students);

-- 5.6 比较运算 <=
SELECT * FROM courses WHERE credit <= (SELECT MAX(credit) FROM courses);

-- 5.7 比较运算 != 或 <>
SELECT * FROM students WHERE age != (SELECT MIN(age) FROM students);

-- =====================================================
-- 测试6: 子查询包含聚合函数
-- =====================================================
-- 6.1 使用 COUNT 聚合函数
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments GROUP BY student_id HAVING COUNT(*) > 1);

-- 6.2 使用 MAX 聚合函数
SELECT * FROM enrollments WHERE score = (SELECT MAX(score) FROM enrollments);

-- 6.3 使用 MIN 聚合函数
SELECT * FROM enrollments WHERE score = (SELECT MIN(score) FROM enrollments WHERE score IS NOT NULL);

-- 6.4 使用 AVG 聚合函数
SELECT * FROM students WHERE age > (SELECT AVG(age) FROM students);

-- 6.5 使用 SUM 聚合函数
SELECT * FROM courses WHERE credit < (SELECT SUM(credit) / COUNT(*) FROM courses);

-- 6.6 子查询中多个聚合函数组合
SELECT * FROM enrollments WHERE score BETWEEN (SELECT MIN(score) FROM enrollments WHERE score IS NOT NULL) 
                                          AND (SELECT MAX(score) FROM enrollments);

-- =====================================================
-- 测试7: 复杂的嵌套子查询
-- =====================================================
-- 7.1 子查询嵌套子查询（2层）
SELECT * FROM students WHERE id IN (
    SELECT student_id FROM enrollments WHERE course_id IN (
        SELECT id FROM courses WHERE credit > 2
    )
);

-- 7.2 子查询嵌套子查询（3层）
SELECT * FROM students WHERE id IN (
    SELECT student_id FROM enrollments WHERE score > (
        SELECT AVG(score) FROM enrollments WHERE course_id IN (
            SELECT id FROM courses WHERE credit >= 3
        )
    )
);

-- 7.3 多个子查询组合（AND）
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments WHERE course_id = 101) 
                         AND age > (SELECT AVG(age) FROM students);

-- 7.4 多个子查询组合（OR）
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments WHERE score > 90) 
                         OR id IN (SELECT student_id FROM enrollments WHERE course_id = 102);

-- =====================================================
-- 测试8: NULL 值处理
-- =====================================================
-- 8.1 子查询结果包含 NULL
SELECT * FROM students WHERE class_id IN (SELECT class_id FROM students);

-- 8.2 NOT IN 子查询包含 NULL（特殊情况）
SELECT * FROM courses WHERE id NOT IN (SELECT course_id FROM enrollments WHERE student_id = 5);

-- 8.3 比较运算中子查询返回 NULL
SELECT * FROM students WHERE age > (SELECT score FROM enrollments WHERE student_id = 5 AND course_id = 104);

-- 8.4 EXISTS 处理 NULL
SELECT * FROM students WHERE EXISTS (SELECT * FROM enrollments WHERE score IS NULL);

-- =====================================================
-- 测试9: 子查询在 SELECT 子句中
-- =====================================================
-- 9.1 SELECT 子句中的标量子查询
SELECT id, name, (SELECT COUNT(*) FROM enrollments e WHERE e.student_id = students.id) FROM students;

-- 9.2 SELECT 子句中使用聚合函数
SELECT id, name, (SELECT MAX(score) FROM enrollments e WHERE e.student_id = students.id) FROM students;

-- 9.3 SELECT 子句中多个子查询
SELECT id, name, 
       (SELECT COUNT(*) FROM enrollments e WHERE e.student_id = students.id),
       (SELECT AVG(score) FROM enrollments e WHERE e.student_id = students.id)
FROM students;

-- =====================================================
-- 测试10: WHERE 子句中多个条件组合子查询
-- =====================================================
-- 10.1 AND 连接多个子查询条件
SELECT * FROM students 
WHERE id IN (SELECT student_id FROM enrollments WHERE score > 80)
  AND age > 20;

-- 10.2 OR 连接多个子查询条件
SELECT * FROM students 
WHERE id IN (SELECT student_id FROM enrollments WHERE course_id = 101)
   OR id IN (SELECT student_id FROM enrollments WHERE course_id = 102);

-- 10.3 NOT 与子查询组合
SELECT * FROM students 
WHERE NOT (id IN (SELECT student_id FROM enrollments WHERE score < 80));

-- 10.4 复杂布尔表达式
SELECT * FROM students 
WHERE (id IN (SELECT student_id FROM enrollments WHERE score > 85) AND age < 22)
   OR (id NOT IN (SELECT student_id FROM enrollments WHERE course_id = 103));

-- =====================================================
-- 测试11: 子查询返回多行的错误情况检测
-- =====================================================
-- 11.1 比较运算符用于多行子查询（应该报错或使用ANY/ALL）
-- SELECT * FROM students WHERE age = (SELECT age FROM students WHERE class_id = 1);

-- =====================================================
-- 测试12: 边界情况测试
-- =====================================================
-- 12.1 空表子查询
DROP TABLE IF EXISTS empty_table;
CREATE TABLE empty_table (id INT);
SELECT * FROM students WHERE id IN (SELECT id FROM empty_table);
SELECT * FROM students WHERE id NOT IN (SELECT id FROM empty_table);
SELECT * FROM students WHERE EXISTS (SELECT * FROM empty_table);
SELECT * FROM students WHERE NOT EXISTS (SELECT * FROM empty_table);

-- 12.2 子查询返回大量数据
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments);

-- 12.3 子查询包含 DISTINCT
SELECT * FROM students WHERE id IN (SELECT DISTINCT student_id FROM enrollments);

-- 12.4 子查询使用 ORDER BY（虽然在子查询中通常无意义）
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments ORDER BY score DESC);

-- =====================================================
-- 测试13: 不同数据类型比较
-- =====================================================
-- 13.1 INT 与子查询结果比较
SELECT * FROM students WHERE age IN (SELECT credit FROM courses);

-- 13.2 字符串与子查询
SELECT * FROM students WHERE name IN (SELECT course_name FROM courses);

-- 13.3 混合类型条件
SELECT * FROM students WHERE id IN (SELECT course_id FROM enrollments) AND name = 'Alice';

-- =====================================================
-- 测试14: 子查询中的 WHERE 条件
-- =====================================================
-- 14.1 子查询带多个 WHERE 条件
SELECT * FROM students WHERE id IN (
    SELECT student_id FROM enrollments WHERE score > 80 AND course_id < 103
);

-- 14.2 子查询带 BETWEEN
SELECT * FROM students WHERE id IN (
    SELECT student_id FROM enrollments WHERE score BETWEEN 80 AND 90
);

-- 14.3 子查询带 LIKE（如果支持）
-- SELECT * FROM students WHERE class_id IN (
--     SELECT class_id FROM students WHERE name LIKE 'A%'
-- );

-- 14.4 子查询带 IS NULL
SELECT * FROM students WHERE id IN (
    SELECT student_id FROM enrollments WHERE score IS NOT NULL
);

-- =====================================================
-- 测试15: 性能测试 - 相同语义的不同写法
-- =====================================================
-- 15.1 使用 IN
SELECT * FROM students WHERE id IN (SELECT student_id FROM enrollments);

-- 15.2 使用 EXISTS（语义相同，性能可能不同）
SELECT * FROM students s WHERE EXISTS (SELECT 1 FROM enrollments e WHERE e.student_id = s.id);

-- =====================================================
-- 清理测试数据
-- =====================================================
DROP TABLE IF EXISTS students;
DROP TABLE IF EXISTS courses;
DROP TABLE IF EXISTS enrollments;
DROP TABLE IF EXISTS grades;
DROP TABLE IF EXISTS empty_table;

