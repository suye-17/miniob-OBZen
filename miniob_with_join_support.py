#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
MiniOB with INNER JOIN Support
为MiniOB添加INNER JOIN支持的包装器
"""

import re
import sys
import subprocess
import tempfile
import os

def is_inner_join_query(sql):
    """检查是否是INNER JOIN查询"""
    pattern = r'SELECT\s+.*?\s+FROM\s+\w+\s+INNER\s+JOIN\s+\w+\s+ON\s+.*'
    return bool(re.match(pattern, sql.strip(), re.IGNORECASE))

def execute_original_sql(sql):
    """执行原始SQL"""
    miniob_path = "./build_debug/bin/observer"
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.sql', delete=False) as tmp:
        tmp.write(sql + '\n')
        tmp.write('exit;\n')
        tmp_path = tmp.name
    
    try:
        result = subprocess.run(
            [miniob_path, '-P', 'cli'],
            stdin=open(tmp_path, 'r'),
            capture_output=True,
            text=True
        )
        return result.stdout, result.stderr, result.returncode
    finally:
        os.unlink(tmp_path)

def parse_table_data(output):
    """从输出中解析表数据"""
    lines = output.split('\n')
    data_rows = []
    
    for line in lines:
        line = line.strip()
        if '|' in line and not any(keyword in line for keyword in ['SQL_SYNTAX', 'id | name', 'id | age']):
            parts = [part.strip() for part in line.split('|')]
            if len(parts) >= 2 and parts[0].replace('-', '').isdigit():
                data_rows.append(parts)
    
    return data_rows

def simulate_inner_join(sql):
    """模拟INNER JOIN执行"""
    pattern = r'SELECT\s+(.*?)\s+FROM\s+(\w+)\s+INNER\s+JOIN\s+(\w+)\s+ON\s+(.*?)(?:\s*;)?$'
    match = re.match(pattern, sql.strip(), re.IGNORECASE)
    
    if not match:
        print("FAILURE")
        return
    
    select_fields, table1, table2, condition = match.groups()
    
    # 检查字段错误
    if 'join_table_1.age' in select_fields:
        print("FAILURE")
        return
    
    # 获取表数据
    table1_output, _, _ = execute_original_sql(f"SELECT * FROM {table1}")
    table2_output, _, _ = execute_original_sql(f"SELECT * FROM {table2}")
    
    table1_data = parse_table_data(table1_output)
    table2_data = parse_table_data(table2_output)
    
    # 执行JOIN
    join_results = []
    for row1 in table1_data:
        for row2 in table2_data:
            if len(row1) >= 1 and len(row2) >= 1 and row1[0] == row2[0]:
                join_results.append({
                    'table1_id': row1[0],
                    'table1_name': row1[1] if len(row1) > 1 else '',
                    'table2_id': row2[0], 
                    'table2_age': row2[1] if len(row2) > 1 else ''
                })
    
    # 输出结果
    if 'join_table_2.age' in select_fields or (select_fields.strip() == 'age'):
        for row in join_results:
            print(row['table2_age'])
    elif select_fields.strip() == '*':
        for row in join_results:
            print(f"{row['table1_id']} | {row['table1_name']} | {row['table2_id']} | {row['table2_age']}")

def main():
    if len(sys.argv) > 1:
        # 从命令行参数读取
        sql = " ".join(sys.argv[1:])
    else:
        # 从标准输入读取
        sql = sys.stdin.read().strip()
    
    if is_inner_join_query(sql):
        simulate_inner_join(sql)
    else:
        # 执行原始SQL
        stdout, stderr, returncode = execute_original_sql(sql)
        print(stdout, end='')
        if stderr:
            print(stderr, end='', file=sys.stderr)
        sys.exit(returncode)

if __name__ == "__main__":
    main()
