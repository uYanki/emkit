#include "i2c_bh1750.h"

/**
 * 光照度 lx
 *
 * 勒克斯 (Lux) 被光均匀照射的物体，距离该光源1米处，在1m2面积上得到的光通量是1lm时，它的照度是1lux
 *
 * 光照度可用照度计直接测量。光照度的单位是勒克斯，是英文lux的音译，也可写为lx。
 * 被光均匀照射的物体，在1平方米面积上得到的光通量是1流明时，它的照度是1勒克斯。有时为了充分利用光源，
 * 常在光源上附加一个反射装置，使得某些方向能够得到比较多的光通量，以增加这一被照面上的照度。例如
 * 汽车前灯、手电筒、摄影灯等。
 *
 * 以下是各种环境照度值：单位lux
 *  - 黑夜：0.001—0.02；
 *  - 月夜：0.02—0.3；
 *  - 阴天室内：5—50；
 *  - 阴天室外：50—500；
 *  - 晴天室内：100—1000；
 *  - 夏季中午太阳光下的照度：约为10*9
 *  - 阅读书刊时所需的照度：50—60；
 *  - 家用摄像机标准照度：1400
 *
 * BH1750FVI 为光照度测量芯片。测量量程可通过命令进行调节。
 * 最小 0.11 lux， 最大 100000 lux
 */

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG    "bh1750"
#define LOG_LOCAL_LEVEL  LOG_LEVEL_INFO

#define CMD_POWER_DOWN   0x00  // 掉电，默认状态
#define CMD_POWER_ON     0x01  // 上电，等待测量命令
#define CMD_MODULE_RESET 0x07  // 清零数据寄存器 (在 PowerOff 模式下无效)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t BH1750_WriteCmd(i2c_bh1750_t* pHandle, uint8_t u8Cmd)
{
    return I2C_Master_TransmitByte(pHandle->hI2C, pHandle->u8SlvAddr, u8Cmd, I2C_FLAG_SLVADDR_7BIT);
}

/**
 * @brief 读原始数据
 * @note  在初始化完成后/连续测量模式下，需间隔 180ms 才能读到正确数据
 */
static inline uint16_t BH1750_ReadData(i2c_bh1750_t* pHandle, uint16_t* pu16Data)
{
    return I2C_Master_ReceiveWord(pHandle->hI2C, pHandle->u8SlvAddr, pu16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_WORD_BIG_ENDIAN);
}

status_t BH1750_Init(i2c_bh1750_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    ERRCHK_RETURN(BH1750_PowerOff(pHandle));
    DelayBlockMs(100);
    ERRCHK_RETURN(BH1750_PowerOn(pHandle));
    ERRCHK_RETURN(BH1750_SetModeRes(pHandle, BH1750_CONTINUE_HIGH_RES));  // 高分辨率连续测量
    ERRCHK_RETURN(BH1750_SetSensitivity(pHandle, 69));                    // 芯片缺省灵敏度倍率

    return ERR_NONE;
}

status_t BH1750_PowerOn(i2c_bh1750_t* pHandle)
{
    return BH1750_WriteCmd(pHandle, CMD_POWER_ON);
}

status_t BH1750_PowerOff(i2c_bh1750_t* pHandle)
{
    return BH1750_WriteCmd(pHandle, CMD_POWER_DOWN);
}

status_t BH1750_Reset(i2c_bh1750_t* pHandle)
{
    return BH1750_WriteCmd(pHandle, CMD_MODULE_RESET);
}

/**
 * @brief 设置测量模式和分辨率
 */
status_t BH1750_SetModeRes(i2c_bh1750_t* pHandle, bh1750_mode_e eMode)
{
    ERRCHK_RETURN(BH1750_WriteCmd(pHandle, (uint8_t)eMode));
    pHandle->_eMeasureMode = eMode;
    return ERR_NONE;
}

/**
 * @brief 设置量程倍率
 * @param[in] u8Sensitivity 取值范围 [31,254], 值越大灵敏度越高
 */
status_t BH1750_SetSensitivity(i2c_bh1750_t* pHandle, uint8_t u8Sensitivity)
{
    u8Sensitivity = CLAMP(u8Sensitivity, 31, 254);

    ERRCHK_RETURN(BH1750_WriteCmd(pHandle, 0x40 + (u8Sensitivity >> 5)));   /* 更改高3bit */
    ERRCHK_RETURN(BH1750_WriteCmd(pHandle, 0x60 + (u8Sensitivity & 0x1F))); /* 更改低5bit */

    /*　更改量程范围后，需要重新发送命令设置测量模式　*/
    ERRCHK_RETURN(BH1750_SetModeRes(pHandle, pHandle->_eMeasureMode));

    pHandle->_u8Sensitivity = u8Sensitivity;

    return ERR_NONE;
}

/**
 * @brief 读取光强度
 */
status_t BH1750_GetLux(i2c_bh1750_t* pHandle, float32_t* pf32Lux)
{
    uint16_t u16Data;

    ERRCHK_RETURN(BH1750_ReadData(pHandle, &u16Data));

    *pf32Lux = (float32_t)(u16Data * 5 * 69) / (6 * pHandle->_u8Sensitivity);

    if (pHandle->_eMeasureMode == BH1750_CONTINUE_HIGH_RES ||  //
        pHandle->_eMeasureMode == BH1750_ONESHOT_HIGH_RES)
    {
        *pf32Lux /= 2.f;
    }

    return ERR_NONE;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void BH1750_Test(i2c_mst_t* hI2C)
{
    i2c_bh1750_t bh1750 = {
        .hI2C      = hI2C,
        .u8SlvAddr = BH1750_ADDRESS_HIGH,
    };

    float32_t f32Lux;

    BH1750_Init(&bh1750);

    bh1750_mode_e eMode[] = {
        BH1750_CONTINUE_HIGH_RES,    //
        BH1750_CONTINUE_MEDIUM_RES,  //
        BH1750_CONTINUE_LOW_RES,     //
        BH1750_ONESHOT_HIGH_RES,     //
        BH1750_ONESHOT_MEDIUM_RES,   //
        BH1750_ONESHOT_LOW_RES,      //
    };

    for (uint8_t i = 0; i < ARRAY_SIZE(eMode); ++i)
    {
        BH1750_PowerOn(&bh1750);
        BH1750_SetModeRes(&bh1750, eMode[i]);

        DelayBlockMs(500);
        if (BH1750_GetLux(&bh1750, &f32Lux) == ERR_NONE)
        {
            LOGI("mode = %d, lux = %.4f", i, f32Lux);
        }

        // 单次测量模式, 没调用 PowerOn, 输出值和上次测量结果一样
        DelayBlockMs(500);
        if (BH1750_GetLux(&bh1750, &f32Lux) == ERR_NONE)
        {
            LOGI("mode = %d, lux = %.4f", i, f32Lux);
        }
    }

    BH1750_SetModeRes(&bh1750, BH1750_CONTINUE_HIGH_RES);

    while (1)
    {
        DelayBlockMs(200);

        if (BH1750_GetLux(&bh1750, &f32Lux) == ERR_NONE)
        {
            LOGI("lux = %f", f32Lux);
        }
    }
}

#endif
