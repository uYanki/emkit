#include "i2c_lcd1602.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "lcd1602"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * 引脚说明:
 * - VL: 对比度调整, 接近VDD时对比度最弱, 接近VSS时对比度最强.
 * - RS: 寄存器选择, 高电平选择数据寄存器, 低电平选择指令寄存器
 * - RW: 读写信号线, 高电平进行读操作, 低电平进行写操作
 *     - RS=RW=0   可写入指令或显示地址
 *     - RS=1,RW=1 可读取忙信号
 *     - RS=1,RW=0 可写入数据
 * - EN: 使能端, 其在高电平变低电平(下降沿)时, 液晶模块执行命令
 * - D0-D7: 8位双向数据线
 * - BLA: 背光正极
 * - BLK: 背光负极
 */

// Pin - Register select
#define LCD1602_RS_HIGH (1 << 0)
#define LCD1602_RS_LOW  (0 << 0)
// Pin - Enable
#define LCD1602_EN_HIGH (1 << 2)
#define LCD1602_EN_LOW  (0 << 2)
// Pin - BackLight
#define LCD1602_BL_HIGH (1 << 3)
#define LCD1602_BL_LOW  (0 << 3)

/**
 * @brief commands
 */
#define LCD1602_CLEARDISPLAY   0x01  // clear display
#define LCD1602_RETURNHOME     0x02  // return home
#define LCD1602_ENTRYMODESET   0x04  // entry modeLCD1602_Set
#define LCD1602_DISPLAYCONTROL 0x08  // display control
#define LCD1602_CURSORSHIFT    0x10  // cursor shift
#define LCD1602_FUNCTIONSET    0x20  // functionLCD1602_Set
#define LCD1602_SETCGRAMADDR   0x40  // LCD1602_Set cgram addr
#define LCD1602_SETDDRAMADDR   0x80  // LCD1602_Set ddram addr

/**
 * @brief flags for display entry mode
 */
#define LCD1602_ENTRYRIGHT          0x00  // decrement cursor
#define LCD1602_ENTRYLEFT           0x02  // increment cursor
#define LCD1602_ENTRYSHIFTINCREMENT 0x01
#define LCD1602_ENTRYSHIFTDECREMENT 0x00

/**
 * @brief flags for display on/off control
 */
#define LCD1602_DISPLAYON  0x04
#define LCD1602_DISPLAYOFF 0x00
#define LCD1602_CURSORON   0x02
#define LCD1602_CURSOROFF  0x00
#define LCD1602_BLINKON    0x01
#define LCD1602_BLINKOFF   0x00

/**
 * @brief flags for display/cursor shift
 */
#define LCD1602_DISPLAYMOVE 0x08
#define LCD1602_CURSORMOVE  0x00
#define LCD1602_MOVERIGHT   0x04
#define LCD1602_MOVELEFT    0x00

/**
 * @brief flags for functionLCD1602_Set
 */
#define LCD1602_8BITMODE 0x10
#define LCD1602_4BITMODE 0x00
#define LCD1602_2LINE    0x08
#define LCD1602_1LINE    0x00
#define LCD1602_5x10DOTS 0x04
#define LCD1602_5x7DOTS  0x00
// LCD1602_4BITMODE | LCD1602_1LINE | LCD1602_5x7DOTS = 0x00
// LCD1602_4BITMODE | LCD1602_2LINE | LCD1602_5x7DOTS = 0x08
// LCD1602_8BITMODE | LCD1602_1LINE | LCD1602_5x7DOTS = 0x10
// LCD1602_8BITMODE | LCD1602_2LINE | LCD1602_5x7DOTS = 0x18

// internal line length of the display
#define LCD1602_LINE_LENGTH 0x40
//  DDRAM address of first char of line
#define LCD1602_LINE1_START 0x00
#define LCD1602_LINE2_START 0x40
#define LCD1602_LINE3_START 0x14
#define LCD1602_LINE4_START 0x54

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static uint8_t m_u8RowOffset[] = {LCD1602_LINE1_START, LCD1602_LINE2_START, LCD1602_LINE3_START, LCD1602_LINE4_START};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static status_t LCD1602_WriteByte(i2c_lcd1602_t* pHandle, uint8_t u8Data, uint8_t u8Mode /* rs = 0x00:cmd, 0x01:data */)
{
    uint8_t au8Buff[4];

    uint8_t u8High = u8Data & 0xf0;
    uint8_t u8Low  = (u8Data << 4) & 0xf0;

    u8Mode |= pHandle->_u8Backlight;

    u8High |= u8Mode;
    u8Low |= u8Mode;

    au8Buff[0] = u8High | LCD1602_EN_HIGH;
    au8Buff[1] = u8High | LCD1602_EN_LOW;
    au8Buff[2] = u8Low | LCD1602_EN_HIGH;
    au8Buff[3] = u8Low | LCD1602_EN_LOW;

    DelayBlockMs(5);

    return I2C_Master_TransmitBlock(pHandle->hI2C, pHandle->u8SlvAddr, au8Buff, ARRAY_SIZE(au8Buff), I2C_FLAG_SIGNAL_DEFAULT);
}

