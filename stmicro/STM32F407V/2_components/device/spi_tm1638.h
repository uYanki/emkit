#ifndef __SPI_TM1638_H__
#define __SPI_TM1638_H__

#include "spimst.h"

/**
 * @brief 8*LED+8*KEY+8*SEG vserion
 * @note  支持同时按下多键(具体看原理图的按键是否接了二极管)
 */

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define TM1638_SPI_TIMING (SPI_FLAG_3WIRE | SPI_FLAG_CPOL_HIGH | SPI_FLAG_CPHA_2EDGE | SPI_FLAG_LSBFIRST | SPI_FLAG_DATAWIDTH_8B | SPI_FLAG_CS_ACTIVE_LOW)

typedef struct {
    __IN spi_mst_t* hSPI;
} spi_tm1638_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void TM1638_Init(spi_tm1638_t* pHandle);

/**
 * @brief Set brightness level（8级可调亮度）
 * @param[in] Brightness
 *   - 0: Display OFF
 *   - 1: Display ON, pulse width is set as 1/16, (spi_tm1638_t* pHandle,lowest brightness.)
 *   - 2: Display ON, pulse width is set as 2/16
 *   - 3: Display ON, pulse width is set as 4/16
 *   - 4: Display ON, pulse width is set as 10/16
 *   - 5: Display ON, pulse width is set as 11/16
 *   - 6: Display ON, pulse width is set as 12/16
 *   - 7: Display ON, pulse width is set as 13/16
 *   - 8: Display ON, pulse width is set as 14/16
 */
void TM1638_SetBrightness(spi_tm1638_t* pHandle, uint8_t u8Brightness);

/**
 * @brief Reset all leds and segs
 */
void TM1638_Clear(spi_tm1638_t* pHandle);

/**
 * @brief Led
 */
void TM1638_SetSingleLed(spi_tm1638_t* pHandle, uint8_t u8Channel, pin_level_e eLevel);
void TM1638_SetMultipleLed(spi_tm1638_t* pHandle, uint8_t u8Data);

/**
 * @brief Digital tube
 */
void TM1638_SetSingleDigital(spi_tm1638_t* pHandle, uint8_t eChannel, uint8_t u8Data);
void TM1638_DisplayChar(spi_tm1638_t* pHandle, uint8_t u8Channel, uint8_t u8AsciiCode, bool bShowDot);
void TM1638_DisplayString(spi_tm1638_t* pHandle, uint8_t u8Channel, const char* szString);
void TM1638_ShowLoadingAnimate(spi_tm1638_t* pHandle, uint16_t u16UpdatePeriodMs);

/**
 * @brief Key
 */
uint8_t TM1638_ScanKeys(spi_tm1638_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void TM1638_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
