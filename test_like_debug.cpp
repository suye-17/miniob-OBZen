#include <iostream>
#include <string>
#include <cstring>

// 复制LIKE匹配算法进行测试
static bool match_like_pattern(const char *text, const char *pattern)
{
  const char *t = text;
  const char *p = pattern;
  
  while (*p) {
    if (*p == '%') {
      p++;
      if (*p == '\0') return true;
      
      while (*t) {
        if (match_like_pattern(t, p)) return true;
        t++;
      }
      return false;
    } 
    else if (*p == '_') {
      if (*t == '\0') return false;
      p++; t++;
    } 
    else {
      if (*t != *p) return false;
      p++; t++;
    }
  }
  
  return *t == '\0';
}

static bool do_like_match(const char *text, const char *pattern)
{
  if (text == nullptr || pattern == nullptr) {
    return false;
  }
  return match_like_pattern(text, pattern);
}

int main() {
    // 测试基本匹配
    std::cout << "Testing basic LIKE matching:" << std::endl;
    
    // 测试数据，模拟数据库中的情况
    char test_data[20];
    memset(test_data, 0, 20);
    strcpy(test_data, "coconut");
    
    std::cout << "Test data: '" << test_data << "'" << std::endl;
    std::cout << "Test data length: " << strlen(test_data) << std::endl;
    std::cout << "Test data as string: '" << std::string(test_data) << "'" << std::endl;
    
    // 测试LIKE匹配
    std::cout << "coconut LIKE 'c%': " << do_like_match(test_data, "c%") << std::endl;
    std::cout << "coconut LIKE 'b%': " << do_like_match(test_data, "b%") << std::endl;
    
    // 测试带填充的情况
    char padded_data[20];
    memset(padded_data, ' ', 19);  // 用空格填充
    padded_data[19] = '\0';
    memcpy(padded_data, "coconut", 7);
    
    std::cout << "\nTesting with padded data:" << std::endl;
    std::cout << "Padded data: '" << padded_data << "'" << std::endl;
    std::cout << "Padded data length: " << strlen(padded_data) << std::endl;
    std::cout << "coconut (padded) LIKE 'c%': " << do_like_match(padded_data, "c%") << std::endl;
    
    return 0;
}


