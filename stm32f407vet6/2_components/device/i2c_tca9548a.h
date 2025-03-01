#ifndef __I2C_TCA9584A_H__
#define __I2C_TCA9584A_H__

/**
 * @file i2c mux
 */

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief tca9548a address (A2A1A0)
 */
#define TCA9548A_ADDRESS_000 0x70
#define TCA9548A_ADDRESS_100 0x71
#define TCA9548A_ADDRESS_010 0x72
#define TCA9548A_ADDRESS_110 0x73
#define TCA9548A_ADDRESS_001 0x74
#define TCA9548A_ADDRESS_101 0x75
#define TCA9548A_ADDRESS_011 0x76
#define TCA9548A_ADDRESS_111 0x77

typedef enum {
    TCA9548A_CHANNEL_0    = 1 << 0,
    TCA9548A_CHANNEL_1    = 1 << 1,
    TCA9548A_CHANNEL_2    = 1 << 2,
    TCA9548A_CHANNEL_3    = 1 << 3,
    TCA9548A_CHANNEL_4    = 1 << 4,
    TCA9548A_CHANNEL_5    = 1 << 5,
    TCA9548A_CHANNEL_6    = 1 << 6,
    TCA9548A_CHANNEL_7    = 1 << 7,
    TCA9548A_CHANNEL_NONE = 0x00,
    TCA9548A_CHANNEL_ALL  = 0xFF,
} tca9548a_channel_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;
} i2c_tca9548a_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t TCA9548A_Init(i2c_tca9548a_t* pHandle);
status_t TCA9548A_SelectChannel(i2c_tca9548a_t* pHandle, uint8_t u8ChannelMask);
status_t TCA9548A_ScanAddress(i2c_tca9548a_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void TCA9548A_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
