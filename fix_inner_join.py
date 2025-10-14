#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
修复INNER JOIN功能的最终解决方案
通过直接修改语法文件来解决冲突
"""

import re
import sys
import subprocess
import tempfile
import os

def backup_yacc_file():
    """备份原始yacc文件"""
    subprocess.run(['cp', '/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y', 
                   '/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y.backup'])

def create_simple_join_grammar():
    """创建一个简化的JOIN语法，避免冲突"""
    
    # 读取当前的yacc文件
    with open('/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y', 'r') as f:
        content = f.read()
    
    # 移除现有的join_stmt规则
    content = re.sub(r'join_stmt:.*?;', '', content, flags=re.DOTALL)
    
    # 在select_stmt规则中添加简单的INNER JOIN支持
    select_stmt_pattern = r'(select_stmt:.*?FROM rel_list where group_by having\s*\{[^}]*\})'
    
    new_select_rule = '''select_stmt:        /*  select 语句的语法解析树*/
    SELECT expression_list FROM rel_list where group_by having
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.expressions.swap(*$2);
        delete $2;
      }

      if ($4 != nullptr) {
        $$->selection.relations.swap(*$4);
        delete $4;
      }

      std::reverse($$->selection.relations.begin(), $$->selection.relations.end());

      if ($5 != nullptr) {
        $$->selection.conditions.swap(*$5);
        delete $5;
      }
      if ($6 != nullptr) {
        $$->selection.group_by.swap(*$6);
        delete $6;
      }
      if ($7 != nullptr) {
        $$->selection.having_conditions.swap(*$7);
        delete $7;
      }
    }
    | SELECT expression_list FROM relation INNER JOIN relation ON condition
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.expressions.swap(*$2);
        delete $2;
      }

      // 添加主表
      $$->selection.relations.push_back($4);
      
      // 添加JOIN表
      JoinSqlNode join_node;
      join_node.type = JoinType::INNER_JOIN;
      join_node.relation = $7;
      if ($9 != nullptr) {
        join_node.conditions.push_back(*$9);
        delete $9;
      }
      $$->selection.joins.push_back(join_node);
    }'''
    
    # 替换select_stmt规则
    content = re.sub(select_stmt_pattern, new_select_rule, content, flags=re.DOTALL)
    
    # 移除join_stmt从command_wrapper
    content = re.sub(r'\|\s*join_stmt', '', content)
    
    # 移除join_stmt的类型声明
    content = re.sub(r'%type\s*<sql_node>\s*join_stmt', '', content)
    
    # 写回文件
    with open('/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y', 'w') as f:
        f.write(content)

def rebuild_and_test():
    """重新编译并测试"""
    print("重新编译...")
    result = subprocess.run(['cmake', '--build', 'build_debug'], 
                          cwd='/home/simpur/miniob-OBZen',
                          capture_output=True, text=True)
    
    if result.returncode != 0:
        print("编译失败:")
        print(result.stderr)
        return False
    
    print("编译成功！测试INNER JOIN...")
    
    # 测试INNER JOIN
    test_sql = """
SELECT * FROM join_table_1 INNER JOIN join_table_2 ON join_table_1.id = join_table_2.id;
exit;
"""
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.sql', delete=False) as tmp:
        tmp.write(test_sql)
        tmp_path = tmp.name
    
    try:
        result = subprocess.run(['/home/simpur/miniob-OBZen/build_debug/bin/observer', '-P', 'cli'],
                              stdin=open(tmp_path, 'r'),
                              capture_output=True, text=True,
                              cwd='/home/simpur/miniob-OBZen')
        
        print("测试结果:")
        print(result.stdout)
        
        if "Failed to parse sql" in result.stdout:
            print("❌ INNER JOIN仍然解析失败")
            return False
        else:
            print("✅ INNER JOIN解析成功！")
            return True
            
    finally:
        os.unlink(tmp_path)

def main():
    print("=== 修复INNER JOIN功能 ===")
    
    # 备份原始文件
    print("1. 备份原始yacc文件...")
    backup_yacc_file()
    
    # 创建简化的JOIN语法
    print("2. 修改语法规则...")
    create_simple_join_grammar()
    
    # 重新编译和测试
    print("3. 重新编译和测试...")
    success = rebuild_and_test()
    
    if success:
        print("\n🎉 INNER JOIN功能修复成功！")
    else:
        print("\n❌ 修复失败，恢复原始文件...")
        subprocess.run(['cp', '/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y.backup',
                       '/home/simpur/miniob-OBZen/src/observer/sql/parser/yacc_sql.y'])

if __name__ == "__main__":
    main()
