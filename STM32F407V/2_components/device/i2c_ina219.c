#include "i2c_ina219.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG    "ina219"
#define LOG_LOCAL_LEVEL  LOG_LEVEL_DEBUG

#define REG_CONFIG       0x00  // configuration 配置
#define REG_SHUNTVOLTAGE 0x01  // shunt voltage 分流电阻两端电压
#define REG_BUSVOLTAGE   0x02  // bus voltage 总线电压(IN-和GND间的压差)
#define REG_POWER        0x03  // power 功率
#define REG_CURRENT      0x04  // current 电流
#define REG_CALIBRATION  0x05  // calibration 基准值

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static inline status_t INA219_ReadWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data);
static inline status_t INA219_WriteWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data);
static inline status_t INA219_ReadWordBits(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint16_t* pu16BitsValue);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t INA219_ReadWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data)
{
    return I2C_Master_ReadWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static inline status_t INA219_WriteWord(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static inline status_t INA219_ReadWordBits(i2c_ina219_t* pHandle, uint8_t u8MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint16_t* pu16BitsValue)
{
    return I2C_Master_ReadWordBits(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8StartBit, u8BitsCount, pu16BitsValue, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

status_t INA219_Init(i2c_ina219_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    return ERR_NONE;
}

status_t INA219_Configure(i2c_ina219_t* pHandle, ina219_bus_range_e eRange, ina219_gain_e eGain, ina219_bus_res_e eBusRes, ina219_shunt_res_e eShuntRes, ina219_mode_e eMode)
{
    switch (eRange)
    {
        case INA219_BUS_RANGE_32V: pHandle->_f32VbusMax = 32.0f; break;
        case INA219_BUS_RANGE_16V: pHandle->_f32VbusMax = 16.0f; break;
        default: return ERR_INVALID_VALUE;
    }

    switch (eGain)
    {
        case INA219_GAIN_320MV: pHandle->_f32VshuntMax = 0.32f; break;
        case INA219_GAIN_160MV: pHandle->_f32VshuntMax = 0.16f; break;
        case INA219_GAIN_80MV: pHandle->_f32VshuntMax = 0.08f; break;
        case INA219_GAIN_40MV: pHandle->_f32VshuntMax = 0.04f; break;
        default: return ERR_INVALID_VALUE;
    }

    uint16_t u16Config = (eRange << 13) | (eGain << 11) | (eBusRes << 7) | (eShuntRes << 3) | (eMode << 0);

    INA219_WriteWord(pHandle, REG_CONFIG, u16Config);

    pHandle->_f32CurrentLSB = pHandle->_f32VshuntMax / pHandle->f32ShuntOhm / 32768.f;
    pHandle->_f32PowerLSB   = pHandle->_f32CurrentLSB * 20;

    uint16_t u16CalibrationValue = (uint16_t)(0.04096f * 32768.0f / pHandle->_f32VshuntMax);

    INA219_WriteWord(pHandle, REG_CALIBRATION, u16CalibrationValue);

    return ERR_NONE;
}

float32_t INA219_GetMaxCurrent(i2c_ina219_t* pHandle)
{
    float32_t f32MaxCurrent  = pHandle->_f32CurrentLSB * 32767;
    float32_t f32MaxPossible = pHandle->_f32VshuntMax / pHandle->f32ShuntOhm;

    return MIN(f32MaxCurrent, f32MaxPossible);
}

float32_t INA219_GetMaxShuntVoltage(i2c_ina219_t* pHandle)
{
    float32_t f32MaxVoltage = INA219_GetMaxCurrent(pHandle) * pHandle->f32ShuntOhm;

    return MIN(f32MaxVoltage, pHandle->_f32VshuntMax);
}

float32_t INA219_GetMaxPower(i2c_ina219_t* pHandle)
{
    return INA219_GetMaxCurrent(pHandle) * pHandle->_f32VbusMax;
}

float32_t INA219_ReadBusPower(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_POWER, &u16Data);

    return u16Data * pHandle->_f32PowerLSB;
}

float32_t INA219_ReadShuntCurrent(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_CURRENT, &u16Data);

    return (f32)(s16)u16Data * pHandle->_f32CurrentLSB;
}

float32_t INA219_ReadShuntVoltage(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_SHUNTVOLTAGE, u16Data);

    return u16Data / 100000.f;
}

float32_t INA219_ReadBusVoltage(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWord(pHandle, REG_BUSVOLTAGE, &u16Data);

    return (u16Data >> 3) * 0.004f;
}

ina219_bus_range_e INA219_GetRange(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 13, 1, &u16Data);

    return (ina219_bus_range_e)u16Data;
}

ina219_gain_e INA219_GetGain(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 11, 2, &u16Data);

    return (ina219_gain_e)u16Data;
}

ina219_bus_res_e INA219_GetBusRes(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 7, 4, &u16Data);

    return (ina219_bus_res_e)u16Data;
}

ina219_shunt_res_e INA219_GetShuntRes(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 3, 4, &u16Data);

    return (ina219_shunt_res_e)u16Data;
}

