#ifndef __DELAY_H__
#define __DELAY_H__

#include <stdbool.h>
#include "stm32h7xx_hal.h"

bool DelayNonBlock(uint32_t nStartTick, uint32_t nWaitTimeMs);

#endif
