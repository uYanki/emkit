#include "gpio_hcsr04.h"
#include "delay.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "hcsr04"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void HCSR04_Init(gpio_hcsr04_t* pHandle)
{
    PIN_SetMode(&pHandle->TRIG, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
    PIN_SetMode(&pHandle->ECHO, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
}

/**
 * @return distance in cm
 */
float32_t HCSR04_MeasureDistance(gpio_hcsr04_t* pHandle)
{
    tick_t tStartTickUs, tDeltaTickUs;

    PIN_WriteLevel(&pHandle->TRIG, PIN_LEVEL_HIGH);
    DelayBlockUs(20);
    PIN_WriteLevel(&pHandle->TRIG, PIN_LEVEL_LOW);

    /* waitting for start of the high level through echo pin */
    while (PIN_ReadLevel(&pHandle->ECHO) == PIN_LEVEL_LOW) {}

    /* record current time */
    tStartTickUs = GetTickUs();

    /* waitting for end of the high level through echo pin */
    while (PIN_ReadLevel(&pHandle->ECHO) == PIN_LEVEL_HIGH) {}

    /* get the time of high level */
    tDeltaTickUs = GetTickUs() - tStartTickUs;

    /* calc distance in unit cm */
    return (float32_t)(tDeltaTickUs / 1000000.0) * 340.0 / 2.0 * 100.0;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void HCSR04_Test(void)
{
    gpio_hcsr04_t hcsr04 = {
        .ECHO = {GPIOA, GPIO_PIN_5},
        .TRIG = {GPIOA, GPIO_PIN_6},
    };

    HCSR04_Init(&hcsr04);

    while (1)
    {
        PRINTLN("distance: %.2f cm", HCSR04_MeasureDistance(&hcsr04));
    }
}

#endif
