#include "spi_mt6701.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mt6701"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

typedef struct {
    uint8_t FieldWeak   : 1;  // 磁场过弱
    uint8_t FieldStrong : 1;  // 磁场过强
    uint8_t BtnPressed  : 1;  // 旋钮被按下
    uint8_t OverSpeed   : 1;  // 超速
    uint8_t _Resv       : 4;
} mt6701_state_t;

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MT6701_Init(spi_mt6701_t* pHandle)
{
}

uint16_t MT6701_ReadAngle(spi_mt6701_t* pHandle)
{
    uint8_t  au8Data[3] = {0};
    uint32_t u24Data    = 0;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_ReceiveBlock(pHandle->hSPI, au8Data, 3);
    SPI_Master_Deselect(pHandle->hSPI);

    u24Data = au8Data[0] << 16;
    u24Data |= au8Data[1] << 8;
    u24Data |= au8Data[2] << 0;

    uint8_t  u6Crc  = u24Data & 0x3F;            // 6bit
    uint8_t  u4Sts  = (u24Data >> 6) & 0xF;      // 4bit
    uint16_t u14Pos = (u24Data >> 10) & 0x3FFF;  // 14bit

    return u14Pos;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void MT6701_Test(void)
{
    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5},
        .MOSI = {GPIOA, GPIO_PIN_5},
        .SCLK = {GPIOA, GPIO_PIN_6},
        .CS   = {GPIOA, GPIO_PIN_3},
    };

    spi_mt6701_t mt6701 = {
        .hSPI = &spi,
    };

    SPI_Master_Init(&spi, 50000, SPI_DUTYCYCLE_50_50, MT6701_SPI_TIMING | SPI_FLAG_SOFT_CS);

    MT6701_Init(&mt6701);

    while (1)
    {
        LOGI("angle: %d", MT6701_ReadAngle(&mt6701));
        DelayBlockMs(10);
    }
}

#endif /* CONFIG_DEMOS_SW */
