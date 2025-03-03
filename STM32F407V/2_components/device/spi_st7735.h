#ifndef __SPI_ST7735_H__
#define __SPI_ST7735_H__

#include "spimst.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_ST7735_SIZE           ST7735_160X128

#define CONFIG_ST7735_BL_CONTROL_SW  0
#define CONFIG_ST7735_RST_CONTROL_SW 1

#define CONFIG_ST7735_ROTATION       (ST7735_MADCTL_MX | ST7735_MADCTL_MV)  // rotate left
// #define CONFIG_ST7735_ROTATION (ST7735_MADCTL_MY | ST7735_MADCTL_MV)  // rotate right

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief CONFIG_ST7735_SIZE
 * @note width x height
 */
#define ST7735_128X128 0  // 1.44
#define ST7735_160X80  1  // 0.96
#define ST7735_80X160  2
#define ST7735_160X128 3  // 1.8
#define ST7735_128X160 4

#if CONFIG_ST7735_SIZE == ST7735_128X128
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 128
#elif CONFIG_ST7735_SIZE == ST7735_160X80
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 80
#elif CONFIG_ST7735_SIZE == ST7735_80X160
#define ST7735_WIDTH  80
#define ST7735_HEIGHT 160
#elif CONFIG_ST7735_SIZE == ST7735_160X128
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#elif CONFIG_ST7735_SIZE == ST7735_128X160
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#endif

// rotation
#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_SPI_TIMING (SPI_FLAG_3WIRE | SPI_FLAG_CPHA_1EDGE | SPI_FLAG_CPOL_LOW | SPI_FLAG_MSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW | SPI_FLAG_FAST_CLOCK_ENABLE)

/**
 * @brief rgb565 color
 */

#define COLOR_RGB565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

enum {
    COLOR_RGB565_BLACK   = 0x0000,
    COLOR_RGB565_BLUE    = 0x001F,
    COLOR_RGB565_RED     = 0xF800,
    COLOR_RGB565_GREEN   = 0x07E0,
    COLOR_RGB565_CYAN    = 0x07FF,
    COLOR_RGB565_MAGENTA = 0xF81F,
    COLOR_RGB565_YELLOW  = 0xFFE0,
    COLOR_RGB565_WHITE   = 0xFFFF,
};

typedef struct {
    __IN spi_mst_t*  hSPI;
    __IN const pin_t DC;
#if CONFIG_ST7735_RST_CONTROL_SW
    __IN const pin_t RST;
#endif
#if CONFIG_ST7735_BL_CONTROL_SW
    __IN const pin_t BL;
#endif

    __IN const uint16_t u16ScreenW;
    __IN const uint16_t u16ScreenH;
    __IN const uint16_t u16OffsetX;
    __IN const uint16_t u16OffsetY;

} spi_st7735_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void ST7735_Init(spi_st7735_t* pHandle);

void ST7735_InvertColor(spi_st7735_t* pHandle, bool bEnable);
void ST7735_BackLight(spi_st7735_t* pHandle, bool bEnable);

bool ST7735_DrawPixel(spi_st7735_t* pHandle, uint16_t x, uint16_t y, uint16_t color);
bool ST7735_FillRect(spi_st7735_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
bool ST7735_FillScreen(spi_st7735_t* pHandle, uint16_t u16Color);

/**
 * @brief fast draw mode
 *
 * @code
 *
 * if (ST7735_StartDraw(pHandle, x, y, w, h) == true)
 * {
 *     for (j = 0; j < h; j++)
 *     {
 *         for (i = 0; i < w; i++)
 *         {
 *             ST7735_PutColor(pHandle, au16Color[j][i]);
 *         }
 *     }
 *
 *     ST7735_StopDraw(pHandle);
 * }
 *
 * @endcode
 */
bool ST7735_StartDraw(spi_st7735_t* pHandle, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void ST7735_PutColor(spi_st7735_t* pHandle, uint16_t u16Color);
void ST7735_StopDraw(spi_st7735_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void ST7735_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
