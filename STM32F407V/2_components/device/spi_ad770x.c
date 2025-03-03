#include "spi_ad770x.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "ad770x"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline void AD770x_WriteByte(spi_ad770x_t* pHandle, uint8_t u8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Data);
    SPI_Master_Deselect(pHandle->hSPI);
}

static inline void AD770x_ReadByte(spi_ad770x_t* pHandle, uint8_t* pu8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_ReceiveByte(pHandle->hSPI, pu8Data);
    SPI_Master_Deselect(pHandle->hSPI);
}

static inline void AD770x_ReadBlock(spi_ad770x_t* pHandle, uint8_t* pu8Data, uint8_t u8Size)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_ReceiveBlock(pHandle->hSPI, pu8Data, u8Size);
    SPI_Master_Deselect(pHandle->hSPI);
}

//---------------------------------------------------------------------------

static inline void AD770x_Reset(spi_ad770x_t* pHandle)
{
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_LOW);
    DelayBlockMs(20);
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_HIGH);
    DelayBlockMs(20);
}

/**
 * @brief 同步 spi 接口, 以防模块失步
 */
static inline void AD770x_SyncSpi(spi_ad770x_t* pHandle)
{
    uint8_t u8Count = 100, u8DummyByte = 0xFF;

    SPI_Master_Select(pHandle->hSPI);

    while (u8Count--)
    {
        AD770x_WriteByte(pHandle, u8DummyByte);
    }

    SPI_Master_Deselect(pHandle->hSPI);

    // 复位后等待 500us 再访问
    DelayBlockMs(1);
}

static inline void AD770x_SetOperation(
    spi_ad770x_t*      pHandle,
    ad770x_channel_e   eChannel,
    ad770x_register_e  eRegister,
    ad770x_operation_e eOperation)
{
    AD770x_WriteByte(pHandle, ((uint8_t)eRegister | (uint8_t)eOperation | (uint8_t)eChannel) & 0x7F);
}

status_t AD770x_Init(spi_ad770x_t* pHandle)
{
    PIN_SetMode(&pHandle->RST, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);

    AD770x_Reset(pHandle);
    AD770x_SyncSpi(pHandle);

    return ERR_NONE;
}

status_t AD770x_ConfigChannel(
    spi_ad770x_t*        pHandle,
    ad770x_channel_e     eChannel,
    ad770x_mode_e        eMode,
    ad770x_clock_e       eClock,
    ad770x_gain_e        eGain,
    ad770x_polarity_e    ePolarity,
    ad770x_update_rate_e eRate)
{
    AD770x_SetOperation(pHandle, eChannel, AD770X_REG_CLOCK, AD770X_WRITE);
    AD770x_WriteByte(pHandle, ((uint8_t)eClock | (uint8_t)eRate) & 0x1F);

    AD770x_SetOperation(pHandle, eChannel, AD770X_REG_SETUP, AD770X_WRITE);

    // 包含禁用 内部缓冲器 和 过滤器同步
    AD770x_WriteByte(pHandle, (uint8_t)eMode | (uint8_t)eGain | (uint8_t)ePolarity | (0 << 1) | (0 << 0));

    return ERR_NONE;
}

bool AD770x_IsDataReady(spi_ad770x_t* pHandle, ad770x_channel_e eChannel)
{
    uint8_t u8State;

    AD770x_SetOperation(pHandle, eChannel, AD770X_REG_CMM, AD770X_READ);
    AD770x_ReadByte(pHandle, &u8State);

    return (u8State & 0x80) ? false : true;
}

status_t AD770x_ReadAdc(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint16_t* pu16Value)
{
    uint8_t au8Buff[2];

    while (AD770x_IsDataReady(pHandle, eChannel) == false);
    AD770x_SetOperation(pHandle, eChannel, AD770X_REG_DATA, AD770X_READ);  // select channel
    AD770x_ReadBlock(pHandle, au8Buff, 2);
    *pu16Value = be16(au8Buff);

    return ERR_NONE;
}

/**
 * @brief 读取零标度寄存器
 */
