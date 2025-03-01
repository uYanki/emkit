#ifndef __I2C_BH1750_H__
#define __I2C_BH1750_H__

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define BH1750_ADDRESS_LOW  0X23
#define BH1750_ADDRESS_HIGH 0X5C

/**
 * @note 测量耗时: 高分辨率 120ms~80ms, 低分辨率 16ms
 * @note 测量模式：单次测量, 完成后自动进入 PowerOff 状态
 *
 */
typedef enum {
    BH1750_CONTINUE_HIGH_RES   = 0x11,  // 连续测量, 0.5lx
    BH1750_CONTINUE_MEDIUM_RES = 0x10,  // 连续测量, 1lx
    BH1750_CONTINUE_LOW_RES    = 0x13,  // 连续测量, 4lx
    BH1750_ONESHOT_HIGH_RES    = 0x21,  // 单次测量, 0.5lx
    BH1750_ONESHOT_MEDIUM_RES  = 0x20,  // 单次测量, 1lx
    BH1750_ONESHOT_LOW_RES     = 0x23,  // 单次测量, 4lx
} bh1750_mode_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;

    bh1750_mode_e _eMeasureMode;   // 测量模式
    uint8_t       _u8Sensitivity;  // 灵敏度倍率

} i2c_bh1750_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t BH1750_Init(i2c_bh1750_t* pHandle);
status_t BH1750_PowerOn(i2c_bh1750_t* pHandle);
status_t BH1750_PowerOff(i2c_bh1750_t* pHandle);
status_t BH1750_Reset(i2c_bh1750_t* pHandle);
status_t BH1750_SetModeRes(i2c_bh1750_t* pHandle, bh1750_mode_e eMode);
status_t BH1750_SetSensitivity(i2c_bh1750_t* pHandle, uint8_t u8Sensitivity);
status_t BH1750_GetLux(i2c_bh1750_t* pHandle, float32_t* pf32Lux);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void BH1750_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
