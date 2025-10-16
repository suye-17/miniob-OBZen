#!/bin/bash
# JOIN字段验证问题调试脚本

echo "========================================="
echo "JOIN字段验证调试脚本"
echo "========================================="
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 1. 检查关键代码是否存在
echo "步骤1: 检查关键代码..."
echo "----------------------------------------"

LOGICAL_PLAN_FILE="src/observer/sql/optimizer/logical_plan_generator.cpp"

if grep -n "bind_expression_fields(condition_copy, all_tables)" "$LOGICAL_PLAN_FILE" > /dev/null; then
    LINE_NUM=$(grep -n "bind_expression_fields(condition_copy, all_tables)" "$LOGICAL_PLAN_FILE" | cut -d: -f1)
    echo -e "${GREEN}✓ 找到字段绑定调用（第${LINE_NUM}行）${NC}"
    
    # 检查错误处理
    if grep -A3 "bind_expression_fields(condition_copy, all_tables)" "$LOGICAL_PLAN_FILE" | grep "return rc" > /dev/null; then
        echo -e "${GREEN}✓ 错误处理代码存在${NC}"
    else
        echo -e "${RED}✗ 警告：错误处理代码可能缺失！${NC}"
    fi
else
    echo -e "${RED}✗ 致命错误：未找到bind_expression_fields调用！${NC}"
    echo -e "${RED}  您的代码版本可能缺少关键的字段验证逻辑！${NC}"
    exit 1
fi

# 2. 检查bind_unbound_field函数
echo ""
echo "步骤2: 检查字段绑定函数..."
echo "----------------------------------------"

if grep -A5 'const FieldMeta \*field_meta = target_table->table_meta().field(field_name);' "$LOGICAL_PLAN_FILE" | grep -q "SCHEMA_FIELD_NOT_EXIST"; then
    echo -e "${GREEN}✓ 字段不存在检测代码正常${NC}"
else
    echo -e "${YELLOW}⚠ 警告：字段不存在检测代码可能有问题${NC}"
fi

# 3. 清理并准备测试环境
echo ""
echo "步骤3: 准备测试环境..."
echo "----------------------------------------"

killall observer 2>/dev/null
rm -rf /tmp/miniob_debug_test
mkdir -p /tmp/miniob_debug_test
echo -e "${GREEN}✓ 环境已清理${NC}"

# 4. 重新编译（使用debug模式）
echo ""
echo "步骤4: 编译项目（debug模式）..."
echo "----------------------------------------"

if bash build.sh > /tmp/build.log 2>&1; then
    echo -e "${GREEN}✓ 编译成功${NC}"
else
    echo -e "${RED}✗ 编译失败，请检查 /tmp/build.log${NC}"
    exit 1
fi

# 5. 启动observer（带详细日志）
echo ""
echo "步骤5: 启动Observer（带详细日志）..."
echo "----------------------------------------"

cat > /tmp/observer_debug.ini << 'EOF'
[LOG]
LOG_FILE_NAME=observer_debug.log
LOG_FILE_PATH=/tmp/
LOG_CONSOLE_LEVEL=1
LOG_FILE_LEVEL=5

[NET]
PORT=18765
LISTEN_ADDR=127.0.0.1

[DB]
DATA_DIR=/tmp/miniob_debug_test
EOF

./build/bin/observer -f /tmp/observer_debug.ini > /tmp/observer_stdout.log 2>&1 &
OBS_PID=$!
echo "Observer PID: $OBS_PID"
sleep 3

if ps -p $OBS_PID > /dev/null; then
    echo -e "${GREEN}✓ Observer启动成功${NC}"
else
    echo -e "${RED}✗ Observer启动失败${NC}"
    cat /tmp/observer_stdout.log
    exit 1
fi

# 6. 运行测试
echo ""
echo "步骤6: 运行测试..."
echo "========================================="

cat << 'SQLEOF' | ./build/bin/obclient -p 18765 > /tmp/test_result.txt 2>&1
CREATE TABLE join_table_1(id int, name char(20));
CREATE TABLE join_table_2(id int, age int);
INSERT INTO join_table_1 VALUES (1, 'Alice'), (2, 'Bob');
INSERT INTO join_table_2 VALUES (1, 25), (2, 30);
Select * from join_table_1 inner join join_table_2 on join_table_1.id=join_table_2.id and join_table_2.level>36;
exit
SQLEOF

