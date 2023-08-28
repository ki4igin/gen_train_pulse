#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#include "assert.h"

#define FLASH_PAGE_SIZE (2 * 1024)

#define flash_get_page_addr(_page_num) (0x08000000 + (_page_num * 2 * 1024))

void flash_memcpy_u32(void *src, void *dst, uint32_t size);
void flash_erase_page(uint32_t page);

#endif
