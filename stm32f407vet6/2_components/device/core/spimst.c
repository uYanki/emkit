#include "spimst.h"
#include <string.h>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "spimst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void SPI_Master_Select(spi_mst_t* pHandle)
{
    switch (pHandle->u16TimingConfig & SPI_FLAG_CS_MODE_Msk)
    {
        case SPI_FLAG_SOFT_CS:
        {
            switch (pHandle->u16TimingConfig & SPI_FLAG_CS_ACTIVE_LEVEL_Msk)
            {
                default:
                case SPI_FLAG_CS_ACTIVE_LOW:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_LOW);
                    break;
                }
                case SPI_FLAG_CS_ACTIVE_HIGH:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_HIGH);
                    break;
                }
            }

            break;
        }
        case SPI_FLAG_HADR_CS:
        case SPI_FLAG_NONE_CS:
        {
            break;
        }
    }
}

void SPI_Master_Deselect(spi_mst_t* pHandle)
{
    switch (pHandle->u16TimingConfig & SPI_FLAG_CS_MODE_Msk)
    {
        case SPI_FLAG_SOFT_CS:
        {
            switch (pHandle->u16TimingConfig & SPI_FLAG_CS_ACTIVE_LEVEL_Msk)
            {
                default:
                case SPI_FLAG_CS_ACTIVE_LOW:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_HIGH);
                    break;
                }
                case SPI_FLAG_CS_ACTIVE_HIGH:
                {
                    PIN_WriteLevel(&pHandle->CS, PIN_LEVEL_LOW);
                    break;
                }
            }

            break;
        }
        case SPI_FLAG_HADR_CS:
        case SPI_FLAG_NONE_CS:
        {
            break;
        }
    }
}

status_t SPI_Master_Init(spi_mst_t* pHandle, u32 u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, u16 u16Flags)
{
    pHandle->u16TimingConfig |= u16Flags & (SPI_FLAG_CS_MODE_Msk | SPI_FLAG_CS_ACTIVE_LEVEL_Msk | SPI_FLAG_WIRE_Msk);

    if ((pHandle->u16TimingConfig & SPI_FLAG_WIRE_Msk) == SPI_FLAG_4WIRE)
    {
        if (memcmp(&pHandle->MOSI, &pHandle->MISO, sizeof(pin_t)) == 0)  // same
        {
            return ERR_NOT_ALLOWED;  // MOSI must be different from MISO
        }
    }

    if (pHandle->SPIx == nullptr)
    {
#if CONFIG_SWSPI_MODULE_SW
        extern const spimst_ops_t g_swSpiOps;
        pHandle->pOps = &g_swSpiOps;
#else
        return ERR_NO_DEVICE;  // swspi master module is disabled
#endif
    }
    else
    {
#if CONFIG_HWSPI_MODULE_SW
        extern const spimst_ops_t g_hwSpiOps;
        pHandle->pOps = &g_hwSpiOps;
#else
        return ERR_NO_DEVICE;  // hwspi master module is disabled
#endif
    }

    if ((pHandle->u16TimingConfig & SPI_FLAG_CS_MODE_Msk) == SPI_FLAG_SOFT_CS)
    {
        if ((pHandle->u16TimingConfig & SPI_FLAG_CS_ACTIVE_LEVEL_Msk) == SPI_FLAG_CS_ACTIVE_LOW)
        {
            PIN_SetMode(&pHandle->CS, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
        }
        else  // SPI_FLAG_CS_ACTIVE_HIGH
        {
            PIN_SetMode(&pHandle->CS, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_DOWN);
        }

        SPI_Master_Deselect(pHandle);
    }

    return pHandle->pOps->Init(pHandle, u32ClockSpeedHz, eDutyCycle, u16Flags);
}

status_t SPI_Master_TransmitBlock(spi_mst_t* pHandle, const u8* cpu8TxData, u16 u16Size)
{
    return pHandle->pOps->TransmitBlock(pHandle, cpu8TxData, u16Size);
}

status_t SPI_Master_ReceiveBlock(spi_mst_t* pHandle, u8* pu8RxData, u16 u16Size)
{
    return pHandle->pOps->ReceiveBlock(pHandle, pu8RxData, u16Size);
}

status_t SPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const u8* cpu8TxData, u8* pu8RxData, u16 u16Size)
{
    return pHandle->pOps->TransmitReceiveBlock(pHandle, cpu8TxData, pu8RxData, u16Size);
}

status_t SPI_Master_TransmitByte(spi_mst_t* pHandle, u8 u8TxData)
{
    return SPI_Master_TransmitBlock(pHandle, (const u8*)&u8TxData, 1);
}

status_t SPI_Master_ReceiveByte(spi_mst_t* pHandle, u8* pu8RxData)
{
    return SPI_Master_ReceiveBlock(pHandle, pu8RxData, 1);
}

status_t SPI_Master_TransmitReceiveByte(spi_mst_t* pHandle, u8 u8TxData, u8* pu8RxData)
{
    return SPI_Master_TransmitReceiveBlock(pHandle, (const u8*)&u8TxData, pu8RxData, 1);
}
