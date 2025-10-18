#pragma once

#include "common/sys/rc.h"
#include <string>
#include <vector>

bool check_date(int year, int month, int day);

RC parse_date(const char* str, int &result);

RC parse_vector_literal(const char* str, std::vector<float> &result);

RC validate_text_storage(size_t data_length, size_t &inline_length, int &overflow_pages);

