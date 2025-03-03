#include "mono_framebuf.h"
#include "mono_font.h"
#include <string.h>  // memset

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mono-fb"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/* Absolute value */
#define ABS(x) ((x) > 0 ? (x) : -(x))

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MonoFramebuf_Fill(mono_framebuf_t* pHandle, mono_color_e eColor)
{
    /* Set memory */

    uint16_t u16Size = pHandle->u16Height * pHandle->u16Height / 8;

    switch (eColor)
    {
        case MONO_COLOR_WHITE:
        {
            memset(pHandle->pu8Buffer, 0xFF, u16Size);
            break;
        }

        case MONO_COLOR_BLACK:
        {
            memset(pHandle->pu8Buffer, 0x00, u16Size);
            break;
        }

        case MONO_COLOR_XOR:
        {
            for (uint8_t i = 0; i < u16Size; i++)
            {
                pHandle->pu8Buffer[i] ^= 0xFF;
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

void MonoFramebuf_DrawPixel(mono_framebuf_t* pHandle, uint16_t x, uint16_t y, mono_color_e eColor)
{
    if (x >= pHandle->u16Width || y >= pHandle->u16Height)
    {
        /* Error */
        return;
    }

    uint16_t u16Byte   = x + (y / 8) * pHandle->u16Width;
    uint16_t u16BitMsk = 1 << (y % 8);

    /* Set eColor */
    switch (eColor)
    {
        case MONO_COLOR_WHITE:
            pHandle->pu8Buffer[u16Byte] |= u16BitMsk;
            break;
        case MONO_COLOR_BLACK:
            pHandle->pu8Buffer[u16Byte] &= ~u16BitMsk;
            break;
        case MONO_COLOR_XOR:
            pHandle->pu8Buffer[u16Byte] ^= u16BitMsk;
            break;
        default:
            break;
    }
}

void MonoFramebuf_SetCursor(mono_framebuf_t* pHandle, uint16_t x, uint16_t y)
{
    /* Set write pointers */
    pHandle->_u16CursorX = x;
    pHandle->_u16CursorY = y;
}

char MonoFramebuf_PutChar(mono_framebuf_t* pHandle, char ch, const mono_font_t* pFont, mono_color_e eForeColor, mono_color_e eBackColor)
{
    uint32_t i, j;

    mono_font_code_t sFontCode = {0};

    GetFontInfo(pFont, ch, &sFontCode);

    uint16_t u16Size    = sFontCode.u16Height * sFontCode.u16Width / 8;
    uint16_t u16StartX  = pHandle->_u16CursorX;
    uint16_t u16CursorX = pHandle->_u16CursorX;
    uint16_t u16CursorY = pHandle->_u16CursorY;

    for (i = 0; i < u16Size; i++)
    {
        uint8_t u8Byte = sFontCode.pu8Buffer[i];

        for (j = 0; j < 8; j++)
        {
            MonoFramebuf_DrawPixel(pHandle, u16CursorX, u16CursorY, (u8Byte & 0x80) ? eForeColor : eBackColor);
            u8Byte <<= 1;
            u16CursorX++;
        }

        if ((u16CursorX - u16StartX) == (sFontCode.u16Width))
        {
            u16CursorX = u16StartX;
            u16CursorY++;
        }
    }

    /* Increase pointer */
    pHandle->_u16CursorX += sFontCode.u16Width;

    /* Return character written */
    return ch;
}

char MonoFramebuf_PutString(mono_framebuf_t* pHandle, char* str, const mono_font_t* pFont, mono_color_e eForeColor, mono_color_e eBackColor)
{
    /* Write characters */
    while (*str)
    {
        /* Write character by character */
        if (MonoFramebuf_PutChar(pHandle, *str, pFont, eForeColor, eBackColor) != *str)
        {
            /* Return error */
            return *str;
        }

        /* Increase string pointer */
        str++;
    }

    /* Everything OK, zero should be returned */
    return *str;
}

void MonoFramebuf_DrawLine(mono_framebuf_t* pHandle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, mono_color_e eColor)
{
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    /* Check for overflow */
    if (x0 >= pHandle->u16Width)
    {
        x0 = pHandle->u16Width - 1;
    }
    if (x1 >= pHandle->u16Width)
    {
        x1 = pHandle->u16Width - 1;
    }
    if (y0 >= pHandle->u16Height)
    {
        y0 = pHandle->u16Height - 1;
    }
    if (y1 >= pHandle->u16Height)
    {
        y1 = pHandle->u16Height - 1;
    }

    dx  = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy  = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx  = (x0 < x1) ? 1 : -1;
    sy  = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0)
    {
        if (y1 < y0)
        {
            tmp = y1;
            y1  = y0;
            y0  = tmp;
        }

        if (x1 < x0)
        {
            tmp = x1;
            x1  = x0;
            x0  = tmp;
        }

        /* Vertical line */
        for (i = y0; i <= y1; i++)
        {
            MonoFramebuf_DrawPixel(pHandle, x0, i, eColor);
        }

        /* Return from function */
        return;
    }

    if (dy == 0)
    {
        if (y1 < y0)
        {
            tmp = y1;
            y1  = y0;
            y0  = tmp;
        }

        if (x1 < x0)
        {
            tmp = x1;
            x1  = x0;
            x0  = tmp;
        }

        /* Horizontal line */
        for (i = x0; i <= x1; i++)
        {
            MonoFramebuf_DrawPixel(pHandle, i, y0, eColor);
        }

        /* Return from function */
        return;
    }

    while (1)
    {
        MonoFramebuf_DrawPixel(pHandle, x0, y0, eColor);
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void MonoFramebuf_DrawRectangle(mono_framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, mono_color_e eColor)
{
    /* Check input parameters */
    if (x >= pHandle->u16Width || y >= pHandle->u16Height)
    {
        /* Return error */
        return;
    }

    /* Check u16Width and height */
    if ((x + w) >= pHandle->u16Width)
    {
        w = pHandle->u16Width - x;
    }
    if ((y + h) >= pHandle->u16Height)
    {
        h = pHandle->u16Height - y;
    }

    /* Draw 4 lines */
    MonoFramebuf_DrawLine(pHandle, x, y, x + w, y, eColor);         /* Top line */
    MonoFramebuf_DrawLine(pHandle, x, y + h, x + w, y + h, eColor); /* Bottom line */
    MonoFramebuf_DrawLine(pHandle, x, y, x, y + h, eColor);         /* Left line */
    MonoFramebuf_DrawLine(pHandle, x + w, y, x + w, y + h, eColor); /* Right line */
}

void MonoFramebuf_FillRectangle(mono_framebuf_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, mono_color_e eColor)
{
    uint8_t i;

    /* Check input parameters */
    if (x >= pHandle->u16Width || y >= pHandle->u16Height)
    {
        /* Return error */
        return;
    }

    /* Check u16Width and height */
    if ((x + w) >= pHandle->u16Width)
    {
        w = pHandle->u16Width - x;
    }
    if ((y + h) >= pHandle->u16Height)
    {
        h = pHandle->u16Height - y;
    }

    /* Draw lines */
    for (i = 0; i <= h; i++)
    {
        /* Draw lines */
        MonoFramebuf_DrawLine(pHandle, x, y + i, x + w, y + i, eColor);
    }
}

void MonoFramebuf_DrawTriangle(mono_framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, mono_color_e eColor)
{
    /* Draw lines */
    MonoFramebuf_DrawLine(pHandle, x1, y1, x2, y2, eColor);
    MonoFramebuf_DrawLine(pHandle, x2, y2, x3, y3, eColor);
    MonoFramebuf_DrawLine(pHandle, x3, y3, x1, y1, eColor);
}

void MonoFramebuf_FillTriangle(mono_framebuf_t* pHandle, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, mono_color_e eColor)
{
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x      = x1;
    y      = y1;

    if (x2 >= x1)
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)
    {
        xinc1     = 0;
        yinc2     = 0;
        den       = deltax;
        num       = deltax / 2;
        numadd    = deltay;
        numpixels = deltax;
    }
    else
    {
        xinc2     = 0;
        yinc1     = 0;
        den       = deltay;
        num       = deltay / 2;
        numadd    = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        MonoFramebuf_DrawLine(pHandle, x, y, x3, y3, eColor);

        num += numadd;
        if (num >= den)
        {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

void MonoFramebuf_DrawCircle(mono_framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, mono_color_e eColor)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    MonoFramebuf_DrawPixel(pHandle, x0, y0 + r, eColor);
    MonoFramebuf_DrawPixel(pHandle, x0, y0 - r, eColor);
    MonoFramebuf_DrawPixel(pHandle, x0 + r, y0, eColor);
    MonoFramebuf_DrawPixel(pHandle, x0 - r, y0, eColor);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        MonoFramebuf_DrawPixel(pHandle, x0 + x, y0 + y, eColor);
        MonoFramebuf_DrawPixel(pHandle, x0 - x, y0 + y, eColor);
        MonoFramebuf_DrawPixel(pHandle, x0 + x, y0 - y, eColor);
        MonoFramebuf_DrawPixel(pHandle, x0 - x, y0 - y, eColor);

        MonoFramebuf_DrawPixel(pHandle, x0 + y, y0 + x, eColor);
        MonoFramebuf_DrawPixel(pHandle, x0 - y, y0 + x, eColor);
        MonoFramebuf_DrawPixel(pHandle, x0 + y, y0 - x, eColor);
        MonoFramebuf_DrawPixel(pHandle, x0 - y, y0 - x, eColor);
    }
}

void MonoFramebuf_FillCircle(mono_framebuf_t* pHandle, int16_t x0, int16_t y0, int16_t r, mono_color_e eColor)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    MonoFramebuf_DrawPixel(pHandle, x0, y0 + r, eColor);
    MonoFramebuf_DrawPixel(pHandle, x0, y0 - r, eColor);
    MonoFramebuf_DrawPixel(pHandle, x0 + r, y0, eColor);
    MonoFramebuf_DrawPixel(pHandle, x0 - r, y0, eColor);
    MonoFramebuf_DrawLine(pHandle, x0 - r, y0, x0 + r, y0, eColor);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        MonoFramebuf_DrawLine(pHandle, x0 - x, y0 + y, x0 + x, y0 + y, eColor);
        MonoFramebuf_DrawLine(pHandle, x0 + x, y0 - y, x0 - x, y0 - y, eColor);

        MonoFramebuf_DrawLine(pHandle, x0 + y, y0 + x, x0 - y, y0 + x, eColor);
        MonoFramebuf_DrawLine(pHandle, x0 + y, y0 - x, x0 - y, y0 - x, eColor);
    }
}

uint8_t* MonoFramebuf_GetBuffer(mono_framebuf_t* pHandle)
{
    return pHandle->pu8Buffer;
}
