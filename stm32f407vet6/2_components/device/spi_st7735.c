#include "spi_st7735.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "st7735"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief commands
 */

#define CMD_NOP     0x00
#define CMD_SWRESET 0x01
#define CMD_RDDID   0x04
#define CMD_RDDST   0x09

#define CMD_SLPIN   0x10
#define CMD_SLPOUT  0x11
#define CMD_PTLON   0x12
#define CMD_NORON   0x13

#define CMD_INVOFF  0x20
#define CMD_INVON   0x21
#define CMD_DISPOFF 0x28
#define CMD_DISPON  0x29
#define CMD_CASET   0x2A
#define CMD_RASET   0x2B
#define CMD_RAMWR   0x2C
#define CMD_RAMRD   0x2E

#define CMD_PTLAR   0x30
#define CMD_COLMOD  0x3A
#define CMD_MADCTL  0x36

#define CMD_FRMCTR1 0xB1
#define CMD_FRMCTR2 0xB2
#define CMD_FRMCTR3 0xB3
#define CMD_INVCTR  0xB4
#define CMD_DISSET5 0xB6

#define CMD_PWCTR1  0xC0
#define CMD_PWCTR2  0xC1
#define CMD_PWCTR3  0xC2
#define CMD_PWCTR4  0xC3
#define CMD_PWCTR5  0xC4
#define CMD_VMCTR1  0xC5

#define CMD_RDID1   0xDA
#define CMD_RDID2   0xDB
#define CMD_RDID3   0xDC
#define CMD_RDID4   0xDD

#define CMD_PWCTR6  0xFC

#define CMD_GMCTRP1 0xE0
#define CMD_GMCTRN1 0xE1

#define DELAY       0x80

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static void ST7735_SetWindow(spi_st7735_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void ST7735_Reset(spi_st7735_t* pHandle);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief 写指令
 */
static inline void ST7735_WriteCmd(spi_st7735_t* pHandle, uint8_t u8Cmd)
{
    PIN_WriteLevel(&pHandle->DC, PIN_LEVEL_LOW);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Cmd);
}

/**
 * @brief 写数据
 */
static inline void ST7735_WriteData(spi_st7735_t* pHandle, uint8_t u8Data)
{
    PIN_WriteLevel(&pHandle->DC, PIN_LEVEL_HIGH);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Data);
}

/**
 * @brief 写数据块
 */
static inline void ST7735_WriteBlockData(spi_st7735_t* pHandle, uint8_t* pu8Data, uint16_t u16Length)
{
    PIN_WriteLevel(&pHandle->DC, PIN_LEVEL_HIGH);
    SPI_Master_TransmitBlock(pHandle->hSPI, pu8Data, u16Length);
}

