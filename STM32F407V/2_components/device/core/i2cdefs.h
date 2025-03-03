#ifndef __I2C_DEFS_H__
#define __I2C_DEFS_H__

#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief slave address size
 */
#define I2C_FLAG_SLVADDR_SIZE_Pos (0U)
#define I2C_FLAG_SLVADDR_SIZE_Len (1U)
#define I2C_FLAG_SLVADDR_SIZE_Msk BITMASK16(I2C_FLAG_SLVADDR_SIZE_Pos, I2C_FLAG_SLVADDR_SIZE_Len)
#define I2C_FLAG_SLVADDR_7BIT     (0U << I2C_FLAG_SLVADDR_SIZE_Pos)
#define I2C_FLAG_SLVADDR_10BIT    (1U << I2C_FLAG_SLVADDR_SIZE_Pos)

/**
 * @brief memory address size
 */
#define I2C_FLAG_MEMADDR_SIZE_Pos (I2C_FLAG_SLVADDR_SIZE_Pos + I2C_FLAG_SLVADDR_SIZE_Len)
#define I2C_FLAG_MEMADDR_SIZE_Len (1U)
#define I2C_FLAG_MEMADDR_SIZE_Msk BITMASK16(I2C_FLAG_MEMADDR_SIZE_Pos, I2C_FLAG_MEMADDR_SIZE_Len)
#define I2C_FLAG_MEMADDR_8BIT     (0U << I2C_FLAG_MEMADDR_SIZE_Pos)
#define I2C_FLAG_MEMADDR_16BIT    (1U << I2C_FLAG_MEMADDR_SIZE_Pos)

/**
 * @brief memory unit size
 */
#define I2C_FLAG_MEMUNIT_SIZE_Pos (I2C_FLAG_MEMADDR_SIZE_Pos + I2C_FLAG_MEMADDR_SIZE_Len)
#define I2C_FLAG_MEMUNIT_SIZE_Len (1U)
#define I2C_FLAG_MEMUNIT_SIZE_Msk BITMASK16(I2C_FLAG_MEMUNIT_SIZE_Pos, I2C_FLAG_MEMUNIT_SIZE_Len)
#define I2C_FLAG_MEMUNIT_8BIT     (0U << I2C_FLAG_MEMUNIT_SIZE_Pos)
#define I2C_FLAG_MEMUNIT_16BIT    (1U << I2C_FLAG_MEMUNIT_SIZE_Pos)

/**
 * @brief signal control
 */
#define I2C_FLAG_SIGNAL_Pos         (I2C_FLAG_MEMUNIT_SIZE_Pos + I2C_FLAG_MEMUNIT_SIZE_Len)
#define I2C_FLAG_SIGNAL_Len         (4U)
#define I2C_FLAG_SIGNAL_Msk         BITMASK16(I2C_FLAG_SIGNAL_Pos, I2C_FLAG_SIGNAL_Len)
#define I2C_FLAG_SIGNAL_DEFAULT     (0U << I2C_FLAG_SIGNAL_Pos)    /*!< Transfer starts with a start signal, stops with a stop signal. */
#define I2C_FLAG_SIGNAL_NOSTART     (BV(0) << I2C_FLAG_SIGNAL_Pos) /*!< Don't send a start condition, address, and sub address */
#define I2C_FLAG_SIGNAL_NORESTART   (BV(1) << I2C_FLAG_SIGNAL_Pos) /*!< Don't send a repeated start condition */
#define I2C_FLAG_SIGNAL_NOSTOP      (BV(2) << I2C_FLAG_SIGNAL_Pos) /*!< Don't send a stop condition. */
#define I2C_FLAG_SIGNAL_IGNORE_NACK (BV(3) << I2C_FLAG_SIGNAL_Pos)

/**
 * @brief word endian
 */
#define I2C_FLAG_WORD_ENDIAN_Pos    (I2C_FLAG_SIGNAL_Pos + I2C_FLAG_SIGNAL_Len)
#define I2C_FLAG_WORD_ENDIAN_Len    (1U)
#define I2C_FLAG_WORD_ENDIAN_Msk    BITMASK16(I2C_FLAG_WORD_ENDIAN_Pos, I2C_FLAG_WORD_ENDIAN_Len)
#define I2C_FLAG_WORD_BIG_ENDIAN    (0 << I2C_FLAG_WORD_ENDIAN_Pos) /*!< low byte at first */
#define I2C_FLAG_WORD_LITTLE_ENDIAN (1 << I2C_FLAG_WORD_ENDIAN_Pos) /*!< high byte at first */

/**
 * @brief i2c duty cycle
 */
typedef enum {
    I2C_DUTYCYCLE_64_36 = 163,  // 255*0.63 (16:9)
    I2C_DUTYCYCLE_67_33 = 171,  // 255*0.67 (2:1)
    I2C_DUTYCYCLE_50_50 = 127,  // 255*0.5 (1:1)
} i2c_duty_cycle_e;

/**
 * @brief i2c master
 */

typedef struct i2cmst_ops i2cmst_ops_t;

typedef struct {
    __IN const pin_t SDA;   // serial data line
    __IN const pin_t SCL;   // serial clock line
    __IN const void* I2Cx;  // hwi2c handler

#if (CONFIG_SWI2C_MODULE_SW)
    u32 u32ClockLowCycleUs;   // [swi2c only]
    u32 u32ClockHighCycleUs;  // [swi2c only]
#endif

    const i2cmst_ops_t* pOps;
} i2c_mst_t;

struct i2cmst_ops {
    status_t (*Init)(i2c_mst_t* pHandle, u32 u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle);
    bool (*IsDeviceReady)(i2c_mst_t* pHandle, u8 u16SlvAddr, u16 u16Flags);
    status_t (*ReadBlock)(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8* pu8Data, u16 u16Size, u16 u16Flags);
    status_t (*WriteBlock)(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, const u8* cpu8Data, u16 u16Size, u16 u16Flags);
    status_t (*ReceiveBlock)(i2c_mst_t* pHandle, u16 u16SlvAddr, u8* pu8Data, u16 u16Size, u16 u16Flags);
    status_t (*TransmitBlock)(i2c_mst_t* pHandle, u16 u16SlvAddr, const u8* cpu8Data, u16 u16Size, u16 u16Flags);
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
