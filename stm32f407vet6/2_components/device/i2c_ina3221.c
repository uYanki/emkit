#include "i2c_ina3221.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG             "ina3221"
#define LOG_LOCAL_LEVEL           LOG_LEVEL_DEBUG

#define REG_CONF                  0
#define REG_CH1_SHUNTV            1
#define REG_CH1_BUSV              2
#define REG_CH2_SHUNTV            3
#define REG_CH2_BUSV              4
#define REG_CH3_SHUNTV            5
#define REG_CH3_BUSV              6
#define REG_CH1_CRIT_ALERT_LIM    7
#define REG_CH1_WARNING_ALERT_LIM 8
#define REG_CH2_CRIT_ALERT_LIM    9
#define REG_CH2_WARNING_ALERT_LIM 10
#define REG_CH3_CRIT_ALERT_LIM    11
#define REG_CH3_WARNING_ALERT_LIM 12
#define REG_SHUNTV_SUM            13
#define REG_SHUNTV_SUM_LIM        14
#define REG_MASK_ENABLE           15
#define REG_PWR_VALID_HI_LIM      16
#define REG_PWR_VALID_LO_LIM      17
#define REG_MANUF_ID              0xFE
#define REG_DIE_ID                0xFF

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

// Configuration register
typedef struct {
    uint16_t mode_shunt_en      : 1;  // 0
    uint16_t mode_bus_en        : 1;  // 1
    uint16_t mode_continious_en : 1;  // 2
    uint16_t shunt_conv_time    : 3;  // 3
    uint16_t bus_conv_time      : 3;  // 6
    uint16_t avg_mode           : 3;  // 9
    uint16_t ch3_en             : 1;  // 12
    uint16_t ch2_en             : 1;  // 13
    uint16_t ch1_en             : 1;  // 14
    uint16_t reset              : 1;  // 15
} ina3221_conf_reg_t;

// Mask/Enable register
typedef struct {
    uint16_t conv_ready          : 1;  // 0
    uint16_t timing_ctrl_alert   : 1;  // 1
    uint16_t pwr_valid_alert     : 1;  // 2
    uint16_t warn_alert_ch3      : 1;  // 3
    uint16_t warn_alert_ch2      : 1;  // 4
    uint16_t warn_alert_ch1      : 1;  // 5
    uint16_t shunt_sum_alert     : 1;  // 6
    uint16_t crit_alert_ch3      : 1;  // 7
    uint16_t crit_alert_ch2      : 1;  // 8
    uint16_t crit_alert_ch1      : 1;  // 9
    uint16_t crit_alert_latch_en : 1;  // 10
    uint16_t warn_alert_latch_en : 1;  // 11
    uint16_t shunt_sum_en_ch3    : 1;  // 12
    uint16_t shunt_sum_en_ch2    : 1;  // 13
    uint16_t shunt_sum_en_ch1    : 1;  // 14
    uint16_t reserved            : 1;  // 15
} ina3221_masken_reg_t;

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline INA3221_ReadWord(i2c_ina3221_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data)
{
    return I2C_Master_ReadWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static inline INA3221_WriteWord(i2c_ina3221_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static inline status_t INA3221_WriteWordBits(i2c_ina3221_t* pHandle, uint8_t u8MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint16_t u16BitsValue)
{
    return I2C_Master_WriteWordBits(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8StartBit, u8BitsCount, u16BitsValue, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_MEMUNIT_16BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

status_t INA3221_Init(i2c_ina3221_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    return ERR_NONE;
}

void INA3221_Reset(i2c_ina3221_t* pHandle)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 15, 1, 1);  // reset
}

void INA3221_SetModePowerDown(i2c_ina3221_t* pHandle)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 1, 2, 0);  // mode_bus_en & mode_continious_en
}

void INA3221_SetModeContinious(i2c_ina3221_t* pHandle)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 2, 1, 1);  // mode_continious_en
}

void INA3221_SetModeTriggered(i2c_ina3221_t* pHandle)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 2, 1, 0);  // mode_continious_en
}

void INA3221_SetShuntMeasEnable(i2c_ina3221_t* pHandle, bool bEnable)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 0, 1, bEnable ? 1 : 0);  // mode_shunt_en
}

void INA3221_SetBusMeasEnable(i2c_ina3221_t* pHandle, bool bEnable)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 1, 1, bEnable ? 1 : 0);  // mode_bus_en
}

void INA3221_SetAveragingMode(i2c_ina3221_t* pHandle, ina3221_avg_mode_e eAvgMode)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 9, 3, eAvgMode);  // avg_mode
}

