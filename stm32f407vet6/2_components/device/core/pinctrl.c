#include "pinctrl.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "pinctrl"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

extern const pin_ops_t g_hwPinOps;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t PIN_SetMode(const pin_t* pHandle, pin_mode_e eMode, pin_pull_e ePull)
{
    return g_hwPinOps.SetMode(pHandle, eMode, ePull);
}

pin_level_e PIN_ReadLevel(const pin_t* pHandle)
{
    return g_hwPinOps.ReadLevel(pHandle);
}

void PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel)
{
    g_hwPinOps.WriteLevel(pHandle, eLevel);
}

void PIN_ToggleLevel(const pin_t* pHandle)
{
    g_hwPinOps.ToggleLevel(pHandle);
}
