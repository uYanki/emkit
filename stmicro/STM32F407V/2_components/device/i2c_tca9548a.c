#include "i2c_tca9548a.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "tca9548a"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t TCA9548A_Init(i2c_tca9548a_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    return ERR_NONE;
}

/**
 * @note 可同时选中多个通道
 */
status_t TCA9548A_SelectChannel(i2c_tca9548a_t* pHandle, uint8_t u8ChannelMask)
{
    return I2C_Master_TransmitByte(pHandle->hI2C, pHandle->u8SlvAddr, u8ChannelMask, I2C_FLAG_SLVADDR_7BIT);
}

status_t TCA9548A_ScanAddress(i2c_tca9548a_t* pHandle)
{
    return I2C_Master_ScanAddress(pHandle->hI2C);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void TCA9548A_Test(i2c_mst_t* hI2C)
{
    i2c_tca9548a_t tca9548a = {
        .hI2C      = hI2C,
        .u8SlvAddr = TCA9548A_ADDRESS_000,
    };

    TCA9548A_Init(&tca9548a);

#if 0

    for (uint8_t i = 0; i < 8; ++i)
    {
        LOGI("single channel: %d", i);
        TCA9548A_SelectChannel(&tca9548a, BV(i));
        TCA9548A_ScanAddress(&tca9548a);
    }

    while (1)
    {
    }

#else

    LOGI("multi channel");
    TCA9548A_SelectChannel(&tca9548a, TCA9548A_CHANNEL_1 | TCA9548A_CHANNEL_2 | TCA9548A_CHANNEL_3);
    TCA9548A_ScanAddress(&tca9548a);

    while (1)
    {
    }

#endif
}

#endif
