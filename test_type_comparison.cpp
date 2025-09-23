#include "src/observer/common/value.h"
#include "src/observer/common/type/char_type.h"
#include "src/observer/common/type/integer_type.h"
#include "src/observer/common/type/float_type.h"
#include <iostream>

int main() {
    std::cout << "测试字符串和数字比较功能..." << std::endl;
    
    // 创建测试值
    Value str_val("123", 4);  // 字符串 "123"
    Value int_val(456);       // 整数 456
    Value str_num("16");      // 字符串 "16"
    Value int_20(20);         // 整数 20
    
    // 创建类型实例
    CharType char_type;
    IntegerType int_type;
    
    // 测试1: 字符串 "123" 与整数 456 比较
    std::cout << "\n测试1: 字符串 '123' 与整数 456 比较" << std::endl;
    int result1 = char_type.compare(str_val, int_val);
    std::cout << "结果: " << result1 << " (应该是负数，因为 123 < 456)" << std::endl;
    
    // 测试2: 整数 20 与字符串 "16" 比较
    std::cout << "\n测试2: 整数 20 与字符串 '16' 比较" << std::endl;
    int result2 = int_type.compare(int_20, str_num);
    std::cout << "结果: " << result2 << " (应该是正数，因为 20 > 16)" << std::endl;
    
    // 测试3: 字符串 "16" 与整数 20 比较  
    std::cout << "\n测试3: 字符串 '16' 与整数 20 比较" << std::endl;
    int result3 = char_type.compare(str_num, int_20);
    std::cout << "结果: " << result3 << " (应该是负数，因为 16 < 20)" << std::endl;
    
    // 测试4: 测试字符串转换
    std::cout << "\n测试4: 字符串转换测试" << std::endl;
    std::cout << "字符串 '123' 转为整数: " << str_val.get_int() << std::endl;
    std::cout << "字符串 '16' 转为整数: " << str_num.get_int() << std::endl;
    
    std::cout << "\n所有测试完成！" << std::endl;
    return 0;
}
