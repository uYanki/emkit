#include "delay.h"

bool DelayNonBlock(uint32_t nStartTick, uint32_t nWaitTime)
{
    uint32_t nDeltaTick;
    uint32_t nEndTick;

    nEndTick = HAL_GetTick();

    if (nEndTick >= nStartTick)
    {
        nDeltaTick = nEndTick - nStartTick;
    }
    else
    {
        nDeltaTick = UINT32_MAX - nStartTick + nEndTick;
    }

    return nDeltaTick >= nWaitTime;
}
