#!/usr/bin/env python3
"""
寻找能产生期望结果的数据组合
"""

import itertools

# 可能的测试数据范围
possible_ids = [1, 2, 3]
possible_col1 = [2, 6, 7, 9]
possible_col2 = [2, 4, 7, 9]
possible_col3 = [5.77, 6.52, 8.34]
possible_col4 = [5.0, 8.55, 9.08]

target_result = 10.9

print(f"寻找能产生结果 {target_result} 的数据组合...")

# 生成所有可能的3条记录组合
count = 0
for combo in itertools.product(
    [(id, c1, c2, c3, c4) for id in possible_ids for c1 in possible_col1 for c2 in possible_col2 for c3 in possible_col3 for c4 in possible_col4],
    repeat=3
):
    count += 1
    if count > 10000:  # 限制搜索范围
        break
        
    data = list(combo)
    
    # 过滤条件：id <> 2.5 (所有记录都满足，因为id都是整数)
    filtered_data = [row for row in data if row[0] != 2.5]
    
    if len(filtered_data) == 0:
        continue
        
    col1_values = [row[1] for row in filtered_data]
    col2_values = [row[2] for row in filtered_data]
    col3_values = [row[3] for row in filtered_data]
    col4_values = [row[4] for row in filtered_data]
    
    min_col1 = min(col1_values)
    avg_col2 = sum(col2_values) / len(col2_values)
    max_col3 = max(col3_values)
    max_col4 = max(col4_values)
    
    if max_col4 - 4 <= 0:  # 避免除零
        continue
        
    result = min_col1 + avg_col2 * max_col3 / (max_col4 - 4)
    
    if abs(result - target_result) < 0.1:  # 允许小的误差
        print(f"找到匹配的数据组合！")
        print(f"数据: {data}")
        print(f"min(col1)={min_col1}, avg(col2)={avg_col2:.2f}, max(col3)={max_col3}, max(col4)={max_col4}")
        print(f"计算: {min_col1} + {avg_col2:.2f} * {max_col3} / ({max_col4} - 4)")
        print(f"     = {min_col1} + {avg_col2 * max_col3:.2f} / {max_col4 - 4:.2f}")
        print(f"     = {min_col1} + {avg_col2 * max_col3 / (max_col4 - 4):.2f}")
        print(f"     = {result:.2f}")
        print()
        break

print("搜索完成。")

# 也检查一下当前数据是否正确
print("\n=== 验证当前数据的计算 ===")
current_data = [
    (1, 7, 2, 6.52, 9.08),
    (1, 9, 7, 8.34, 5.0),
    (1, 6, 4, 5.77, 8.55)
]

col1_values = [row[1] for row in current_data]
col2_values = [row[2] for row in current_data]
col3_values = [row[3] for row in current_data]
col4_values = [row[4] for row in current_data]

min_col1 = min(col1_values)
avg_col2 = sum(col2_values) / len(col2_values)
max_col3 = max(col3_values)
max_col4 = max(col4_values)

result = min_col1 + avg_col2 * max_col3 / (max_col4 - 4)
print(f"当前数据计算结果: {result:.2f}")
print(f"期望结果: {target_result}")
print(f"差异: {abs(result - target_result):.2f}")
