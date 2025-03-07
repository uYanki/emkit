#ifndef __I2C_INA219_H__
#define __I2C_INA219_H__

#include "i2cmst.h"

/**
 * @ref https://kkgithub.com/jarzebski/Arduino-INA219
 */

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define INA219_ADDRESS_0 0x40  // A0 = GND, A1 = GND
#define INA219_ADDRESS_1 0x41  // A0 = VS+, A1 = GND
#define INA219_ADDRESS_2 0x42  // A0 = SDA, A1 = GND
#define INA219_ADDRESS_3 0x43  // A0 = SCL, A1 = GND
#define INA219_ADDRESS_4 0x44  // A0 = GND, A1 = VS+
#define INA219_ADDRESS_5 0x45  // A0 = VS+, A1 = VS+
#define INA219_ADDRESS_6 0x46  // A0 = SDA, A1 = VS+
#define INA219_ADDRESS_7 0x47  // A0 = SCL, A1 = VS+
#define INA219_ADDRESS_8 0x48  // A0 = GND, A1 = SDA
#define INA219_ADDRESS_9 0x49  // A0 = VS+, A1 = SDA
#define INA219_ADDRESS_A 0x4A  // A0 = SDA, A1 = SDA
#define INA219_ADDRESS_B 0x4B  // A0 = SCL, A1 = SDA
#define INA219_ADDRESS_C 0x4C  // A0 = GND, A1 = SCL
#define INA219_ADDRESS_D 0x4D  // A0 = VS+, A1 = SCL
#define INA219_ADDRESS_E 0x4E  // A0 = SDA, A1 = SCL
#define INA219_ADDRESS_F 0x4F  // A0 = SCL, A1 = SCL

/**
 * @brief 检测电压范围
 */
typedef enum {
    INA219_BUS_RANGE_16V = 0b0,  // ±16V
    INA219_BUS_RANGE_32V = 0b1,  // ±32V
} ina219_bus_range_e;

/**
 * @brief PGA gain
 */
typedef enum {
    INA219_GAIN_40MV  = 0b00,  // ±40 mV
    INA219_GAIN_80MV  = 0b01,  // ±80 mV
    INA219_GAIN_160MV = 0b10,  // ±160 mV
    INA219_GAIN_320MV = 0b11,  // ±320 mV
} ina219_gain_e;

typedef enum {
    INA219_BUS_RES_9BIT  = 0b0000,
    INA219_BUS_RES_10BIT = 0b0001,
    INA219_BUS_RES_11BIT = 0b0010,
    INA219_BUS_RES_12BIT = 0b0011,
} ina219_bus_res_e;

typedef enum {
    INA219_SHUNT_RES_9_BIT_1_SAMPLES    = 0b0000,  // 9 bit / 1 samples
    INA219_SHUNT_RES_10_BIT_1_SAMPLES   = 0b0001,  // 10 bit / 1 samples
    INA219_SHUNT_RES_11_BIT_1_SAMPLES   = 0b0010,  // 11 bit / 1 samples
    INA219_SHUNT_RES_12_BIT_1_SAMPLES   = 0b0011,  // 12 bit / 1 samples
    INA219_SHUNT_RES_12_BIT_2_SAMPLES   = 0b1001,  // 12 bit / 2 samples
    INA219_SHUNT_RES_12_BIT_4_SAMPLES   = 0b1010,  // 12 bit / 4 samples
    INA219_SHUNT_RES_12_BIT_8_SAMPLES   = 0b1011,  // 12 bit / 8 samples
    INA219_SHUNT_RES_12_BIT_16_SAMPLES  = 0b1100,  // 12 bit / 16 samples
    INA219_SHUNT_RES_12_BIT_32_SAMPLES  = 0b1101,  // 12 bit / 32 samples
    INA219_SHUNT_RES_12_BIT_64_SAMPLES  = 0b1110,  // 12 bit / 64 samples
    INA219_SHUNT_RES_12_BIT_128_SAMPLES = 0b1111,  // 12 bit / 128 samples
} ina219_shunt_res_e;

typedef enum {
    INA219_MODE_POWER_DOWN     = 0b000,  // power down
    INA219_MODE_SHUNT_TRIG     = 0b001,  // shunt voltage triggered
    INA219_MODE_BUS_TRIG       = 0b010,  // bus voltage triggered
    INA219_MODE_SHUNT_BUS_TRIG = 0b011,  // shunt and bus triggered
    INA219_MODE_ADC_OFF        = 0b100,  // adc off
    INA219_MODE_SHUNT_CONT     = 0b101,  // shunt voltage continuous
    INA219_MODE_BUS_CONT       = 0b110,  // bus voltage continuous
    INA219_MODE_SHUNT_BUS_CONT = 0b111,  // shunt and bus voltage continuous
} ina219_mode_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;
    __IN float32_t  f32ShuntOhm;     // 采样电阻阻值
    float32_t       _f32CurrentLSB;  // 电流分辨率
    float32_t       _f32PowerLSB;    // 功率辨率
    float32_t       _f32VshuntMax;   // 分流电阻两端的最大电压
    float32_t       _f32VbusMax;     // 总线最大电压(IN-~GND间的压差)
} i2c_ina219_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t INA219_Init(i2c_ina219_t* pHandle);
status_t INA219_Configure(i2c_ina219_t* pHandle, ina219_bus_range_e eRange, ina219_gain_e eGain, ina219_bus_res_e eBusRes, ina219_shunt_res_e eShuntRes, ina219_mode_e eMode);

float32_t INA219_GetMaxCurrent(i2c_ina219_t* pHandle);       // A
float32_t INA219_GetMaxShuntVoltage(i2c_ina219_t* pHandle);  // V
float32_t INA219_GetMaxPower(i2c_ina219_t* pHandle);         // W
float32_t INA219_ReadBusPower(i2c_ina219_t* pHandle);        // W
float32_t INA219_ReadShuntCurrent(i2c_ina219_t* pHandle);    // A
float32_t INA219_ReadShuntVoltage(i2c_ina219_t* pHandle);    // V
float32_t INA219_ReadBusVoltage(i2c_ina219_t* pHandle);      // V

ina219_bus_range_e INA219_GetRange(i2c_ina219_t* pHandle);
ina219_gain_e      INA219_GetGain(i2c_ina219_t* pHandle);
ina219_bus_res_e   INA219_GetBusRes(i2c_ina219_t* pHandle);
ina219_shunt_res_e INA219_GetShuntRes(i2c_ina219_t* pHandle);
ina219_mode_e      INA219_GetMode(i2c_ina219_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void INA219_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
