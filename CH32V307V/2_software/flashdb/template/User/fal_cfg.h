/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

// #include <rtconfig.h>
// #include <board.h>

#define FAL_PART_HAS_TABLE_CFG

#define FDB_USING_NATIVE_ASSERT

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev ch32v3_onchip_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &ch32v3_onchip_flash,                                           \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD,       "app",     "ch32_onchip",         0,   192*1024, 0}, \
    {FAL_PART_MAGIC_WORD,       "cfg",     "ch32_onchip",   192*1024,  4*1024, 0}, \
    {FAL_PART_MAGIC_WORD,       "bak",     "ch32_onchip",   196*1024,  284*1024, 0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