void ST7735_Init(spi_st7735_t* pHandle)
{
    // clang-format off

    static const uint8_t au8InitCmd[] = {

        // Init for 7735R, part 1 (red or green tab)
        15,  // 15 commands in list:
        CMD_SWRESET,
        DELAY,  //  1: Software reset, 0 args, w/delay
        150,    //     150 ms delay
        CMD_SLPOUT,
        DELAY,  //  2: Out of sleep mode, 0 args, w/delay
        255,    //     500 ms delay
        CMD_FRMCTR1,
        3,  //  3: Frame rate ctrl - normal mode, 3 args:
        0x01,
        0x2C,
        0x2D,  //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        CMD_FRMCTR2,
        3,  //  4: Frame rate control - idle mode, 3 args:
        0x01,
        0x2C,
        0x2D,  //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        CMD_FRMCTR3,
        6,  //  5: Frame rate ctrl - partial mode, 6 args:
        0x01,
        0x2C,
        0x2D,  //     Dot inversion mode
        0x01,
        0x2C,
        0x2D,  //     Line inversion mode
        CMD_INVCTR,
        1,     //  6: Display inversion ctrl, 1 arg, no delay:
        0x07,  //     No inversion
        CMD_PWCTR1,
        3,  //  7: Power control, 3 args, no delay:
        0xA2,
        0x02,  //     -4.6V
        0x84,  //     AUTO mode
        CMD_PWCTR2,
        1,     //  8: Power control, 1 arg, no delay:
        0xC5,  //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
        CMD_PWCTR3,
        2,     //  9: Power control, 2 args, no delay:
        0x0A,  //     Opamp current small
        0x00,  //     Boost frequency
        CMD_PWCTR4,
        2,     // 10: Power control, 2 args, no delay:
        0x8A,  //     BCLK/2, Opamp current small & Medium low
        0x2A,
        CMD_PWCTR5,
        2,  // 11: Power control, 2 args, no delay:
        0x8A,
        0xEE,
        CMD_VMCTR1,
        1,  // 12: Power control, 1 arg, no delay:
        0x0E,
        CMD_INVOFF,
        0,  // 13: Don't invert display, no args, no delay
        CMD_MADCTL,
        1,                       // 14: Memory access control (directions), 1 arg:
        CONFIG_ST7735_ROTATION,  //     row addr/col addr, bottom to top refresh
        CMD_COLMOD,
        1,     // 15: set color mode, 1 arg, no delay:
        0x05,  //     16-bit color

#if (CONFIG_ST7735_SIZE == ST7735_128X128) || (CONFIG_ST7735_SIZE == ST7735_160X128) || (CONFIG_ST7735_SIZE == ST7735_128X160)

        // Init for 7735R, part 2 (1.44" display)
        2,  //  2 commands in list:
        CMD_CASET,
        4,  //  1: Column addr set, 4 args, no delay:
        0x00,
        0x00,  //     XSTART = 0
        0x00,
        0x7F,  //     XEND = 127
        CMD_RASET,
        4,  //  2: Row addr set, 4 args, no delay:
        0x00,
        0x00,  //     XSTART = 0
        0x00,
        0x7F,  //     XEND = 127

#elif (CONFIG_ST7735_SIZE == ST7735_160X80) || (CONFIG_ST7735_SIZE == ST7735_80X160)

        // Init for 7735S, part 2 (160x80 display)
        3,  //  3 commands in list:
        CMD_CASET,
        4,  //  1: Column addr set, 4 args, no delay:
        0x00,
        0x00,  //     XSTART = 0
        0x00,
        0x4F,  //     XEND = 79
        CMD_RASET,
        4,  //  2: Row addr set, 4 args, no delay:
        0x00,
        0x00,  //     XSTART = 0
        0x00,
        0x9F,  //     XEND = 159
        CMD_INVON,
        0,  //  3: Invert colors

#endif

        // Init for 7735R, part 3 (red or green tab)
        4,  //  4 commands in list:
        CMD_GMCTRP1,
        16,  //  1: Gamma Adjustments (pos. polarity), 16 args, no delay:
        0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
        CMD_GMCTRN1,
        16,  //  2: Gamma Adjustments (neg. polarity), 16 args, no delay:
        0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
        CMD_NORON,
        DELAY,  //  3: Normal display on, no args, w/delay
        10,     //     10 ms delay
        CMD_DISPON,
        DELAY,  //  4: Main screen turn on, no args w/delay
        100,    //     100 ms delay

        // end
        0,
    };

    // clang-format on

    PIN_SetMode(&pHandle->DC, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);

#if CONFIG_ST7735_RST_CONTROL_SW
    PIN_SetMode(&pHandle->RST, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
#endif

#if CONFIG_ST7735_BL_CONTROL_SW
    PIN_SetMode(&pHandle->BL, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
#endif

    ST7735_Reset(pHandle);

    SPI_Master_Select(pHandle->hSPI);

    const uint8_t* cpu8Addr = au8InitCmd;
    uint8_t        u8CmdCnt, u8ArgCnt;
    uint16_t       u16TimeMs;

    while (1)
    {
        // count of command
        u8CmdCnt = *cpu8Addr++;
        if (u8CmdCnt == 0)
        {
            break;
        }
        while (u8CmdCnt--)
        {
            // write comand
            ST7735_WriteCmd(pHandle, *cpu8Addr++);
            // counf of arguments
            u8ArgCnt = *cpu8Addr++;
            // if high bit set, delay follows args
            if (u8ArgCnt & DELAY)
            {
                u16TimeMs = *cpu8Addr++;

                if (u16TimeMs == 255)
                {
                    u16TimeMs = 500;
                }

                DelayBlockMs(u16TimeMs);
            }
            else
            {
                ST7735_WriteBlockData(pHandle, (uint8_t*)cpu8Addr, u8ArgCnt);
                cpu8Addr += u8ArgCnt;
            }
        }
    }

    SPI_Master_Deselect(pHandle->hSPI);
}

static void ST7735_Reset(spi_st7735_t* pHandle)
{
#if CONFIG_ST7735_RST_CONTROL_SW
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_LOW);
    DelayBlockMs(10);
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_HIGH);
    DelayBlockMs(10);
#else
    // send soft reset command
#endif
}

