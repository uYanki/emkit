#include "typebasic.h"
#include "delay.h"

#ifndef CONFIG_TIMEBASE_SOURCE
#define CONFIG_TIMEBASE_SOURCE TIMEBASE_TIM_UP
#endif

#define LOG_LOCAL_TAG   "drv_tim"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief CONFIG_TIMEBASE_SOURCE
 */
#define TIMEBASE_SYSTICK 0 /*<! SysTick, counter down */
#define TIMEBASE_TIM_UP  1 /*<! TIM, counter up */
// #define TIMEBASE_TIM_DOWN 2 /*<! TIM, counter down */

#if (CONFIG_TIMEBASE_SOURCE == TIMEBASE_SYSTICK)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

tick_t GetTickUs(void)
{
    /**
     * @note SysTick 为 24-bit 递减计数器，当 SysTick->VAL 减到 0 时会将 SysTick->LOAD 里的值赋到 SysTick->VAL 里.
     */

    tick_t TimeUs   = 0;
    tick_t PeriodUs = 1000 / uwTickFreq;

    TimeUs += PeriodUs * HAL_GetTick();
    TimeUs += PeriodUs * (SysTick->LOAD - SysTick->VAL) / SysTick->LOAD;

    return TimeUs;
}

#elif (CONFIG_TIMEBASE_SOURCE == TIMEBASE_TIM_UP)

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static volatile tick_t m_Tick = 0ULL;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief redirect DelayInit()
 */
void $Sub$$DelayInit()
{
    $Super$$DelayInit();

    // clkin = 144MHz
    // psc = 12 => tick = 12MHz = 1/12 us
    // arr = 12000 => irq freq = arr * psc = 1KHz;
    // GetTick100ns(){ n * tick * 12 /10 }

    /* enable tmr1 clock */
    crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);  // 启用外设时钟

    /* time base configuration */
    tmr_base_init(TMR1, 12000 - 1, 12 - 1);
    tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

    /* overflow interrupt enable */
    tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);

    /* tmr overflow interrupt nvic init */
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 0, 0);  // 启用定时器中断

    /* enable tmr */
    tmr_counter_enable(TMR1, TRUE);
}

tick_t GetTick100ns(void)
{
    return (tick_t)(m_Tick) * (tick_t)(10000) + (tick_t)(TMR1->cval) * 12 / 10;
}

tick_t GetTickUs(void)
{
    return GetTick100ns() / 10;
}

/**
 * @brief redirect TMR1_OVF_TMR10_IRQHandler()
 */
void $Sub$$TMR1_OVF_TMR10_IRQHandler(void)
{
    extern void $Super$$TMR1_OVF_TMR10_IRQHandler(void);

    if (tmr_interrupt_flag_get(TMR1, TMR_OVF_FLAG) != RESET)
    {
        m_Tick++;  // timer overflow handler. period = 1ms.
    }

    $Super$$TMR1_OVF_TMR10_IRQHandler();
}

#endif