ina219_mode_e INA219_GetMode(i2c_ina219_t* pHandle)
{
    uint16_t u16Data;

    INA219_ReadWordBits(pHandle, REG_CONFIG, 0, 3, &u16Data);

    return (ina219_mode_e)u16Data;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void INA219_Test(i2c_mst_t* hI2C)
{
    i2c_ina219_t ina219 = {
        .hI2C        = hI2C,
        .u8SlvAddr   = INA219_ADDRESS_1,
        .f32ShuntOhm = 0.1f,  // 0.1 ohm
    };

    I2C_Master_Hexdump(ina219.hI2C, ina219.u8SlvAddr, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_BIG_ENDIAN | I2C_FLAG_MEMUNIT_16BIT);

    INA219_Init(&ina219);

    // Configure INA219
    INA219_Configure(&ina219, INA219_BUS_RANGE_16V, INA219_GAIN_160MV, INA219_BUS_RES_12BIT, INA219_SHUNT_RES_12_BIT_128_SAMPLES, INA219_MODE_SHUNT_BUS_CONT);

    // Display configuration
    PRINTF("Mode:                 ");
    switch (INA219_GetMode(&ina219))
    {
        case INA219_MODE_POWER_DOWN: PRINTLN("Power-Down"); break;
        case INA219_MODE_SHUNT_TRIG: PRINTLN("Shunt Voltage, Triggered"); break;
        case INA219_MODE_BUS_TRIG: PRINTLN("Bus Voltage, Triggered"); break;
        case INA219_MODE_SHUNT_BUS_TRIG: PRINTLN("Shunt and Bus, Triggered"); break;
        case INA219_MODE_ADC_OFF: PRINTLN("ADC Off"); break;
        case INA219_MODE_SHUNT_CONT: PRINTLN("Shunt Voltage, Continuous"); break;
        case INA219_MODE_BUS_CONT: PRINTLN("Bus Voltage, Continuous"); break;
        case INA219_MODE_SHUNT_BUS_CONT: PRINTLN("Shunt and Bus, Continuous"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Range:                ");
    switch (INA219_GetRange(&ina219))
    {
        case INA219_BUS_RANGE_16V: PRINTLN("16V"); break;
        case INA219_BUS_RANGE_32V: PRINTLN("32V"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Gain:                 ");
    switch (INA219_GetGain(&ina219))
    {
        case INA219_GAIN_40MV: PRINTLN("+/- 40mV"); break;
        case INA219_GAIN_80MV: PRINTLN("+/- 80mV"); break;
        case INA219_GAIN_160MV: PRINTLN("+/- 160mV"); break;
        case INA219_GAIN_320MV: PRINTLN("+/- 320mV"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Bus resolution:       ");
    switch (INA219_GetBusRes(&ina219))
    {
        case INA219_BUS_RES_9BIT: PRINTLN("9-bit"); break;
        case INA219_BUS_RES_10BIT: PRINTLN("10-bit"); break;
        case INA219_BUS_RES_11BIT: PRINTLN("11-bit"); break;
        case INA219_BUS_RES_12BIT: PRINTLN("12-bit"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTF("Shunt resolution:     ");
    switch (INA219_GetShuntRes(&ina219))
    {
        case INA219_SHUNT_RES_9_BIT_1_SAMPLES: PRINTLN("9-bit / 1 sample"); break;
        case INA219_SHUNT_RES_10_BIT_1_SAMPLES: PRINTLN("10-bit / 1 sample"); break;
        case INA219_SHUNT_RES_11_BIT_1_SAMPLES: PRINTLN("11-bit / 1 sample"); break;
        case INA219_SHUNT_RES_12_BIT_1_SAMPLES: PRINTLN("12-bit / 1 sample"); break;
        case INA219_SHUNT_RES_12_BIT_2_SAMPLES: PRINTLN("12-bit / 2 samples"); break;
        case INA219_SHUNT_RES_12_BIT_4_SAMPLES: PRINTLN("12-bit / 4 samples"); break;
        case INA219_SHUNT_RES_12_BIT_8_SAMPLES: PRINTLN("12-bit / 8 samples"); break;
        case INA219_SHUNT_RES_12_BIT_16_SAMPLES: PRINTLN("12-bit / 16 samples"); break;
        case INA219_SHUNT_RES_12_BIT_32_SAMPLES: PRINTLN("12-bit / 32 samples"); break;
        case INA219_SHUNT_RES_12_BIT_64_SAMPLES: PRINTLN("12-bit / 64 samples"); break;
        case INA219_SHUNT_RES_12_BIT_128_SAMPLES: PRINTLN("12-bit / 128 samples"); break;
        default: PRINTLN("unknown"); break;
    }

    PRINTLN("Max current:          %.5f A", INA219_GetMaxCurrent(&ina219));
    PRINTLN("Max shunt voltage:    %.5f V", INA219_GetMaxShuntVoltage(&ina219));
    PRINTLN("Max power:            %.5f W", INA219_GetMaxPower(&ina219));

    while (1)
    {
        PRINTLN("Bus voltage:   %.5f V", INA219_ReadBusVoltage(&ina219));
        PRINTLN("Bus power:     %.5f W", INA219_ReadBusPower(&ina219));
        PRINTLN("Shunt voltage: %.5f V", INA219_ReadShuntVoltage(&ina219));
        PRINTLN("Shunt current: %.5f A", INA219_ReadShuntCurrent(&ina219));
        DelayBlockMs(1000);
    }
}

#endif