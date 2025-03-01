#include "main.h"
#include "stm32f4xx.h"
#include "system/sleep.h"
#include "bsp/uart.h"

// clang-format off

__asm int readr0(void){ 
     bx lr
}
 
__asm int readr1(void){
     MOV     R0, R1
     bx lr
}
 
__asm int readr2(void){
     MOV     R0, R2
     bx lr
}
 
__asm int readlr(void){
     MOV     R0, lr 
     bx lr
}
 
__asm int readsp(void){
     MOV     R0, sp  
     bx lr
}
 
__asm int readpc(void){
     MOV     R0, pc  
     bx lr
}

// clang-format on

void flash_foo1()
{
    u32 p = readlr() - 7u;
    printf("* %s in 0x%x\n", __func__, p);
}

void flash_foo2()
{
    u32 p = readlr() - 7u;
    printf("* %s in 0x%x\n", __func__, p);
}

#pragma arm section code = "RAMCODE"  // start

void ram_foo1()
{
    u32 p = readlr() - 7u;
    printf("* %s in 0x%x\n", __func__, p);
}

void ram_foo2()
{
    u32 p = readlr() - 7u;
    printf("* %s in 0x%x\n", __func__, p);
}

__attribute__((section("ROMCODE"))) void rxm_foo()
{
    u32 p = readlr() - 7u;
    printf("* %s in 0x%x\n", __func__, p);
}

#pragma arm section  // end

__attribute__((section("RAMCODE"))) void ram_foo3()
{
    vu32 addr;  // 2 bytes

    __asm volatile {
        mov addr, __current_pc()  // 4 bytes
    }

    printf("* %s in 0x%x\n", __func__, addr - 6);
}

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

    flash_foo1();
    flash_foo2();

    ram_foo1();
    ram_foo2();
    ram_foo3();

    rxm_foo();

    printf("<-> flash_foo1 = 0x%p\n", flash_foo1);
    printf("<-> flash_foo2 = 0x%p\n", flash_foo2);
    printf("<-> ram_foo1 = 0x%p\n", ram_foo1);
    printf("<+> ram_foo2 = 0x%p\n", ram_foo2);
    printf("<+> ram_foo3 = 0x%p\n", ram_foo3);
    printf("<*> rxm_foo = 0x%p\n", rxm_foo);

    while (1)
    {
    }
}
