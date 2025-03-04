#include <string.h>  // memcpy

#include <pico/stdlib.h>
#include <hardware/flash.h>
#include <hardware/sync.h>

#include "para_table.h"

void paratbl_config(para_tbl_t* tbl, void* ram_address, u32 flash_start, u32 length)
{
    tbl->ram_address   = (u8*)ram_address;
    tbl->ram_length    = length;
    tbl->flash_start   = flash_start;
    tbl->flash_offset  = (256 * 1024u);
    tbl->flash_address = (u8*)XIP_BASE + tbl->flash_start + tbl->flash_offset;

    tbl->flash_page_size   = FLASH_PAGE_SIZE;
    tbl->flash_pages_count = tbl->ram_length / tbl->flash_page_size;
    if (tbl->ram_length % tbl->flash_page_size > 0) {
        ++tbl->flash_pages_count;
    }

    tbl->flash_end = tbl->flash_start + tbl->flash_pages_count * tbl->flash_page_size;
}

void paratbl_save(para_tbl_t* tbl)
{
    // We're going to erase and reprogram a region 256k from the start of flash.
    // Once done, we can access this at XIP_BASE + 256k.

    uint8_t* ram_address  = tbl->ram_address;
    uint32_t flash_offset = tbl->flash_start + tbl->flash_offset;
    uint32_t page_size    = tbl->flash_page_size;
    uint32_t pages_count  = tbl->flash_pages_count;

    uint32_t ints = save_and_disable_interrupts();

    while (pages_count--) {
        flash_range_erase(flash_offset, page_size);
        flash_range_program(flash_offset, ram_address, page_size);
        flash_offset += page_size;
        ram_address += page_size;
    }

    restore_interrupts(ints);
}

void paratbl_load(para_tbl_t* tbl)
{
    memcpy(tbl->ram_address, tbl->flash_address, tbl->ram_length);
}