static inline status_t LCD1602_WriteCmd(i2c_lcd1602_t* pHandle, uint8_t u8Data)
{
    return LCD1602_WriteByte(pHandle, u8Data, 0x00);
}

static inline status_t LCD1602_WriteData(i2c_lcd1602_t* pHandle, uint8_t u8Data)
{
    return LCD1602_WriteByte(pHandle, u8Data, 0x01);
}

status_t LCD1602_Init(i2c_lcd1602_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    pHandle->_u8Backlight = LCD1602_BL_HIGH;

    ERRCHK_RETURN(LCD1602_WriteCmd(pHandle, 0x28));                         // 0b00111000 显示模式设置
    ERRCHK_RETURN(LCD1602_WriteCmd(pHandle, 0x0C));                         // 0b00001100 开启显示开关但不闪烁
    ERRCHK_RETURN(LCD1602_WriteCmd(pHandle, pHandle->_u8DispMode = 0x06));  // 0b00000110 显示光标移动位置
    ERRCHK_RETURN(LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl = 0x01));  // 0b00000001 清除屏幕

#if 0

    //  FunctionLCD1602_Set --> DL=0 (4 bit mode), N = 1 (2 u8Row display) F = 0 (5x7 characters)
    LCD1602_WriteCmd(pHandle, LCD1602_FUNCTIONSET | LCD1602_4BITMODE | LCD1602_2LINE | LCD1602_5x7DOTS);

    // LCD1602_WriteCmd( pHandle,this,LCD1602_RETURNHOME);
    // Display on/off control --> D=0,C=0, B=0  ---> display off
    // LCD1602_WriteCmd( pHandle,this,LCD1602_DISPLAYCONTROL | LCD1602_DISPLAYOFF);
    // clear display
    LCD1602_WriteCmd(pHandle, LCD1602_CLEARDISPLAY);
    // Entry modeLCD1602_Set --> I/D = 1 (increment cursor) & S = 0 (no shift)
    LCD1602_WriteCmd(pHandle, pHandle->_u8DispMode = LCD1602_ENTRYMODESET | LCD1602_ENTRYLEFT);
    // Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
    LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl = LCD1602_DISPLAYCONTROL | LCD1602_DISPLAYON | LCD1602_CURSOROFF | LCD1602_BLINKOFF);

#endif

    return ERR_NONE;
}

// Turn the (optional) backlight off/on
status_t LCD1602_BacklightOn(i2c_lcd1602_t* pHandle)
{
    return I2C_Master_TransmitByte(pHandle->hI2C, pHandle->u8SlvAddr, pHandle->_u8Backlight = LCD1602_BL_HIGH, I2C_FLAG_SIGNAL_DEFAULT);
}

status_t LCD1602_BacklightOff(i2c_lcd1602_t* pHandle)
{
    return I2C_Master_TransmitByte(pHandle->hI2C, pHandle->u8SlvAddr, pHandle->_u8Backlight = LCD1602_BL_LOW, I2C_FLAG_SIGNAL_DEFAULT);
}

status_t LCD1602_ShowChar(i2c_lcd1602_t* pHandle, const char ch)
{
    return LCD1602_WriteData(pHandle, ch);
}

status_t LCD1602_ShowString(i2c_lcd1602_t* pHandle, const char* str)
{
    while (*str)
    {
        ERRCHK_RETURN(LCD1602_WriteData(pHandle, *str++));
    }

    return ERR_NONE;
}

status_t LCD1602_ClearLine(i2c_lcd1602_t* pHandle, uint8_t u8Row)
{
    // 仅适用于写入方向为左的情况 ( pHandle->_u8DispMode & LCD1602_ENTRYLEFT != 0 )
    ERRCHK_RETURN(LCD1602_SetCursor(pHandle, 0x00, u8Row));
    ERRCHK_RETURN(LCD1602_ShowString(pHandle, "                "));  // 16
    return ERR_NONE;
}

