#include "i2c_ssd1306.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "ssd1306"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

/**
 * @brief commands
 */

#define CMD_HARGEPUMP           0x8D
#define CMD_COLUMNADDR          0x21
#define CMD_COMSCANDEC          0xC8
#define CMD_COMSCANINC          0xC0
#define CMD_DISPLAYALLON        0xA5
#define CMD_DISPLAYALLON_RESUME 0xA4
#define CMD_DISPLAYOFF          0xAE
#define CMD_DISPLAYON           0xAF
#define CMD_EXTERNALVCC         0x01
#define CMD_INVERTDISPLAY       0xA7
#define CMD_MEMORYMODE          0x20
#define CMD_NORMALDISPLAY       0xA6
#define CMD_PAGEADDR            0x22
#define CMD_SEGREMAP            0xA0
#define CMD_SETCOMPINS          0xDA
#define CMD_SETCONTRAST         0x81
#define CMD_SETDISPLAYCLOCKDIV  0xD5
#define CMD_SETDISPLAYOFFSET    0xD3
#define CMD_SETHIGHCOLUMN       0x10
#define CMD_SETLOWCOLUMN        0x00
#define CMD_SETMULTIPLEX        0xA8
#define CMD_SETPRECHARGE        0xD9
#define CMD_SETSEGMENTREMAP     0xA1
#define CMD_SETSTARTLINE        0x40
#define CMD_SETVCOMDETECT       0xDB
#define CMD_SWITCHCAPVCC        0x02

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief 写指令
 */
