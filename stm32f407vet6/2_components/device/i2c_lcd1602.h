#ifndef __I2C_LCD1602_H__
#define __I2C_LCD1602_H__

/**
 * @brief lcd1602 over pcf8574 (Liquid Crystal Display)
 */

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LCD1602_DISPLAY_LINES  2   // rows, number of visible lines of the display
#define LCD1602_DISPLAY_LENGTH 16  // column, visibles characters per line of the display

#define LCD1602_ADDRESS_A000   0x20
#define LCD1602_ADDRESS_A001   0x21
#define LCD1602_ADDRESS_A010   0x22
#define LCD1602_ADDRESS_A011   0x23
#define LCD1602_ADDRESS_A100   0x24
#define LCD1602_ADDRESS_A101   0x25
#define LCD1602_ADDRESS_A110   0x26
#define LCD1602_ADDRESS_A111   0x27

typedef struct {
    __IN const i2c_mst_t* hI2C;
    __IN const uint8_t    u8SlvAddr;

    uint8_t _u8DispMode;
    uint8_t _u8DispCtrl;
    uint8_t _u8Backlight;

} i2c_lcd1602_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t LCD1602_Init(i2c_lcd1602_t* pHandle);

status_t LCD1602_ShowChar(i2c_lcd1602_t* pHandle, const char ch);
status_t LCD1602_ShowString(i2c_lcd1602_t* pHandle, const char* str);
status_t LCD1602_ShowStringAt(i2c_lcd1602_t* pHandle, uint8_t u8Col, uint8_t u8Row, const char* str);
status_t LCD1602_ShowStringWrap(i2c_lcd1602_t* pHandle, uint8_t u8Col, uint8_t u8Row, const char* str);

status_t LCD1602_CreateChar(i2c_lcd1602_t* pHandle, uint8_t u8Location, uint8_t u8CharMap[]);

status_t LCD1602_ClearLine(i2c_lcd1602_t* pHandle, uint8_t u8Row);
status_t LCD1602_Clear(i2c_lcd1602_t* pHandle);

status_t LCD1602_SetCursor(i2c_lcd1602_t* pHandle, uint8_t u8Col, uint8_t u8Row);

status_t LCD1602_ReturnHome(i2c_lcd1602_t* pHandle);

status_t LCD1602_DisplayOn(i2c_lcd1602_t* pHandle);
status_t LCD1602_DisplayOff(i2c_lcd1602_t* pHandle);

status_t LCD1602_CursorOn(i2c_lcd1602_t* pHandle);
status_t LCD1602_CursorOff(i2c_lcd1602_t* pHandle);
status_t LCD1602_BlinkOn(i2c_lcd1602_t* pHandle);
status_t LCD1602_BlinkOff(i2c_lcd1602_t* pHandle);

status_t LCD1602_ScrollLeft(i2c_lcd1602_t* pHandle);
status_t LCD1602_ScrollRight(i2c_lcd1602_t* pHandle);

status_t LCD1602_LeftToRight(i2c_lcd1602_t* pHandle);
status_t LCD1602_RightToLeft(i2c_lcd1602_t* pHandle);
status_t LCD1602_AutoScrollOn(i2c_lcd1602_t* pHandle);
status_t LCD1602_AutoScrollOff(i2c_lcd1602_t* pHandle);

status_t LCD1602_BacklightOn(i2c_lcd1602_t* pHandle);
status_t LCD1602_BacklightOff(i2c_lcd1602_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void LCD1602_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
