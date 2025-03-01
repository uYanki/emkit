#ifndef __I2C_EEPROM_H__
#define __I2C_EEPROM_H__

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_EEPROM_ADDRESS_OVERFLOW_CHECK_SW 1

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief eeprom capacity in bytes
 * @note 部分容量的设备地址段的低位会用于寄存器地址寻址
 */

#define AT24CXX_ADDRESS_A000 0x50
#define AT24CXX_ADDRESS_A001 0x51
#define AT24CXX_ADDRESS_A010 0x52
#define AT24CXX_ADDRESS_A011 0x53
#define AT24CXX_ADDRESS_A100 0x54
#define AT24CXX_ADDRESS_A101 0x55
#define AT24CXX_ADDRESS_A110 0x56
#define AT24CXX_ADDRESS_A111 0x57

// unit: bytes
typedef enum {
    AT24C01  = 128,    /* 7B */
    AT24C02  = 256,    /* 8B */
    AT24C04  = 512,    /* 9B */
    AT24C08  = 1024,   /* 10B */
    AT24C16  = 2048,   /* 11B */
    AT24C32  = 4096,   /* 12B */
    AT24C64  = 8192,   /* 13B */
    AT24C128 = 16384,  /* 14B */
    AT24C256 = 32768,  /* 15B */
    AT24C512 = 65536,  /* 16B */
    AT24CM01 = 131072, /* 17B, AT24C1024 */
    AT24CM02 = 262144, /* 18B, AT24C2048 */
} eeprom_capacity_e;

typedef struct {
    __IN i2c_mst_t*        hI2C;
    __IN uint8_t           u8SlvAddr;  //< 7-bit slave address
    __IN eeprom_capacity_e eCapacity;  //< eeprom capacity
} i2c_eeprom_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t EEPROM_Init(i2c_eeprom_t* pHandle);
status_t EEPROM_WriteBlock(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, const uint8_t* cpu8Buffer, uint16_t u16Size);
status_t EEPROM_ReadBlock(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, uint8_t* pu8Buffer, uint16_t u16Size);
status_t EEPROM_FillByte(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, uint16_t u16Size, uint8_t u8Data);
status_t EEPROM_Hexdump(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, uint16_t u16Size);
status_t EEPROM_DetectCapacity(i2c_eeprom_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void EEPROM_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
