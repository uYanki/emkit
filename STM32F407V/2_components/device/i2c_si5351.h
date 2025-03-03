#ifndef __I2C_SI5351_H__
#define __I2C_SI5351_H__

#include "i2cmst.h"

/**
 * @brief SiLabs Si5351 chip variant
 * - Si5351A (8 output clocks, XTAL input)
 * - Si5351A MSOP10 (3 output clocks, XTAL input)
 * - Si5351B (8 output clocks, XTAL/VXCO input)
 * - Si5351C (8 output clocks, XTAL/CLKIN input)
 *
 * @ref https://github.com/adafruit/Adafruit_Si5351_Library
 */

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define SI5351_ADDRESS 0x60  // Assumes ADDR pin = low

typedef enum {
    SI5351_PLL_A = 0,
    SI5351_PLL_B,
} si5351_pll_e;

typedef enum {
    SI5351_CHANNEL_0 = 0,
    SI5351_CHANNEL_1,
    SI5351_CHANNEL_2,
    // SI5351_CHANNEL_3,
    // SI5351_CHANNEL_4,
    // SI5351_CHANNEL_5,
    // SI5351_CHANNEL_6,
    // SI5351_CHANNEL_7,
} si5351_channel_e;

typedef enum {
    SI5351_CRYSTAL_LOAD_6PF  = (1 << 6),
    SI5351_CRYSTAL_LOAD_8PF  = (2 << 6),
    SI5351_CRYSTAL_LOAD_10PF = (3 << 6)
} si5351_crystal_load_e;

typedef enum {
    SI5351_CRYSTAL_FREQ_25MHZ = (25000000),
    SI5351_CRYSTAL_FREQ_27MHZ = (27000000)
} si5351_crystal_freq_e;

typedef enum {
    SI5351_MULTISYNTH_DIV_4 = 4,
    SI5351_MULTISYNTH_DIV_6 = 6,
    SI5351_MULTISYNTH_DIV_8 = 8
} si5351_multisynth_div_e;

typedef enum {
    SI5351_R_DIV_1   = 0,
    SI5351_R_DIV_2   = 1,
    SI5351_R_DIV_4   = 2,
    SI5351_R_DIV_8   = 3,
    SI5351_R_DIV_16  = 4,
    SI5351_R_DIV_32  = 5,
    SI5351_R_DIV_64  = 6,
    SI5351_R_DIV_128 = 7,
} si5351_r_div_e;

typedef struct {
    __IN i2c_mst_t*            hI2C;
    __IN uint8_t               u8SlvAddr;
    __IN si5351_crystal_freq_e eCrystalFreq;   //!< Crystal frequency
    __IN si5351_crystal_load_e eCrystalLoad;   //!< Crystal load capacitors
    __IN uint32_t              u32CrystalPPM;  //!< Frequency synthesis

    uint8_t  _au8LastRdivVal[3];
    bool     _bPllaConfigured;  //!< Phase-locked loop A configured
    uint32_t _u32PllaFreq;      //!< Phase-locked loop A frequency
    bool     _bPllbConfigured;  //!< Phase-locked loop B configured
    uint32_t _u32PllbFreq;      //!< Phase-locked loop B frequency
} i2c_si5351_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t SI5351_Init(i2c_si5351_t* pHandle);
status_t SI5351_SetClockBuilderData(i2c_si5351_t* pHandle);
status_t SI5351_SetupPLLInt(i2c_si5351_t* pHandle, si5351_pll_e ePll, uint8_t u8Mult);
status_t SI5351_SetupPLL(i2c_si5351_t* pHandle, si5351_pll_e ePll, uint8_t u8Multiplier, uint32_t u32Numerator, uint32_t u32Denominator);
status_t SI5351_SetupMultisynthInt(i2c_si5351_t* pHandle, si5351_channel_e eChannel, si5351_pll_e ePllSrc, si5351_multisynth_div_e eDivider);
status_t SI5351_SetupRdiv(i2c_si5351_t* pHandle, si5351_channel_e eChannel, si5351_r_div_e eDivider);
status_t SI5351_SetupMultisynth(i2c_si5351_t* pHandle, si5351_channel_e eChannel, si5351_pll_e ePllSrc, uint32_t u32Divider, uint32_t u32Numerator, uint32_t u32Denominator);
status_t SI5351_EnableOutputs(i2c_si5351_t* pHandle, bool bEnabled);
status_t SI5351_EnableSpreadSpectrum(i2c_si5351_t* pHandle, bool bEnabled);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void SI5351_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif