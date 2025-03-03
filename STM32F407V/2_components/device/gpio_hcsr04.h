#ifndef __GPIO_HCSR04_H__
#define __GPIO_HCSR04_H__

#include "typebasic.h"
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    __IN const pin_t ECHO;
    __IN const pin_t TRIG;
} gpio_hcsr04_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void      HCSR04_Init(gpio_hcsr04_t* pHandle);
float32_t HCSR04_MeasureDistance(gpio_hcsr04_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void HCSR04_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
