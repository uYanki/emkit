#include "swrst.h"

// NVIC_CoreReset:   复位内核, 不包含外设（寄存器内容保持不变）
// NVIC_SystemReset: 复位芯片, 包含外设（寄存器被恢复为默认值）

void NVIC_CoreReset_C(void)
{
    __DSB();

    SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |
                  (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                  SCB_AIRCR_VECTRESET_Msk);  // VECTRESET

    __DSB();

    while (1);
}

// clang-format off
__asm void NVIC_CoreReset_ASM(void)
{
  LDR R0, =0xE000ED0C
  LDR R1, =0x05FA0001 // VECTRESET
  STR R1, [R0]
deadloop_Core
  B deadloop_Core
}
// clang-format on

void NVIC_SystemReset_C(void)
{
    __DSB();

    SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |
                  (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                  SCB_AIRCR_SYSRESETREQ_Msk);  // SYSRESETREQ
    __DSB();

    while (1);
}

// clang-format off
__asm void NVIC_SystemReset_ASM(void)
{
  LDR R0, =0xE000ED0C // SCB->AIRCR address
  LDR R1, =0x05FA0004 // VECTKEY = 0x05FA, SYSRESETREQ = 0x4;                         
  STR R1, [R0]      
deadloop_Sys
  B deadloop_Sys                               
}
// clang-format on
