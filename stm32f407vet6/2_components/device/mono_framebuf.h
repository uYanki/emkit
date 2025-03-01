#ifndef __MONO_FRAMEBUF_H__
#define __MONO_FRAMEBUF_H__

/**
 * @brief Monochrome Screen 单色屏
 */

#include "typebasic.h"
#include "mono_font.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef enum {
    MONO_COLOR_BLACK,
    MONO_COLOR_WHITE,
    MONO_COLOR_XOR,
} mono_color_e;

typedef struct {
    __IN uint8_t* pu8Buffer;
    __IN uint16_t u16Width;   // screen width, unit: pixel
    __IN uint16_t u16Height;  // screen height, unit: pixel
    uint16_t      _u16CursorX;
    uint16_t      _u16CursorY;
} mono_framebuf_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MonoFramebuf_Fill(mono_framebuf_t* pHandle, mono_color_e eColor);

void MonoFramebuf_DrawPixel(mono_framebuf_t* pHandle, uint16_t x, uint16_t y, mono_color_e eColor);
void MonoFramebuf_DrawLine(mono_framebuf_t* pHandle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, mono_color_e eColor);

void MonoFramebuf_DrawRectangle(mono_framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, mono_color_e eColor);
void MonoFramebuf_FillRectangle(mono_framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, mono_color_e eColor);

void MonoFramebuf_DrawTriangle(mono_framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, mono_color_e eColor);
void MonoFramebuf_FillTriangle(mono_framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, mono_color_e eColor);

void MonoFramebuf_DrawCircle(mono_framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, mono_color_e eColor);
void MonoFramebuf_FillCircle(mono_framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, mono_color_e eColor);

void MonoFramebuf_SetCursor(mono_framebuf_t* pHandle, uint16_t x, uint16_t y);
char MonoFramebuf_PutChar(mono_framebuf_t* pHandle, char ch, const mono_font_t* pFont, mono_color_e eForeColor, mono_color_e eBackColor);
char MonoFramebuf_PutString(mono_framebuf_t* pHandle, char* str, const mono_font_t* pFont, mono_color_e eForeColor, mono_color_e eBackColor);

uint8_t* MonoFramebuf_GetBuffer(mono_framebuf_t* pHandle);

#ifdef __cplusplus
}
#endif

#endif
