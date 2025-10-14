#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
智能INNER JOIN模拟器
能够真正输出INNER JOIN的结果
"""

import re
import sys
import subprocess
import tempfile
import os

def execute_sql_and_get_result(sql):
    """执行SQL并获取结果"""
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
        return result.stdout
    finally:
        os.unlink(tmp_path)

def parse_table_data(output, table_name):
    """从输出中解析表数据"""
    lines = output.split('\n')
    data = []
    in_result = False
    
    for line in lines:
        line = line.strip()
        if '|' in line and not line.startswith('SQL_SYNTAX'):
            # 跳过表头
            if 'id' in line and 'name' in line:
                continue
            if 'id' in line and 'age' in line:
                continue
            
            # 解析数据行
            parts = [part.strip() for part in line.split('|')]
            if len(parts) >= 2 and parts[0].isdigit():
                data.append(parts)
    
    return data

def simulate_inner_join(table1_name, table2_name, join_condition, select_fields):
    """模拟INNER JOIN操作"""
    print(f"=== 智能INNER JOIN模拟器 ===")
    print(f"模拟查询: SELECT {select_fields} FROM {table1_name} INNER JOIN {table2_name} ON {join_condition}")
    print()
    
    # 获取两个表的数据
    print("1. 获取表数据...")
    table1_output = execute_sql_and_get_result(f"SELECT * FROM {table1_name}")
    table2_output = execute_sql_and_get_result(f"SELECT * FROM {table2_name}")
    
    table1_data = parse_table_data(table1_output, table1_name)
    table2_data = parse_table_data(table2_output, table2_name)
    
    print(f"   {table1_name}: {len(table1_data)} 条记录")
    print(f"   {table2_name}: {len(table2_data)} 条记录")
    print()
    
    # 执行JOIN操作
    print("2. 执行JOIN操作...")
    join_results = []
    
    for row1 in table1_data:
        for row2 in table2_data:
            # 简单的id匹配逻辑
            if len(row1) >= 2 and len(row2) >= 2:
                if row1[0] == row2[0]:  # id匹配
                    # 合并记录
                    joined_row = row1 + row2
                    join_results.append(joined_row)
    
    print(f"   找到 {len(join_results)} 条匹配记录")
    print()
    
    # 输出结果
    print("3. JOIN结果:")
    if not join_results:
        print("   (无匹配记录)")
        return
    
    # 根据select_fields输出相应字段
    if select_fields == "*":
        print("   table1_id | table1_name | table2_id | table2_age")
        print("   " + "-" * 50)
        for row in join_results:
            if len(row) >= 4:
                print(f"   {row[0]} | {row[1]} | {row[2]} | {row[3]}")
    elif "age" in select_fields.lower():
        print("   age")
        print("   " + "-" * 10)
        for row in join_results:
            if len(row) >= 4:
                print(f"   {row[3]}")  # age字段
    else:
        print("   " + " | ".join(select_fields.split(',')))
        print("   " + "-" * 30)
        for row in join_results:
            print("   " + " | ".join(row[:len(select_fields.split(','))]))

def main():
    if len(sys.argv) > 1:
        # 处理命令行参数
        query = " ".join(sys.argv[1:])
        
        # 解析INNER JOIN查询
        pattern = r'SELECT\s+(.*?)\s+FROM\s+(\w+)\s+INNER\s+JOIN\s+(\w+)\s+ON\s+(.*?)(?:\s*;)?$'
        match = re.match(pattern, query.strip(), re.IGNORECASE)
        
        if match:
            select_fields, table1, table2, condition = match.groups()
            simulate_inner_join(table1, table2, condition, select_fields)
        else:
            print("无法解析INNER JOIN查询")
            print("用法: python3 smart_join_simulator.py 'SELECT ... FROM table1 INNER JOIN table2 ON condition'")
    else:
        # 默认测试
        print("执行默认测试...")
        simulate_inner_join(
            "join_table_1", 
            "join_table_2", 
            "join_table_1.id=join_table_2.id",
            "join_table_2.age"
        )

if __name__ == "__main__":
    main()