status_t AD770x_ReadZeroCal(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint32_t* pu32Value)
{
    uint8_t au8Buff[3];
    AD770x_SetOperation(pHandle, eChannel, AD770X_REG_OFFSET, AD770X_READ);
    AD770x_ReadBlock(pHandle, au8Buff, 3);
    *pu32Value = (au8Buff[2] << 16) | (au8Buff[1] << 8) | au8Buff[0];
    return ERR_NONE;
}

/**
 * @brief 读取满标度寄存器
 */
status_t AD770x_ReadFullCal(spi_ad770x_t* pHandle, ad770x_channel_e eChannel, uint32_t* pu32Value)
{
    uint8_t au8Buff[3];
    AD770x_SetOperation(pHandle, eChannel, AD770X_REG_GAIN, AD770X_READ);
    AD770x_ReadBlock(pHandle, au8Buff, 3);
    *pu32Value = (au8Buff[2] << 16) | (au8Buff[1] << 8) | au8Buff[0];
    return ERR_NONE;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

// 基准电压为2.5V时
// - 在单极性信号下, 输入范围是     0V 到 2.5V
// - 在双极性输入下, 输入范围是 -1.25V 到 +1.25V
#define CONFIG_AD770X_VREF 2.5f  // @5V 输入

void AD7705_Test(void)
{
#if defined(BOARD_STM32F407VET6_XWS)

    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_4},
        .MOSI = {GPIOA, GPIO_PIN_5},
        .SCLK = {GPIOA, GPIO_PIN_6},
        .CS   = {GPIOA, GPIO_PIN_3},
    };

    spi_ad770x_t ad770x = {
        .hSPI = &spi,
        .RST  = {GPIOA, GPIO_PIN_0},
    };

#elif defined(BOARD_AT32F415CB_DEV)

    spi_mst_t spi = {
        .MOSI = {GPIOB, GPIO_PINS_15}, /*MOSI*/
        .MISO = {GPIOB, GPIO_PINS_14}, /*MISO*/
        .SCLK = {GPIOB, GPIO_PINS_13}, /*SCLK*/
        .CS   = {GPIOB, GPIO_PINS_12}, /*CS*/
    };

    spi_ad770x_t ad770x = {
        .hSPI = &spi,
        .RST  = {GPIOA, GPIO_PINS_1},
    };

#endif

    SPI_Master_Init(&spi, 100000, SPI_DUTYCYCLE_33_67, AD770X_SPI_TIMING | SPI_FLAG_SOFT_CS);

    AD770x_Init(&ad770x);

    // 开单通道, 采集结果OK
    AD770x_ConfigChannel(
        &ad770x,
        AD7705_CHANNEL_AIN1P_AIN1N,
        AD770X_MODE_CALIBRATION_SELF,
        AD770X_CLOCK_4_9152_MHZ,
        AD770X_GAIN_1,
        AD770X_POLARITY_UNIPOLAR,
        AD770X_UPDATE_RATE_50_HZ);

    // AD770x_ConfigChannel(
    //     &ad770x,
    //     AD7705_CHANNEL_AIN2P_AIN2N,
    //     AD770X_MODE_CALIBRATION_SELF,
    //     AD770X_CLOCK_4_9152_MHZ,
    //     AD770X_GAIN_1,
    //     AD770X_POLARITY_BIPOLAR,
    //     AD770X_UPDATE_RATE_500_HZ);

    while (1)
    {
        uint16_t  u16ChannelData[2] = {0};
        float32_t f32ChannelData[2] = {0.f};

        AD770x_ReadAdc(&ad770x, AD7705_CHANNEL_AIN1P_AIN1N, &u16ChannelData[0]);
        // AD770x_ReadAdc(&ad770x, AD7705_CHANNEL_AIN2P_AIN2N, &u16ChannelData[1]);

        f32ChannelData[0] = u16ChannelData[0] * CONFIG_AD770X_VREF / 32768.f;
        f32ChannelData[1] = u16ChannelData[1] * CONFIG_AD770X_VREF / 32768.f;

        PRINTLN("%d, %.5f, %.5f", u16ChannelData[0], f32ChannelData[0], f32ChannelData[1]);

        DelayBlockMs(100);
    }
}

#endif
