#include "i2c_mpu6050.h"
#include <math.h>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mpu6050"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief registers
 */
#define REG_AUX_VDDIO         0x01
#define REG_SMPLRT_DIV        0x19
#define REG_CONFIG            0x1A
#define REG_GYRO_CONFIG       0x1B
#define REG_ACCEL_CONFIG      0x1C
#define REG_MOTION_THRESH     0x1F
#define REG_FIFO_EN           0x23
#define REG_INT_PIN_CFG       0x37
#define REG_INT_ENABLE        0x38
#define REG_INT_STATUS        0x3A
#define REG_ACCEL_XOUT_H      0x3B
#define REG_ACCEL_XOUT_L      0x3C
#define REG_ACCEL_YOUT_H      0x3D
#define REG_ACCEL_YOUT_L      0x3E
#define REG_ACCEL_ZOUT_H      0x3F
#define REG_ACCEL_ZOUT_L      0x40
#define REG_TEMP_OUT_H        0x41
#define REG_TEMP_OUT_L        0x42
#define REG_GYRO_XOUT_H       0x43
#define REG_GYRO_XOUT_L       0x44
#define REG_GYRO_YOUT_H       0x45
#define REG_GYRO_YOUT_L       0x46
#define REG_GYRO_ZOUT_H       0x47
#define REG_GYRO_ZOUT_L       0x48
#define REG_MOT_DETECT_STATUS 0x61
#define REG_SIGNAL_PATH_RESET 0x68
#define REG_MOT_DETECT_CTRL   0x69
#define REG_USER_CTRL         0x6A
#define REG_PWR_MGMT_1        0x6B
#define REG_PWR_MGMT_2        0x6C
#define REG_FIFO_COUNTH       0x72
#define REG_FIFO_COUNTL       0x73
#define REG_FIFO_R_W          0x74
#define REG_WHO_AM_I          0x75

/* Who I am register value */
#define MPU6050_I_AM 0x68

/**
 * @brief Accelerometer sensitivity
 */
#define MPU6050_ACCE_SENS_2G  16384.f /*!< Range is +- 2G */
#define MPU6050_ACCE_SENS_4G  8192.f  /*!< Range is +- 4G */
#define MPU6050_ACCE_SENS_8G  4096.f  /*!< Range is +- 8G */
#define MPU6050_ACCE_SENS_16G 2048.f  /*!< Range is +- 16G */

/**
 * @brief Gyroscope sensitivity
 */
#define MPU6050_GYRO_SENS_250S  131.f /*!< Range is +- 250 degrees/s */
#define MPU6050_GYRO_SENS_500S  65.5f /*!< Range is +- 500 degrees/s */
#define MPU6050_GYRO_SENS_1000S 32.8f /*!< Range is +- 1000 degrees/s */
#define MPU6050_GYRO_SENS_2000S 16.4f /*!< Range is +- 2000 degrees/s */

/**
 * @brief Temperature sensitivity and offset
 */