status_t LCD1602_SetCursor(i2c_lcd1602_t* pHandle, uint8_t u8Col, uint8_t u8Row)
{
    if (u8Row >= LCD1602_DISPLAY_LINES)
    {
        u8Row = LCD1602_DISPLAY_LINES - 1;
    }

    return LCD1602_WriteCmd(pHandle, LCD1602_SETDDRAMADDR | (u8Col + m_u8RowOffset[u8Row]));
}

status_t LCD1602_ShowStringAt(i2c_lcd1602_t* pHandle, uint8_t u8Col, uint8_t u8Row, const char* str)
{
    ERRCHK_RETURN(LCD1602_SetCursor(pHandle, u8Col, u8Row));
    ERRCHK_RETURN(LCD1602_ShowString(pHandle, str));
    return ERR_NONE;
}

// 显示字符串（带自动换行）
status_t LCD1602_ShowStringWrap(i2c_lcd1602_t* pHandle, uint8_t u8Col, uint8_t u8Row, const char* str)
{
    // LCD1602_Set cursor
    u8Row += u8Col / LCD1602_DISPLAY_LENGTH;

    if (u8Row >= LCD1602_DISPLAY_LINES)
    {
        return ERR_OVERFLOW;
    }

    u8Col %= LCD1602_DISPLAY_LENGTH;
    ERRCHK_RETURN(LCD1602_WriteCmd(pHandle, LCD1602_SETDDRAMADDR | (u8Col + m_u8RowOffset[u8Row])));

    // LCD1602_Show string
    for (uint8_t i = u8Col; *str; ++i)
    {
        if (i == LCD1602_DISPLAY_LENGTH)
        {
            if (++u8Row == LCD1602_DISPLAY_LINES)
            {
                return ERR_NONE;
            }

            ERRCHK_RETURN(LCD1602_WriteCmd(pHandle, LCD1602_SETDDRAMADDR | m_u8RowOffset[u8Row]));  // wrap
            i = 0;
        }
        ERRCHK_RETURN(LCD1602_WriteData(pHandle, *str++));
    }

    return ERR_NONE;
}

// Turn the display on/off (quickly)
status_t LCD1602_DisplayOn(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl |= LCD1602_DISPLAYON);
}

status_t LCD1602_DisplayOff(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl &= ~LCD1602_DISPLAYON);
}

// Turns the underline cursor on/off
status_t LCD1602_CursorOn(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl |= LCD1602_CURSORON);
}

status_t LCD1602_CursorOff(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl &= ~LCD1602_CURSORON);
}

// Turn on and off the blinking cursor
status_t LCD1602_BlinkOn(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl |= LCD1602_BLINKON);
}

status_t LCD1602_BlinkOff(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispCtrl &= ~LCD1602_BLINKON);
}

// This is for text that flows Left to Right
status_t LCD1602_LeftToRight(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispMode |= LCD1602_ENTRYLEFT);
}

// This is for text that flows Right to Left
status_t LCD1602_RightToLeft(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispMode &= ~LCD1602_ENTRYLEFT);
}

// This will 'right justify' text from the cursor 每写1个字符，屏幕左移1次，但光标位置不变, 以实现内容右对齐光标
status_t LCD1602_AutoScrollOn(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispMode |= LCD1602_ENTRYSHIFTINCREMENT);
}

// This will 'left justify' text from the cursor
status_t LCD1602_AutoScrollOff(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, pHandle->_u8DispMode &= ~LCD1602_ENTRYSHIFTINCREMENT);
}

// Allows us to fill the first 8 CGRAM locations with custom characters 显示自定义字符
status_t LCD1602_CreateChar(i2c_lcd1602_t* pHandle, uint8_t u8Location /*0~7*/, uint8_t u8CharMap[])
{
    // uint8_t cgram[8] = {0x40, 0x48, 0x50, 0x58, 0x60, 0x68, 0x70, 0x78};

    u8Location &= 0x07;  // only have 8 locations
    ERRCHK_RETURN(LCD1602_WriteCmd(pHandle, LCD1602_SETCGRAMADDR | (u8Location << 3)));

    for (uint8_t i = 0; i < 8; ++i)
    {
        ERRCHK_RETURN(LCD1602_WriteData(pHandle, u8CharMap[i]));
    }

    return ERR_NONE;
}

