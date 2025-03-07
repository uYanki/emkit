#ifndef __I2C_MPU6050_H__
#define __I2C_MPU6050_H__

#include "i2cmst.h"
#include "delay.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_MPU6050_KALMAN_FILTER_SW 1  // 卡尔曼滤波器

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define MPU6050_ADDRESS_LOW  0x68
#define MPU6050_ADDRESS_HIGH 0x69

/**
 * @brief Accelerometer sensitivity
 */
typedef enum {
    MPU6050_ACCELEROMETER_2G  = 0x00, /*!< Range is +- 2G */
    MPU6050_ACCELEROMETER_4G  = 0x01, /*!< Range is +- 4G */
    MPU6050_ACCELEROMETER_8G  = 0x02, /*!< Range is +- 8G */
    MPU6050_ACCELEROMETER_16G = 0x03  /*!< Range is +- 16G */
} mpu6050_accel_sen_e;

/**
 * @brief Gyroscope sensitivity
 */
typedef enum {
    MPU6050_GYROSCOPE_250S  = 0x00, /*!< Range is +- 250 degrees/s */
    MPU6050_GYROSCOPE_500S  = 0x01, /*!< Range is +- 500 degrees/s */
    MPU6050_GYROSCOPE_1000S = 0x02, /*!< Range is +- 1000 degrees/s */
    MPU6050_GYROSCOPE_2000S = 0x03  /*!< Range is +- 2000 degrees/s */
} mpu6050_gyro_sen_e;

/**
 * @brief Kalman filter
 */
#if CONFIG_MPU6050_KALMAN_FILTER_SW

typedef struct {
    float32_t Q_angle;
    float32_t Q_bias;
    float32_t R_measure;
    float32_t angle;
    float32_t bias;
    float32_t P[2][2];
} mpu6050_kalman_filter_t;

#endif

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr; /*!< I2C address of device */

    float32_t _f32AccelSen; /*!< Accelerometer corrector from raw data to "g" */
    float32_t _f32GyroSen;  /*!< Gyroscope corrector from raw data to "degrees/s" */

#if CONFIG_MPU6050_KALMAN_FILTER_SW
    mpu6050_kalman_filter_t _KalmanFilterX;  // roll
    mpu6050_kalman_filter_t _KalmanFilterY;  // picth
    tick_t                  _LastTime;
#endif

    /**
     * @brief RawData, it is set by GetRawData()
     */
    __OUT int16_t s16AccelX; /*!< Raw Accelerometer X */
    __OUT int16_t s16AccelY; /*!< Raw Accelerometer Y */
    __OUT int16_t s16AccelZ; /*!< Raw Accelerometer Z */
    __OUT int16_t s16GyroX;  /*!< Raw Gyroscope X */
    __OUT int16_t s16GyroY;  /*!< Raw Gyroscope Y */
    __OUT int16_t s16GyroZ;  /*!< Raw Gyroscope Z */

    /**
     * @brief FloatData, it is set by ConvRawDataToFloatData()
     */
    __OUT float32_t f32AccelX; /*!< Accelerometer X */
    __OUT float32_t f32AccelY; /*!< Accelerometer Y */
    __OUT float32_t f32AccelZ; /*!< Accelerometer Z */
    __OUT float32_t f32GyroX;  /*!< Gyroscope X */
    __OUT float32_t f32GyroY;  /*!< Gyroscope Y */
    __OUT float32_t f32GyroZ;  /*!< Gyroscope Z */
    __OUT float32_t f32Temp;   /*!< Temperature */

    /**
     * @brief Angle, it is set by GetAngle()
     */
    __OUT float32_t f32Pitch; /*!< 俯仰角 */
    __OUT float32_t f32Roll;  /*!< 翻滚角 */

} i2c_mpu6050_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t MPU6050_Init(i2c_mpu6050_t* pHandle, mpu6050_accel_sen_e eAccelSen, mpu6050_gyro_sen_e eGyroSen);
status_t MPU6050_GetRawData(i2c_mpu6050_t* pHandle);
void     MPU6050_ConvRawDataToFloatData(i2c_mpu6050_t* pHandle);

void MPU6050_GetAngle_Euler(i2c_mpu6050_t* pHandle);
#if CONFIG_MPU6050_KALMAN_FILTER_SW
void MPU6050_GetAngle_Kalman(i2c_mpu6050_t* pHandle);
#endif

#if CONFIG_DEMOS_SW
void MPU6050_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif  // !__I2C_MPU6050_H__