static void ST7735_SetWindow(spi_st7735_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t data[4] = {0x00};

    x1 += pHandle->u16OffsetX;
    x2 += pHandle->u16OffsetX;
    y1 += pHandle->u16OffsetY;
    y2 += pHandle->u16OffsetY;

    // column address set
    data[0] = x1 >> 8;
    data[1] = x1 & 0xFF;
    data[2] = x2 >> 8;
    data[3] = x2 & 0xFF;
    ST7735_WriteCmd(pHandle, CMD_CASET);
    ST7735_WriteBlockData(pHandle, data, sizeof(data));

    // row address set
    data[0] = y1 >> 8;
    data[1] = y1 & 0xFF;
    data[2] = y2 >> 8;
    data[3] = y2 & 0xFF;
    ST7735_WriteCmd(pHandle, CMD_RASET);
    ST7735_WriteBlockData(pHandle, data, sizeof(data));

    // write to RAM
    ST7735_WriteCmd(pHandle, CMD_RAMWR);
}

void ST7735_InvertColor(spi_st7735_t* pHandle, bool bEnable)
{
    SPI_Master_Select(pHandle->hSPI);
    ST7735_WriteCmd(pHandle, bEnable ? CMD_INVON : CMD_INVOFF);
    SPI_Master_Deselect(pHandle->hSPI);
}

void ST7735_BackLight(spi_st7735_t* pHandle, bool bEnable)
{
#if CONFIG_ST7735_BL_CONTROL_SW
    PIN_WriteLevel(&pHandle->BL, bEnable ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);
#endif
}

bool ST7735_DrawPixel(spi_st7735_t* pHandle, uint16_t x, uint16_t y, uint16_t u16Color)
{
    if (x >= pHandle->u16ScreenW && y >= pHandle->u16ScreenH)
    {
        return false;
    }

    uint8_t au8Data[2];
    au8Data[0] = u16Color >> 8;
    au8Data[1] = u16Color & 0xFF;

    SPI_Master_Select(pHandle->hSPI);
    ST7735_SetWindow(pHandle, x, y, x + 1, y + 1);
    ST7735_WriteBlockData(pHandle, au8Data, sizeof(au8Data));
    SPI_Master_Deselect(pHandle->hSPI);

    return true;
}

bool ST7735_FillRect(spi_st7735_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t u16Color)
{
    if ((x >= pHandle->u16ScreenW) || (y >= pHandle->u16ScreenH))
    {
        return false;
    }

    if ((x + w) > pHandle->u16ScreenW)
    {
        w = pHandle->u16ScreenW - x;
    }

    if ((y + h) > pHandle->u16ScreenH)
    {
        h = pHandle->u16ScreenH - y;
    }

    uint32_t u32Size = w * h;
    uint8_t  au8Data[2];
    au8Data[0] = u16Color >> 8;
    au8Data[1] = u16Color & 0xFF;

    SPI_Master_Select(pHandle->hSPI);
    ST7735_SetWindow(pHandle, x, y, x + w - 1, y + h - 1);
    PIN_WriteLevel(&pHandle->DC, PIN_LEVEL_HIGH);

    while (u32Size--)
    {
        SPI_Master_TransmitBlock(pHandle->hSPI, au8Data, sizeof(au8Data));
    }

    SPI_Master_Deselect(pHandle->hSPI);

    return true;
}

bool ST7735_FillScreen(spi_st7735_t* pHandle, uint16_t u16Color)
{
    ST7735_FillRect(pHandle, 0, 0, pHandle->u16ScreenW, pHandle->u16ScreenH, u16Color);
}

bool ST7735_StartDraw(spi_st7735_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    if ((x >= pHandle->u16ScreenW) || (y >= pHandle->u16ScreenH))
    {
        return false;
    }

    if (((x + w) > pHandle->u16ScreenW) || ((y + h) > pHandle->u16ScreenH))
    {
        return false;
    }

    SPI_Master_Select(pHandle->hSPI);
    ST7735_SetWindow(pHandle, x, y, x + w - 1, y + h - 1);
    PIN_WriteLevel(&pHandle->DC, PIN_LEVEL_HIGH);

    return true;
}

void ST7735_PutColor(spi_st7735_t* pHandle, uint16_t u16Color)
{
    uint8_t au8Data[2];

    au8Data[0] = u16Color >> 8;
    au8Data[1] = u16Color & 0xFF;

    SPI_Master_TransmitBlock(pHandle->hSPI, au8Data, sizeof(au8Data));
}

