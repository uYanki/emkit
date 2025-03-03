#ifndef __SPI_W25QXX_H__
#define __SPI_W25QXX_H__

#include "spimst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define W25QXX_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_CPOL_LOW | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW | SPI_FLAG_FAST_CLOCK_ENABLE)

/**
 * @brief W25QXX Device ID
 */
#define W25Q80_MAN_ID  0xEF13
#define W25Q16_MAN_ID  0xEF14
#define W25Q32_MAN_ID  0xEF15
#define W25Q64_MAN_ID  0xEF16
#define W25Q128_MAN_ID 0xEF17
#define W25Q256_MAN_ID 0xEF18
#define W25Q512_MAN_ID 0xEF19

/**
 * @brief W25QXX Jedec ID
 */
#define SST25VF016B_JEDEC_ID 0xBF2541
#define MX25L1606E_JEDEC_ID  0xC22015
#define W25Q64BV_JEDEC_ID    0xEF4017
#define W25Q128_JEDEC_ID     0xEF4018

/**
 * @brief address mode
 */
typedef enum {
    W25QXX_ADDRESS_MODE_3_BYTE = 0x00, /**< 3 byte mode */
    W25QXX_ADDRESS_MODE_4_BYTE = 0x01, /**< 4 byte mode */
} w25qxx_address_mode_e;

typedef struct {
    __IN spi_mst_t* hSPI;

    w25qxx_address_mode_e eAddrMode;  // 0:24bit, 1:32bit
} spi_w25qxx_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t W25Qxx_Init(spi_w25qxx_t* pHandle);
status_t W25Qxx_ReadDeviceID(spi_w25qxx_t* pHandle, __OUT uint8_t ID[2]);
status_t W25Qxx_ReadUniqueID(spi_w25qxx_t* pHandle, __OUT uint8_t ID[8]);
status_t W25Qxx_ReadJedecID(spi_w25qxx_t* pHandle, __OUT uint8_t ID[3]);
status_t W25Qxx_PowerDown(spi_w25qxx_t* pHandle);
status_t W25Qxx_WakeUp(spi_w25qxx_t* pHandle);
status_t W25Qxx_ReadData(spi_w25qxx_t* pHandle, uint32_t u32ReadAddr, uint32_t u32Size, uint8_t* pu8Data);
status_t W25Qxx_WriteData(spi_w25qxx_t* pHandle, uint32_t u32WriteAddr, uint32_t u32Size, const uint8_t* cpu8Data);
status_t W25Qxx_EraseSector(spi_w25qxx_t* pHandle, uint32_t u32Address);
status_t W25Qxx_EraseChip(spi_w25qxx_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void W25Qxx_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
