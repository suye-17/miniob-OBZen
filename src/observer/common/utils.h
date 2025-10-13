#pragma once

#include "common/sys/rc.h"
#include <string>

bool check_date(int year, int month, int day);

RC parse_date(const char *str, int &result);