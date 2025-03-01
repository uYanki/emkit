#include "spi_tm1638.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "tm1638"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief 数据命令设置 Instruction description
 */
#define CMD_DATA_RW           0x40
#define OPT_WRITE_DISP_DATA   0x00  // 写数据到显示寄存器 write data to display register
#define OPT_READ_KEYSCAN_DATA 0x02  // 读键扫数据 read key scan data
#define OPT_ADDR_AUTOINC      0x00  // 自动地址增加 auto address add
#define OPT_ADDR_FIXED        0x04  // 地址固定 fixed address
#define OPT_NORMAL_MODE       0x00  // 普通模式 normal mode
#define OPT_TEST_MODE         0x08  // 测试模式 test mode

/**
 * @brief 显示控制命令设置 Display ControlInstruction Set
 */
#define CMD_DISP_CTRL 0x80
#define OPT_TURN_OFF  0x00  // 显示关 show turn off
#define OPT_TURN_ON   0x08  // 显示开 show turn on

/**
 * @brief 地址命令设置 Address Instruction Set
 */
#define CMD_SET_ADDR  0xC0
#define OPT_MIN_ADDR  0x00  // min address
#define OPT_MAX_ADDR  0x0F  // max address
#define OPT_SEG0_ADDR 0x00  // leftmost segment address
#define OPT_LED0_ADDR 0x01  // leftmost LED address

#define ADDR_CNT      (OPT_MAX_ADDR - OPT_MIN_ADDR + 1)

/**
 * @brief SegTbl
 */
#define TM1638_ASCII_OFFSET 32   // Ascii table offset to jump over first missing 32 chars
#define TM1638_DOT_MASK_DEC 128  // 0x80 Mask to  switch on decimal point in seven seg.

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

/**
 * @brief 共阴数码管码表
 *
 * Common-Cathode 共阴 √
 * Common-Anode   共阳
 *
 */

