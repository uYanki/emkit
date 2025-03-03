#ifndef __PV_WAVE_H__
#define __PV_WAVE_H__

/**
 * @brief 波形发生器 (DAC+DMA)
 *
 *   u32WaveTargetFreq(RW): 目标输出波形频率.
 *   u32WaveActualFreq(R): 实际输出波形频率.
 *      - 注意: 由于硬件性能限制导致大部分高频率段输出有误
 *   u16WaveFreqUnit(RW):
 *      - 单位: 毫赫 mHz, 赫兹 Hz ( 1e3 mHz = 1 Hz)
 *
 *   u16WaveType(RW): 波形类型
 *      * 预设波形:
 *         - @ WaveType_Noise:
 *              u16WaveAmplitude(RW): 噪声幅度
 *         - @ WaveType_Triangle:
 *              u16WaveAmplitude(RW): 三角波振幅
 *      * 自定义波形:
 *         - @ WaveType_Custom: 自定义波形
 *              u16WaveAlign(RW):  数据对齐方式  |     12bit       |      8bit
 *              u16WaveSizea(RW):  数据点数量    |    最多80点     |    最多160点
 *              u16WaveDataa(RW):  数据点数值    |  每个数值为1点   |  每个数值为2点
 *
 *   u16WaveConfig(RW): 写0触发波形配置, 配置成功自动置为1, 配置失败自动置为2
 */

typedef enum {
    WaveConfig_Doit    = 0,  // 触发波形配置
    WaveConfig_Success = 1,  // 波形配置成功
    WaveConfig_Failure = 2,  // 波形配置失败
    _WaveConfig_Count  = 3,
} WaveConfig_e;

typedef enum {
    WaveFreqUnit_Hz     = 0,
    WaveFreqUnit_mHz    = 1,
    _WaveFreqUnit_Count = 2,
} WaveFreqUnit_e;

typedef enum {
    WaveType_None     = 0,  // 无
    WaveType_Noise    = 1,  // 噪声
    WaveType_Triangle = 2,  // 三角波
    WaveType_Sine     = 3,  // 正弦波
    WaveType_Custom   = 4,  // 自定义
    _WaveType_Count   = 5,
} WaveType_e;

typedef enum {
    WaveAlign_12Bit_Right = 0,  // 12位右对齐
    WaveAlign_12Bit_Left  = 1,  // 12位左对齐
    WaveAlign_8Bit_Right  = 2,  //  8位右对齐
    _WaveAlign_Count      = 3,
} WaveAlign_e;

// 电压范围 ( 0~4095 -> 0~3.3v )
typedef enum {
    WaveAmplitude_1      = 0x0,  // 0 ~ 1
    WaveAmplitude_3      = 0x1,  // 0 ~ 3
    WaveAmplitude_7      = 0x2,  // 0 ~ 7
    WaveAmplitude_15     = 0x3,  // 0 ~ 15
    WaveAmplitude_31     = 0x4,  // 0 ~ 31
    WaveAmplitude_63     = 0x5,  // 0 ~ 63
    WaveAmplitude_127    = 0x6,  // 0 ~ 127
    WaveAmplitude_255    = 0x7,  // 0 ~ 255
    WaveAmplitude_511    = 0x8,  // 0 ~ 511
    WaveAmplitude_1023   = 0x9,  // 0 ~ 1023
    WaveAmplitude_2047   = 0xA,  // 0 ~ 2047
    WaveAmplitude_4095   = 0xB,  // 0 ~ 4095
    _WaveAmplitude_Count = 12,
} WaveAmplitude_e;

// 波形同步输出(共用时钟源), [unsupport]
typedef enum {
    WaveSync_Disable,  // 同步输出
    WaveSync_Enable,   // 异步输出
} WaveSync_e;

#endif