void INA3221_SetBusConversionTime(i2c_ina3221_t* pHandle, ina3221_conv_time_e eConvTime)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 6, 3, eConvTime);  // bus_conv_time
}

void INA3221_SetShuntConversionTime(i2c_ina3221_t* pHandle, ina3221_conv_time_e eConvTime)
{
    INA3221_WriteWordBits(pHandle, REG_CONF, 3, 3, eConvTime);  // shunt_conv_time
}

void INA3221_SetPwrValidUpLimit(i2c_ina3221_t* pHandle, int16_t s16VoltagemV)
{
    INA3221_WriteWord(pHandle, REG_PWR_VALID_HI_LIM, s16VoltagemV);
}

void INA3221_SetPwrValidLowLimit(i2c_ina3221_t* pHandle, int16_t s16VoltagemV)
{
    INA3221_WriteWord(pHandle, REG_PWR_VALID_LO_LIM, s16VoltagemV);
}

void INA3221_SetShuntSumAlertLimit(i2c_ina3221_t* pHandle, int32_t s32VoltageuV)
{
    int16_t s16Data = s32VoltageuV / 20;
    INA3221_WriteWord(pHandle, REG_SHUNTV_SUM_LIM, s16Data);
}

void INA3221_SetCurrentSumAlertLimit(i2c_ina3221_t* pHandle, int32_t s32CurrentmA)
{
    int16_t s16ShuntuV = s32CurrentmA * (int32_t)pHandle->u32ShuntRes[INA3221_CH1];
    INA3221_SetShuntSumAlertLimit(pHandle, s16ShuntuV);
}

void INA3221_SetWarnAlertLatchEnable(i2c_ina3221_t* pHandle, bool bEnable)
{
    INA3221_WriteWordBits(pHandle, REG_MASK_ENABLE, 11, 1, bEnable ? 1 : 0);  // warn_alert_latch_en
}

void INA3221_SetCritAlertLatchEnable(i2c_ina3221_t* pHandle, bool bEnable)
{
    INA3221_WriteWordBits(pHandle, REG_MASK_ENABLE, 10, 1, bEnable ? 1 : 0);  // crit_alert_latch_en
}

void INA3221_ReadFlags(i2c_ina3221_t* pHandle)
{
    INA3221_ReadWord(pHandle, REG_MASK_ENABLE, &pHandle->u16MaskenReg);
}

bool INA3221_GetTimingCtrlAlertFlag(i2c_ina3221_t* pHandle)
{
    return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->timing_ctrl_alert;
}

bool INA3221_GetPwrValidAlertFlag(i2c_ina3221_t* pHandle)
{
    return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->pwr_valid_alert;
}

bool INA3221_GetCurrentSumAlertFlag(i2c_ina3221_t* pHandle)
{
    return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->shunt_sum_alert;
}

bool INA3221_GetConversionReadyFlag(i2c_ina3221_t* pHandle)
{
    return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->conv_ready;
}

uint16_t INA3221_GetManufID(i2c_ina3221_t* pHandle)
{
    uint16_t u16ID;
    INA3221_ReadWord(pHandle, REG_MANUF_ID, &u16ID);
    return u16ID;
}

uint16_t INA3221_GetDieID(i2c_ina3221_t* pHandle)
{
    uint16_t u16ID;
    INA3221_ReadWord(pHandle, REG_DIE_ID, &u16ID);
    return u16ID;
}

void INA3221_SetChannelEnable(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, bool bEnable)
{
    switch (eChannel)
    {
        case INA3221_CH1:
        {
            INA3221_WriteWordBits(pHandle, REG_CONF, 14, 1, bEnable ? 1 : 0);  // ch1_en
            break;
        }
        case INA3221_CH2:
        {
            INA3221_WriteWordBits(pHandle, REG_CONF, 13, 1, bEnable ? 1 : 0);  // ch2_en
            break;
        }
        case INA3221_CH3:
        {
            INA3221_WriteWordBits(pHandle, REG_CONF, 12, 1, bEnable ? 1 : 0);  // ch3_en
            break;
        }
    }
}

void INA3221_SetWarnAlertShuntLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32VoltageuV)
{
    uint8_t u8MemAddr;
    int16_t s16Data = 0;

    switch (eChannel)
    {
        case INA3221_CH1:
        {
            u8MemAddr = REG_CH1_WARNING_ALERT_LIM;
            break;
        }
        case INA3221_CH2:
        {
            u8MemAddr = REG_CH2_WARNING_ALERT_LIM;
            break;
        }
        case INA3221_CH3:
        {
            u8MemAddr = REG_CH3_WARNING_ALERT_LIM;
            break;
        }
        default: return;
    }

    s16Data = s32VoltageuV / 5;
    INA3221_WriteWord(pHandle, u8MemAddr, s16Data);
}