static const uint8_t m_aSegTbl[] = {
    0x00, /* (space) */
    0x86, /* ! */
    0x22, /* " */
    0x7E, /* # */
    0x6D, /* $ */
    0xD2, /* % */
    0x46, /* & */
    0x20, /* ' */
    0x29, /* ( */
    0x0B, /* ) */
    0x21, /* * */
    0x70, /* + */
    0x10, /* , */
    0x40, /* - */
    0x80, /* . */
    0x52, /* / */
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F, /* 9 */
    0x09, /* : */
    0x0D, /* ; */
    0x61, /* < */
    0x48, /* = */
    0x43, /* > */
    0xD3, /* ? */
    0x5F, /* @ */
    0x77, /* A */
    0x7C, /* B */
    0x39, /* C */
    0x5E, /* D */
    0x79, /* E */
    0x71, /* F */
    0x3D, /* G */
    0x76, /* H */
    0x30, /* I */
    0x1E, /* J */
    0x75, /* K */
    0x38, /* L */
    0x15, /* M */
    0x37, /* N */
    0x3F, /* O */
    0x73, /* P */
    0x6B, /* Q */
    0x33, /* R */
    0x6D, /* S */
    0x78, /* T */
    0x3E, /* U */
    0x3E, /* V */
    0x2A, /* W */
    0x76, /* X */
    0x6E, /* Y */
    0x5B, /* Z */
    0x39, /* [ */
    0x64, /* \ */
    0x0F, /* ] */
    0x23, /* ^ */
    0x08, /* _ */
    0x02, /* ` */
    0x5F, /* a */
    0x7C, /* b */
    0x58, /* c */
    0x5E, /* d */
    0x7B, /* e */
    0x71, /* f */
    0x6F, /* g */
    0x74, /* h */
    0x10, /* i */
    0x0C, /* j */
    0x75, /* k */
    0x30, /* l */
    0x14, /* m */
    0x54, /* n */
    0x5C, /* o */
    0x73, /* p */
    0x67, /* q */
    0x50, /* r */
    0x6D, /* s */
    0x78, /* t */
    0x1C, /* u */
    0x1C, /* v */
    0x14, /* w */
    0x76, /* x */
    0x6E, /* y */
    0x5B, /* z */
          // Note : Removed last 4 characters to reduce program size as of v 1.3.0
    0x46, /* { */
    0x30, /* | */
    0x70, /* } */
    0x01, /* ~ */
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline void TM1638_WriteByte(spi_tm1638_t* pHandle, uint8_t u8Byte)
{
    SPI_Master_TransmitByte(pHandle->hSPI, u8Byte);
}

static inline void TM1638_WriteCmd(spi_tm1638_t* pHandle, uint8_t u8Cmd)
{
    SPI_Master_Select(pHandle->hSPI);
    DelayBlockUs(1);

    TM1638_WriteByte(pHandle, u8Cmd);

    SPI_Master_Deselect(pHandle->hSPI);

    DelayBlockUs(1);
}

static inline void TM1638_WriteData(spi_tm1638_t* pHandle, uint8_t u8Addr, uint8_t u8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    DelayBlockUs(1);

    TM1638_WriteByte(pHandle, u8Addr);
    TM1638_WriteByte(pHandle, u8Data);

    SPI_Master_Deselect(pHandle->hSPI);
    DelayBlockUs(1);
}

void TM1638_Init(spi_tm1638_t* pHandle)
{
    SPI_Master_Deselect(pHandle->hSPI);

    TM1638_Clear(pHandle);
    TM1638_SetBrightness(pHandle, 5);
}

void TM1638_Clear(spi_tm1638_t* pHandle)
{
    uint8_t u8Times = ADDR_CNT;

    // set auto increment mode
    TM1638_WriteCmd(pHandle, CMD_DATA_RW | OPT_ADDR_AUTOINC);

    SPI_Master_Select(pHandle->hSPI);
    DelayBlockUs(1);

    // set starting address
    TM1638_WriteByte(pHandle, CMD_SET_ADDR | OPT_SEG0_ADDR);
    // reset allleds and segs
    while (u8Times--)
    {
        TM1638_WriteByte(pHandle, 0x00);
    }

    SPI_Master_Deselect(pHandle->hSPI);
    DelayBlockUs(1);
}

void TM1638_SetBrightness(spi_tm1638_t* pHandle, uint8_t u8Brightness)
{
    if (u8Brightness < 1 || u8Brightness > 8)
    {
        TM1638_WriteCmd(pHandle, CMD_DISP_CTRL | OPT_TURN_OFF);
    }
    else
    {
        u8Brightness -= 1;
        TM1638_WriteCmd(pHandle, CMD_DISP_CTRL | OPT_TURN_ON | u8Brightness);
    }
}

void TM1638_SetSingleLed(spi_tm1638_t* pHandle, uint8_t u8Channel, pin_level_e eLevel)
{
    TM1638_WriteCmd(pHandle, CMD_DATA_RW | OPT_ADDR_FIXED);
    TM1638_WriteData(pHandle, CMD_SET_ADDR | OPT_LED0_ADDR | (u8Channel << 1), (eLevel == PIN_LEVEL_LOW) ? 0U : 1U);
}

/**
 * @param[in] u8Data each bit corresponds to one channel
 */
void TM1638_SetMultipleLed(spi_tm1638_t* pHandle, uint8_t u8Data)
{
    for (uint8_t u8Index = 0; u8Index < 8; u8Index++)
    {
        if (u8Data & BV(u8Index))
        {
            TM1638_SetSingleLed(pHandle, u8Index, PIN_LEVEL_HIGH);
        }
        else
        {
            TM1638_SetSingleLed(pHandle, u8Index, PIN_LEVEL_LOW);
        }
    }
}

void TM1638_SetSingleDigital(spi_tm1638_t* pHandle, uint8_t u8Channel, uint8_t u8Data)
{
    TM1638_WriteCmd(pHandle, CMD_DATA_RW | OPT_ADDR_FIXED);
    TM1638_WriteData(pHandle, CMD_SET_ADDR | OPT_SEG0_ADDR | (u8Channel << 1), u8Data);
}

uint8_t TM1638_ScanKeys(spi_tm1638_t* pHandle)
{
    uint8_t u8KeyMsk = 0;
    uint8_t u8KeyIdx, au8KeySts[4];

    SPI_Master_Select(pHandle->hSPI);
    DelayBlockUs(1);

    TM1638_WriteByte(pHandle, CMD_DATA_RW | OPT_READ_KEYSCAN_DATA);
    SPI_Master_ReceiveBlock(pHandle->hSPI, &au8KeySts[0], ARRAY_SIZE(au8KeySts));

    SPI_Master_Deselect(pHandle->hSPI);
    DelayBlockUs(1);

    // 注：多种版本的按键不同，需自行更改

#if 1
    // 8 keys
    for (u8KeyIdx = 0; u8KeyIdx < ARRAY_SIZE(au8KeySts); ++u8KeyIdx)
    {
        u8KeyMsk |= au8KeySts[u8KeyIdx] << u8KeyIdx;
    }
    return u8KeyMsk;
#elif 0
    //  16 keys
#else
    //  24 keys

    /**
     * @brief  Scan all 24 keys connected to TM1638
     * @note
     *                   SEG1         SEG2         SEG3       ......      SEG8
     *                     |            |            |                      |
     *         K1  --  |K1_SEG1|    |K1_SEG2|    |K1_SEG3|    ......    |K1_SEG8|
     *         K2  --  |K2_SEG1|    |K2_SEG2|    |K2_SEG3|    ......    |K2_SEG8|
     *         K3  --  |K3_SEG1|    |K3_SEG2|    |K3_SEG3|    ......    |K3_SEG8|
     *
     * @param  Keys: pointer to save key scan result
     *         - bit0 =>K1_SEG1, bit1 =>K1_SEG2, ..., bit7 =>K1_SEG8,
     *         - bit8 =>K2_SEG1, bit9 =>K2_SEG2, ..., bit15=>K2_SEG8,
     *         - bit16=>K3_SEG1, bit17=>K3_SEG2, ..., bit23=>K3_SEG8,
     *
     */

    uint32_t u32KeyMsk = 0;
    uint8_t  u8KeyChk  = 0x01;

    for (uint8_t i = 0; i < 3; i++)
    {
        for (int8_t j = 3; j >= 0; j--)
        {
            u32KeyMsk <<= 1U;

            if (au8KeySts[j] & (u8KeyChk << 4))
            {
                u32KeyMsk |= 1U;
            }

            u32KeyMsk <<= 1U;

            if (au8KeySts[j] & u8KeyChk)
            {
                u32KeyMsk |= 1U;
            }
        }

        u8KeyChk <<= 1;
    }

#endif
}

void TM1638_DisplayChar(spi_tm1638_t* pHandle, uint8_t eChannel, uint8_t u8AsciiCode, bool bShowDot)
{
    TM1638_SetSingleDigital(pHandle, eChannel, m_aSegTbl[u8AsciiCode - TM1638_ASCII_OFFSET] | (bShowDot ? TM1638_DOT_MASK_DEC : 0));
}

void TM1638_DisplayString(spi_tm1638_t* pHandle, uint8_t eChannel, const char* szString)
{
    while (eChannel < 8)
    {
        TM1638_DisplayChar(pHandle, eChannel, *szString, *(szString + 1) == '.');

        if (*szString++ == '\0')
        {
            break;
        }

        eChannel++;
    }
}

void TM1638_ShowLoadingAnimate(spi_tm1638_t* pHandle, uint16_t u16UpdatePeriodMs)
{
    uint8_t u8Channel;

    TM1638_SetSingleDigital(pHandle, 0, 0x01);
    DelayBlockMs(u16UpdatePeriodMs);

    for (u8Channel = 0; u8Channel <= 7; ++u8Channel)
    {
        TM1638_SetSingleDigital(pHandle, u8Channel - 1, 0x00);
        TM1638_SetSingleDigital(pHandle, u8Channel, 0x01);
        DelayBlockMs(u16UpdatePeriodMs);
    }

    TM1638_SetSingleDigital(pHandle, 7, 0x02);
    DelayBlockMs(u16UpdatePeriodMs);
    TM1638_SetSingleDigital(pHandle, 7, 0x04);
    DelayBlockMs(u16UpdatePeriodMs);
    TM1638_SetSingleDigital(pHandle, 7, 0x08);
    DelayBlockMs(u16UpdatePeriodMs);
    for (u8Channel = 7; u8Channel > 0; --u8Channel)
    {
        TM1638_SetSingleDigital(pHandle, u8Channel, 0x00);
        TM1638_SetSingleDigital(pHandle, u8Channel - 1, 0x08);
        DelayBlockMs(u16UpdatePeriodMs);
    }

    TM1638_SetSingleDigital(pHandle, 0, 0x10);
    DelayBlockMs(u16UpdatePeriodMs);
    TM1638_SetSingleDigital(pHandle, 0, 0x20);
    DelayBlockMs(u16UpdatePeriodMs);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

static void TM1638_ShowSegTable(spi_tm1638_t* pHandle)  // debug only
{
    uint8_t i, j;

    for (i = 0; i < sizeof(m_aSegTbl); i += 8)
    {
        for (j = 0; j < 8; ++j)
        {
            TM1638_SetSingleDigital(pHandle, (uint8_t)j, m_aSegTbl[i + j]);
        }

        while (TM1638_ScanKeys(pHandle) == 0)
        {
        }  // wait key press
        while (TM1638_ScanKeys(pHandle) != 0)
        {
        }  // wait key release
    }
}

void TM1638_Test(void)
{
    uint8_t u8KeyMsk;

    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5}, /* DIO */
        .MOSI = {GPIOA, GPIO_PIN_5},
        .SCLK = {GPIOA, GPIO_PIN_6}, /* CLK */
        .CS   = {GPIOA, GPIO_PIN_3}, /* STB */
    };

    spi_tm1638_t tm1638 = {
        .hSPI = &spi,
    };

    SPI_Master_Init(&spi, 100000, SPI_DUTYCYCLE_50_50, TM1638_SPI_TIMING | SPI_FLAG_SOFT_CS);

    TM1638_Init(&tm1638);

    TM1638_ShowSegTable(&tm1638);

    for (uint8_t u8Times = 0; u8Times < 2; ++u8Times)
    {
        TM1638_ShowLoadingAnimate(&tm1638, 50);
    }

    TM1638_DisplayString(&tm1638, 0, "helowrld");

    DelayBlockS(2);

    TM1638_DisplayString(&tm1638, 0, "key:    ");

    while (1)
    {
        u8KeyMsk = TM1638_ScanKeys(&tm1638);
        TM1638_SetSingleDigital(&tm1638, 4, u8KeyMsk);
        TM1638_SetMultipleLed(&tm1638, u8KeyMsk);
        DelayBlockMs(100);
    }
}

#endif
