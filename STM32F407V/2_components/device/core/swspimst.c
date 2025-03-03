#include "spidefs.h"

#if CONFIG_SWSPI_MODULE_SW

#include <string.h>
#include "delay.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "swspimst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static void SWSPI_Master_SetClockIdle(spi_mst_t* pHandle);
static u8   SWSPI_Master_ShiftIn(spi_mst_t* pHandle);
static void SWSPI_Master_ShiftOut(spi_mst_t* pHandle, u8 u8TxData);
static u8   SWSPI_Master_ShiftInOut3Wire(spi_mst_t* pHandle, u8 u8TxData);
static u8   SWSPI_Master_ShiftInOut4Wire(spi_mst_t* pHandle, u8 u8TxData);

static status_t SWSPI_Master_Init(spi_mst_t* pHandle, u32 u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, u16 u16Flags);
static status_t SWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const u8* cpu8TxData, u16 u16Size);
static status_t SWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, u8* pu8RxData, u16 u16Size);
static status_t SWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const u8* cpu8TxData, u8* pu8RxData, u16 u16Size);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const spimst_ops_t g_swSpiOps = {
    .Init                 = SWSPI_Master_Init,
    .TransmitBlock        = SWSPI_Master_TransmitBlock,
    .ReceiveBlock         = SWSPI_Master_ReceiveBlock,
    .TransmitReceiveBlock = SWSPI_Master_TransmitReceiveBlock,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static status_t SWSPI_Master_SetClock(spi_mst_t* pHandle, u32 u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle)
{
    u32 u32ClockCycleUs;

    ASSERT(u32ClockSpeedHz, "clock freq must greater than 0");

    u32ClockCycleUs           = 1000000UL / u32ClockSpeedHz;
    pHandle->u32FirstCycleUs  = (u32ClockCycleUs * ((u32)eDutyCycle + 1)) >> 8;
    pHandle->u32SecondCycleUs = u32ClockCycleUs - pHandle->u32FirstCycleUs;

    return ERR_NONE;
}

static void SWSPI_Master_SetClockIdle(spi_mst_t* pHandle)
{
    // clock steady state

    switch (pHandle->u16TimingConfig & SPI_FLAG_CPOL_Msk)
    {
        default:
        case SPI_FLAG_CPOL_LOW:
        {
            PIN_WriteLevel(&pHandle->SCLK, PIN_LEVEL_LOW);
            break;
        }
        case SPI_FLAG_CPOL_HIGH:
        {
            PIN_WriteLevel(&pHandle->SCLK, PIN_LEVEL_HIGH);
            break;
        }
    }
}

/**
 * @brief msbfirst
 */
static u8 SWSPI_Master_ShiftIn(spi_mst_t* pHandle)
{
    u8 u8RxData = 0x00, u8Mask = 0x80;

    switch (pHandle->u16TimingConfig & SPI_FLAG_CPHA_Msk)
    {
        default:
        case SPI_FLAG_CPHA_1EDGE:
        {
            do
            {  // first edge

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

                if (PIN_ReadLevel(&pHandle->MISO) == PIN_LEVEL_HIGH)
                {
                    u8RxData |= u8Mask;
                }

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

            } while (u8Mask >>= 1);

            break;
        }
        case SPI_FLAG_CPHA_2EDGE:
        {
            do
            {  // second edge

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

                if (PIN_ReadLevel(&pHandle->MISO) == PIN_LEVEL_HIGH)
                {
                    u8RxData |= u8Mask;
                }

            } while (u8Mask >>= 1);

            break;
        }
    }

    return u8RxData;
}

/**
 * @brief msbfirst
 */
static void SWSPI_Master_ShiftOut(spi_mst_t* pHandle, u8 u8TxData)
{
    u8 u8Mask = 0x80;

    switch (pHandle->u16TimingConfig & SPI_FLAG_CPHA_Msk)
    {
        default:
        case SPI_FLAG_CPHA_1EDGE:
        {
            do
            {  // first edge

                PIN_WriteLevel(&pHandle->MOSI, (u8TxData & u8Mask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

            } while (u8Mask >>= 1);

            break;
        }
        case SPI_FLAG_CPHA_2EDGE:
        {
            do
            {  // second edge

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_WriteLevel(&pHandle->MOSI, (u8TxData & u8Mask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

            } while (u8Mask >>= 1);

            break;
        }
    }
}

/**
 * @brief msbfirst
 */
static u8 SWSPI_Master_ShiftInOut3Wire(spi_mst_t* pHandle, u8 u8TxData)
{
    u8 u8RxData = 0x00, u8Mask = 0x80;

    switch (pHandle->u16TimingConfig & SPI_FLAG_CPHA_Msk)
    {
        case SPI_FLAG_CPHA_1EDGE:
        {
            do
            {  // first edge

                PIN_SetMode(&pHandle->MOSI, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
                PIN_WriteLevel(&pHandle->MOSI, (u8TxData & u8Mask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

                PIN_SetMode(&pHandle->MISO, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
                if (PIN_ReadLevel(&pHandle->MISO) == PIN_LEVEL_HIGH)
                {
                    u8RxData |= u8Mask;
                }

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

            } while (u8Mask >>= 1);

            break;
        }
        case SPI_FLAG_CPHA_2EDGE:
        {
            do
            {  // second edge

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_SetMode(&pHandle->MOSI, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
                PIN_WriteLevel(&pHandle->MOSI, (u8TxData & u8Mask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

                PIN_SetMode(&pHandle->MISO, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
                if (PIN_ReadLevel(&pHandle->MISO) == PIN_LEVEL_HIGH)
                {
                    u8RxData |= u8Mask;
                }

            } while (u8Mask >>= 1);

            break;
        }

        default: break;
    }

    return u8RxData;
}

/**
 * @brief lsbfirst
 */
static u8 SWSPI_Master_ShiftInOut4Wire(spi_mst_t* pHandle, u8 u8TxData)
{
    u8 u8RxData = 0x00, u8Mask = 0x80;

    switch (pHandle->u16TimingConfig & SPI_FLAG_CPHA_Msk)
    {
        case SPI_FLAG_CPHA_1EDGE:
        {
            do
            {  // first edge

                PIN_WriteLevel(&pHandle->MOSI, (u8TxData & u8Mask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

                if (PIN_ReadLevel(&pHandle->MISO) == PIN_LEVEL_HIGH)
                {
                    u8RxData |= u8Mask;
                }

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

                PIN_ToggleLevel(&pHandle->SCLK);

            } while (u8Mask >>= 1);

            break;
        }
        case SPI_FLAG_CPHA_2EDGE:
        {
            do
            {  // second edge

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32FirstCycleUs);
                }

                PIN_WriteLevel(&pHandle->MOSI, (u8TxData & u8Mask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);

                PIN_ToggleLevel(&pHandle->SCLK);

                if ((pHandle->u16TimingConfig & SPI_FLAG_FAST_CLOCK_Msk) == SPI_FLAG_FAST_CLOCK_DISABLE)
                {
                    DelayBlockUs(pHandle->u32SecondCycleUs);
                }

                if (PIN_ReadLevel(&pHandle->MISO) == PIN_LEVEL_HIGH)
                {
                    u8RxData |= u8Mask;
                }

            } while (u8Mask >>= 1);

            break;
        }
        default: break;
    }

    return u8RxData;
}

static status_t SWSPI_Master_Init(spi_mst_t* pHandle, u32 u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, u16 u16Flags)
{
    pHandle->u16TimingConfig |= u16Flags & (SPI_FLAG_CPOL_Msk | SPI_FLAG_WIRE_Msk | SPI_FLAG_CPHA_Msk | SPI_FLAG_FIRSTBIT_Msk | SPI_FLAG_DATAWIDTH_Msk | SPI_FLAG_FAST_CLOCK_Msk);

    switch (pHandle->u16TimingConfig & SPI_FLAG_WIRE_Msk)
    {
        case SPI_FLAG_3WIRE:
        {
            PIN_SetMode(&pHandle->MOSI, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
            PIN_SetMode(&pHandle->SCLK, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
            break;
        }
        case SPI_FLAG_4WIRE:
        {
            PIN_SetMode(&pHandle->MISO, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
            PIN_SetMode(&pHandle->MOSI, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
            PIN_SetMode(&pHandle->SCLK, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
            break;
        }
    }

    ERRCHK_RETURN(SWSPI_Master_SetClock(pHandle, u32ClockSpeedHz, eDutyCycle));

    return ERR_NONE;
}

static status_t SWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const u8* cpu8TxData, u16 u16Size)
{
    bool bLsbfirst = (pHandle->u16TimingConfig & SPI_FLAG_FIRSTBIT_Msk) == SPI_FLAG_LSBFIRST;

    PIN_SetMode(&pHandle->MOSI, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);  // for 3wire

    SWSPI_Master_SetClockIdle(pHandle);  // clock steady state

    switch (pHandle->u16TimingConfig & SPI_FLAG_DATAWIDTH_Msk)
    {
        default:
        case SPI_FLAG_DATAWIDTH_8B:
        {
            u8 u8TxData;

            while (u16Size--)
            {
                u8TxData = *cpu8TxData;

                if (bLsbfirst)
                {
                    u8TxData = revbit8(u8TxData);
                }

                SWSPI_Master_ShiftOut(pHandle, u8TxData);

                cpu8TxData += 1;
            }

            break;
        }
        case SPI_FLAG_DATAWIDTH_16B:
        {
            u16 u16TxData;

            while (u16Size--)
            {
                u16TxData = he16(cpu8TxData);

                if (bLsbfirst)
                {
                    u16TxData = revbit16(u16TxData);
                }

                SWSPI_Master_ShiftOut(pHandle, 0xFF & (u16TxData >> 8));
                SWSPI_Master_ShiftOut(pHandle, 0xFF & (u16TxData >> 0));

                cpu8TxData += 2;
            }

            break;
        }
        case SPI_FLAG_DATAWIDTH_32B:
        {
            u32 u32TxData;

            while (u16Size--)
            {
                u32TxData = he32(cpu8TxData);

                if (bLsbfirst)
                {
                    u32TxData = revbit32(u32TxData);
                }

                SWSPI_Master_ShiftOut(pHandle, 0xFF & (u32TxData >> 24));
                SWSPI_Master_ShiftOut(pHandle, 0xFF & (u32TxData >> 16));
                SWSPI_Master_ShiftOut(pHandle, 0xFF & (u32TxData >> 8));
                SWSPI_Master_ShiftOut(pHandle, 0xFF & (u32TxData >> 0));

                cpu8TxData += 4;
            }

            break;
        }
    }

    return ERR_NONE;
}

static status_t SWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, u8* pu8RxData, u16 u16Size)
{
    bool bLsbfirst = (pHandle->u16TimingConfig & SPI_FLAG_FIRSTBIT_Msk) == SPI_FLAG_LSBFIRST;

    PIN_SetMode(&pHandle->MISO, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);  // for 3wire

    SWSPI_Master_SetClockIdle(pHandle);  // clock steady state

    switch (pHandle->u16TimingConfig & SPI_FLAG_DATAWIDTH_Msk)
    {
        default:
        case SPI_FLAG_DATAWIDTH_8B:
        {
            u8 u8RxData;

            while (u16Size--)
            {
                u8RxData = SWSPI_Master_ShiftIn(pHandle);

                if (bLsbfirst)
                {
                    u8RxData = revbit8(u8RxData);
                }

                *pu8RxData++ = u8RxData;
            }

            break;
        }
        case SPI_FLAG_DATAWIDTH_16B:
        {
            u16 u16RxData;

            while (u16Size--)
            {
                u16RxData = SWSPI_Master_ShiftIn(pHandle);

                if (bLsbfirst)
                {
                    u16RxData = revbit16(u16RxData);
                }

                memcpy((void*)*pu8RxData, (void*)&u16RxData, sizeof(u16));

                pu8RxData += 2;
            }

            break;
        }
        case SPI_FLAG_DATAWIDTH_32B:
        {
            u32 u32RxData;

            while (u16Size--)
            {
                u32RxData = SWSPI_Master_ShiftIn(pHandle);

                if (bLsbfirst)
                {
                    u32RxData = revbit32(u32RxData);
                }

                memcpy((void*)*pu8RxData, (void*)&u32RxData, sizeof(u32));

                pu8RxData += 4;
            }

            break;
        }
    }

    return ERR_NONE;
}

static status_t SWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const u8* cpu8TxData, u8* pu8RxData, u16 u16Size)
{
    bool bLsbfirst = (pHandle->u16TimingConfig & SPI_FLAG_FIRSTBIT_Msk) == SPI_FLAG_LSBFIRST;

    SWSPI_Master_SetClockIdle(pHandle);  // clock steady state

    switch (pHandle->u16TimingConfig & SPI_FLAG_DATAWIDTH_Msk)
    {
        default:
        case SPI_FLAG_DATAWIDTH_8B:
        {
            u8 u8TxData, u8RxData;

            switch (pHandle->u16TimingConfig & SPI_FLAG_WIRE_Msk)
            {
                default:
                case SPI_FLAG_3WIRE:
                {
                    while (u16Size--)
                    {
                        u8TxData = *cpu8TxData++;

                        if (bLsbfirst)
                        {
                            u8TxData = revbit8(u8TxData);
                        }

                        u8RxData = SWSPI_Master_ShiftInOut3Wire(pHandle, u8TxData);

                        if (bLsbfirst)
                        {
                            u8RxData = revbit8(u8RxData);
                        }

                        *pu8RxData++ = u8RxData;
                    }

                    break;
                }
                case SPI_FLAG_4WIRE:
                {
                    while (u16Size--)
                    {
                        u8TxData = *cpu8TxData++;

                        if (bLsbfirst)
                        {
                            u8TxData = revbit8(u8TxData);
                        }

                        u8RxData = SWSPI_Master_ShiftInOut4Wire(pHandle, u8TxData);

                        if (bLsbfirst)
                        {
                            u8RxData = revbit8(u8RxData);
                        }

                        *pu8RxData++ = u8RxData;
                    }

                    break;
                }
            }

            break;
        }
        case SPI_FLAG_DATAWIDTH_16B:
        {
            break;
        }
        case SPI_FLAG_DATAWIDTH_32B:
        {
            break;
        }
    }

    return ERR_NONE;
}

#endif  // CONFIG_SWSPI_MODULE_SW
