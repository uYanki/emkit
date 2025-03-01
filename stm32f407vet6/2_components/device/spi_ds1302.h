#ifndef __SPI_DS1302_H__
#define __SPI_DS1302_H__

/**
 * @brief RTC: real-time clock
 *
 * @note
 *  ① 若出现秒卡在85，其他的卡在165的现象，供电接5V
 *  ② 外接的 32.768K 晶振要用内部电容为 6PF 的。
 *
 * @note 引脚 (可用SPI接口)
 *  - CE、RST: 使能引脚，读写时需保持高电平
 *  - I/O、DAT：数据线
 *  - SCLK、CLK：串行时钟线
 */

#include "spimst.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define DS1302_SPI_TIMING (SPI_FLAG_3WIRE | SPI_FLAG_CPOL_LOW | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_LSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_HIGH)

typedef struct {
    uint8_t u8Year;
    uint8_t u8YearDay;
    uint8_t u8Month;
    uint8_t u8MonthDay;
    uint8_t u8WeekDay;
    uint8_t u8Hour;
    uint8_t u8Minute;
    uint8_t u8Second;
} rtc_time_t;

typedef struct {
    spi_mst_t* hSPI;
} spi_ds1302_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void DS1302_Init(spi_ds1302_t* pHandle);

void DS1302_StartTimer(spi_ds1302_t* pHandle);
void DS1302_StopTimer(spi_ds1302_t* pHandle);

void DS1302_GetTime(spi_ds1302_t* pHandle, rtc_time_t* pTime);
void DS1302_SetTime(spi_ds1302_t* pHandle, rtc_time_t* pTime);

/**
 * @brief 31 bytes RAM for user (掉电丢失)
 * @param[in] u8MemAddr 0~31
 */
uint8_t DS1302_ReadRam(spi_ds1302_t* pHandle, uint8_t u8MemAddr);
bool    DS1302_WriteRam(spi_ds1302_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data);
void    DS1302_ClearRam(spi_ds1302_t* pHandle);
void    DS1302_Hexdump(spi_ds1302_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void DS1302_Test(void);
#endif

#endif