void ST7735_StopDraw(spi_st7735_t* pHandle)
{
    SPI_Master_Deselect(pHandle->hSPI);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void ST7735_Test(void)
{
#if defined(BOARD_CS32F103C8T6_QG)

    spi_mst_t spi = {
        .MISO = {LCD_SDA_PIN},
        .MOSI = {LCD_SDA_PIN}, /*SDA*/
        .SCLK = {LCD_SCL_PIN}, /*SCL*/
        .CS   = {LCD_CS_PIN},  /*CS*/
    };

    spi_st7735_t st7735 = {
        .hSPI = &spi,
        .DC   = {LCD_DC_PIN}, /*DC*/
#if CONFIG_ST7735_RST_CONTROL_SW
        .RST = {LCD_RES_PIN}, /*RST*/
#endif
#if CONFIG_ST7735_BL_CONTROL_SW
        .BL = {
            LCD_BL_PIN, /*BL*/
#endif
            .u16ScreenW = ST7735_WIDTH,
            .u16ScreenH = ST7735_HEIGHT,
            .u16OffsetX = 1,
            .u16OffsetY = 2,
        };

#elif defined(BOARD_STM32F407VET6_XWS)

    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5},
        .MOSI = {GPIOA, GPIO_PIN_5}, /*SDA*/
        .SCLK = {GPIOA, GPIO_PIN_6}, /*SCL*/
        .CS   = {GPIOA, GPIO_PIN_3}, /*CS*/
    };

    spi_st7735_t st7735 = {
        .hSPI = &spi,
        .DC   = {GPIOC, GPIO_PIN_0 }, /*DC*/
#if CONFIG_ST7735_RST_CONTROL_SW
        .RST = {GPIOA, GPIO_PIN_1 }, /*RST*/
#endif
#if CONFIG_ST7735_BL_CONTROL_SW
        .BL = {GPIOC, GPIO_PIN_13}, /*BL*/
#endif
        .u16ScreenW = ST7735_WIDTH,
        .u16ScreenH = ST7735_HEIGHT,
        .u16OffsetX = 1,
        .u16OffsetY = 2,
    };

#elif defined(BOARD_AT32F415CB_DEV)

    spi_mst_t spi = {
#if 0
        .MISO = {GPIOA, GPIO_PINS_7}, 
        .MOSI = {GPIOA, GPIO_PINS_7}, /*SDA*/
        .SCLK = {GPIOA, GPIO_PINS_5}, /*SCL*/
        .CS   = {GPIOB, GPIO_PINS_7}, /*CS*/
        .SPIx = SPI1,
#else
        .MISO = {GPIOB, GPIO_PINS_15}, /*SDA*/
        .MOSI = {GPIOB, GPIO_PINS_15},
        .SCLK = {GPIOB, GPIO_PINS_13}, /*SCL*/
        .CS   = {GPIOB, GPIO_PINS_7 }, /*CS*/
        .SPIx = SPI2,
#endif
    };

    spi_st7735_t st7735 = {
        .hSPI = &spi,
        .DC   = {GPIOB, GPIO_PINS_6 }, /*DC*/
#if CONFIG_ST7735_RST_CONTROL_SW
        .RST = {GPIOB, GPIO_PINS_5 }, /*RST*/
#endif
#if CONFIG_ST7735_BL_CONTROL_SW
        .BL = {GPIOC, GPIO_PINS_13}, /*BL*/
#endif
        .u16ScreenW = ST7735_WIDTH,
        .u16ScreenH = ST7735_HEIGHT,
        .u16OffsetX = 1,
        .u16OffsetY = 2,
    };

#endif

    SPI_Master_Init(&spi, 1000000, SPI_DUTYCYCLE_50_50, ST7735_SPI_TIMING | SPI_FLAG_SOFT_CS);

    ST7735_Init(&st7735);
    ST7735_BackLight(&st7735, true);

    while (1)
    {
        ST7735_FillRect(&st7735, 0, 0, st7735.u16ScreenW, st7735.u16ScreenH, COLOR_RGB565_CYAN);
        DelayBlockMs(1000);
        ST7735_FillRect(&st7735, 0, 0, 128, 128, COLOR_RGB565_BLUE);
        DelayBlockMs(1000);
        ST7735_FillRect(&st7735, 0, 0, 64, 64, COLOR_RGB565_GREEN);
        DelayBlockMs(1000);
    }
}

#endif
