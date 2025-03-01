#include "i2c_as5600.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "as5600"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief register
 */
#define REG_ZMCO      0x00  // 角度烧录次数 R
#define REG_ZPOS_H    0x01  // 起始位置 R/W/P
#define REG_ZPOS_L    0x02  // 起始位置 R/W/P
#define REG_MPOS_H    0x03  // 结束位置 R/W/P
#define REG_MPOS_L    0x04  // 结束位置 R/W/P
#define REG_MANG_H    0x05  // R/W/P
#define REG_MANG_L    0x06  // R/W/P
#define REG_CONF_H    0x07  // R/W/P
#define REG_CONF_L    0x08  // R/W/P
#define REG_RAW_ANG_H 0x0C  // 原始角度(未经缩放) R
#define REG_RAW_ANG_L 0x0D  // R
#define REG_ANG_H     0x0E  // 角度 R
#define REG_ANG_L     0x0F  // R
#define REG_STAT      0x0B  // 状态 R
#define REG_AGC       0x1A  // 自动增益 R
#define REG_MAG_H     0x1B  // R
#define REG_MAG_L     0x1C  // R
#define REG_BURN      0xFF  // W

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static inline status_t AS5600_ReadByte(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data);
static inline status_t AS5600_WriteByte(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data);
static inline status_t AS5600_ReadWord(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data);
static inline status_t AS5600_WriteWord(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data);
static inline status_t AS5600_WriteBits(i2c_as5600_t* pHandle, uint16_t u8MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t u8BitsValue);
static inline status_t AS5600_ReadBits(i2c_as5600_t* pHandle, uint16_t u8MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t* pu8BitsValue);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t AS5600_ReadByte(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data)
{
    return I2C_Master_ReadBlock(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu8Data, 1, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t AS5600_WriteByte(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data)
{
    return I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t AS5600_ReadWord(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data)
{
    return I2C_Master_ReadWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static inline status_t AS5600_WriteWord(i2c_as5600_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

static inline status_t AS5600_WriteBits(i2c_as5600_t* pHandle, uint16_t u8MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t u8BitsValue)
{
    return I2C_Master_WriteByteBits(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8StartBit, u8BitsCount, u8BitsValue, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t AS5600_ReadBits(i2c_as5600_t* pHandle, uint16_t u8MemAddr, uint8_t u8StartBit, uint8_t u8BitsCount, uint8_t* pu8BitsValue)
{
    return I2C_Master_ReadByteBits(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8StartBit, u8BitsCount, pu8BitsValue, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

status_t AS5600_Init(i2c_as5600_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    return ERR_NONE;
}

status_t AS5600_ReadAngle(i2c_as5600_t* pHandle, uint16_t* pu16Position)
{
    return AS5600_ReadWord(pHandle, REG_ANG_H, pu16Position);
}

status_t AS5600_ReadRawAngle(i2c_as5600_t* pHandle, uint16_t* pu16Position)
{
    return AS5600_ReadWord(pHandle, REG_RAW_ANG_H, pu16Position);
}

status_t AS5600_SetStartPosition(i2c_as5600_t* pHandle, uint16_t u16Position)
{
    return AS5600_WriteWord(pHandle, REG_ZPOS_H, u16Position);
}

status_t AS5600_GetStartPosition(i2c_as5600_t* pHandle, uint16_t* pu16Position)
{
    return AS5600_ReadWord(pHandle, REG_ZPOS_H, pu16Position);
}

status_t AS5600_SetStopPosition(i2c_as5600_t* pHandle, uint16_t u16Position)
{
    return AS5600_WriteWord(pHandle, REG_MPOS_H, u16Position);
}

status_t AS5600_GetStopPosition(i2c_as5600_t* pHandle, uint16_t* pu16Position)
{
    return AS5600_ReadWord(pHandle, REG_MPOS_H, pu16Position);
}

status_t AS5600_SetMaxAngle(i2c_as5600_t* pHandle, uint16_t u16Angle)
{
    return AS5600_WriteWord(pHandle, REG_MANG_H, u16Angle);
}

status_t AS5600_GetMaxAngle(i2c_as5600_t* pHandle, uint16_t* pu16Angle)
{
    return AS5600_ReadWord(pHandle, REG_MANG_H, pu16Angle);
}

status_t AS5600_SetWatchDog(i2c_as5600_t* pHandle, bool bEnable)
{
    return AS5600_WriteBits(pHandle, REG_CONF_H, 5, 1, bEnable);
}

status_t AS5600_GetWatchDog(i2c_as5600_t* pHandle, bool* pbEnable)
{
    uint8_t* u8Conf;
    ERRCHK_RETURN(AS5600_WriteBits(pHandle, REG_CONF_H, 5, 1, &u8Conf));
    *pbEnable = u8Conf ? true : false;
    return ERR_NONE;
}

status_t AS5600_SetFastFilterThreshold(i2c_as5600_t* pHandle, as5600_fast_filter_threshold_e eThreshold)
{
    return AS5600_WriteBits(pHandle, REG_CONF_H, 2, 3, eThreshold);
}

status_t AS5600_GetFastFilterThreshold(i2c_as5600_t* pHandle, as5600_fast_filter_threshold_e* peThreshold)
{
    return AS5600_ReadBits(pHandle, REG_CONF_H, 2, 3, (uint8_t*)peThreshold);
}

status_t AS5600_SetSlowFilter(i2c_as5600_t* pHandle, as5600_slow_filter_e eFilter)
{
    return AS5600_WriteBits(pHandle, REG_CONF_H, 0, 2, eFilter);
}

status_t AS5600_GetSlowFilter(i2c_as5600_t* pHandle, as5600_slow_filter_e* peFilter)
{
    return AS5600_ReadBits(pHandle, REG_CONF_H, 0, 2, (uint8_t*)peFilter);
}

status_t AS5600_SetPwmFrequency(i2c_as5600_t* pHandle, as5600_pwm_frequency_e eFreq)
{
    return AS5600_WriteBits(pHandle, REG_CONF_L, 6, 2, eFreq);
}

status_t AS5600_GetPwmFrequency(i2c_as5600_t* pHandle, as5600_pwm_frequency_e* peFreq)
{
    return AS5600_ReadBits(pHandle, REG_CONF_L, 6, 2, (uint8_t*)peFreq);
}

status_t AS5600_SetOutputStage(i2c_as5600_t* pHandle, as5600_output_stage_e eStage)
{
    return AS5600_WriteBits(pHandle, REG_CONF_L, 4, 2, eStage);
}

status_t AS5600_GetOutputStage(i2c_as5600_t* pHandle, as5600_output_stage_e* peStage)
{
    return AS5600_ReadBits(pHandle, REG_CONF_L, 4, 2, (uint8_t*)peStage);
}

status_t AS5600_SetHysteresis(i2c_as5600_t* pHandle, as5600_hysteresis_e eHysteresis)
{
    return AS5600_WriteBits(pHandle, REG_CONF_L, 2, 2, eHysteresis);
}

status_t AS5600_GetHysteresis(i2c_as5600_t* pHandle, as5600_hysteresis_e* peHysteresis)
{
    return AS5600_ReadBits(pHandle, REG_CONF_L, 2, 2, (uint8_t*)peHysteresis);
}

status_t AS5600_SetPowerMode(i2c_as5600_t* pHandle, as5600_power_mode_e eMode)
{
    return AS5600_WriteBits(pHandle, REG_CONF_L, 0, 2, eMode);
}

status_t AS5600_GetPowerMode(i2c_as5600_t* pHandle, as5600_power_mode_e* peMode)
{
    return AS5600_ReadBits(pHandle, REG_CONF_L, 0, 2, (uint8_t*)peMode);
}

status_t AS5600_GetStatus(i2c_as5600_t* pHandle, uint8_t* pu8Status)
{
    return AS5600_ReadByte(pHandle, REG_STAT, pu8Status);
}

/**
 * @brief 检测磁铁是否存在
 */
status_t AS5600_DetectMagnet(i2c_as5600_t* pHandle, bool* pbExist)
{
    uint8_t u8State;
    ERRCHK_RETURN(AS5600_GetStatus(pHandle, &u8State));
    *pbExist = (u8State & AS5600_STATUS_MD) ? true : false;
    return ERR_NONE;
}

/**
 * @brief Automatic Gain Control
 */
status_t AS5600_GetAGC(i2c_as5600_t* pHandle, uint8_t* pu8AGC)
{
    return AS5600_ReadByte(pHandle, REG_AGC, pu8AGC);
}

status_t AS5600_GetMagnitude(i2c_as5600_t* pHandle, uint16_t* pu16Magnitude)
{
    return AS5600_ReadWord(pHandle, REG_MAG_H, pu16Magnitude);
}

/**
 * @brief 角度烧录 (ZPOS,MPOS)
 *
 * - 前提: 检测到磁铁存在时, 即 REG_STATMD = 1, 方可执行.
 * - 执行烧录: 往 REG_BURN 写入 AS5600_BURN_ANGLE (最多执行3次)
 * - 状态: ZMCO 显示角度烧录次数
 *
 *  @note REG_ANG 和 REG_RAW_ANG 的关系
 *
 *       [0,REG_ZPOS)        REG_ANG = 0, AOUT = 0, PwmDuty = 3%
 *       [REG_ZPOS,REG_MPOS] coeff = REG_RAW_ANG*(REG_MPOS-REG_ZPOS)/4095, REG_ANG = coeff*REG_RAW_ANG, AOUT = coeff * VIN, PwmDuty = (coeff * 100) %
 *       (REG_MPOS,4095]     REG_ANG = 4095, AOUT = VIN, PwmDuty = 97%
 *
 * @brief 设置烧录 (ZPOS,MPOS)
 *
 * - 前提: REG_ZMCO = 0 时，即 REG_ZPOS 和 REG_MPOS 未被永久性写入时，才可对 MANG 进行写入。
 * - 执行烧录: 往 REG_BURN 写入 AS5600_BURN_SETTING (最多执行1次)
 *
 * @note 烧录时序看手册 !!
 *
 * @note 编程次数用完了没关系！！每次上电重新配置相关寄存器即可，只是掉电不保存而已。
 *
 */

status_t AS5600_SetBurn(i2c_as5600_t* pHandle, as5600_burn_e eBurn)
{
    return AS5600_WriteByte(pHandle, REG_BURN, (uint8_t)eBurn);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void AS5600_Test(i2c_mst_t* hI2C)
{
    i2c_as5600_t as5600 = {
        .hI2C      = hI2C,
        .u8SlvAddr = AS5600_ADDRESS_LOW,
    };

    bool     bMagnetExist = false;
    uint16_t u16Angle, u16RawAngle;

    AS5600_Init(&as5600);

#if 1
    AS5600_SetStartPosition(&as5600, 0);
    AS5600_SetStopPosition(&as5600, 4095);
#else
    AS5600_SetStartPosition(&as5600, 1024);
    AS5600_SetStopPosition(&as5600, 1024 + 2048);
#endif

    AS5600_SetOutputStage(&as5600, AS5600_OUTPUT_STAGE_PWM);
    AS5600_SetPwmFrequency(&as5600, AS5600_PWM_FREQUENCY_920HZ);

    // AS5600_SetOutputStage(&as5600, AS5600_OUTPUT_STAGE_ANALOG_FULL);

    I2C_Master_Hexdump(as5600.hI2C, as5600.u8SlvAddr, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);

    LOGI("magnet detecting");

    do
    {
        AS5600_DetectMagnet(&as5600, &bMagnetExist);
    } while (bMagnetExist == false);

    LOGI("magnet detected");

    while (1)
    {
        if (AS5600_ReadAngle(&as5600, &u16Angle) == ERR_NONE &&
            AS5600_ReadRawAngle(&as5600, &u16RawAngle) == ERR_NONE)
        {
            LOGI("angle = %d, raw angle = %d", u16Angle, u16RawAngle);
        }
    }
}

#endif
