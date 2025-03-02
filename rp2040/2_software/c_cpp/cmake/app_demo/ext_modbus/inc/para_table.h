
#pragma once

// params table

#include "usdk.types.h"

typedef struct {
    u8* ram_address;
    u32 ram_length;
    u8* flash_address;
    u32 flash_start;
    u32 flash_offset;
    u32 flash_end;
    u16 flash_page_size;
    u16 flash_pages_count;
} para_tbl_t;

void paratbl_config(para_tbl_t* tbl, void* ram_address, u32 flash_start, u32 length);
void paratbl_save(para_tbl_t* tbl);
void paratbl_load(para_tbl_t* tbl);