#ifndef __MONO_FONT_H__
#define __MONO_FONT_H__

#include "typebasic.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define FONT_SRC_CPU_FLASH (0u) /*<! 片上 Flash */
#define FONT_SRC_EXT_FLASH (1u) /*<! 片外 Flash */
#define FONT_SRC_SDCARD    (2u) /*<! 内存卡 */

typedef struct mono_font      mono_font_t;
typedef struct mono_font_code mono_font_code_t;

struct mono_font {
    uint8_t  u8FontSrc; /*<! 字体来源 */
    void*    pvAddress; /*<! 储存地址 */
    uint16_t u16Width;  /*<! 字体宽度 unit: pixel */
    uint16_t u16Height; /*<! 字体高度 unit: pixel */
    bool (*pfnGetMonoFont)(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode);
};

struct mono_font_code {
    __INOUT uint8_t* pu8Buffer; /*<! 字体缓冲区 */
    __OUT uint16_t   u16Width;  /*<! 字体宽度 unit: pixel */
    __OUT uint16_t   u16Height; /*<! 字体高度 unit: pixel */
};

extern const mono_font_t g_Font_Conslons_8x16_CpuFlash;
extern const mono_font_t g_Font_Conslons_16x24_CpuFlash;
extern const mono_font_t g_Font_Conslons_24x32_CpuFlash;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool GetFontInfo(const mono_font_t* pFont, char Char, mono_font_code_t* pFontCode);

#ifdef __cplusplus
}
#endif

#endif
