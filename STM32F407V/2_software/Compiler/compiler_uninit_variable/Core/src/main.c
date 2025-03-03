#include "main.h"
#include "stm32f4xx.h"
#include "system/sleep.h"
#include "bsp/uart.h"
#include "core_cm4.h"

#if (__ARMCC_VERSION >= 6000000)  // Arm Compiler 6
#define __NOINIT __attribute__((section(".bss.ARM.__at_0x2001FF00")))
#elif (__ARMCC_VERSION >= 5000000)  // Arm Compiler 5
// #define __NOINIT __attribute__((at(0x2001FF00)));
#define __NOINIT __attribute__((zero_init, used, section(".swrst_keep")))
#else
#error "unsupported version"
#endif

u32 a __NOINIT;
u32   b;

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

int main()
{
    usdk_preinit();

    ++a, ++b;
    printf("%d,%d\n", a, b);

    sleep_s(1);
    NVIC_SystemReset();  // rst

    while (1)
    {
    }
}
