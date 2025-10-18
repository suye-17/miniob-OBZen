#pragma once

#include "common/types.h"
#include "page.h"

enum class PageType : uint32_t
{
  UNKNOWN_PAGE  = 0,    // 未知
  RECORD_PAGE   = 1,    // 记录页面
  TEXT_OVERFLOW = 2     // TEXT溢出页面
};

// TEXT溢出页专用头部结构
struct OverflowPageHeader
{
  PageType page_type;         // 4字节：页面类型标识
  PageNum next_page;          // 4字节：下一个溢出页（BP_INVALID_PAGE_NUM表示结束）
  uint32_t data_length;       // 4字节：本页存储的数据长度
  uint32_t total_length;      // 4字节：整个TEXT的总长度（第一页有效）
} __attribute__((packed));      

// 溢出页头访问器（仅在已经确定是溢出页的上下文中使用）
OverflowPageHeader *overflow_header(char *page_data);
const OverflowPageHeader *overflow_header(const char *page_data);

// 获取页面类型（仅用于溢出页上下文，不做通用页类型探测）
PageType get_page_type(const char* page_data);

// 设置页面类型（仅用于溢出页上下文）  
void set_page_type(char* page_data, PageType type);

// 计算payload容量
uint32_t overflow_payload_capcity();