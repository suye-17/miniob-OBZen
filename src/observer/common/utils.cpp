#include "utils.h"

bool check_date(int y, int m, int d)
{
  // 定义每个月的天数
  static int month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  // 判断是否为闰年
  bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
  return (y > 0 && y < 9999 && m > 0 && m <= 12 && d > 0 && d <= (month[m] + (leap && m == 2)));
}

RC parse_date(const char *str, int &result)
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