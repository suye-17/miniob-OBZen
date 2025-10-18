#include "page_type.h"
#include "page.h"

OverflowPageHeader *overflow_header(char *page_data)
{
  if (page_data == nullptr) {
    return nullptr;
  }
  return reinterpret_cast<OverflowPageHeader *>(page_data);
}

const OverflowPageHeader *overflow_header(const char *page_data)
{
  if (page_data == nullptr) {
    return nullptr;
  }
  return reinterpret_cast<const OverflowPageHeader *>(page_data);
}

PageType get_page_type(const char* page_data)
{
  if (page_data == nullptr) {
    return PageType::UNKNOWN_PAGE;
  }
  const OverflowPageHeader* header = overflow_header(page_data);
  return header != nullptr ? header->page_type : PageType::UNKNOWN_PAGE;
}

void set_page_type(char* page_data, PageType type)
{
  if (page_data == nullptr) {
    return;  
  }
  OverflowPageHeader* header = overflow_header(page_data);
  if (header != nullptr) {
    header->page_type = type;
  }
}

uint32_t overflow_payload_capacity()
{
  return BP_PAGE_DATA_SIZE - sizeof(OverflowPageHeader);
}
