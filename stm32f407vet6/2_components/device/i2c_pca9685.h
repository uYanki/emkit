#ifndef __I2C_PCA9685_H__
#define __I2C_PCA9685_H__

/**
 * @brief 16通道12位PWM发生器
 */

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_PCA9685_SERVO_CONTROL_SW 0  // SG90 舵机控制

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define PCA9685_ADDRESS_A00000  0x40  // 单从机地址
#define PCA9685_ADDRESS_GENERIC 0x70  // 通用地址

typedef enum {
    PCA9685_CHANNEL_0  = 0x00,
    PCA9685_CHANNEL_1  = 0x01,
    PCA9685_CHANNEL_2  = 0x02,
    PCA9685_CHANNEL_3  = 0x03,
    PCA9685_CHANNEL_4  = 0x04,
    PCA9685_CHANNEL_5  = 0x05,
    PCA9685_CHANNEL_6  = 0x06,
    PCA9685_CHANNEL_7  = 0x07,
    PCA9685_CHANNEL_8  = 0x08,
    PCA9685_CHANNEL_9  = 0x09,
    PCA9685_CHANNEL_10 = 0x0A,
    PCA9685_CHANNEL_11 = 0x0B,
    PCA9685_CHANNEL_12 = 0x0C,
    PCA9685_CHANNEL_13 = 0x0D,
    PCA9685_CHANNEL_14 = 0x0E,
    PCA9685_CHANNEL_15 = 0x0F,
} pca9685_channel_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;
#if CONFIG_PCA9685_SERVO_CONTROL_SW
    float32_t _f32PeriodUs;  // private
#endif
} i2c_pca9685_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t PCA9685_Init(i2c_pca9685_t* pHandle);

/**
 * @brief 设置占空比
 * @note  PwmDuty = (u16PulseOff-u16PulseOn)/4096
 * @param[in] u16PulseOn  脉冲起始时间 [0,u16PulseOff]
 * @param[in] u16PulseOff 脉冲结束时间 [u16PulseOff,4095]
 */
status_t PCA9685_SetDuty(i2c_pca9685_t* pHandle, pca9685_channel_e eChannel, uint16_t u16PulseOn, uint16_t u16PulseOff);
status_t PCA9685_SetAllDuty(i2c_pca9685_t* pHandle, uint16_t u16PulseOn, uint16_t u16PulseOff);

/**
 * @brief 设置频率
 */
status_t PCA9685_SetFreq(i2c_pca9685_t* pHandle, float32_t f32Freq);

#if CONFIG_PCA9685_SERVO_CONTROL_SW
/**
 * @brief 设置舵机角度
 * @note  需提前设置 PWM 的工作频率为 50Hz/60Hz
 * @param[in] u8Angle 舵机角度, 取值 [0,180]
 */
status_t PCA9685_SetAngle_Servo(i2c_pca9685_t* pHandle, pca9685_channel_e eChannel, uint8_t u8Angle);
status_t PCA9685_SetAngle_AllServo(i2c_pca9685_t* pHandle, uint8_t u8Angle);
#endif

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void PCA9685_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
