#include "mono_font.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mono-font"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static bool GetConslonsCode_CpuFlash(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode);
static bool GetGBKCode_SDCard(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode);
static bool GetGBKCode_ExtFlash(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static const uint8_t m_FontTable_Conslons_Ascii8x16[] = {
#include "./resources/mono_font_conslons_ascii_8x16.h"
};

static const uint8_t m_FontTable_Conslons_Ascii16x24[] = {
#include "./resources/mono_font_conslons_ascii_16x24.h"
};

static const uint8_t m_FontTable_Conslons_Ascii24x32[] = {
#include "./resources/mono_font_conslons_ascii_24x32.h"
};

const mono_font_t g_Font_Conslons_8x16_CpuFlash = {
    .u8FontSrc      = FONT_SRC_CPU_FLASH,
    .u16Width       = 8,
    .u16Height      = 16,
    .pvAddress      = (void*)m_FontTable_Conslons_Ascii8x16,
    .pfnGetMonoFont = GetConslonsCode_CpuFlash,
};

const mono_font_t g_Font_Conslons_16x24_CpuFlash = {
    .u8FontSrc      = FONT_SRC_CPU_FLASH,
    .u16Width       = 16,
    .u16Height      = 24,
    .pvAddress      = (void*)m_FontTable_Conslons_Ascii16x24,
    .pfnGetMonoFont = GetConslonsCode_CpuFlash,
};

const mono_font_t g_Font_Conslons_24x32_CpuFlash = {
    .u8FontSrc      = FONT_SRC_CPU_FLASH,
    .u16Width       = 24,
    .u16Height      = 32,
    .pvAddress      = (void*)m_FontTable_Conslons_Ascii24x32,
    .pfnGetMonoFont = GetConslonsCode_CpuFlash,
};

#if 0

const mono_font_t g_Font_GB2312_16x16_SDCard = {
    .u8FontSrc      = FONT_SRC_SDCARD,
    .u16Width       = 16,
    .u16Height      = 16,
    .pvAddress      = nullptr,
    .pfnGetMonoFont = GetGBKCode_SDCard,
};

#endif

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool GetFontInfo(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode)
{
    return pFont->pfnGetMonoFont(pFont, Char, pFontCode);
}

static bool GetConslonsCode_CpuFlash(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode)
{
    uint8_t  u8Count    = (pFont->u16Width * pFont->u16Height) >> 3;  // 要读取字节数量
    uint16_t u16Offset  = (Char - ' ') * u8Count;                     // 字模偏移地址
    uint8_t* pu8Address = (uint8_t*)(pFont->pvAddress) + u16Offset;   // 字模起始地址

    if (pFontCode->pu8Buffer == nullptr)
    {
        pFontCode->pu8Buffer = pu8Address;
    }
    else
    {
        memcpy(pFontCode->pu8Buffer, pu8Address, u8Count);
    }

    pFontCode->u16Width  = pFont->u16Width;
    pFontCode->u16Height = pFont->u16Height;

    return true;
}

static bool GetGBKCode_ExtFlash(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode)
{
    if (pFontCode->pu8Buffer == nullptr)
    {
        return false;
    }

    pFontCode->u16Width  = pFont->u16Width;
    pFontCode->u16Height = pFont->u16Height;

    return false;
}

static bool GetGBKCode_SDCard(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode)
{
    if (pFontCode->pu8Buffer == nullptr)
    {
        return false;
    }

    uint8_t  u8Count   = (pFont->u16Width * pFont->u16Height) >> 3;                     // 要读取字节数量
    uint32_t u32Offset = (((Char >> 8) - 0xA1) * 94 + (Char & 0xFF) - 0xA1) * u8Count;  // 字模偏移地址
    uint32_t pu8Table  = (uint32_t)(pFont->pvAddress) + u32Offset;                      // 字模起始地址

#if 0
    static FIL     fnew; /* file objects */
    static FATFS   fs;   /* Work area (file system object) for logical drives */
    static FRESULT res_sd;
    static UINT    br; /* File R/W count */
    static uint8_t everRead = 0;

    // 读取字模
    if (everRead == 0)
    {
        // 第一次使用，挂载文件系统，初始化sd
        res_sd   = f_mount(&fs, "0:", 1);
        everRead = 1;
    }
    res_sd = f_open(&fnew, CONFIG_GBKCODE_FILE_NAME, FA_OPEN_EXISTING | FA_READ);
    if (res_sd == FR_OK)
    {
        f_lseek(&fnew, nOffset);                // 指针偏移
        f_read(&fnew, pu8Buffer, nCount, &br);  // 16*16大小的汉字 其字模 占用16*16/8个字节
        f_close(&fnew);
        return true;
    }
#endif

    pFontCode->u16Width  = pFont->u16Width;
    pFontCode->u16Height = pFont->u16Height;

    return false;
}
