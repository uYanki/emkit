#include "logic_loader.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "logic loader"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

#define __nop()         __NOP()

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline void GenerateClockPulse(fpag_loader_t* pHandle)
{
    PIN_WriteLevel(&pHandle->DCLK, PIN_LEVEL_HIGH);

    __nop();
    __nop();
    __nop();
    __nop();

    PIN_WriteLevel(&pHandle->DCLK, PIN_LEVEL_LOW);

    __nop();
    __nop();
    __nop();
    __nop();
}

err_t LogicLoader_Init(fpag_loader_t* pHandle)
{
    PIN_SetMode(&pHandle->DCLK, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
    PIN_SetMode(&pHandle->DATA, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
    PIN_SetMode(&pHandle->RST, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
    PIN_SetMode(&pHandle->CONFIG, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
    PIN_SetMode(&pHandle->STATUS, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
    PIN_SetMode(&pHandle->DONE, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

    return ERR_NONE;
}

err_t LogicLoader_Program(fpag_loader_t* pHandle, const uint8_t* cpu8Buffer, uint32_t u32Size, bit_order_e eBitOrder)
{
    uint8_t u8Retries = 1;

    uint16_t u16Timeout;
    uint8_t  u8Pulse;

__sof:

    do
    {
        PIN_WriteLevel(&pHandle->DCLK, PIN_LEVEL_LOW);
        PIN_WriteLevel(&pHandle->CONFIG, PIN_LEVEL_LOW);
        PIN_WriteLevel(&pHandle->DCLK, PIN_LEVEL_LOW);

        DelayBlockUs(2);
        PIN_WriteLevel(&pHandle->CONFIG, PIN_LEVEL_HIGH);

        u16Timeout = 0;

        while (++u16Timeout)
        {
            __nop();
            __nop();

            if (PIN_ReadLevel(&pHandle->STATUS) != PIN_LEVEL_LOW)
            {
                // success
                goto __prog;  // ↓
            }
        }

    } while (u8Retries--);

    LOGD("fail\n");
    return ERR_FAIL;

__prog:

    DelayBlockUs(30);

    // burn it

    for (uint32_t i = 0; i < u32Size; ++i)
    {
        uint8_t u8Data = cpu8Buffer[i];

        for (uint8_t j = 0; j < 8; ++j)
        {
            if (eBitOrder == MSBFirst)
            {
                PIN_WriteLevel(&pHandle->DATA, (u8Data & 0x80) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);
                u8Data <<= 1;
            }
            else if (eBitOrder == LSBFirst)
            {
                PIN_WriteLevel(&pHandle->DATA, (u8Data & 0x01) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);
                u8Data >>= 1;
            }

            GenerateClockPulse(pHandle);
        }
    }

    // check status

    PIN_WriteLevel(&pHandle->DATA, PIN_LEVEL_LOW);

    u16Timeout = 0;

    while (++u16Timeout)
    {
        // provide data clock pulse

        u8Pulse = (PIN_ReadLevel(&pHandle->DONE) != PIN_LEVEL_LOW) ? 2 : 10;

        while (u8Pulse--)
        {
            GenerateClockPulse(pHandle);
        }

        if (PIN_ReadLevel(&pHandle->DONE) != PIN_LEVEL_LOW)
        {
            // success
            goto __eof;  // ↓
        }
    }

    // fail, reprog
    goto __sof;  // ↑

__eof:

    DelayBlockUs(710);
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_LOW);
    DelayBlockMs(20);
    PIN_WriteLevel(&pHandle->RST, PIN_LEVEL_HIGH);

    LOGD("success");
    return ERR_NONE;
}
