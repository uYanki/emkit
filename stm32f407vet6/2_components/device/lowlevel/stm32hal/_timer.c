#include "typebasic.h"
#include "delay.h"

#define LOG_LOCAL_TAG   "drv_tim"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#include "tim.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define htimx htim11

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static volatile tick_t m_Tick = 0U;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief redirect DelayInit()
 */
void $Sub$$DelayInit()
{
    extern void $Super$$DelayInit();
    $Super$$DelayInit();

    // clkin = 200MHz
    // psc = 20 => tick = 10MHz = 0.1us
    // arr = 10000 => irq freq = arr * psc = 1KHz;
    // GetTick100ns(){ TIM_GetCounter() }

    // clkin = 168MHz
    // psc = 12 => tick = 14MHz = 0.1us / 1.4
    // arr = 14000 => irq freq = arr * psc = 1KHz;
    // GetTick100ns(){ n * tick * 1.4 }

    MX_TIM11_Init();

    htimx.Instance               = TIM11;  // 168MHz
    htimx.Init.Prescaler         = 12 - 1;
    htimx.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htimx.Init.Period            = 14000 - 1;
    htimx.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htimx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_Base_Init(&htimx);

    HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

    HAL_TIM_Base_Start_IT(&htimx);
}

tick_t GetTick100ns(void)
{
    return (tick_t)(m_Tick) * (tick_t)(10000) + (tick_t)(TIM11->CNT * 14 / 10);
}

tick_t GetTickUs(void)
{
    return GetTick100ns() / 10;
}

/**
 * @brief redirect HAL_TIM_PeriodElapsedCallback()
 */
void $Sub$$HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    extern void $Super$$HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim);

    if (htim == &htimx)  // period = 1ms
    {
        m_Tick++;  // IncTick()
    }

    $Super$$HAL_TIM_PeriodElapsedCallback(htim);
}
