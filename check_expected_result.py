#!/usr/bin/env python3
"""
检查期望结果的计算
"""

# 当前数据
data = [
    (1, 7, 2, 6.52, 9.08),
    (1, 9, 7, 8.34, 5.0),
    (1, 6, 4, 5.77, 8.55)
]

print("=== 当前数据计算 ===")
col1_values = [row[1] for row in data]
col2_values = [row[2] for row in data]
col3_values = [row[3] for row in data]
col4_values = [row[4] for row in data]

min_col1 = min(col1_values)
avg_col2 = sum(col2_values) / len(col2_values)
max_col3 = max(col3_values)
max_col4 = max(col4_values)

print(f"min(col1) = {min_col1}")
print(f"avg(col2) = {avg_col2}")
print(f"max(col3) = {max_col3}")
print(f"max(col4) = {max_col4}")
print(f"max(col4) - 4 = {max_col4 - 4}")

result = min_col1 + avg_col2 * max_col3 / (max_col4 - 4)
print(f"结果 = {min_col1} + {avg_col2} * {max_col3} / {max_col4 - 4}")
print(f"     = {min_col1} + {avg_col2 * max_col3} / {max_col4 - 4}")
print(f"     = {min_col1} + {avg_col2 * max_col3 / (max_col4 - 4)}")
print(f"     = {result}")

print("\n=== 如果期望结果是10.9，反推可能的数据 ===")
# 假设其他值不变，看看需要什么样的max(col4)
target = 10.9
# target = min_col1 + avg_col2 * max_col3 / (max_col4 - 4)
# 10.9 = 6 + 4.33 * 8.34 / (max_col4 - 4)
# 4.9 = 36.14 / (max_col4 - 4)
# max_col4 - 4 = 36.14 / 4.9
# max_col4 = 36.14 / 4.9 + 4

needed_denominator = avg_col2 * max_col3 / (target - min_col1)
needed_max_col4 = needed_denominator + 4
print(f"如果结果要是{target}，需要max(col4) = {needed_max_col4}")

print("\n=== 如果期望结果是10.72，反推可能的数据 ===")
target = 10.72
needed_denominator = avg_col2 * max_col3 / (target - min_col1)
needed_max_col4 = needed_denominator + 4
print(f"如果结果要是{target}，需要max(col4) = {needed_max_col4}")
