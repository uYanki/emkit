#include "sdkinc.h"

#if CONFIG_DEMOS_SW

#include "qr_encode.h"

#define CONFIG_QRCODE_DISPLAY_MODE QRCODE_DISPLAY_IMAGE

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "qrcode"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief CONFIG_QRCODE_DISPLAY_MODE
 */
#define QRCODE_DISPLAY_ASCII 0  // ascii (PRINTF)
#define QRCODE_DISPLAY_IMAGE 1  // st7735 (SPI LCD)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

uint8_t encode_version = 15;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#if CONFIG_QRCODE_DISPLAY_MODE == QRCODE_DISPLAY_IMAGE

#include "spi_st7735.h"

/**
 * @param[in] u8Side       length of square QRcode side
 * @param[in] u8Scale      point size
 * @param[in] cpu8BitTable bit table of QRcode
 */
static void ST7735_DrowQRCode(spi_st7735_t* pHandle, uint16_t x, uint16_t y, uint8_t u8Side, uint8_t u8Scale, const uint8_t* cpu8BitTable)
{
    uint16_t u16Row, u16Col;

    for (u16Row = 0; u16Row < u8Side; ++u16Row)
    {
        uint16_t _y = y;

        for (u16Col = 0; u16Col < u8Side; ++u16Col)
        {
            uint16_t u16Index   = u16Row * u8Side + u16Col;
            uint8_t  u8BitValue = cpu8BitTable[u16Index / 8] & (1 << (7 - u16Index % 8));

            ST7735_FillRect(pHandle, x, y, u8Scale, u8Scale, u8BitValue ? COLOR_RGB565_WHITE : COLOR_RGB565_BLACK);

            y += u8Scale;
        }

        y = _y;
        x += u8Scale;
    }
}

#endif

void QRCode_Test()
{
    uint8_t au8BitTable[QR_MAX_BITDATA];

    uint8_t u8Side = qr_encode(QR_LEVEL_L /* ECC Level */, 0, "123abc你好!?", 0, au8BitTable);
    PRINTF("side: %d\n", u8Side);

#if CONFIG_QRCODE_DISPLAY_MODE == QRCODE_DISPLAY_ASCII

    int i, j;

    for (i = 0; i < u8Side + 2; i++)
    {
        PRINTF("██");
    }

    PRINTF("\n");
    for (i = 0; i < u8Side; i++)
    {
        PRINTF("██");
        for (j = 0; j < u8Side; j++)
        {
            uint16_t a = i * u8Side + j;
            PRINTF((au8BitTable[a / 8] & (1 << (7 - a % 8))) ? "  " : "██");
        }
        PRINTF("██");
        PRINTF("\n");
    }

    for (i = 0; i < u8Side + 2; i++)
    {
        PRINTF("██");
    }

    PRINTF("\n");

#elif CONFIG_QRCODE_DISPLAY_MODE == QRCODE_DISPLAY_IMAGE

    static spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5}, /*SDA*/
        .MOSI = {GPIOA, GPIO_PIN_5},
        .SCLK = {GPIOA, GPIO_PIN_6}, /*SCL*/
        .CS   = {GPIOA, GPIO_PIN_3}, /*CS*/
    };

    static spi_st7735_t st7735 = {
        .hSPI = &spi,
        .DC   = {GPIOC, GPIO_PIN_0 }, /*DC*/
#if CONFIG_ST7735_RST_CONTROL_SW
        .RST = {GPIOA, GPIO_PIN_0 }, /*RST*/
#endif
#if CONFIG_ST7735_BL_CONTROL_SW
        .BL = {GPIOC, GPIO_PIN_13}, /*BL*/
#endif
        .u16ScreenW = ST7735_WIDTH,
        .u16ScreenH = ST7735_HEIGHT,
        .u16OffsetX = 1,
        .u16OffsetY = 2,
    };

    SPI_Master_Init(&spi, 1000000, SPI_DUTYCYCLE_50_50, ST7735_SPI_TIMING | SPI_FLAG_SOFT_CS);

    ST7735_Init(&st7735);
    ST7735_BackLight(&st7735, true);

    ST7735_FillRect(&st7735, 0, 0, st7735.u16ScreenW, st7735.u16ScreenH, COLOR_RGB565_BLACK);
    ST7735_DrowQRCode(&st7735, 10, 10, u8Side, 4, (const uint8_t*)au8BitTable);

    while (1)
    {
    }

#endif
}

#endif
