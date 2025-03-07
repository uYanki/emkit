#ifndef __I2C_SSD1306_H__
#define __I2C_SSD1306_H__

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define SSD1306_ADDRESS_LOW  0x3C
#define SSD1306_ADDRESS_HIGH 0x3D

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;

    /**
     * @note
     *                       | u8Cols | u8Rows
     *     0.91inch          |   ?    |   ?
     *     0.96inch (128x64) |  128   |   8
     *     1.14inch          |   ?    |   ?
     */

    __IN const uint8_t u8Cols;  // x
    __IN const uint8_t u8Rows;  // y

} i2c_ssd1306_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief 初始化
 */
status_t SSD1306_Init(i2c_ssd1306_t* pHandle);

/**
 * @brief 清屏
 */
void SSD1306_ClearScreen(i2c_ssd1306_t* pHandle);

/**
 * @brief 唤醒
 */
void SSD1306_Wakeup(i2c_ssd1306_t* pHandle);

/**
 * @brief 休眠(功耗<10uA)
 */
void SSD1306_Sleep(i2c_ssd1306_t* pHandle);

/**
 * @brief 设置起始点坐标
 * @param u8Col [0,u8Cols)
 * @param u8Row [0,u8Rows)
 */
void SSD1306_SetCursor(i2c_ssd1306_t* pHandle, uint8_t u8Col, uint8_t u8Row);

/**
 * @brief 全屏填充
 */
void SSD1306_FillScreen(i2c_ssd1306_t* pHandle, uint8_t u8Data);

/**
 * @brief 局部刷新
 * @note BufferSize = @ref u8RowSpan * @ref u8ColSpan
 * @param u8RowStart [0, @ref u8Cols)
 * @param u8ColStart [0, @ref u8Rows)
 * @param u8ColSpan  [0, @ref u8Cols - @ref u8RowStart)
 * @param u8RowSpan  [0, @ref u8Rows - @ref u8ColStart)
 */
void SSD1306_DisplayBuffer(i2c_ssd1306_t* pHandle, uint8_t u8RowStart, uint8_t u8ColStart, uint8_t u8RowSpan, uint8_t u8ColSpan, const uint8_t* cpu8Buffer);

/**
 * @brief 全局刷新
 * @note BufferSize = @ref u8Rows * @ref u8Cols
 */
void SSD1306_FillBuffer(i2c_ssd1306_t* pHandle, const uint8_t* cpu8Buffer);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void SSD1306_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
