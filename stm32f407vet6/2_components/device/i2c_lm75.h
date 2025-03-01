#ifndef __I2C_LM75_H__
#define __I2C_LM75_H__

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief lm75 address (A2A1A0)
 */
#define LM75_ADDRESS_A000 0x48
#define LM75_ADDRESS_A001 0x49
#define LM75_ADDRESS_A010 0x4A
#define LM75_ADDRESS_A011 0x4B
#define LM75_ADDRESS_A100 0x4C
#define LM75_ADDRESS_A101 0x4D
#define LM75_ADDRESS_A110 0x4E
#define LM75_ADDRESS_A111 0x4F

// bit[0] mode
typedef enum {
    LM75_MODE_NORMAL   = 0,
    LM75_MODE_SHUTDOWN = 1,
} lm75_device_mode_e;

// bit[1] os operation
typedef enum {
    LM75_OS_OPERATION_COMPARATOR = 0,  // 比较器
    LM75_OS_OPERATION_INTERRUPT  = 1,  // 中断
} lm75_os_operation_mode_e;

// bit[2] os polarity / os active level
typedef enum {
    LM75_OS_POLARITY_LOW  = 0,
    LM75_OS_POLARITY_HIGH = 1,
} lm75_os_polarity_e;

// bit[4,3] fault queue
typedef enum {
    LM75_FAULT_QUEUE_1 = 0,
    LM75_FAULT_QUEUE_2 = 1,
    LM75_FAULT_QUEUE_3 = 2,
    LM75_FAULT_QUEUE_6 = 3,
} lm75_fault_queue_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;
} i2c_lm75_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t LM75_Init(i2c_lm75_t* pHandle);

status_t LM75_ReadTemp(i2c_lm75_t* pHandle, float32_t* pTemperature);

status_t LM75_SetHysteresis(i2c_lm75_t* pHandle, float32_t f32Hysteresis);
status_t LM75_GetHysteresis(i2c_lm75_t* pHandle, float32_t* pf32Hysteresis);

status_t LM75_SetOverTempThold(i2c_lm75_t* pHandle, float32_t f32Threshold);
status_t LM75_GetOverTempThold(i2c_lm75_t* pHandle, float32_t* pf32Threshold);

status_t LM75_SetDeviceMode(i2c_lm75_t* pHandle, lm75_device_mode_e eMode);
status_t LM75_GetDeviceMode(i2c_lm75_t* pHandle, lm75_device_mode_e* peMode);

status_t LM75_SetOperMode(i2c_lm75_t* pHandle, lm75_os_operation_mode_e eMode);
status_t LM75_GetOperMode(i2c_lm75_t* pHandle, lm75_os_operation_mode_e* peMode);

status_t LM75_SetOutputSignalPolarity(i2c_lm75_t* pHandle, lm75_os_polarity_e ePolarity);
status_t LM75_GetOutputSignalPolarity(i2c_lm75_t* pHandle, lm75_os_polarity_e* pePolarity);

status_t LM75_SetFaultQueue(i2c_lm75_t* pHandle, lm75_fault_queue_e eFaultQueue);
status_t LM75_GetFaultQueue(i2c_lm75_t* pHandle, lm75_fault_queue_e* peFaultQueue);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void LM75_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
