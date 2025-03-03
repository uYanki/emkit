#ifndef __SPI_MST_H__
#define __SPI_MST_H__

#include "spidefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t SPI_Master_Init(spi_mst_t* pHandle, u32 u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, u16 u16Flags);

status_t SPI_Master_TransmitBlock(spi_mst_t* pHandle, const u8* pu8TxData, u16 u16Size);
status_t SPI_Master_ReceiveBlock(spi_mst_t* pHandle, u8* pu8RxData, u16 u16Size);

status_t SPI_Master_TransmitReceiveByte(spi_mst_t* pHandle, u8 u8TxData, u8* pu8RxData);
status_t SPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const u8* pu8TxData, u8* pu8RxData, u16 u16Size);

status_t SPI_Master_TransmitByte(spi_mst_t* pHandle, u8 u8TxData);
status_t SPI_Master_ReceiveByte(spi_mst_t* pHandle, u8* pu8RxData);

void SPI_Master_Select(spi_mst_t* pHandle);
void SPI_Master_Deselect(spi_mst_t* pHandle);

#ifdef __cplusplus
}
#endif

#endif