#define MPU6050_TEMP_SENSITIVE 340.f
#define MPU6050_TEMP_OFFSET    36.53f

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t MPU6050_Init(i2c_mpu6050_t* pHandle, mpu6050_accel_sen_e eAccelSen, mpu6050_gyro_sen_e eGyroSen)
{
#if CONFIG_MPU6050_KALMAN_FILTER_SW

    pHandle->_KalmanFilterX.Q_angle   = 0.001f;
    pHandle->_KalmanFilterX.Q_bias    = 0.003f;
    pHandle->_KalmanFilterX.R_measure = 0.03f;

    pHandle->_KalmanFilterY.Q_angle   = 0.001f;
    pHandle->_KalmanFilterY.Q_bias    = 0.003f;
    pHandle->_KalmanFilterY.R_measure = 0.03f;

    pHandle->_LastTime = 0;

#endif

    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // there is no device with valid slave address
    }

    /* Reset */

    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_PWR_MGMT_1, 0x80, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    DelayBlockMs(1000);

    /* Wakeup */
    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_PWR_MGMT_1, 0x00, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    /* Check who I am */

    uint8_t u8WhoAmI = 0x00;

    ERRCHK_RETURN(I2C_Master_ReadBlock(pHandle->hI2C, pHandle->u8SlvAddr, REG_WHO_AM_I, &u8WhoAmI, 1, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    if (u8WhoAmI != MPU6050_I_AM)
    {
        return ERR_NO_DEVICE;  // connected device with address is not MPU6050
    }

    /* Config accelerometer */
    ERRCHK_RETURN(I2C_Master_WriteByteBits(pHandle->hI2C, pHandle->u8SlvAddr, REG_ACCEL_CONFIG, 3, 2, eAccelSen, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    switch (eAccelSen)
    {
        default:
        case MPU6050_ACCELEROMETER_2G: pHandle->_f32AccelSen = MPU6050_ACCE_SENS_2G; break;
        case MPU6050_ACCELEROMETER_4G: pHandle->_f32AccelSen = MPU6050_ACCE_SENS_4G; break;
        case MPU6050_ACCELEROMETER_8G: pHandle->_f32AccelSen = MPU6050_ACCE_SENS_8G; break;
        case MPU6050_ACCELEROMETER_16G: pHandle->_f32AccelSen = MPU6050_ACCE_SENS_16G; break;
    }

    /* Config gyroscope */
    ERRCHK_RETURN(I2C_Master_WriteByteBits(pHandle->hI2C, pHandle->u8SlvAddr, REG_GYRO_CONFIG, 3, 2, eGyroSen, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    switch (eGyroSen)
    {
        default:
        case MPU6050_GYROSCOPE_250S: pHandle->_f32GyroSen = MPU6050_GYRO_SENS_250S; break;
        case MPU6050_GYROSCOPE_500S: pHandle->_f32GyroSen = MPU6050_GYRO_SENS_500S; break;
        case MPU6050_GYROSCOPE_1000S: pHandle->_f32GyroSen = MPU6050_GYRO_SENS_1000S; break;
        case MPU6050_GYROSCOPE_2000S: pHandle->_f32GyroSen = MPU6050_GYRO_SENS_2000S; break;
    }

    /* Sampling rate without frequency division */
    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_SMPLRT_DIV, 0x00, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    /* Lowpass filter: 256Hz~260Hz */
    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_CONFIG, 0x00, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    /* Disable fifo */
    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_FIFO_EN, 0x00, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    /* Disable interrupt */
    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_INT_ENABLE, 0x00, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    /* Disable i2c master */
    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_USER_CTRL, 0x00, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    /* Enable accel & gyro */
    ERRCHK_RETURN(I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, REG_PWR_MGMT_2, 0x00, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    return ERR_NONE;
}

status_t MPU6050_GetRawData(i2c_mpu6050_t* pHandle)
{
    uint8_t aRawData[14];

    /* Read full raw data, 14bytes */
    ERRCHK_RETURN(I2C_Master_ReadBlock(pHandle->hI2C, pHandle->u8SlvAddr, REG_ACCEL_XOUT_H, &aRawData[0], ARRAY_SIZE(aRawData), I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT));

    /* Accelerometer */
    pHandle->s16AccelX = (int16_t)((aRawData[0] << 8) | aRawData[1]);
    pHandle->s16AccelY = (int16_t)((aRawData[2] << 8) | aRawData[3]);
    pHandle->s16AccelZ = (int16_t)((aRawData[4] << 8) | aRawData[5]);

    /* Gyroscope  */
    pHandle->s16GyroX = (int16_t)((aRawData[8] << 8) | aRawData[9]);
    pHandle->s16GyroY = (int16_t)((aRawData[10] << 8) | aRawData[11]);
    pHandle->s16GyroZ = (int16_t)((aRawData[12] << 8) | aRawData[13]);

    /* Temperature */
    pHandle->f32Temp = (float32_t)(int16_t)(aRawData[6] << 8 | aRawData[7]) / MPU6050_TEMP_SENSITIVE + MPU6050_TEMP_OFFSET;

    return ERR_NONE;
}

void MPU6050_ConvRawDataToFloatData(i2c_mpu6050_t* pHandle)
{
    /* Accelerometer */
    pHandle->f32AccelX = (float32_t)(pHandle->s16AccelX) / pHandle->_f32AccelSen;
    pHandle->f32AccelY = (float32_t)(pHandle->s16AccelY) / pHandle->_f32AccelSen;
    pHandle->f32AccelZ = (float32_t)(pHandle->s16AccelZ) / pHandle->_f32AccelSen;

    /* Gyroscope  */
    pHandle->f32GyroX = (float32_t)(pHandle->s16GyroX) / pHandle->_f32GyroSen;
    pHandle->f32GyroY = (float32_t)(pHandle->s16GyroY) / pHandle->_f32GyroSen;
    pHandle->f32GyroZ = (float32_t)(pHandle->s16GyroZ) / pHandle->_f32GyroSen;
}

#if CONFIG_MPU6050_KALMAN_FILTER_SW

float32_t KalmanFilter_PredictAngle(mpu6050_kalman_filter_t* pKalman, float32_t newAngle, float32_t newRate, float32_t dt)
{
    float32_t rate = newRate - pKalman->bias;
    pKalman->angle += dt * rate;

    pKalman->P[0][0] += dt * (dt * pKalman->P[1][1] - pKalman->P[0][1] - pKalman->P[1][0] + pKalman->Q_angle);
    pKalman->P[0][1] -= dt * pKalman->P[1][1];
    pKalman->P[1][0] -= dt * pKalman->P[1][1];
    pKalman->P[1][1] += dt * pKalman->Q_bias;

    float32_t S = pKalman->P[0][0] + pKalman->R_measure;
    float32_t K[2];
    K[0] = pKalman->P[0][0] / S;
    K[1] = pKalman->P[1][0] / S;

    float32_t y = newAngle - pKalman->angle;
    pKalman->angle += K[0] * y;
    pKalman->bias += K[1] * y;

    float32_t P00_temp = pKalman->P[0][0];
    float32_t P01_temp = pKalman->P[0][1];

    pKalman->P[0][0] -= K[0] * P00_temp;
    pKalman->P[0][1] -= K[0] * P01_temp;
    pKalman->P[1][0] -= K[1] * P00_temp;
    pKalman->P[1][1] -= K[1] * P01_temp;

    return pKalman->angle;
};

void MPU6050_GetAngle_Kalman(i2c_mpu6050_t* pHandle)
{
    float32_t f32Roll, f32RollSqrt, f32Pitch;

    float32_t f32DeltaTimeMs;

    f32DeltaTimeMs     = (float32_t)(GetTickUs() - pHandle->_LastTime) / 1000000;
    pHandle->_LastTime = GetTickUs();

    f32RollSqrt = sqrt(pHandle->s16AccelX * pHandle->s16AccelX + pHandle->s16AccelZ * pHandle->s16AccelZ);

    if (f32RollSqrt != 0.0f)
    {
        f32Roll = atan(pHandle->s16AccelY / f32RollSqrt) * M_RAD2DEG;
    }

    f32Pitch = atan2(-pHandle->s16AccelX, pHandle->s16AccelZ) * M_RAD2DEG;

    if ((f32Pitch < -90.0f && pHandle->f32Pitch > 90.0f) || (f32Pitch > 90.0f && pHandle->f32Pitch < -90.0f))
    {
        pHandle->f32Pitch = pHandle->_KalmanFilterY.angle = f32Pitch;
    }
    else
    {
        pHandle->f32Pitch = KalmanFilter_PredictAngle(&pHandle->_KalmanFilterY, f32Pitch, pHandle->f32GyroY, f32DeltaTimeMs);
    }

    if (fabs(pHandle->f32Pitch) > 90.0f)
    {
        pHandle->f32GyroX = -pHandle->f32GyroX;
    }

    pHandle->f32Roll = KalmanFilter_PredictAngle(&pHandle->_KalmanFilterX, f32Roll, pHandle->f32GyroX, f32DeltaTimeMs);
}

#endif

void MPU6050_GetAngle_Euler(i2c_mpu6050_t* pHandle)
{
    pHandle->f32Pitch = -atan2(pHandle->f32AccelX, pHandle->f32AccelZ) * M_RAD2DEG;
    pHandle->f32Roll  = atan2(pHandle->f32AccelY, pHandle->f32AccelZ) * M_RAD2DEG;

    if (pHandle->f32AccelZ < 0)
    {
        if (pHandle->f32Roll > 90.f)
        {
            pHandle->f32Roll = 180.f - pHandle->f32Roll;
        }
        else if (pHandle->f32Roll < -90.f)
        {
            pHandle->f32Roll = -pHandle->f32Roll - 180.f;
        }
    }
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void MPU6050_Test(i2c_mst_t* hI2C)
{
    i2c_mpu6050_t mpu6050 = {
        .hI2C      = hI2C,
        .u8SlvAddr = MPU6050_ADDRESS_LOW,
    };

    MPU6050_Init(&mpu6050, MPU6050_ACCELEROMETER_2G, MPU6050_GYROSCOPE_250S);

    I2C_Master_Hexdump(mpu6050.hI2C, mpu6050.u8SlvAddr, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);

    while (1)
    {
        if (MPU6050_GetRawData(&mpu6050) == ERR_NONE)
        {
#if 1

            MPU6050_ConvRawDataToFloatData(&mpu6050);

            MPU6050_GetAngle_Euler(&mpu6050);
            LOGI("euler  %f,%f", mpu6050.f32Pitch, mpu6050.f32Roll);

#if CONFIG_MPU6050_KALMAN_FILTER_SW

            // 直接计算的欧拉角会含较多噪声, 用卡尔曼滤波后波形就比较平滑
            // 当存在俯仰角 pitch 时，卡尔曼滤波计算出的翻滚角 poll 有偏差？？

            MPU6050_GetAngle_Kalman(&mpu6050);
            LOGI("kalman %f,%f", mpu6050.f32Pitch, mpu6050.f32Roll);

#endif

#else

            LOGI("%5d, %5d, %5d, %5d, %5d, %5d, %f",                       //
                 mpu6050.s16AccelX, mpu6050.s16AccelY, mpu6050.s16AccelZ,  //
                 mpu6050.s16GyroX, mpu6050.s16GyroY, mpu6050.s16GyroZ, mpu6050.f32Temp);

#endif
        }
    }
}

#endif