void INA3221_SetCritAlertShuntLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32VoltageuV)
{
    uint8_t u8MemAddr;
    int16_t s16Data = 0;

    switch (eChannel)
    {
        case INA3221_CH1:
        {
            u8MemAddr = REG_CH1_CRIT_ALERT_LIM;
            break;
        }
        case INA3221_CH2:
        {
            u8MemAddr = REG_CH2_CRIT_ALERT_LIM;
            break;
        }
        case INA3221_CH3:
        {
            u8MemAddr = REG_CH3_CRIT_ALERT_LIM;
            break;
        }
        default: return;
    }

    s16Data = s32VoltageuV / 5;
    INA3221_WriteWord(pHandle, u8MemAddr, s16Data);
}

void INA3221_SetWarnAlertCurrentLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32CurrentmA)
{
    int32_t s32ShuntuV = s32CurrentmA * (int32_t)pHandle->u32ShuntRes[eChannel];
    INA3221_SetWarnAlertShuntLimit(pHandle, eChannel, s32ShuntuV);
}

void INA3221_SetCritAlertCurrentLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32CurrentmA)
{
    int32_t s32ShuntuV = s32CurrentmA * (int32_t)pHandle->u32ShuntRes[eChannel];
    INA3221_SetCritAlertShuntLimit(pHandle, eChannel, s32ShuntuV);
}

void INA3221_SetCurrentSumEnable(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, bool bEnable)
{
    switch (eChannel)
    {
        case INA3221_CH1:
        {
            INA3221_WriteWordBits(pHandle, REG_MASK_ENABLE, 14, 1, bEnable ? 1 : 0);  // shunt_sum_en_ch1
            break;
        }
        case INA3221_CH2:
        {
            INA3221_WriteWordBits(pHandle, REG_MASK_ENABLE, 13, 1, bEnable ? 1 : 0);  // shunt_sum_en_ch2
            break;
        }
        case INA3221_CH3:
        {
            INA3221_WriteWordBits(pHandle, REG_MASK_ENABLE, 12, 1, bEnable ? 1 : 0);  // shunt_sum_en_ch3
            break;
        }
        default: return;
    }
}

int32_t INA3221_GetShuntVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel)
{
    int32_t  s32Res;
    uint8_t  u8MemAddr;
    uint16_t u16Data;

    switch (eChannel)
    {
        case INA3221_CH1:
        {
            u8MemAddr = REG_CH1_SHUNTV;
            break;
        }
        case INA3221_CH2:
        {
            u8MemAddr = REG_CH2_SHUNTV;
            break;
        }
        case INA3221_CH3:
        {
            u8MemAddr = REG_CH3_SHUNTV;
            break;
        }
        default: return 0;
    }

    INA3221_ReadWord(pHandle, u8MemAddr, &u16Data);

    s32Res = (int16_t)u16Data;
    s32Res *= 5;  // 1 LSB = 5uV

    return s32Res;
}

bool INA3221_GetWarnAlertFlag(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel)
{
    switch (eChannel)
    {
        case INA3221_CH1:
        {
            return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->warn_alert_ch1;
        }
        case INA3221_CH2:
        {
            return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->warn_alert_ch2;
        }
        case INA3221_CH3:
        {
            return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->warn_alert_ch3;
        }
        default:
        {
            return false;
        }
    }
}

bool INA3221_GetCritAlertFlag(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel)
{
    switch (eChannel)
    {
        case INA3221_CH1:
        {
            return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->crit_alert_ch1;
        }
        case INA3221_CH2:
        {
            return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->crit_alert_ch2;
        }
        case INA3221_CH3:
        {
            return ((ina3221_masken_reg_t*)&pHandle->u16MaskenReg)->crit_alert_ch3;
        }
        default:
        {
            return false;
        }
    }
}

int32_t INA3221_EstimateOffsetVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, uint32_t u32BusV)
{
    float32_t bias_in    = 10.0;                                     // Input bias current at IN– in uA
    float32_t r_in       = 0.670;                                    // Input resistance at IN– in MOhm
    uint32_t  adc_step   = 40;                                       // smallest shunt ADC step in uV
    float32_t shunt_res  = pHandle->u32ShuntRes[eChannel] / 1000.0;  // convert to Ohm
    float32_t filter_res = pHandle->u32FilterRes[eChannel];
    int32_t   offset     = 0.0;
    float32_t reminder;

    offset = (shunt_res + filter_res) * (u32BusV / r_in + bias_in) - bias_in * filter_res;

    // Round the offset to the closest shunt ADC value
    reminder = offset % adc_step;
    if (reminder < adc_step / 2)
    {
        offset -= reminder;
    }
    else
    {
        offset += adc_step - reminder;
    }

    return offset;
}

