#include "utils.h"
#include <vector>
#include <string>
#include <sstream>
#include <cstring>

bool check_date(int y, int m, int d)
{
    // 定义每个月的天数
    static int month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // 判断是否为闰年
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    return (y > 0 && y < 9999 && m > 0 && m <= 12 && d > 0 && d <= (month[m] + (leap && m == 2)));
}

RC parse_date(const char* str, int &result)
{
    int y, m, d;
    if (sscanf(str, "%d-%d-%d", &y, &m, &d) != 3) {
        return RC::INVALID_ARGUMENT;
    }
    if (!check_date(y, m, d)) {
        return RC::INVALID_ARGUMENT;
    }
    result = y * 10000 + m * 100 + d;
    return RC::SUCCESS;
}

RC parse_vector_literal(const char* str, std::vector<float> &result)
{
    // 清空结果向量
    result.clear();
    
    // 输入验证
    if (!str || strlen(str) == 0) {
        return RC::INVALID_ARGUMENT;
    }
    
    std::string input(str);
    
    // 去除首尾空格
    size_t start = input.find_first_not_of(" \t");
    size_t end = input.find_last_not_of(" \t");
    if (start == std::string::npos) {
        return RC::INVALID_ARGUMENT;  // 全是空格
    }
    input = input.substr(start, end - start + 1);
    
    // 检查方括号格式
    if (input.length() < 2 || input[0] != '[' || input.back() != ']') {
        return RC::SYNTAX_ERROR;
    }
    
    // 提取方括号内容
    std::string content = input.substr(1, input.length() - 2);
    
    // 处理空向量 "[]"
    if (content.empty() || content.find_first_not_of(" \t") == std::string::npos) {
        return RC::SUCCESS;  // 空向量是有效的
    }
    
    // 使用stringstream解析逗号分隔的值
    std::stringstream ss(content);
    std::string item;
    
    while (getline(ss, item, ',')) {
        // 去除item的首尾空格
        size_t item_start = item.find_first_not_of(" \t");
        size_t item_end = item.find_last_not_of(" \t");
        
        if (item_start == std::string::npos) {
            // 空项目（如连续逗号或末尾逗号）
            return RC::SYNTAX_ERROR;
        }
        
        std::string trimmed = item.substr(item_start, item_end - item_start + 1);
        
        // 转换为浮点数
        char* endptr;
        float value = strtof(trimmed.c_str(), &endptr);
        
        // 检查转换是否成功（endptr应该指向字符串末尾）
        if (*endptr != '\0') {
            result.clear();  // 失败时清空结果
            return RC::SYNTAX_ERROR;
        }
        
        result.push_back(value);
    }
    
    return RC::SUCCESS;
}