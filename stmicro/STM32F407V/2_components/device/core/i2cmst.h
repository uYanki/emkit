#ifndef __I2C_MST_H__
#define __I2C_MST_H__

#include "i2cdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t I2C_Master_Init(i2c_mst_t* pHandle, u32 u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle);

/**
 * @brief block access
 */

status_t I2C_Master_ReadBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8* pu8Data, u16 u16Size, u16 u16Flags);
status_t I2C_Master_WriteBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, const u8* cpu8Data, u16 u16Size, u16 u16Flags);

status_t I2C_Master_TransmitBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, const u8* cpu8TxData, u16 u16Size, u16 u16Flags);
status_t I2C_Master_ReceiveBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u8* pu8RxData, u16 u16Size, u16 u16Flags);

/**
 * @brief byte access
 */

status_t I2C_Master_WriteByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8Data, u16 u16Flags);
status_t I2C_Master_ReadByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8* pu8Data, u16 u16Flags);

status_t I2C_Master_TransmitByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u8 u8TxData, u16 u16Flags);
status_t I2C_Master_ReceiveByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u8* pu8RxData, u16 u16Flags);

/**
 * @brief word access
 */

status_t I2C_Master_TransmitWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16Data, u16 u16Flags);
status_t I2C_Master_ReceiveWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16* pu16Data, u16 u16Flags);

status_t I2C_Master_ReadWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u16* pu16Data, u16 u16Flags);
status_t I2C_Master_WriteWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u16 u16Data, u16 u16Flags);

/**
 * @brief bits access
 */

status_t I2C_Master_ReadByteBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u8* pu8BitsValue, u16 u16Flags);
status_t I2C_Master_WriteByteBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u8 u8BitsValue, u16 u16Flags);

status_t I2C_Master_ReadWordBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u16* pu16BitsValue, u16 u16Flags);
status_t I2C_Master_WriteWordBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u16 u16BitsValue, u16 u16Flags);

/**
 * @brief tools
 */

bool     I2C_Master_IsDeviceReady(i2c_mst_t* pHandle, u8 u16SlvAddr, u16 u16Flags);
u8       I2C_Master_ScanAddress(i2c_mst_t* pHandle);
status_t I2C_Master_Hexdump(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16Flags);
status_t I2C_Master_HexdumpWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16Flags);

#ifdef __cplusplus
}
#endif

#endif