float32_t INA3221_GetCurrent(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel)
{
    int32_t   shunt_uV  = 0;
    float32_t current_A = 0;

    shunt_uV  = INA3221_GetShuntVoltage(pHandle, eChannel);
    current_A = shunt_uV / (int32_t)pHandle->u32ShuntRes[eChannel] / 1000.0;
    return current_A;
}

float32_t INA3221_GetCurrentCompensated(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel)
{
    int32_t   shunt_uV  = 0;
    int32_t   bus_V     = 0;
    float32_t current_A = 0.0;
    int32_t   offset_uV = 0;

    shunt_uV  = INA3221_GetShuntVoltage(pHandle, eChannel);
    bus_V     = INA3221_GetVoltage(pHandle, eChannel);
    offset_uV = INA3221_EstimateOffsetVoltage(pHandle, eChannel, bus_V);

    current_A = (shunt_uV - offset_uV) / (int32_t)pHandle->u32ShuntRes[eChannel] / 1000.0;

    return current_A;
}

float32_t INA3221_GetVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel)
{
    float32_t f32VoltageV = 0.0f;
    uint8_t   u8MemAddr;
    uint16_t  u16Data = 0;

    switch (eChannel)
    {
        case INA3221_CH1:
        {
            u8MemAddr = REG_CH1_BUSV;
            break;
        }
        case INA3221_CH2:
        {
            u8MemAddr = REG_CH2_BUSV;
            break;
        }
        case INA3221_CH3:
        {
            u8MemAddr = REG_CH3_BUSV;
            break;
        }
        default: return 0.f;
    }

    INA3221_ReadWord(pHandle, u8MemAddr, &u16Data);

    f32VoltageV = u16Data / 1000.0f;

    return f32VoltageV;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void INA3221_Test(i2c_mst_t* hI2C)
{
    i2c_ina3221_t ina3221 = {
        .hI2C         = hI2C,
        .u8SlvAddr    = INA3221_ADDRESS_GND,
        .u32ShuntRes  = {10, 10, 10}, // Ohm
        .u32FilterRes = {0,  0,  0 }, // Ohm
    };

    INA3221_Init(&ina3221);
    INA3221_Reset(&ina3221);

    INA3221_SetModeContinious(&ina3221);  // 连续模式
    INA3221_SetChannelEnable(&ina3221, INA3221_CH3, true);

    while (1)
    {
        float32_t af32Current[3];
        float32_t af32CurrentCompensated[3];
        float32_t af32Voltage[3];

        af32Current[0]            = INA3221_GetCurrent(&ina3221, INA3221_CH1);
        af32CurrentCompensated[0] = INA3221_GetCurrentCompensated(&ina3221, INA3221_CH1);
        af32Voltage[0]            = INA3221_GetVoltage(&ina3221, INA3221_CH1);

        af32Current[1]            = INA3221_GetCurrent(&ina3221, INA3221_CH2);
        af32CurrentCompensated[1] = INA3221_GetCurrentCompensated(&ina3221, INA3221_CH2);
        af32Voltage[1]            = INA3221_GetVoltage(&ina3221, INA3221_CH2);

        af32Current[2]            = INA3221_GetCurrent(&ina3221, INA3221_CH3);
        af32CurrentCompensated[2] = INA3221_GetCurrentCompensated(&ina3221, INA3221_CH3);
        af32Voltage[2]            = INA3221_GetVoltage(&ina3221, INA3221_CH3);

        LOGI("Channel 1:");
        LOGI(" - Current: %.3f A", af32Current[0]);
        LOGI(" - Compensated current: %.3f", af32CurrentCompensated[0]);
        LOGI(" - Voltage: %.3f V", af32Voltage[0]);

        LOGI("Channel 2:");
        LOGI(" - Current: %.3f A", af32Current[1]);
        LOGI(" - Compensated current: %.3f", af32CurrentCompensated[1]);
        LOGI(" - Voltage: %.3f V", af32Voltage[1]);

        LOGI("Channel 3:");
        LOGI(" - Current: %.3f A", af32Current[2]);
        LOGI(" - Compensated current: %.3f", af32CurrentCompensated[2]);
        LOGI(" - Voltage: %.3f V", af32Voltage[2]);

        LOGI("-------------------------------------");

        PeriodicTask(5 * UNIT_S, {
            LOGI("[trg]");
            INA3221_SetModeTriggered(&ina3221);  // 切入触发模式, 触发采样
        });

        DelayBlockMs(1000);
    }
}

#endif
