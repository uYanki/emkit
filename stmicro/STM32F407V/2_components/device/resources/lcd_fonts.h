#ifndef FONTS_H
#define FONTS_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "xsdk_types.h"

typedef struct {
    uint8_t         FontWidth;  /*!< Font u16Width in pixels */
    uint8_t         FontHeight; /*!< Font height in pixels */
    const uint16_t* data;       /*!< Pointer to data font data array */
} FontDef_t;

/**
 * @brief  String length and height
 */
typedef struct {
    uint16_t Length; /*!< String length in units of pixels */
    uint16_t Height; /*!< String height in units of pixels */
} FONTS_SIZE_t;

/**
 * @}
 */

/**
 * @defgroup FONTS_FontVariables
 * @brief    Library font variables
 * @{
 */

/**
 * @brief  n x m pixels font size structure
 */
extern FontDef_t Font_7x10;
extern FontDef_t Font_11x18;
extern FontDef_t Font_16x26;

/**
 * @brief  Calculates string length and height in units of pixels depending on string and font used
 * @param  *str: String to be checked for length and height
 * @param  *SizeStruct: Pointer to empty @ref FONTS_SIZE_t structure where informations will be saved
 * @param  *Font: Pointer to @ref FontDef_t font used for calculations
 * @retval Pointer to string used for length and height
 */
char* FONTS_GetStringSize(char* str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font);

#ifdef __cplusplus
}
#endif

#endif
