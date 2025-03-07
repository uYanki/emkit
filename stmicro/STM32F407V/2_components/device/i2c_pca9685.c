#include "i2c_pca9685.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "pca9685"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

/**
 * @brief register
 * @note http://nicekwell.net/blog/20161213/pca9685-16lu-12wei-pwmxin-hao-fa-sheng-qi.html
 */

#define REG_MODE1        0x00  // mode register 1
#define REG_MODE2        0x01  // mode register 2
#define REG_SUBADR1      0x02  // iic bus subaddress 1
#define REG_SUBADR2      0x03  // iic bus subaddress 2
#define REG_SUBADR3      0x04  // iic bus subaddress 3
#define REG_ALLCALLADR   0x05  // led all call iic bus address
#define REG_LED0_ON_L    0x06  // led0 output and brightness control byte 0
#define REG_LED0_ON_H    0x07  // led0 output and brightness control byte 1
#define REG_LED0_OFF_L   0x08  // led0 output and brightness control byte 2
#define REG_LED0_OFF_H   0x09  // led0 output and brightness control byte 3
#define REG_ALLLED_ON_L  0xFA  // load all the ledn_on registers byte 0
#define REG_ALLLED_ON_H  0xFB  // load all the ledn_on registers byte 1
#define REG_ALLLED_OFF_L 0xFC  // load all the ledn_off registers byte 0
#define REG_ALLLED_OFF_H 0xFD  // load all the ledn_off registers byte 1
#define REG_PRESCALE     0xFE  // prescaler for pwm output frequency

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t PCA9685_ReadByte(i2c_pca9685_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data)
{
    return I2C_Master_ReadBlock(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu8Data, 1, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t PCA9685_WriteByte(i2c_pca9685_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data)
{
    return I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t PCA9685_WriteWord(i2c_pca9685_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_LITTLE_ENDIAN);
}

status_t PCA9685_Init(i2c_pca9685_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

#if CONFIG_PCA9685_SERVO_CONTROL_SW
    pHandle->_f32PeriodUs = 0;
#endif

    ERRCHK_RETURN(PCA9685_WriteByte(pHandle, REG_MODE1, 0x00));  // reset

    return ERR_NONE;
}

status_t PCA9685_SetDuty(i2c_pca9685_t* pHandle, pca9685_channel_e eChannel, uint16_t u16PulseOn, uint16_t u16PulseOff)
{
    ERRCHK_RETURN(PCA9685_WriteWord(pHandle, REG_LED0_ON_L + 4 * eChannel, u16PulseOn & 0x0FFF));
    ERRCHK_RETURN(PCA9685_WriteWord(pHandle, REG_LED0_OFF_L + 4 * eChannel, u16PulseOff & 0x0FFF));
    return ERR_NONE;
}

status_t PCA9685_SetAllDuty(i2c_pca9685_t* pHandle, uint16_t u16PulseOn, uint16_t u16PulseOff)
{
    ERRCHK_RETURN(PCA9685_WriteWord(pHandle, REG_ALLLED_ON_L, u16PulseOn & 0x0FFF));
    ERRCHK_RETURN(PCA9685_WriteWord(pHandle, REG_ALLLED_OFF_L, u16PulseOff & 0x0FFF));
    return ERR_NONE;
}

status_t PCA9685_SetFreq(i2c_pca9685_t* pHandle, float32_t f32PwmFreq)
{
    uint8_t u8Prescale;
    uint8_t u8OldMode;

#if CONFIG_PCA9685_SERVO_CONTROL_SW
    pHandle->_f32PeriodUs = 1000000.f / f32PwmFreq;
#endif

    // prescaleval=round(osc_cloc/4096/freq)-1
    // Correct for overshoot in the frequency setting -> freq *= 0.915

    if (f32PwmFreq < 500.f)
    {
        u8Prescale = 25000000.f / 4096.f / f32PwmFreq;
    }
    else
    {
        u8Prescale = 25000000.f / 4096.f / f32PwmFreq * 0.915f;
    }

    ERRCHK_RETURN(PCA9685_ReadByte(pHandle, REG_MODE1, &u8OldMode));
    // setup sleep mode, Low power mode. Oscillator off (bit4: 1-sleep, 0-normal)
    ERRCHK_RETURN(PCA9685_WriteByte(pHandle, REG_MODE1, (u8OldMode & 0x7F) | 0x10));
    // set the prescaler
    ERRCHK_RETURN(PCA9685_WriteByte(pHandle, REG_PRESCALE, u8Prescale));
    // setup normal mode (bit4: 1-sleep, 0-normal)
    ERRCHK_RETURN(PCA9685_WriteByte(pHandle, REG_MODE1, u8OldMode));
    DelayBlockMs(5);
    // set the MODE1 register to turn on auto increment.
    // ERRCHK_RETURN(PCA9685_WriteByte(pHandle, REG_MODE1, u8OldMode | 0xA1));
    ERRCHK_RETURN(PCA9685_WriteByte(pHandle, REG_MODE1, u8OldMode | 0x80));

    return ERR_NONE;
}

#if CONFIG_PCA9685_SERVO_CONTROL_SW

// Freq=50Hz, Duty=4095∗((angle/180.0)∗2.0+0.5)/20.0

status_t PCA9685_SetAngle_Servo(i2c_pca9685_t* pHandle, pca9685_channel_e eChannel, uint8_t u8Angle)
{
    ASSERT(pHandle->_f32PeriodUs > 0.00001f, );
    float32_t f32PulseOff = (float32_t)u8Angle / 180 * 2000 + 500;  // map [0,180] to [500,2500]
    ERRCHK_RETURN(PCA9685_SetDuty(pHandle, eChannel, 0, 4095.f * f32PulseOff / pHandle->_f32PeriodUs));
    return ERR_NONE;
}

status_t PCA9685_SetAngle_AllServo(i2c_pca9685_t* pHandle, uint8_t u8Angle)
{
    ASSERT(pHandle->_f32PeriodUs > 0.00001f, );
    float32_t f32PulseOff = (float32_t)u8Angle / 180 * 2000 + 500;  // map [0,180] to [500,2500]
    ERRCHK_RETURN(PCA9685_SetAllDuty(pHandle, 0, 4095.f * f32PulseOff / pHandle->_f32PeriodUs));
    return ERR_NONE;
}

#endif

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void PCA9685_Test(i2c_mst_t* hI2C)
{
    i2c_pca9685_t pca9685 = {
        .hI2C      = hI2C,
        .u8SlvAddr = PCA9685_ADDRESS_A00000,
    };

    PCA9685_Init(&pca9685);

#if CONFIG_PCA9685_SERVO_CONTROL_SW

    PCA9685_SetFreq(&pca9685, 50);

    int16_t s16Angle = 0;

    // 在开始前，可先手动调整转叶的位置
    PCA9685_SetAngle_AllServo(&pca9685, s16Angle);

    getchar();

    while (1)
    {
        for (s16Angle = 0; s16Angle < 180; s16Angle += 30)
        {
            PCA9685_SetAngle_AllServo(&pca9685, s16Angle);
            LOGI("angle = %d", s16Angle);
            DelayBlockS(2);
        }
        for (s16Angle = 180; s16Angle > 0; s16Angle -= 30)
        {
            PCA9685_SetAngle_AllServo(&pca9685, s16Angle);
            LOGI("angle = %d", s16Angle);
            DelayBlockS(2);
        }
    }

#else

    uint16_t u16PhaseOffset = 200;  // 相移

    uint16_t u16PulseStart = 0;
    uint16_t u16PulseWidth = 400;

    PCA9685_SetFreq(&pca9685, 1000);  // 上限频率 1.5k

    for (pca9685_channel_e u8Channel = PCA9685_CHANNEL_0; u8Channel < PCA9685_CHANNEL_15; u8Channel++)
    {
        PCA9685_SetDuty(&pca9685, u8Channel, u16PulseStart, u16PulseStart + u16PulseWidth);
        u16PulseStart += u16PhaseOffset;
    }

    while (1)
    {
    }

#endif
}
#endif
