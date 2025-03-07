#include "i2c_si5351.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "si5351"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

/* See http://www.silabs.com/Support%20Documents/TechnicalDocs/AN619.pdf for registers 26..41 */
#define REG_0_DEVICE_STATUS                       0
#define REG_1_INTERRUPT_STATUS_STICKY             1
#define REG_2_INTERRUPT_STATUS_MASK               2
#define REG_3_OUTPUT_ENABLE_CONTROL               3
#define REG_9_OEB_PIN_ENABLE_CONTROL              9
#define REG_15_PLL_INPUT_SOURCE                   15
#define REG_16_CLK0_CONTROL                       16
#define REG_17_CLK1_CONTROL                       17
#define REG_18_CLK2_CONTROL                       18
#define REG_19_CLK3_CONTROL                       19
#define REG_20_CLK4_CONTROL                       20
#define REG_21_CLK5_CONTROL                       21
#define REG_22_CLK6_CONTROL                       22
#define REG_23_CLK7_CONTROL                       23
#define REG_24_CLK3_0_DISABLE_STATE               24
#define REG_25_CLK7_4_DISABLE_STATE               25
#define REG_42_MULTISYNTH0_PARAMETERS_1           42
#define REG_43_MULTISYNTH0_PARAMETERS_2           43
#define REG_44_MULTISYNTH0_PARAMETERS_3           44
#define REG_45_MULTISYNTH0_PARAMETERS_4           45
#define REG_46_MULTISYNTH0_PARAMETERS_5           46
#define REG_47_MULTISYNTH0_PARAMETERS_6           47
#define REG_48_MULTISYNTH0_PARAMETERS_7           48
#define REG_49_MULTISYNTH0_PARAMETERS_8           49
#define REG_50_MULTISYNTH1_PARAMETERS_1           50
#define REG_51_MULTISYNTH1_PARAMETERS_2           51
#define REG_52_MULTISYNTH1_PARAMETERS_3           52
#define REG_53_MULTISYNTH1_PARAMETERS_4           53
#define REG_54_MULTISYNTH1_PARAMETERS_5           54
#define REG_55_MULTISYNTH1_PARAMETERS_6           55
#define REG_56_MULTISYNTH1_PARAMETERS_7           56
#define REG_57_MULTISYNTH1_PARAMETERS_8           57
#define REG_58_MULTISYNTH2_PARAMETERS_1           58
#define REG_59_MULTISYNTH2_PARAMETERS_2           59
#define REG_60_MULTISYNTH2_PARAMETERS_3           60
#define REG_61_MULTISYNTH2_PARAMETERS_4           61
#define REG_62_MULTISYNTH2_PARAMETERS_5           62
#define REG_63_MULTISYNTH2_PARAMETERS_6           63
#define REG_64_MULTISYNTH2_PARAMETERS_7           64
#define REG_65_MULTISYNTH2_PARAMETERS_8           65
#define REG_66_MULTISYNTH3_PARAMETERS_1           66
#define REG_67_MULTISYNTH3_PARAMETERS_2           67
#define REG_68_MULTISYNTH3_PARAMETERS_3           68
#define REG_69_MULTISYNTH3_PARAMETERS_4           69
#define REG_70_MULTISYNTH3_PARAMETERS_5           70
#define REG_71_MULTISYNTH3_PARAMETERS_6           71
#define REG_72_MULTISYNTH3_PARAMETERS_7           72
#define REG_73_MULTISYNTH3_PARAMETERS_8           73
#define REG_74_MULTISYNTH4_PARAMETERS_1           74
#define REG_75_MULTISYNTH4_PARAMETERS_2           75
#define REG_76_MULTISYNTH4_PARAMETERS_3           76
#define REG_77_MULTISYNTH4_PARAMETERS_4           77
#define REG_78_MULTISYNTH4_PARAMETERS_5           78
#define REG_79_MULTISYNTH4_PARAMETERS_6           79
#define REG_80_MULTISYNTH4_PARAMETERS_7           80
#define REG_81_MULTISYNTH4_PARAMETERS_8           81
#define REG_82_MULTISYNTH5_PARAMETERS_1           82
#define REG_83_MULTISYNTH5_PARAMETERS_2           83
#define REG_84_MULTISYNTH5_PARAMETERS_3           84
#define REG_85_MULTISYNTH5_PARAMETERS_4           85
#define REG_86_MULTISYNTH5_PARAMETERS_5           86
#define REG_87_MULTISYNTH5_PARAMETERS_6           87
#define REG_88_MULTISYNTH5_PARAMETERS_7           88
#define REG_89_MULTISYNTH5_PARAMETERS_8           89
#define REG_90_MULTISYNTH6_PARAMETERS             90
#define REG_91_MULTISYNTH7_PARAMETERS             91
#define REG_092_CLOCK_6_7_OUTPUT_DIVIDER          92
#define REG_149_SPREAD_SPECTRUM_PARAMETERS        149
#define REG_165_CLK0_INITIAL_PHASE_OFFSET         165
#define REG_166_CLK1_INITIAL_PHASE_OFFSET         166
#define REG_167_CLK2_INITIAL_PHASE_OFFSET         167
#define REG_168_CLK3_INITIAL_PHASE_OFFSET         168
#define REG_169_CLK4_INITIAL_PHASE_OFFSET         169
#define REG_170_CLK5_INITIAL_PHASE_OFFSET         170
#define REG_177_PLL_RESET                         177
#define REG_183_CRYSTAL_INTERNAL_LOAD_CAPACITANCE 183

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static inline status_t SI5351_WriteByte(i2c_si5351_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data);
static inline status_t SI5351_WriteBlock(i2c_si5351_t* pHandle, uint8_t u8MemAddr, const uint8_t* cpu8Data, uint8_t u8Size);
static inline status_t SI5351_ReadByte(i2c_si5351_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

/* Test setup from SI5351 ClockBuilder
 * -----------------------------------
 * XTAL:      25     MHz
 * Channel 0: 120.00 MHz
 * Channel 1: 12.00  MHz
 * Channel 2: 13.56  MHz
 */
static const uint8_t m_si5351_regs_15to92_149to170[100][2] = {
    {15,  0x00}, /* Input source = crystal for PLLA and PLLB */
    {16,  0x4F}, /* CLK0 Control: 8mA drive, Multisynth 0 as CLK0 source, Clock not inverted, Source = PLLA, Multisynth 0 in integer mode, clock powered up */
    {17,  0x4F}, /* CLK1 Control: 8mA drive, Multisynth 1 as CLK1 source, Clock not inverted, Source = PLLA, Multisynth 1 in integer mode, clock powered up */
    {18,  0x6F}, /* CLK2 Control: 8mA drive, Multisynth 2 as CLK2 source, Clock not inverted, Source = PLLB, Multisynth 2 in integer mode, clock powered up */
    {19,  0x80}, /* CLK3 Control: Not used ... clock powered down */
    {20,  0x80}, /* CLK4 Control: Not used ... clock powered down */
    {21,  0x80}, /* CLK5 Control: Not used ... clock powered down */
    {22,  0x80}, /* CLK6 Control: Not used ... clock powered down */
    {23,  0x80}, /* CLK7 Control: Not used ... clock powered down */
    {24,  0x00}, /* Clock disable state 0..3 (low when disabled) */
    {25,  0x00}, /* Clock disable state 4..7 (low when disabled) */
    /* PLL_A Setup */
    {26,  0x00},
    {27,  0x05},
    {28,  0x00},
    {29,  0x0C},
    {30,  0x66},
    {31,  0x00},
    {32,  0x00},
    {33,  0x02},
    /* PLL_B Setup */
    {34,  0x02},
    {35,  0x71},
    {36,  0x00},
    {37,  0x0C},
    {38,  0x1A},
    {39,  0x00},
    {40,  0x00},
    {41,  0x86},
    /* Multisynth Setup */
    {42,  0x00},
    {43,  0x01},
    {44,  0x00},
    {45,  0x01},
    {46,  0x00},
    {47,  0x00},
    {48,  0x00},
    {49,  0x00},
    {50,  0x00},
    {51,  0x01},
    {52,  0x00},
    {53,  0x1C},
    {54,  0x00},
    {55,  0x00},
    {56,  0x00},
    {57,  0x00},
    {58,  0x00},
    {59,  0x01},
    {60,  0x00},
    {61,  0x18},
    {62,  0x00},
    {63,  0x00},
    {64,  0x00},
    {65,  0x00},
    {66,  0x00},
    {67,  0x00},
    {68,  0x00},
    {69,  0x00},
    {70,  0x00},
    {71,  0x00},
    {72,  0x00},
    {73,  0x00},
    {74,  0x00},
    {75,  0x00},
    {76,  0x00},
    {77,  0x00},
    {78,  0x00},
    {79,  0x00},
    {80,  0x00},
    {81,  0x00},
    {82,  0x00},
    {83,  0x00},
    {84,  0x00},
    {85,  0x00},
    {86,  0x00},
    {87,  0x00},
    {88,  0x00},
    {89,  0x00},
    {90,  0x00},
    {91,  0x00},
    {92,  0x00},
    /* Misc Config Register */
    {149, 0x00},
    {150, 0x00},
    {151, 0x00},
    {152, 0x00},
    {153, 0x00},
    {154, 0x00},
    {155, 0x00},
    {156, 0x00},
    {157, 0x00},
    {158, 0x00},
    {159, 0x00},
    {160, 0x00},
    {161, 0x00},
    {162, 0x00},
    {163, 0x00},
    {164, 0x00},
    {165, 0x00},
    {166, 0x00},
    {167, 0x00},
    {168, 0x00},
    {169, 0x00},
    {170, 0x00}
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t SI5351_WriteByte(i2c_si5351_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data)
{
    return I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t SI5351_WriteBlock(i2c_si5351_t* pHandle, uint8_t u8MemAddr, const uint8_t* cpu8Data, uint8_t u8Size)
{
    return I2C_Master_WriteBlock(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, cpu8Data, u8Size, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t SI5351_ReadByte(i2c_si5351_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data)
{
    return I2C_Master_ReadByte(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu8Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

status_t SI5351_Init(i2c_si5351_t* pHandle)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    for (uint8_t i = 0; i < 3; i++)
    {
        pHandle->_au8LastRdivVal[i] = 0;
    }

    /* Disable all outputs setting CLKx_DIS high */
    SI5351_WriteByte(pHandle, REG_3_OUTPUT_ENABLE_CONTROL, 0xFF);

    /* Power down all output drivers */
    SI5351_WriteByte(pHandle, REG_16_CLK0_CONTROL, BV(7));
    SI5351_WriteByte(pHandle, REG_17_CLK1_CONTROL, BV(7));
    SI5351_WriteByte(pHandle, REG_18_CLK2_CONTROL, BV(7));
    SI5351_WriteByte(pHandle, REG_19_CLK3_CONTROL, BV(7));
    SI5351_WriteByte(pHandle, REG_20_CLK4_CONTROL, BV(7));
    SI5351_WriteByte(pHandle, REG_21_CLK5_CONTROL, BV(7));
    SI5351_WriteByte(pHandle, REG_22_CLK6_CONTROL, BV(7));
    SI5351_WriteByte(pHandle, REG_23_CLK7_CONTROL, BV(7));

    /* Set the load capacitance for the XTAL */
    SI5351_WriteByte(pHandle, REG_183_CRYSTAL_INTERNAL_LOAD_CAPACITANCE, pHandle->eCrystalLoad);

    /* Disable spread spectrum output. */
    SI5351_EnableSpreadSpectrum(pHandle, false);

    /* Set interrupt masks as required (see Register 2 description in AN619).
       By default, ClockBuilder Desktop sets this register to 0x18.
       Note that the least significant nibble must remain 0x8, but the most
       significant nibble may be modified to suit your needs. */

    /* Reset the PLL config fields just in case we call init again */
    pHandle->_bPllaConfigured = false;
    pHandle->_u32PllaFreq     = 0;
    pHandle->_bPllbConfigured = false;
    pHandle->_u32PllbFreq     = 0;

    return ERR_NONE;
}

/**
 *  @brief  Configures the Si5351 with config settings generated in
 *          ClockBuilder. You can use this function to make sure that
 *          your HW is properly configure and that there are no problems
 *          with the board itself.
 *
 *          Running this function should provide the following output:
 *          * Channel 0: 120.00 MHz
 *          * Channel 1: 12.00  MHz
 *          * Channel 2: 13.56  MHz
 *
 *  @note   This will overwrite all of the config registers!
 */
status_t SI5351_SetClockBuilderData(i2c_si5351_t* pHandle)
{
    uint16_t i = 0;

    /* Disable all outputs setting CLKx_DIS high */
    SI5351_WriteByte(pHandle, REG_3_OUTPUT_ENABLE_CONTROL, 0xFF);

    /* Writes configuration data to device using the register map contents
           generated by ClockBuilder Desktop (registers 15-92 + 149-170) */
    for (i = 0; i < sizeof(m_si5351_regs_15to92_149to170) / 2; i++)
    {
        SI5351_WriteByte(pHandle, m_si5351_regs_15to92_149to170[i][0], m_si5351_regs_15to92_149to170[i][1]);
    }

    /* Apply soft reset */
    SI5351_WriteByte(pHandle, REG_177_PLL_RESET, 0xAC);

    /* Enabled desired outputs (see Register 3) */
    SI5351_WriteByte(pHandle, REG_3_OUTPUT_ENABLE_CONTROL, 0x00);

    return ERR_NONE;
}

/**
 * @brief  Sets the multiplier for the specified PLL using integer values
 *
 * @param[in]  ePll   The PLL to configure
 * @param[in]  u8Mult  The PLL integer multiplier (must be between 15 and 90)
 */
status_t SI5351_SetupPLLInt(i2c_si5351_t* pHandle, si5351_pll_e ePll, uint8_t u8Mult)
{
    return SI5351_SetupPLL(pHandle, ePll, u8Mult, 0, 1);
}

/**
 *  @brief  Sets the multiplier for the specified PLL
 *
 *  @param[in] ePll            The PLL to configure
 *  @param[in]  u8Multiplier   The PLL integer multiplier (must be between 15 and 90)
 *  @param[in]  u32Numerator   The 20-bit numerator for fractional output (0..1,048,575).
 *                             Set this to '0' for integer output.
 *  @param[in]  u32Denominator The 20-bit denominator for fractional output (1..1,048,575).
 *                             Set this to '1' or higher to avoid divider by zero errors.
 *
 *  @section PLL Configuration
 *
 *  fVCO is the PLL output, and must be between 600..900MHz, where:
 *
 *      fVCO = fXTAL * (a+(b/c))
 *
 *  fXTAL = the crystal input frequency
 *  a     = an integer between 15 and 90
 *  b     = the fractional numerator (0..1,048,575)
 *  c     = the fractional denominator (1..1,048,575)
 *
 *  NOTE: Try to use integers whenever possible to avoid clock jitter
 *  (only use the a part, setting b to '0' and c to '1').
 *
 *  See: http://www.silabs.com/Support%20Documents/TechnicalDocs/AN619.pdf
 */
status_t SI5351_SetupPLL(i2c_si5351_t* pHandle, si5351_pll_e ePll, uint8_t u8Multiplier, uint32_t u32Numerator, uint32_t u32Denominator)
{
    uint32_t P1; /* PLL config register P1 */
    uint32_t P2; /* PLL config register P2 */
    uint32_t P3; /* PLL config register P3 */

    /* Basic validation */
    ASSERT((u8Multiplier > 14) && (u8Multiplier < 91), "invaild value"); /* u8Mult = 15..90 */
    ASSERT(u32Denominator > 0, "invaild value");                         /* Avoid divide by zero */
    ASSERT(u32Numerator <= 0xFFFFF, "invaild value");                    /* 20-bit limit */
    ASSERT(u32Denominator <= 0xFFFFF, "invaild value");                  /* 20-bit limit */

    /* Feedback Multisynth Divider Equation
     *
     * where: a = u8Mult, b = num and c = denom
     *
     * P1 register is an 18-bit value using following formula:
     *
     * 	P1[17:0] = 128 * u8Mult + floor(128*(num/denom)) - 512
     *
     * P2 register is a 20-bit value using the following formula:
     *
     * 	P2[19:0] = 128 * num - denom * floor(128*(num/denom))
     *
     * P3 register is a 20-bit value using the following formula:
     *
     * 	P3[19:0] = denom
     */

    /* Set the main PLL config registers */
    if (u32Numerator == 0)
    {
        /* Integer mode */
        P1 = 128 * u8Multiplier - 512;
        P2 = u32Numerator;
        P3 = u32Denominator;
    }
    else
    {
        /* Fractional mode */
        P1 = (uint32_t)(128 * u8Multiplier + floor(128 * ((float)u32Numerator / (float)u32Denominator)) - 512);
        P2 = (uint32_t)(128 * u32Numerator - u32Denominator * floor(128 * ((float)u32Numerator / (float)u32Denominator)));
        P3 = u32Denominator;
    }

    /* Get the appropriate starting point for the PLL registers */
    uint8_t u8MemAddr = (ePll == SI5351_PLL_A ? 26 : 34);

    /* The datasheet is a nightmare of typos and inconsistencies here! */
    uint8_t au8Data[8];
    au8Data[0] = (P3 & 0x0000FF00) >> 8;
    au8Data[1] = (P3 & 0x000000FF);
    au8Data[2] = (P1 & 0x00030000) >> 16;
    au8Data[3] = (P1 & 0x0000FF00) >> 8;
    au8Data[4] = (P1 & 0x000000FF);
    au8Data[5] = ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16);
    au8Data[6] = (P2 & 0x0000FF00) >> 8;
    au8Data[7] = (P2 & 0x000000FF);
    SI5351_WriteBlock(pHandle, u8MemAddr, &au8Data[0], ARRAY_SIZE(au8Data));

    /* Reset both PLLs */
    SI5351_WriteByte(pHandle, REG_177_PLL_RESET, (1 << 7) | (1 << 5));

    /* Store the frequency settings for use with the Multisynth helper */

    float fvco = pHandle->eCrystalFreq * (u8Multiplier + ((float)u32Numerator / (float)u32Denominator));

    if (ePll == SI5351_PLL_A)
    {
        pHandle->_bPllaConfigured = true;
        pHandle->_u32PllaFreq     = (uint32_t)floor(fvco);
    }
    else  // if(ePll == SI5351_PLL_B)
    {
        pHandle->_bPllbConfigured = true;
        pHandle->_u32PllbFreq     = (uint32_t)floor(fvco);
    }

    return ERR_NONE;
}

/**
 *  @brief  Configures the Multisynth divider using integer output.
 *
 *  @param[in]  eChannel The output channel to use (0..2)
 *  @param[in]  ePllSrc	  The PLL input source to use
 *  @param[in]  eDivider  The integer divider for the Multisynth output
 */
status_t SI5351_SetupMultisynthInt(i2c_si5351_t* pHandle, si5351_channel_e eChannel, si5351_pll_e ePllSrc, si5351_multisynth_div_e eDivider)
{
    return SI5351_SetupMultisynth(pHandle, eChannel, ePllSrc, eDivider, 0, 1);
}

/**
 * @param[in] eChannel Enables or disables output
 * @param[in] eDivider Set of output divider values (2^n, n=1..7)
 */
status_t SI5351_SetupRdiv(i2c_si5351_t* pHandle, si5351_channel_e eChannel, si5351_r_div_e eDivider)
{
    uint8_t u8MemAddr, u8Data;

    switch (eChannel)
    {
        case SI5351_CHANNEL_0: u8MemAddr = REG_44_MULTISYNTH0_PARAMETERS_3; break;
        case SI5351_CHANNEL_1: u8MemAddr = REG_52_MULTISYNTH1_PARAMETERS_3; break;
        case SI5351_CHANNEL_2: u8MemAddr = REG_60_MULTISYNTH2_PARAMETERS_3; break;
        default: return ERR_INVALID_VALUE;
    }

    SI5351_ReadByte(pHandle, u8MemAddr, &u8Data);

    u8Data &= 0x0F;
    uint8_t u8Divider = eDivider;
    u8Divider &= 0x07;
    u8Divider <<= 4;
    u8Data |= u8Divider;
    pHandle->_au8LastRdivVal[eChannel] = u8Divider;

    return SI5351_WriteByte(pHandle, u8MemAddr, u8Data);
}

/**
 * @brief  Configures the Multisynth divider, which determines the
 *         output clock frequency based on the specified PLL input.
 *
 * @param[in]  eChannel   The output channel to use
 * @param[in]  ePllSrc	  The PLL input source to use
 * @param[in]  eDivider   The integer divider for the Multisynth output.
 *                        If fractional output is used, this value must be between 8 and 900.
 * @param[in]  u32Numerator   The 20-bit numerator for fractional output (0..1,048,575). Set this to '0' for integer output.
 * @param[in]  u32Denominator The 20-bit denominator for fractional output (1..1,048,575). Set this to '1' or higher to avoid divide by zero errors.
 *
 * @section Output Clock Configuration
 *
 * The multisynth dividers are applied to the specified PLL output,
 * and are used to reduce the PLL output to a valid range (500kHz
 * to 160MHz). The relationship can be seen in this formula, where
 * fVCO is the PLL output frequency and MSx is the multisynth
 * divider:
 *
 *     fOUT = fVCO / MSx
 *
 * Valid multisynth dividers are 4, 6, or 8 when using integers,
 * or any fractional values between 8 + 1/1,048,575 and 900 + 0/1
 *
 * The following formula is used for the fractional mode divider:
 *
 *     a + b / c
 *
 * a = The integer value, which must be 4, 6 or 8 in integer mode (MSx_INT=1)
 *     or 8..900 in fractional mode (MSx_INT=0).
 * b = The fractional numerator (0..1,048,575)
 * c = The fractional denominator (1..1,048,575)
 *
 * @note   Try to use integers whenever possible to avoid clock jitter
 *
 * @note   For output frequencies > 150MHz, you must set the divider
 *         to 4 and adjust to PLL to generate the frequency (for example
 *         a PLL of 640 to generate a 160MHz output clock). This is not
 *         yet supported in the driver, which limits frequencies to
 *         500kHz .. 150MHz.
 *
 * @note   For frequencies below 500kHz (down to 8kHz) Rx_DIV must be
 *         used, but this isn't currently implemented in the driver.
 */
status_t SI5351_SetupMultisynth(i2c_si5351_t* pHandle, si5351_channel_e eChannel, si5351_pll_e ePllSrc, uint32_t u32Divider, uint32_t u32Numerator, uint32_t u32Denominator)
{
    uint32_t P1; /* Multisynth config register P1 */
    uint32_t P2; /* Multisynth config register P2 */
    uint32_t P3; /* Multisynth config register P3 */

    /* Basic validation */
    ASSERT(u32Divider > 3, "invaild value");            /* Divider integer value */
    ASSERT(u32Divider < 2049, "invaild value");         /* Divider integer value */
    ASSERT(u32Denominator > 0, "invaild value");        /* Avoid divide by zero */
    ASSERT(u32Numerator <= 0xFFFFF, "invaild value");   /* 20-bit limit */
    ASSERT(u32Denominator <= 0xFFFFF, "invaild value"); /* 20-bit limit */

    /* Make sure the requested PLL has been bInitialised */
    switch (ePllSrc)
    {
        case SI5351_PLL_A:
        {
            if (pHandle->_bPllaConfigured == false)
            {
                return ERR_NOT_ALLOWED;
            }
            break;
        }
        case SI5351_PLL_B:
        {
            if (pHandle->_bPllbConfigured == false)
            {
                return ERR_NOT_ALLOWED;
            }
            break;
        }
        default: return ERR_INVALID_VALUE;
    }

    /* Output Multisynth Divider Equations
     *
     * where: a = eDivider, b = num and c = denom
     *
     * P1 register is an 18-bit value using following formula:
     *
     * 	P1[17:0] = 128 * a + floor(128*(b/c)) - 512
     *
     * P2 register is a 20-bit value using the following formula:
     *
     * 	P2[19:0] = 128 * b - c * floor(128*(b/c))
     *
     * P3 register is a 20-bit value using the following formula:
     *
     * 	P3[19:0] = c
     */

    /* Set the main PLL config registers */
    if (u32Numerator == 0)
    {
        /* Integer mode */
        P1 = 128 * u32Divider - 512;
        P2 = 0;
        P3 = u32Denominator;
    }
    else if (u32Denominator == 1)
    {
        /* Fractional mode, simplified calculations */
        P1 = 128 * u32Divider + 128 * u32Numerator - 512;
        P2 = 128 * u32Numerator - 128;
        P3 = 1;
    }
    else
    {
        /* Fractional mode */
        P1 = (uint32_t)(128 * u32Divider + floor(128 * ((float)u32Numerator / (float)u32Denominator)) - 512);
        P2 = (uint32_t)(128 * u32Numerator - u32Denominator * floor(128 * ((float)u32Numerator / (float)u32Denominator)));
        P3 = u32Denominator;
    }

    /* Get the appropriate starting point for the PLL registers */
    uint8_t u8MemAddr = 0;
    switch (eChannel)
    {
        case SI5351_CHANNEL_0: u8MemAddr = REG_42_MULTISYNTH0_PARAMETERS_1; break;
        case SI5351_CHANNEL_1: u8MemAddr = REG_50_MULTISYNTH1_PARAMETERS_1; break;
        case SI5351_CHANNEL_2: u8MemAddr = REG_58_MULTISYNTH2_PARAMETERS_1; break;
        default: return ERR_INVALID_VALUE;
    }

    /* Set the MSx config registers */
    /* Burst mode: register address auto-increases */
    uint8_t au8Data[8];
    au8Data[0] = HIBYTE(P3);
    au8Data[1] = LOBYTE(P3);
    au8Data[2] = ((P1 & 0x30000) >> 16) | pHandle->_au8LastRdivVal[eChannel];
    au8Data[3] = HIBYTE(P1);
    au8Data[4] = LOBYTE(P1);
    au8Data[5] = ((P3 & 0xF0000) >> 12) | ((P2 & 0xF0000) >> 16);
    au8Data[6] = HIBYTE(P2);
    au8Data[7] = LOBYTE(P2);
    SI5351_WriteBlock(pHandle, u8MemAddr, &au8Data[0], ARRAY_SIZE(au8Data));

    /* Configure the clk control and enable the output */
    /* TODO: Check if the clk control byte needs to be updated. */
    uint8_t clkControlReg = 0x0F; /* 8mA drive strength, MS0 as CLK0 source, Clock  not inverted, powered up */

    if (ePllSrc == SI5351_PLL_B)
    {
        clkControlReg |= (1 << 5); /* Uses PLLB */
    }
    if (u32Numerator == 0)
    {
        clkControlReg |= (1 << 6); /* Integer mode */
    }

    switch (eChannel)
    {
        case 0: SI5351_WriteByte(pHandle, REG_16_CLK0_CONTROL, clkControlReg); break;
        case 1: SI5351_WriteByte(pHandle, REG_17_CLK1_CONTROL, clkControlReg); break;
        case 2: SI5351_WriteByte(pHandle, REG_18_CLK2_CONTROL, clkControlReg); break;
    }

    return ERR_NONE;
}

/**
 * @brief     Enables or disables all clock outputs
 * @param[in] bEnabled Whether output is bEnabled
 */
status_t SI5351_EnableOutputs(i2c_si5351_t* pHandle, bool bEnabled)
{
    /* Enabled desired outputs (see Register 3) */
    SI5351_WriteByte(pHandle, REG_3_OUTPUT_ENABLE_CONTROL, bEnabled ? 0x00 : 0xFF);

    return ERR_NONE;
}

status_t SI5351_EnableSpreadSpectrum(i2c_si5351_t* pHandle, bool bEnabled)
{
    uint8_t u8Data;

    SI5351_ReadByte(pHandle, REG_149_SPREAD_SPECTRUM_PARAMETERS, &u8Data);

    if (bEnabled)
    {
        u8Data |= BV(7);
    }
    else
    {
        u8Data &= ~BV(7);
    }

    SI5351_WriteByte(pHandle, REG_149_SPREAD_SPECTRUM_PARAMETERS, u8Data);

    return ERR_NONE;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void SI5351_Test(i2c_mst_t* hI2C)
{
    i2c_si5351_t si5351 = {
        .hI2C          = hI2C,
        .u8SlvAddr     = SI5351_ADDRESS,
        .eCrystalFreq  = SI5351_CRYSTAL_FREQ_25MHZ,
        .eCrystalLoad  = SI5351_CRYSTAL_LOAD_10PF,
        .u32CrystalPPM = 30,
    };

    /* Initialise the sensor */
    SI5351_Init(&si5351);

    /* INTEGER ONLY MODE --> most accurate output */
    /* Setup PLLA to integer only mode @ 900MHz (must be 600..900MHz) */
    /* Set Multisynth 0 to 112.5MHz using integer only mode (eDivider by 4/6/8) */
    /* 25MHz * 36 = 900 MHz, then 900 MHz / 8 = 112.5 MHz */
    SI5351_SetupPLLInt(&si5351, SI5351_PLL_A, 36);
    SI5351_SetupMultisynthInt(&si5351, SI5351_CHANNEL_0, SI5351_PLL_A, SI5351_MULTISYNTH_DIV_8);

    /* FRACTIONAL MODE --> More flexible but introduce clock jitter */
    /* Setup PLLB to fractional mode @616.66667MHz (XTAL * 24 + 2/3) */
    /* Setup Multisynth 1 to 13.55311MHz (PLLB/45.5) */
    SI5351_SetupPLL(&si5351, SI5351_PLL_B, 24, 2, 3);
    SI5351_SetupMultisynth(&si5351, SI5351_CHANNEL_1, SI5351_PLL_B, 45, 1, 2);

    /* Multisynth 2 is not yet used and won't be bEnabled, but can be */
    /* Use PLLB @ 616.66667MHz, then divide by 900 -> 685.185 KHz */
    /* then divide by 64 for 10.706 KHz */
    /* configured using either PLL in either integer or fractional mode */
    SI5351_SetupMultisynth(&si5351, SI5351_CHANNEL_2, SI5351_PLL_B, 900, 0, 1);
    SI5351_SetupRdiv(&si5351, SI5351_CHANNEL_2, SI5351_R_DIV_64);

    /* Enable the clocks */
    SI5351_EnableOutputs(&si5351, true);

    while (1);
}

#endif