#include "stm32f4xx.h"

#include "system/autoinit.h"
#include "system/swrst.h"

#include "bsp/key.h"
#include "bsp/led.h"
#include "bsp/uart.h"

#include "usdk.defs.h"

int main()
{
    u32 blink = 0;

    printf("\n");

    DelayInit();
    sleep_s(2);

    while (1)
    {
        if (DelayNonBlockS(blink, 1))
        {
            led_tgl(LED1);
            blink = HAL_GetTick();
        }

        if (key_is_press(KEY2))
        {
            NVIC_CoreReset_C();
        }
        if (key_is_press(KEY3))
        {
            NVIC_SystemReset_C();
        }
    }
}

#if 0

void init_fpu()
{
    // This enables the floating point unit
    SCB->CPACR |= 0xF << 20;
}

#endif

//

void usdk_preinit(void)
{
    // sleep
    sleep_init();
    // hw_uart
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    UART_Config(&USART_InitStructure);
}

//

int display_reset_reason(void)
{
    // 上电打印寄存器值
    printf("<*> CSR = 0x%X\n", RCC->CSR);

    if (RCC->CSR & BV(31))
    {  // 1.低功耗管理复位
        printf("<*> 1.Low-power reset\n");
    }
    if (RCC->CSR & BV(30))
    {  // 2.窗口看门狗复位
        printf("<*> 2.Window watchdog reset\n");
    }
    if (RCC->CSR & BV(29))
    {  // 3.独立看门狗复位
        printf("<*> 3.(Independent watchdog reset\n");
    }
    if (RCC->CSR & BV(28))
    {  // 4.软件复位
        printf("<*> 4.Software reset\n");
    }
    if (RCC->CSR & BV(27))
    {  // 5.上电/掉电复位
        printf("<*> 5.POR/PDR reset\n");
    }
    if (RCC->CSR & BV(26))
    {  // 6.PIN引脚复位
        printf("<*> 6.PIN reset\n");
    }

    // 清除复位标志
    RCC->CSR = 0x01000000;

    return INIT_RESULT_SUCCESS;
}

USDK_INIT_EXPORT(display_reset_reason, INIT_LEVEL_DEVICE)

//

static int display_system_clock(void)
{
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    printf("<*> SYSCLK:%luHz\n", RCC_Clocks.SYSCLK_Frequency);
    printf("<*> HCLK:%luHz\n", RCC_Clocks.HCLK_Frequency);    // AHB
    printf("<*> PCLK1:%luHz\n", RCC_Clocks.PCLK1_Frequency);  // APB1
    printf("<*> PCLK2:%luHz\n", RCC_Clocks.PCLK2_Frequency);  // APB2

    return INIT_RESULT_SUCCESS;
}

USDK_INIT_EXPORT(display_system_clock, INIT_LEVEL_DEVICE)
