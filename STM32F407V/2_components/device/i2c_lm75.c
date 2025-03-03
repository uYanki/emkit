#include "i2c_lm75.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "lm75"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief register
 */
#define LM75_REG_CONF  0x01  // configure
#define LM75_REG_TEMP  0x00  // temperature
#define LM75_REG_TOS   0x03  // overtemperature shutdown, Tth(ots), default = 80°C
#define LM75_REG_THYST 0x02  // hysteresis, Thys, default = 75°C

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static status_t LM75_WriteWord(i2c_lm75_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static status_t LM75_ReadWord(i2c_lm75_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data)
{
    return I2C_Master_ReadWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static status_t LM75_WriteBits(i2c_lm75_t* pHandle, uint16_t u16MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t u8BitsValue)
{
    return I2C_Master_WriteByteBits(pHandle->hI2C, pHandle->u8SlvAddr, u16MemAddr, u8StartBit, u8BitsCount, u8BitsValue, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static status_t LM75_ReadBits(i2c_lm75_t* pHandle, uint16_t u16MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t* pu8BitsValue)
{
    return I2C_Master_ReadByteBits(pHandle->hI2C, pHandle->u8SlvAddr, u16MemAddr, u8StartBit, u8BitsCount, pu8BitsValue, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static status_t LM75_GetLimit(i2c_lm75_t* pHandle, uint8_t u8MemAddr, float32_t* pf32Value)
{
    uint16_t u16Data;

    ERRCHK_RETURN(LM75_ReadWord(pHandle, u8MemAddr, &u16Data));

    u16Data >>= 7;

    if (u16Data & 0x0100)
    {
        *pf32Value = -0.5f * ((~u16Data & 0xFF) + 1);
    }
    else
    {
        *pf32Value = 0.5f * u16Data;
    }

    return ERR_NONE;
}

static status_t LM75_SetLimit(i2c_lm75_t* pHandle, uint8_t u8MemAddr, float32_t f32Value)
{
    uint16_t u16Data;

    if (f32Value > 0)
    {
        u16Data = f32Value * 2;
        u16Data &= 0xFF;
    }
    else
    {
        u16Data = -f32Value * 2;
        u16Data &= 0xFF;
        u16Data = ~(u16Data - 1);
        u16Data |= 1 << 15;
    }

    u16Data <<= 7;

    return LM75_WriteWord(pHandle, u8MemAddr, u16Data);
}

status_t LM75_Init(i2c_lm75_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    return ERR_NONE;
}

status_t LM75_ReadTemp(i2c_lm75_t* pHandle, float32_t* pTemperature)
{
    uint16_t u16Data;

    ERRCHK_RETURN(LM75_ReadWord(pHandle, LM75_REG_TEMP, &u16Data));

    u16Data >>= 5;

    if (u16Data & 0x0400)
    {
        u16Data |= 0xF800U;
        u16Data = -(~u16Data + 1);
    }

    *pTemperature = u16Data * 0.125f;

    return ERR_NONE;
}

status_t LM75_SetHysteresis(i2c_lm75_t* pHandle, float32_t f32Hysteresis)
{
    return LM75_SetLimit(pHandle, LM75_REG_THYST, f32Hysteresis);
}

status_t LM75_GetHysteresis(i2c_lm75_t* pHandle, float32_t* pf32Hysteresis)
{
    return LM75_GetLimit(pHandle, LM75_REG_THYST, pf32Hysteresis);
}

status_t LM75_SetOverTempThold(i2c_lm75_t* pHandle, float32_t f32Threshold)
{
    return LM75_SetLimit(pHandle, LM75_REG_TOS, f32Threshold);
}

status_t LM75_GetOverTempThold(i2c_lm75_t* pHandle, float32_t* pf32Threshold)
{
    return LM75_GetLimit(pHandle, LM75_REG_TOS, pf32Threshold);
}

status_t LM75_SetDeviceMode(i2c_lm75_t* pHandle, lm75_device_mode_e eMode)
{
    return LM75_WriteBits(pHandle, LM75_REG_CONF, 0, 1, eMode);
}

status_t LM75_GetDeviceMode(i2c_lm75_t* pHandle, lm75_device_mode_e* peMode)
{
    return LM75_ReadBits(pHandle, LM75_REG_CONF, 0, 1, peMode);
}

status_t LM75_SetOperMode(i2c_lm75_t* pHandle, lm75_os_operation_mode_e eMode)
{
    return LM75_WriteBits(pHandle, LM75_REG_CONF, 1, 1, eMode);
}

status_t LM75_GetOperMode(i2c_lm75_t* pHandle, lm75_os_operation_mode_e* peMode)
{
    return LM75_ReadBits(pHandle, LM75_REG_CONF, 1, 1, peMode);
}

status_t LM75_SetOutputSignalPolarity(i2c_lm75_t* pHandle, lm75_os_polarity_e ePolarity)
{
    return LM75_WriteBits(pHandle, LM75_REG_CONF, 2, 1, ePolarity);
}

status_t LM75_GetOutputSignalPolarity(i2c_lm75_t* pHandle, lm75_os_polarity_e* pePolarity)
{
    return LM75_ReadBits(pHandle, LM75_REG_CONF, 2, 1, pePolarity);
}

status_t LM75_SetFaultQueue(i2c_lm75_t* pHandle, lm75_fault_queue_e eFaultQueue)
{
    return LM75_WriteBits(pHandle, LM75_REG_CONF, 3, 2, eFaultQueue);
}

status_t LM75_GetFaultQueue(i2c_lm75_t* pHandle, lm75_fault_queue_e* peFaultQueue)
{
    return LM75_ReadBits(pHandle, LM75_REG_CONF, 3, 2, peFaultQueue);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void LM75_Test(i2c_mst_t* hI2C)
{
    i2c_lm75_t lm75 = {
        .hI2C      = hI2C,
        .u8SlvAddr = LM75_ADDRESS_A000,
    };

    float32_t f32OverTemp, f32Hysteresis;

    LM75_SetDeviceMode(&lm75, LM75_MODE_NORMAL);
    LM75_SetOperMode(&lm75, LM75_OS_OPERATION_COMPARATOR);

    LM75_SetOverTempThold(&lm75, 33.0f);
    LM75_GetOverTempThold(&lm75, &f32OverTemp);

    LM75_SetHysteresis(&lm75, 31.0f);
    LM75_GetHysteresis(&lm75, &f32Hysteresis);

    // 温度上升到 f32OverTemp 时指示灯亮，待温度回降 f32Hysteresis 时指示灯灯灭（大约有0.5度的误差）
    LOGI("overtemp = %.3f , hysteresis = %.3f", f32OverTemp, f32Hysteresis);

    while (1)
    {
        float32_t f32Temperature;

        // 读温度，贴皮肤上，温度变化蛮快的
        if (LM75_ReadTemp(&lm75, &f32Temperature) == ERR_NONE)
        {
            LOGI("temperature = %.3f", f32Temperature);
        }

        DelayBlockS(1);
    }
}

#endif
