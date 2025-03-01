#ifndef __PIN_DEFS_H__
#define __PIN_DEFS_H__

#include "typebasic.h"
#include "busconf.h"
#include "errno.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef enum {
    PIN_LEVEL_LOW,
    PIN_LEVEL_HIGH,
} pin_level_e;

typedef enum {
    PIN_MODE_INPUT_FLOATING,    /* input */
    PIN_MODE_OUTPUT_OPEN_DRAIN, /* open drain output */
    PIN_MODE_OUTPUT_PUSH_PULL,  /* push pull output  */
    PIN_MODE_HIGH_RESISTANCE,   /* high resistance */
} pin_mode_e;

typedef enum {
    PIN_PULL_NONE, /* no pull */
    PIN_PULL_DOWN, /* pull down */
    PIN_PULL_UP,   /* pull up  */
} pin_pull_e;

typedef struct {
    void* Port;
    u16   Pin;
} pin_t;

typedef struct {
    status_t (*SetMode)(const pin_t* pHandle, pin_mode_e eMode, pin_pull_e ePull);
    pin_level_e (*ReadLevel)(const pin_t* pHandle);
    void (*WriteLevel)(const pin_t* pHandle, pin_level_e eLevel);
    void (*ToggleLevel)(const pin_t* pHandle);
} pin_ops_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
