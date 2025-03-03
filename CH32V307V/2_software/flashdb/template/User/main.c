/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/

#include "debug.h"
#include "flashdb.h"

/* Global typedef */

/* Global define */

/* Global Variable */
const struct fal_partition* part_dev = NULL;

static struct fdb_kvdb kvdb = {0};

static uint32_t boot_count    = 0;
static time_t   boot_time[10] = {0, 1, 2, 3};

/* default KV nodes */
static struct fdb_default_kv_node default_kv_table[] = {
    {"username",   "armink",    0                 }, /* string KV */
    {"password",   "123456",    0                 }, /* string KV */
    {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
    {"boot_time",  &boot_time,  sizeof(boot_time) }, /* int array type KV */
};

#define FDB_LOG_TAG "[sample][kvdb][basic]"

void kvdb_basic_sample()
{
    struct fdb_blob blob;

    FDB_INFO("==================== kvdb_basic_sample ====================\n");

    { /* GET the KV value */
        /* get the "boot_count" KV value */
        fdb_kv_get_blob(&kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
        /* the blob.saved.len is more than 0 when get the value successful */
        if (blob.saved.len > 0)
        {
            FDB_INFO("get the 'boot_count' value is %d\n", boot_count);
        }
        else
        {
            FDB_INFO("get the 'boot_count' failed\n");
        }
    }

    { /* CHANGE the KV value */
        /* increase the boot count */
        boot_count++;
        /* change the "boot_count" KV's value */
        fdb_kv_set_blob(&kvdb, "boot_count", fdb_blob_make(&blob, &boot_count, sizeof(boot_count)));
        FDB_INFO("set the 'boot_count' value to %d\n", boot_count);
    }

    FDB_INFO("===========================================================\n");
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    struct fdb_default_kv default_kv;

    default_kv.kvs = default_kv_table;
    default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);

    fdb_err_t result = fdb_kvdb_init(&kvdb, "env", "cfg", &default_kv, NULL);

    printf("Init result  = %d\r\n", result);

    fdb_kv_print(&kvdb);
    kvdb_basic_sample();

    for (uint16_t i = 0; i < 10; i++)
    {
        kvdb_basic_sample();
    }

    // fal_init();
    part_dev = fal_partition_find("cfg");

    fal_partition_erase(part_dev, 0x0, 256);
    uint8_t buf[256] = {0};
    int     result1  = fal_partition_read(part_dev, 0x0, buf, 256);
    printf("read result1: %X\r\n", result1);
    for (uint16_t i = 0; i < 256; i++)
    {
        printf("result1[%d] = %02X\r\n", i, buf[i]);
    }

    while (1)
    {
    }
}
