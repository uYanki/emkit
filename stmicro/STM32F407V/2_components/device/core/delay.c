#include "delay.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "delay"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void DelayBlock(tick_t TickWait)
{
    tick_t TickEnd;

    if (TickWait == 0)
    {
        return;
    }

    TickEnd = GetTickUs() + TickWait;

    while (GetTickUs() < TickEnd)
    {
    }
}

bool DelayNonBlock(tick_t TickStart, tick_t TickWait)
{
    tick_t TickEnd = TickStart + TickWait;

    if (GetTickUs() < TickEnd)
    {
        return false;  // waiting
    }
    else
    {
        return true;  // time is up
    }
}
