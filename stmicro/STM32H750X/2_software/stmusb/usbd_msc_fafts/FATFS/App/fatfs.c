/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file   fatfs.c
 * @brief  Code for fatfs applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */

#define LOG_LOCAL_TAG   "fafts"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE

#ifdef HAL_RTC_MODULE_ENABLED
extern RTC_HandleTypeDef hrtc;
#endif

BYTE work[_MAX_SS] = {0};

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
    /* additional user code for init */

    // if (W25Qxx_Init(&w25qxx) == ERR_NONE)
    {
#if 0  // 触发格式�?
        W25Qxx_EraseSector(&w25qxx, 0);
        DelayBlockMs(1000);
#endif
    }

 //   LOGD("path %s", USERPath);
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */

#ifdef HAL_RTC_MODULE_ENABLED

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    if (HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) == HAL_OK)
    {
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        WORD date = (2000 + sDate.Year - 1980) << 9;
        date      = date | (sDate.Month << 5) | sDate.Date;

        WORD time = sTime.Hours << 11;
        time      = time | (sTime.Minutes << 5) | (sTime.Seconds > 1);
        DWORD dt  = (date << 16) | time;

        return dt;
    }

#endif

    return 0;

  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

FRESULT CreateFile(void)
{
    FIL     file;
    FRESULT f_res = FR_OK;

    BYTE au8WriteBuffer[32] = "hello world !!";
    UINT bw;

    f_res = f_open(&file, "hello.txt", FA_OPEN_ALWAYS | FA_WRITE);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_write(&file, au8WriteBuffer, sizeof(au8WriteBuffer), &bw);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_close(&file);

   // LOGD("WR \"%s\" (%d bytes)", au8WriteBuffer, bw);

    return f_res;
}

FRESULT ReadFile(void)
{
    FIL     file;
    FRESULT f_res;

    BYTE au8ReadBuffer[32] = {0};
    UINT bw;

    f_res = f_open(&file, "hello.txt", FA_READ);

    if (f_res != FR_OK)
    {
        return f_res;
    }

    f_res = f_read(&file, au8ReadBuffer, sizeof(au8ReadBuffer), &bw);

    if (f_res != FR_OK)
    {
        f_close(&file);
        return f_res;
    }

  //  LOGD("RD \"%s\"", au8ReadBuffer);

    f_res = f_close(&file);

    return f_res;
}

FRESULT InitFileSys(void)
{
    FRESULT res = FR_NO_FILESYSTEM;

    res = f_mount(&USERFatFS, USERPath, 1);

    if (res == FR_NO_FILESYSTEM)
    {
      //  LOGD("init fs");

        // No Disk file system,format disk !
        res = f_mkfs(USERPath, FM_FAT, 4096, work, sizeof work);

        if (res != FR_OK)
        {
        //    LOGW("fail to mkfs, errcode %d", res);
            goto __exit;
        }

        f_mount(NULL, USERPath, 1);  // unmount
        res = f_mount(&USERFatFS, USERPath, 1);

        if (res != FR_OK)
        {
          //  LOGW("fail to mount, errcode %d", res);
            goto __exit;
        }
    }

    if (res == FR_OK)
    {
        if (CreateFile() == FR_OK)
        {
            return ReadFile();
        }
    }

__exit:
    return res;
}

/* USER CODE END Application */