echo ""
echo "测试输出："
echo "----------------------------------------"
cat /tmp/test_result.txt | tail -10

# 提取最后一个查询的结果
LAST_RESULT=$(cat /tmp/test_result.txt | grep -E "^(FAILURE|SUCCESS|id \|)" | tail -1)

echo ""
echo "关键结果："
echo "----------------------------------------"
if echo "$LAST_RESULT" | grep -q "FAILURE"; then
    echo -e "${GREEN}✓ 测试返回 FAILURE（正确！）${NC}"
    echo -e "${GREEN}  字段验证机制工作正常${NC}"
elif echo "$LAST_RESULT" | grep -q "id |"; then
    echo -e "${RED}✗ 测试返回表头（错误！）${NC}"
    echo -e "${RED}  查询执行了但不应该执行${NC}"
    echo -e "${RED}  实际输出: $LAST_RESULT${NC}"
    
    # 检查日志中的错误信息
    echo ""
    echo "检查日志中的字段验证信息..."
    echo "----------------------------------------"
    if grep -i "field not found" /tmp/observer_debug.log | tail -5; then
        echo -e "${YELLOW}⚠ 日志中有字段未找到的警告，但查询仍然执行了！${NC}"
    else
        echo -e "${RED}✗ 日志中没有字段验证的警告信息${NC}"
        echo -e "${RED}  这表明字段验证逻辑可能被跳过了${NC}"
    fi
else
    echo -e "${YELLOW}⚠ 未知结果: $LAST_RESULT${NC}"
fi

# 7. 查看详细日志
echo ""
echo "步骤7: 检查详细日志..."
echo "========================================="

echo ""
echo "查找字段绑定相关日志..."
echo "----------------------------------------"
grep -i "bind\|field\|join" /tmp/observer_debug.log | grep -i "warn\|error" | tail -10

echo ""
echo "查找JOIN条件相关日志..."
echo "----------------------------------------"
grep -i "join condition" /tmp/observer_debug.log | tail -5

# 8. 清理
echo ""
echo "步骤8: 清理测试环境..."
echo "----------------------------------------"
kill $OBS_PID 2>/dev/null
sleep 1
echo -e "${GREEN}✓ Observer已停止${NC}"

# 9. 总结
echo ""
echo "========================================="
echo "调试总结"
echo "========================================="
echo ""
echo "日志文件位置："
echo "  - Observer日志: /tmp/observer_debug.log"
echo "  - 测试结果: /tmp/test_result.txt"
echo "  - Observer输出: /tmp/observer_stdout.log"
echo ""

if echo "$LAST_RESULT" | grep -q "FAILURE"; then
    echo -e "${GREEN}结论：代码工作正常，字段验证机制正确！${NC}"
    echo ""
    echo "如果您在测试中看到表头而不是FAILURE，可能的原因："
    echo "  1. 测试框架的输出格式问题"
    echo "  2. 数据库中有缓存或旧数据"
    echo "  3. 使用了不同的代码版本"
    echo ""
    echo "建议："
    echo "  - 查看日志文件：less /tmp/observer_debug.log"
    echo "  - 重新运行测试：python3 test/case/miniob_test.py --test-case=join-field-validation"
else
    echo -e "${RED}结论：检测到问题！查询不应该执行但执行了！${NC}"
    echo ""
    echo "请检查以下内容："
    echo "  1. 代码版本是否完整（特别是第308行的bind_expression_fields调用）"
    echo "  2. 查看详细日志：/tmp/observer_debug.log"
    echo "  3. 确认没有使用cascade优化器（可能跳过了验证）"
    echo ""
    echo "需要的信息（请提供给开发者）："
    echo "  - git log -1"
    echo "  - grep -n 'bind_expression_fields(condition_copy' src/observer/sql/optimizer/logical_plan_generator.cpp"
    echo "  - cat /tmp/observer_debug.log | grep -i 'field not found'"
fi

echo ""
echo "========================================="
echo "调试脚本完成"
echo "========================================="

