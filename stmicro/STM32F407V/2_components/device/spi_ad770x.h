#ifndef __SPI_AD770X_H__
#define __SPI_AD770X_H__

#include "spimst.h"

// VDD   | Vref (推荐值)
// 3.3v  | 1.225v
// 5.25v | 2.5v

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define AD770X_SPI_TIMING (SPI_FLAG_4WIRE | SPI_FLAG_CPOL_HIGH | SPI_FLAG_CPHA_2EDGE | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW)

/**
 * @brief REG_COMM
 */

// bit[6:4]
typedef enum {
    AD770X_REG_CMM    = 0x00 << 4,  // 通信 communication, 8 bit
    AD770X_REG_SETUP  = 0x01 << 4,  // 设置 setup, 8 bit
    AD770X_REG_CLOCK  = 0x02 << 4,  // 时钟 clock, 8 bit
    AD770X_REG_DATA   = 0x03 << 4,  // 数据 data, 16 bit, contains conversion result
    AD770X_REG_TEST   = 0x04 << 4,  // 测试 test, 8 bit （勿更改此寄存器）
    AD770X_REG_OFFSET = 0x06 << 4,  // 校准 offset, 24 bit (read it after zero calibration)
    AD770X_REG_GAIN   = 0x07 << 4,  // 校准 gain, 24 bit (read it after full calibration)
} ad770x_register_e;

// bit[3]
typedef enum {
    AD770X_WRITE = 0x00 << 3,
    AD770X_READ  = 0x01 << 3,
} ad770x_operation_e;

// bit[1:0]
typedef enum {
    AD7705_CHANNEL_AIN1P_AIN1N = 0x00 << 0,  // AIN1+, AIN1-
    AD7705_CHANNEL_AIN2P_AIN2N = 0x01 << 0,  // AIN2+, AIN2-
    AD7705_CHANNEL_AIN1N_AIN1N = 0x02 << 0,  // AIN1-, AIN1-
    AD7705_CHANNEL_AIN1N_AIN2N = 0x03 << 0,  // AIN1-, AIN2-
    AD7706_CHANNEL_AIN1_COM    = 0x00 << 0,  // AIN1, COMMON
    AD7706_CHANNEL_AIN2_COM    = 0x01 << 0,  // AIN2, COMMON
    AD7706_CHANNEL_COM_COM     = 0x02 << 0,  // COMMON, COMMON
    AD7706_CHANNEL_AIN3_COM    = 0x03 << 0,  // AIN1, COMMON
} ad770x_channel_e;

/**
 * @brief REG_SETUP
 */

// bit[7:6]
typedef enum {
    AD770X_MODE_NORMAL           = 0x00 << 6,  // 正常 (完成校准后会自动回到该模式)
    AD770X_MODE_CALIBRATION_SELF = 0x01 << 6,  // 自校准 (内部自动短接 AIN+ AIN-, 校准零位，内部短接到 Vref 校准满位)
    AD770X_MODE_CALIBRATION_ZERO = 0x02 << 6,  // 校准零刻度 (需将 AIN+ AIN- 短接), POR 0x1F4000, set FSYNC high before calibration, FSYNC low after calibration
    AD770X_MODE_CALIBRATION_FULL = 0x03 << 6,  // 校准满刻度 (需将 AIN+ AIN- 接最大输入电压源), POR 0x5761AB, set FSYNC high before calibration, FSYNC low after calibration
} ad770x_mode_e;

// bit[5:3]
typedef enum {
    AD770X_GAIN_1   = 0x00 << 3,
    AD770X_GAIN_2   = 0x01 << 3,
    AD770X_GAIN_4   = 0x02 << 3,
    AD770X_GAIN_8   = 0x03 << 3,
    AD770X_GAIN_16  = 0x04 << 3,
    AD770X_GAIN_32  = 0x05 << 3,
    AD770X_GAIN_64  = 0x06 << 3,
    AD770X_GAIN_128 = 0x07 << 3,
} ad770x_gain_e;

// bit[2]
typedef enum {
    AD770X_POLARITY_BIPOLAR  = 0x00 << 2,  // 双极性输入 (-VREF/2 ~ +VREF2)
    AD770X_POLARITY_UNIPOLAR = 0x01 << 2,  // 单极性输入
} ad770x_polarity_e;

// bit[1] 内部缓冲器

// bit[0] 过滤器同步 filter synchronization

/**
 * @brief REG_CLOCK
 */

// bit[4]: clock state 时钟使能, 0:enable, 1:disable
// bit[3]: clock divider 时钟二分频, 0:disable, 1:enable
// bit[2]: master clock (mclk) 外部晶振频率/外部时钟输入, 0:4.9152MHz, 1:2MHz
typedef enum {
    AD770X_CLOCK_DISABLE    = 0x01 << 4,
    // 时钟输入:
    AD770X_CLOCK_1_MHZ      = 0x00 << 2,  // 0b00
    AD770X_CLOCK_2_MHZ      = 0x02 << 2,  // 0b10
    AD770X_CLOCK_2_4576_MHZ = 0x01 << 2,  // 0b01
    AD770X_CLOCK_4_9152_MHZ = 0x03 << 2,  // 0b11
} ad770x_clock_e;

// bit [1:0]
typedef enum {
    // for clock = 1mhz / 2mhz
    AD770X_UPDATE_RATE_20_HZ  = 0x00 << 0,
    AD770X_UPDATE_RATE_25_HZ  = 0x01 << 0,
    AD770X_UPDATE_RATE_100_HZ = 0x02 << 0,
    AD770X_UPDATE_RATE_200_HZ = 0x03 << 0,
    // for clock = 2.4576mhz / 4.9152mhz
    AD770X_UPDATE_RATE_50_HZ  = 0x00 << 0,
    AD770X_UPDATE_RATE_60_HZ  = 0x01 << 0,
    AD770X_UPDATE_RATE_250_HZ = 0x02 << 0,
    AD770X_UPDATE_RATE_500_HZ = 0x03 << 0,
} ad770x_update_rate_e;

typedef struct {
    __IN spi_mst_t*  hSPI;
    __IN const pin_t RST;
} spi_ad770x_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t AD770x_Init(spi_ad770x_t* pHandle);
bool     AD770x_IsDataReady(spi_ad770x_t* pHandle, ad770x_channel_e eChannel);

status_t AD770x_ConfigChannel(
    spi_ad770x_t*        pHandle,
    ad770x_channel_e     eChannel,
    ad770x_mode_e        eMode,
    ad770x_clock_e       eClock,
    ad770x_gain_e        eGain,
    ad770x_polarity_e    ePolarity,
    ad770x_update_rate_e eRate);

status_t AD770x_ReadAdc(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint16_t* pu16Value);
status_t AD770x_ReadZeroCal(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint32_t* pu32Value);
status_t AD770x_ReadFullCal(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint32_t* pu32Value);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void AD7705_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
