#ifndef __I2C_INA3221_H__
#define __I2C_INA3221_H__

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define INA3221_ADDRESS_GND 0x40  // A0 pin -> GND
#define INA3221_ADDRESS_VCC 0x41  // A0 pin -> VCC
#define INA3221_ADDRESS_SDA 0x42  // A0 pin -> SDA
#define INA3221_ADDRESS_SCL 0x43  // A0 pin -> SCL

// Channels
typedef enum {
    INA3221_CH1 = 0,
    INA3221_CH2,
    INA3221_CH3,
    INA3221_CH_NUM
} ina3221_ch_e;

// Conversion times
typedef enum {
    REG_CONF_CT_140US = 0,
    REG_CONF_CT_204US,
    REG_CONF_CT_332US,
    REG_CONF_CT_588US,
    REG_CONF_CT_1100US,
    REG_CONF_CT_2116US,
    REG_CONF_CT_4156US,
    REG_CONF_CT_8244US
} ina3221_conv_time_e;

// Averaging modes
typedef enum {
    REG_CONF_AVG_1 = 0,
    REG_CONF_AVG_4,
    REG_CONF_AVG_16,
    REG_CONF_AVG_64,
    REG_CONF_AVG_128,
    REG_CONF_AVG_256,
    REG_CONF_AVG_512,
    REG_CONF_AVG_1024
} ina3221_avg_mode_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;

    // Shunt resistance in mOhm
    __IN uint32_t u32ShuntRes[INA3221_CH_NUM];

    // Series filter resistance in Ohm
    __IN uint32_t u32FilterRes[INA3221_CH_NUM];

    // Value of Mask/Enable register. ( set by INA3221_ReadFlags() )
    uint16_t u16MaskenReg;
} i2c_ina3221_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

// Initializes INA3221
status_t INA3221_Init(i2c_ina3221_t* pHandle);

// Resets INA3221
void INA3221_Reset(i2c_ina3221_t* pHandle);

// Sets operating mode to power-down
void INA3221_SetModePowerDown(i2c_ina3221_t* pHandle);

// Sets operating mode to continious
void INA3221_SetModeContinious(i2c_ina3221_t* pHandle);

// Sets operating mode to triggered (single-shot)
void INA3221_SetModeTriggered(i2c_ina3221_t* pHandle);

// Enables/Disables shunt-voltage measurement
void INA3221_SetShuntMeasEnable(i2c_ina3221_t* pHandle, bool bEnable);

// Enables/Disables bus-voltage measurement
void INA3221_SetBusMeasEnable(i2c_ina3221_t* pHandle, bool bEnable);

// Sets averaging mode. Sets number of samples that are collected
// and averaged togehter.
void INA3221_SetAveragingMode(i2c_ina3221_t* pHandle, ina3221_avg_mode_e eAvgMode);

// Sets bus-voltage conversion time.
void INA3221_SetBusConversionTime(i2c_ina3221_t* pHandle, ina3221_conv_time_e eConvTime);

// Sets shunt-voltage conversion time.
void INA3221_SetShuntConversionTime(i2c_ina3221_t* pHandle, ina3221_conv_time_e eConvTime);

// Sets power-valid upper-limit voltage. The power-valid condition
// is reached when all bus-voltage channels exceed the value set.
// When the powervalid condition is met, the PV alert pin asserts high.
void INA3221_SetPwrValidUpLimit(i2c_ina3221_t* pHandle, int16_t s16VoltagemV);

// Sets power-valid lower-limit voltage. If any bus-voltage channel drops
// below the power-valid lower-limit, the PV alert pin pulls low.
void INA3221_SetPwrValidLowLimit(i2c_ina3221_t* pHandle, int16_t s16VoltagemV);

// Sets the value that is compared to the Shunt-Voltage Sum register value
// following each completed cycle of all selected channels to detect
// for system overcurrent events.
void INA3221_SetShuntSumAlertLimit(i2c_ina3221_t* pHandle, int32_t s16VoltagemV);

// Sets the current value that is compared to the sum all currents.
// This function is a helper for INA3221_SetShuntSumAlertLim(). It onverts current
// value to shunt voltage value.
void INA3221_SetCurrentSumAlertLimit(i2c_ina3221_t* pHandle, int32_t s32CurrentmA);

// Enables/Disables warning alert latch.
void INA3221_SetWarnAlertLatchEnable(i2c_ina3221_t* pHandle, bool bEnable);

// Enables/Disables critical alert latch.
void INA3221_SetCritAlertLatchEnable(i2c_ina3221_t* pHandle, bool bEnable);

// Reads flags from Mask/Enable register.
// When Mask/Enable register is read, flags are cleared.
// Use INA3221_GetTimingCtrlAlertFlag(), INA3221_GetPwrValidAlertFlag(),
// INA3221_GetCurrentSumAlertFlag() and INA3221_GetConvReadyFlag() to get flags after
// INA3221_ReadFlags() is called.
void INA3221_ReadFlags(i2c_ina3221_t* pHandle);

// Gets timing-control-alert flag indicator.
bool INA3221_GetTimingCtrlAlertFlag(i2c_ina3221_t* pHandle);

// Gets power-valid-alert flag indicator.
bool INA3221_GetPwrValidAlertFlag(i2c_ina3221_t* pHandle);

// Gets summation-alert flag indicator.
bool INA3221_GetCurrentSumAlertFlag(i2c_ina3221_t* pHandle);

// Gets Conversion-ready flag.
bool INA3221_GetConversionReadyFlag(i2c_ina3221_t* pHandle);

// Gets manufacturer ID.
// Should read 0x5449.
uint16_t INA3221_GetManufID(i2c_ina3221_t* pHandle);

// Gets die ID.
// Should read 0x3220.
uint16_t INA3221_GetDieID(i2c_ina3221_t* pHandle);

// Enables/Disables channel measurements
void INA3221_SetChannelEnable(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, bool bEnable);

// Sets warning alert shunt voltage limit
void INA3221_SetWarnAlertShuntLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32VoltageuV);

// Sets critical alert shunt voltage limit
void INA3221_SetCritAlertShuntLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32VoltageuV);

// Sets warning alert current limit
void INA3221_SetWarnAlertCurrentLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32CurrentmA);

// Sets critical alert current limit
void INA3221_SetCritAlertCurrentLimit(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, int32_t s32CurrentmA);

// Includes/Excludes channel to fill Shunt-Voltage Sum register.
void INA3221_SetCurrentSumEnable(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, bool bEnable);

// Gets shunt voltage in uV.
int32_t INA3221_GetShuntVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel);

// Gets warning alert flag.
bool INA3221_GetWarnAlertFlag(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel);

// Gets critical alert flag.
bool INA3221_GetCritAlertFlag(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel);

// Estimates offset voltage added by the series filter resitors
int32_t INA3221_EstimateOffsetVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel, uint32_t u32BusVoltage);

// Gets current in A.
float32_t INA3221_GetCurrent(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel);

// Gets current compensated with calculated offset voltage.
float32_t INA3221_GetCurrentCompensated(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel);

// Gets bus voltage in V.
float32_t INA3221_GetVoltage(i2c_ina3221_t* pHandle, ina3221_ch_e eChannel);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void INA3221_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif