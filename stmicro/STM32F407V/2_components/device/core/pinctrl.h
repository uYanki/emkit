#ifndef __PINCTRL_H__
#define __PINCTRL_H__

#include "pindefs.h"
#include "delay.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t    PIN_SetMode(const pin_t* pHandle, pin_mode_e eMode, pin_pull_e ePull);
pin_level_e PIN_ReadLevel(const pin_t* pHandle);
void        PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel);
void        PIN_ToggleLevel(const pin_t* pHandle);

#ifdef __cplusplus
}
#endif

#endif