static inline void SSD1306_WriteCmd(i2c_ssd1306_t* pHandle, uint8_t u8Cmd)
{
    I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, 0x00, u8Cmd, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

/**
 * @brief 写数据
 */
static inline void SSD1306_WriteData(i2c_ssd1306_t* pHandle, uint8_t u8Data)
{
    I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, 0x40, u8Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

/**
 * @brief 写数据块
 */
static inline void SSD1306_WriteBlockData(i2c_ssd1306_t* pHandle, const uint8_t* cpu8Data, uint16_t u16Length)
{
    I2C_Master_WriteBlock(pHandle->hI2C, pHandle->u8SlvAddr, 0x40, (uint8_t*)cpu8Data, u16Length, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

status_t SSD1306_Init(i2c_ssd1306_t* pHandle)
{
    ASSERT(pHandle->u8Rows, "rows must greate than 0");
    ASSERT(pHandle->u8Cols, "cols must greate than 0");

    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    SSD1306_WriteCmd(pHandle, 0xAE);  // display off

    SSD1306_WriteCmd(pHandle, 0x20);  // Set Memory Addressing Mode
    SSD1306_WriteCmd(pHandle, 0x10);  // 00, Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid

    SSD1306_WriteCmd(pHandle, 0xB0);  // Set Page Start Address for Page Addressing Mode,0-7
    SSD1306_WriteCmd(pHandle, 0xC8);  // Set COM Output Scan Direction
    SSD1306_WriteCmd(pHandle, 0x00);  // set low column address
    SSD1306_WriteCmd(pHandle, 0x10);  // set high column address
    SSD1306_WriteCmd(pHandle, 0x40);  // set start line address
    SSD1306_WriteCmd(pHandle, 0x81);  // set contrast control register
    SSD1306_WriteCmd(pHandle, 0xFF);  // brightness (0x00~0xFF)
    SSD1306_WriteCmd(pHandle, 0xA1);  // set segment re-map 0 to 127
    SSD1306_WriteCmd(pHandle, 0xA6);  // set normal display

    if (pHandle->u8Rows == 32 / 8)  // 091inch
    {
        SSD1306_WriteCmd(pHandle, 0xA8);  // set multiplex ratio (16 to 63)
        SSD1306_WriteCmd(pHandle, 0x1F);  // 31=32-1 pixel

        SSD1306_WriteCmd(pHandle, 0xDA);  // set com pins hardware configuration
        SSD1306_WriteCmd(pHandle, 0x02);
    }
    else if (pHandle->u8Rows == 64 / 8)  // 096inch
    {
        SSD1306_WriteCmd(pHandle, 0xA8);  // set multiplex ratio (16 to 63)
        SSD1306_WriteCmd(pHandle, 0x3F);  // 63=64-1 pixel

        SSD1306_WriteCmd(pHandle, 0xDA);  // set com pins hardware configuration
        SSD1306_WriteCmd(pHandle, 0x12);
    }
    else
    {
        return ERR_NOT_SUPPORTED;  // unsupported size
    }

    SSD1306_WriteCmd(pHandle, 0xA4);  // 0xA4,Output follows RAM content;0xA5,Output ignores RAM content

    SSD1306_WriteCmd(pHandle, 0xD3);  // -set display offset
    SSD1306_WriteCmd(pHandle, 0x00);  // -not offset

    SSD1306_WriteCmd(pHandle, 0xD5);  // set display clock divide ratio/oscillator frequency
    SSD1306_WriteCmd(pHandle, 0xF0);  // set divide ratio

    SSD1306_WriteCmd(pHandle, 0xD9);  // set pre-charge period
    SSD1306_WriteCmd(pHandle, 0x22);  //

    SSD1306_WriteCmd(pHandle, 0xDB);  // set vcomh
    SSD1306_WriteCmd(pHandle, 0x20);  // 0x20, 0.77xVcc

    SSD1306_WriteCmd(pHandle, 0x8D);  // set DC-DC enable
    SSD1306_WriteCmd(pHandle, 0x14);

    SSD1306_WriteCmd(pHandle, 0xAF);  // turn on SSD1306 panel

    return ERR_NONE;
}

void SSD1306_ClearScreen(i2c_ssd1306_t* pHandle)
{
    SSD1306_FillScreen(pHandle, 0x00);
}

void SSD1306_Wakeup(i2c_ssd1306_t* pHandle)
{
    SSD1306_WriteCmd(pHandle, 0x8D);  // 设置电荷泵
    SSD1306_WriteCmd(pHandle, 0x14);  // 开启电荷泵
    SSD1306_WriteCmd(pHandle, 0xAF);  // SSD1306唤醒
}

void SSD1306_Sleep(i2c_ssd1306_t* pHandle)
{
    SSD1306_WriteCmd(pHandle, 0x8D);  // 设置电荷泵
    SSD1306_WriteCmd(pHandle, 0x10);  // 关闭电荷泵
    SSD1306_WriteCmd(pHandle, 0xAE);  // SSD1306休眠
}

void SSD1306_SetCursor(i2c_ssd1306_t* pHandle, uint8_t u8Col, uint8_t u8Row)
{
    // Set the current RAM page address.
    SSD1306_WriteCmd(pHandle, 0xB0 + u8Row);

    SSD1306_WriteCmd(pHandle, ((u8Col & 0xF0) >> 4) | 0x10);
    SSD1306_WriteCmd(pHandle, u8Col & 0x0F);
}

void SSD1306_FillScreen(i2c_ssd1306_t* pHandle, uint8_t u8Data)
{
    static uint8_t s_au8Buff[16] = {0x00};

    const uint8_t u8ColSpan = ARRAY_SIZE(s_au8Buff);

    if ((pHandle->u8Cols % u8ColSpan) != 0)
    {
        ASSERT(0, "u8ColSpan and u8Cols need to have an integer multiple relationship");  // 倍数关系
    }

    for (uint8_t u8Idx = 0; u8Idx < u8ColSpan; ++u8Idx)
    {
        s_au8Buff[u8Idx] = u8Data;
    }

    for (uint8_t u8Row = 0; u8Row < pHandle->u8Rows; ++u8Row)
    {
        for (uint8_t u8Col = 0; u8Col < pHandle->u8Cols; u8Col += u8ColSpan)
        {
            SSD1306_SetCursor(pHandle, u8Col, u8Row);
            SSD1306_WriteBlockData(pHandle, (uint8_t*)&s_au8Buff[0], u8ColSpan);
        }
    }
}

void SSD1306_DisplayBuffer(i2c_ssd1306_t* pHandle, uint8_t u8RowStart, uint8_t u8ColStart, uint8_t u8RowSpan, uint8_t u8ColSpan, const uint8_t* cpu8Buffer)
{
    while (u8RowSpan--)
    {
        SSD1306_SetCursor(pHandle, u8ColStart, u8RowStart++);
        SSD1306_WriteBlockData(pHandle, cpu8Buffer, u8ColSpan);
        cpu8Buffer += u8ColSpan;
    }
}

void SSD1306_FillBuffer(i2c_ssd1306_t* pHandle, const uint8_t* cpu8Buffer)
{
    SSD1306_SetCursor(pHandle, 0, 0);
    SSD1306_WriteBlockData(pHandle, cpu8Buffer, pHandle->u8Rows * pHandle->u8Cols);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#include "./resources/oled_image_spaceman.h"

void SSD1306_Test(i2c_mst_t* hI2C)
{
    i2c_ssd1306_t ssd1306 = {
        .hI2C      = hI2C,
        .u8Cols    = 128,
        .u8Rows    = 64 / 8,
        .u8SlvAddr = SSD1306_ADDRESS_LOW,
    };

    SSD1306_Init(&ssd1306);
    SSD1306_ClearScreen(&ssd1306);

    SSD1306_DisplayBuffer(&ssd1306, 2, 0, 4, 25, ICON_TEMP);    // 温度图标
    SSD1306_DisplayBuffer(&ssd1306, 2, 48, 4, 19, ICON_WATER);  // 湿度图标
    SSD1306_DisplayBuffer(&ssd1306, 2, 41, 1, 3, ICON_DEGREE);  // 摄氏度(1个点)

    uint8_t  u8FrameCount = ARRAY_SIZE(GIF_SPACEMAN);
    uint16_t u16FrameSize = ARRAY_SIZE(GIF_SPACEMAN[0]);  //  frame size

    while (1)
    {
        const uint8_t* pBuffer = (const uint8_t*)GIF_SPACEMAN;

        for (uint8_t u8FrameIndex = 0; u8FrameIndex < u8FrameCount; ++u8FrameIndex, pBuffer += u16FrameSize)
        {
            SSD1306_DisplayBuffer(&ssd1306, 2, 95, 4, 32, pBuffer);  // 太空人
            DelayBlockMs(1000 / 39);                                 // fps
        }
    }
}

#endif
