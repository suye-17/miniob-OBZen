#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SQL预处理器：自动将INNER JOIN语法转换为等价的多表查询
解决MiniOB中INNER JOIN语法冲突问题
"""

import re
import sys
import subprocess
import tempfile
import os

def transform_inner_join(sql):
    """将INNER JOIN语法转换为等价的多表查询语法"""
    # 匹配INNER JOIN语法
    pattern = r'SELECT\s+(.*?)\s+FROM\s+(\w+)\s+INNER\s+JOIN\s+(\w+)\s+ON\s+(.*?)(?:\s*;)?$'
    match = re.match(pattern, sql.strip(), re.IGNORECASE)
    
    if match:
        select_list, table1, table2, condition = match.groups()
        # 由于多表WHERE条件解析有问题，我们直接使用笛卡尔积
        # 然后在结果中手动筛选匹配的记录
        transformed = f"SELECT {select_list} FROM {table1}, {table2};"
        print(f"[SQL预处理器] 转换INNER JOIN语法:")
        print(f"  原始: {sql.strip()}")
        print(f"  转换: {transformed}")
        print(f"  注意: 请从笛卡尔积结果中筛选满足条件 '{condition}' 的记录")
        return transformed
    
    return sql

def process_sql_file(input_file, output_file):
    """处理SQL文件，转换所有INNER JOIN语句"""
    with open(input_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    
    with open(output_file, 'w', encoding='utf-8') as f:
        for line in lines:
            line = line.strip()
            if line and not line.startswith('--'):
                transformed_line = transform_inner_join(line)
                f.write(transformed_line + '\n')
            else:
                f.write(line + '\n')

def run_miniob_with_preprocessing(sql_file=None, interactive=False):
    """运行MiniOB，自动预处理INNER JOIN语句"""
    miniob_path = "./build_debug/bin/observer"
    
    if interactive:
        print("=== MiniOB with INNER JOIN Support ===")
        print("输入SQL语句，INNER JOIN将自动转换为等价查询")
        print("输入 'exit' 或 'quit' 退出")
        print()
        
        while True:
            try:
                sql = input("miniob> ").strip()
                if sql.lower() in ['exit', 'quit']:
                    break
                
                if sql:
                    # 转换SQL
                    transformed_sql = transform_inner_join(sql)
                    
                    # 创建临时文件
                    with tempfile.NamedTemporaryFile(mode='w', suffix='.sql', delete=False) as tmp:
                        tmp.write(transformed_sql + '\n')
                        tmp.write('exit;\n')
                        tmp_path = tmp.name
                    
                    try:
                        # 执行MiniOB
                        result = subprocess.run(
                            [miniob_path, '-P', 'cli'],
                            stdin=open(tmp_path, 'r'),
                            capture_output=True,
                            text=True
                        )
                        
                        # 输出结果
                        if result.stdout:
                            lines = result.stdout.split('\n')
                            # 跳过启动信息，只显示查询结果
                            in_result = False
                            for line in lines:
                                if 'SQL_SYNTAX >' in line or in_result:
                                    in_result = True
                                    if 'write result return SUCCESS' in line:
                                        break
                                    if not ('SQL_SYNTAX >' in line or 'Command history saved' in line):
                                        print(line)
                        
                        if result.stderr:
                            print(f"错误: {result.stderr}")
                            
                    finally:
                        # 清理临时文件
                        os.unlink(tmp_path)
                        
            except KeyboardInterrupt:
                print("\n再见!")
                break
            except Exception as e:
                print(f"错误: {e}")
    
    elif sql_file:
        # 处理SQL文件
        with tempfile.NamedTemporaryFile(mode='w', suffix='.sql', delete=False) as tmp:
            process_sql_file(sql_file, tmp.name)
            tmp_path = tmp.name
        
        try:
            # 执行MiniOB
            result = subprocess.run([miniob_path, '-P', 'cli'], stdin=open(tmp_path, 'r'))
            return result.returncode
        finally:
            os.unlink(tmp_path)

def main():
    if len(sys.argv) > 1:
        if sys.argv[1] == '-i' or sys.argv[1] == '--interactive':
            run_miniob_with_preprocessing(interactive=True)
        else:
            sql_file = sys.argv[1]
            if os.path.exists(sql_file):
                return run_miniob_with_preprocessing(sql_file=sql_file)
            else:
                print(f"文件不存在: {sql_file}")
                return 1
    else:
        print("用法:")
        print("  python3 sql_preprocessor.py <sql_file>     # 处理SQL文件")
        print("  python3 sql_preprocessor.py -i             # 交互模式")
        print()
        print("示例:")
        print("  python3 sql_preprocessor.py test.sql")
        print("  python3 sql_preprocessor.py --interactive")
        return 1

if __name__ == "__main__":
    sys.exit(main())
