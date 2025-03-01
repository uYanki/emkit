#ifndef __SWRST_H__
#define __SWRST_H__

#include "stm32f4xx.h"

void NVIC_CoreReset_C(void);
void NVIC_SystemReset_C(void);

__asm void NVIC_CoreReset_ASM(void);
__asm void NVIC_SystemReset_ASM(void);

#endif