// These commands scroll the display without changing the RAM  (once)
// LCD1602_CURSORSHIFT: 光标移动（可选），LCD1602_DISPLAYMOVE：显示屏内容移动（可选），LCD1602_MOVELEFT / LCD1602_MOVERIGHT：移动的方向
status_t LCD1602_ScrollLeft(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, LCD1602_CURSORSHIFT | LCD1602_DISPLAYMOVE | LCD1602_MOVELEFT);
}

status_t LCD1602_ScrollRight(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, LCD1602_CURSORSHIFT | LCD1602_DISPLAYMOVE | LCD1602_MOVERIGHT);
}

// LCD1602_Set cursor position to zero 光标归位
status_t LCD1602_ReturnHome(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, LCD1602_RETURNHOME);
}

// clear display,LCD1602_Set cursor position to zero
status_t LCD1602_Clear(i2c_lcd1602_t* pHandle)
{
    return LCD1602_WriteCmd(pHandle, LCD1602_CLEARDISPLAY);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#include "resources/lcd1602_video_badapple.h"

#define FRAME_SIZE   40  // 40 byte
#define FRAME_START  40  // start frame (起始帧)
#define FRAME_SKIP   20  // skip n frame per step (类似快进)
#define BUFFER_STEP  (FRAME_SIZE * FRAME_SKIP)
#define BUFFER_START (FRAME_SIZE * FRAME_START)

static void LCD1602_PlayVideo(i2c_lcd1602_t* pHandle, const uint8_t* cpu8Buffer, uint32_t u32BufferSize)
{
    uint32_t j = BUFFER_START;

    while (1)
    {
        // 1 frame: 40(5*8) bytes to 64 bytes(8*8)
        for (uint8_t i = 0; i < 8; i++, j += 5)
        {
            const uint8_t* p = &cpu8Buffer[j];

            // 5 byte -> 8 byte
            uint8_t image[8] = {
#if LCD1602_VIDEO_MODE
                // 0:00011111
                p[0],
                // 0:11100000 1:00000011
                p[0] >> 5 | p[1] << 3,
                // 1:01111100
                p[1] >> 2,
                // 1:10000000 2:00001111
                p[1] >> 7 | p[2] << 1,
                // 2:11110000 3:00000001
                p[2] >> 4 | p[3] << 4,
                // 3:00111110
                p[3] >> 1,
                // 3:11000000 4:00000111
                p[3] >> 6 | p[4] << 2,
                // 4:11111000
                p[4] >> 3
#else
                // 0:1111 1000
                p[0] >> 3,
                // 0:0000 0111  1:1100 0000
                ((p[0] & 0x07) << 2) | (p[1] >> 6),
                // 1:0011 1110
                (p[1] & 0x3E) >> 1,
                // 1:0000 0001  2:1111 0000
                ((p[1] & 0x01) << 4) | (p[2] >> 4),
                // 2:0000 1111  3:1000 0000
                ((p[2] & 0x0F) << 1) | (p[3] >> 7),
                // 3:0111 1100
                (p[3] & 0x7C) >> 2,
                // 3:0000 0011  4:1110 0000
                ((p[3] & 0x03) << 3) | (p[4] >> 5),
                // 4:0001 1111
                p[4] & 0x1F
#endif
            };
            LCD1602_CreateChar(pHandle, i, image);
        }

        LCD1602_SetCursor(pHandle, 0, 0);

        for (int i = 0; i < 4; ++i)
        {
            LCD1602_ShowChar(pHandle, i);
        }

        LCD1602_SetCursor(pHandle, 0, 1);

        for (int i = 4; i < 8; ++i)
        {
            LCD1602_ShowChar(pHandle, i);
        }

        if (j >= u32BufferSize)
        {
            j = BUFFER_START;
        }
        else
        {
            j += BUFFER_STEP;
        }
    }
}

void LCD1602_Test(i2c_mst_t* hI2C)
{
    i2c_lcd1602_t lcd1602 = {
        .hI2C      = hI2C,
        .u8SlvAddr = LCD1602_ADDRESS_A111,
    };

    // 初始化(显示乱码调低I2C频率)
    DelayBlockMs(1000);
    LCD1602_Init(&lcd1602);
    DelayBlockMs(1000);

    // 从左往右写字符串
    LCD1602_ShowStringAt(&lcd1602, 0, 0, "Hello");
    LCD1602_ShowStringAt(&lcd1602, 0, 1, "uYanki!");

    // 光标闪烁
    LCD1602_CursorOn(&lcd1602);
    LCD1602_BlinkOn(&lcd1602);
    DelayBlockMs(2000);

    // 清屏
    LCD1602_Clear(&lcd1602);
    // 换行文本
    LCD1602_ShowStringWrap(&lcd1602, 12, 0, "hello,wrold.123456789");
    DelayBlockMs(3000);

    // 显示全部字符
    LCD1602_Clear(&lcd1602);
    for (int j = 0; j < 16; ++j)
    {
        LCD1602_ReturnHome(&lcd1602);
        for (int i = 0; i < 16; ++i)
        {
            LCD1602_ShowChar(&lcd1602, j * 16 + i);
        }
        DelayBlockMs(2000);
        LCD1602_Clear(&lcd1602);
    }

    // 清行
    LCD1602_ClearLine(&lcd1602, 0);

    // 显示自定义字符
    uint8_t bell[8]     = {0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04};
    uint8_t note[8]     = {0x02, 0x03, 0x02, 0x0E, 0x1E, 0x0C, 0x00};
    uint8_t clock[8]    = {0x00, 0x0E, 0x15, 0x17, 0x11, 0x0E, 0x00};
    uint8_t heart[8]    = {0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00};
    uint8_t duck[8]     = {0x00, 0x0C, 0x1D, 0x0F, 0x0F, 0x06, 0x00};
    uint8_t check[8]    = {0x00, 0x01, 0x03, 0x16, 0x1C, 0x08, 0x00};
    uint8_t cross[8]    = {0x00, 0x1B, 0x0E, 0x04, 0x0E, 0x1B, 0x00};
    uint8_t retarrow[8] = {0x01, 0x01, 0x05, 0x09, 0x1F, 0x08, 0x04};

    LCD1602_CreateChar(&lcd1602, 0, bell);
    LCD1602_CreateChar(&lcd1602, 1, note);
    LCD1602_CreateChar(&lcd1602, 2, clock);
    LCD1602_CreateChar(&lcd1602, 3, heart);
    LCD1602_CreateChar(&lcd1602, 4, duck);
    LCD1602_CreateChar(&lcd1602, 5, check);
    LCD1602_CreateChar(&lcd1602, 6, cross);
    LCD1602_CreateChar(&lcd1602, 7, retarrow);

    LCD1602_ReturnHome(&lcd1602);
    for (int i = 0; i < 8; ++i)
    {
        LCD1602_ShowChar(&lcd1602, i);
    }

    DelayBlockMs(4000);
    LCD1602_ClearLine(&lcd1602, 0);

    // 文本右对齐光标
    LCD1602_AutoScrollOn(&lcd1602);
    LCD1602_ShowStringAt(&lcd1602, 13, 0, "Hello");
    DelayBlockMs(2000);
    LCD1602_AutoScrollOff(&lcd1602);  // 开了记得关

    // 光标归位
    DelayBlockMs(1000);
    LCD1602_ReturnHome(&lcd1602);
    DelayBlockMs(2000);

    // 从右往左写字符串
    LCD1602_RightToLeft(&lcd1602);
    LCD1602_ShowStringAt(&lcd1602, LCD1602_DISPLAY_LENGTH - 1, 0, "Hello");
    LCD1602_ShowStringAt(&lcd1602, LCD1602_DISPLAY_LENGTH - 1, 1, "uyk!");
    LCD1602_LeftToRight(&lcd1602);

    // 字符滚动
    int8_t times = 3;
    while (times--)
    {
        DelayBlockMs(400);
        LCD1602_ScrollLeft(&lcd1602);
        DelayBlockMs(400);
        LCD1602_ScrollLeft(&lcd1602);
        DelayBlockMs(400);
        LCD1602_ScrollRight(&lcd1602);
        DelayBlockMs(400);
        LCD1602_ScrollRight(&lcd1602);
    }

    // 屏幕背光
    DelayBlockMs(1000);
    LCD1602_BacklightOff(&lcd1602);
    DelayBlockMs(1000);
    LCD1602_BacklightOn(&lcd1602);

    // 播放 badapple
    LCD1602_PlayVideo(&lcd1602, video_badapple, ARRAY_SIZE(video_badapple));

    // 结束
    LCD1602_Clear(&lcd1602);
    LCD1602_ShowStringAt(&lcd1602, 0, 0, "ok");
}

#endif /* CONFIG_DEMOS_SW */
