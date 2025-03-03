#ifndef __SPI_MFRC522_H__
#define __SPI_MFRC522_H__

#include "spimst.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define RC522_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPOL_LOW | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW)

typedef struct {
    __IN spi_mst_t*  hSPI;
    __IN const pin_t RST;
} spi_rc522_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t RC522_Init(spi_rc522_t* pHandle);

void     RC522_AntennaOn(spi_rc522_t* pHandle);   // 开启天线
void     RC522_AntennaOff(spi_rc522_t* pHandle);  // 关闭天线
status_t RC522_Halt(spi_rc522_t* pHandle);        // 休眠

status_t RC522_Request(spi_rc522_t* pHandle, uint8_t u8ReqCode, uint8_t* pu8TagType);                                                // 寻卡
status_t RC522_Anticoll(spi_rc522_t* pHandle, uint8_t* pu8SnrNum);                                                                   // 防冲撞
status_t RC522_Select(spi_rc522_t* pHandle, uint8_t* pu8SnrNum);                                                                     // 选卡
status_t RC522_AuthState(spi_rc522_t* pHandle, uint8_t u8AuthMode, uint8_t u8BlockAddr, uint8_t* pu8Sectorkey, uint8_t* pu8SnrNum);  // 密码校验

status_t RC522_Read(spi_rc522_t* pHandle, uint8_t u8BlockAddr, uint8_t* pu8Data);   // 读数据
status_t RC522_Write(spi_rc522_t* pHandle, uint8_t u8BlockAddr, uint8_t* pu8Data);  // 写数据

bool RC522_IsDataBlock(spi_rc522_t* pHandle, uint8_t u8BlockAddr);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void RC522_Test(void);
#endif

#endif
