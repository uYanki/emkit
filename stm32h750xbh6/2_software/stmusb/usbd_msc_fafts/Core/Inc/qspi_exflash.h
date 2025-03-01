#ifndef __QSPI_W25QXX_H__
#define __QSPI_W25QXX_H__

#include "sdkinc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_W25Qxx_FLASH_SIZE               W25Q64JV_FLASH_SIZE
#define CONFIG_W25Qxx_SECTOR_SIZE              W25Q64JV_SECTOR_SIZE
#define CONFIG_W25Qxx_SUBSECTOR_SIZE           W25Q64JV_SUBSECTOR_SIZE
#define CONFIG_W25Qxx_PAGE_SIZE                W25Q64JV_PAGE_SIZE

#define CONFIG_W25Qxx_BULK_ERASE_MAX_TIME      W25Q64JV_BULK_ERASE_MAX_TIME
#define CONFIG_W25Qxx_SECTOR_ERASE_MAX_TIME    W25Q64JV_SECTOR_ERASE_MAX_TIME
#define CONFIG_W25Qxx_SUBSECTOR_ERASE_MAX_TIME W25Q64JV_SUBSECTOR_ERASE_MAX_TIME

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define W25Qxx_MEM_ADDR QSPI_BASE  // 内存映射模式的地址

/**
 * @brief flash jedec id
 */
#define FLASH_ID_W25Q16  0xEF4015
#define FLASH_ID_W25Q32  0xEF4016
#define FLASH_ID_W25Q64  0XEF4017
#define FLASH_ID_W25Q128 0XEF4018
#define FLASH_ID_W25Q256 0XEF4019

/**
 * @brief w25q256
 *
 * @note 若使用改型号, 需进行以下改动
 *
 *  1. 写状态寄存器，以启用4地址模式
 *  2. 将 sCommand.AddressSize = QSPI_ADDRESS_24_BITS;
 *     改为 sCommand.AddressSize = QSPI_ADDRESS_32_BITS;
 */

/**
 * @brief w25q128fv
 */

#define W25Q128FV_FLASH_SIZE               0x1000000 /* 128 MBits => 16MBytes */
#define W25Q128FV_SECTOR_SIZE              0x10000   /* 256 sectors of 64KBytes */
#define W25Q128FV_SUBSECTOR_SIZE           0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q128FV_PAGE_SIZE                0x100     /* 65536 pages of 256 bytes */

#define W25Q128FV_BULK_ERASE_MAX_TIME      250000  // 整片擦除所需时间
#define W25Q128FV_SECTOR_ERASE_MAX_TIME    3000
#define W25Q128FV_SUBSECTOR_ERASE_MAX_TIME 800

/**
 * @brief w25q64jv
 */

#define W25Q64JV_FLASH_SIZE               0x800000 /* 64 MBits => 8MBytes */
#define W25Q64JV_SECTOR_SIZE              0x10000  /* 128 sectors of 64KBytes */
#define W25Q64JV_SUBSECTOR_SIZE           0x1000   /* 2048 subsectors of 4kBytes */
#define W25Q64JV_PAGE_SIZE                0x100    /* 32768 pages of 256 bytes */

#define W25Q64JV_BULK_ERASE_MAX_TIME      100000
#define W25Q64JV_SECTOR_ERASE_MAX_TIME    1500
#define W25Q64JV_SUBSECTOR_ERASE_MAX_TIME 400

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool W25Qx_QSPI_Reset(void);
u32  W25Qx_QSPI_ReadJedecID(void);
u16  W25Qx_QSPI_ReadDeviceID(void);
bool W25Qx_QSPI_EnterMemoryMappedMode(void);
bool W25Qx_QSPI_EraseSector(u32 u32SectorAddress);
bool W25Qx_QSPI_EraseBlock_32K(u32 u32BlockAddress);
bool W25Qx_QSPI_EraseBlock_64K(uint32_t u32BlockAddress);
bool W25Qx_QSPI_EraseChip(void);
bool W25Qx_QSPI_WriteBuffer(u8* pu8Data, u32 u32WriteAddr, u32 u32Size);
bool W25Qx_QSPI_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);

bool W25Qx_QSPI_Test(void);

#ifdef __cplusplus
}
#endif

#endif  // __QSPI_W25QXX_H__
