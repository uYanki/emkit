#include "i2cmst.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "i2cmst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t I2C_Master_Init(i2c_mst_t* pHandle, u32 u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle)
{
    if (pHandle->I2Cx == nullptr)
    {
#if CONFIG_SWI2C_MODULE_SW
        extern const i2cmst_ops_t g_swI2cOps;
        pHandle->pOps = &g_swI2cOps;
#else
        return ERR_NO_DEVICE;  // swi2c master module is disabled
#endif
    }
    else
    {
#if CONFIG_HWI2C_MODULE_SW
        extern const i2cmst_ops_t g_hwI2cOps;
        pHandle->pOps = &g_hwI2cOps;
#else
        return ERR_NO_DEVICE;  // hwi2c master module is disabled
#endif
    }

    return pHandle->pOps->Init(pHandle, u32ClockFreqHz, eDutyCycle);
}

bool I2C_Master_IsDeviceReady(i2c_mst_t* pHandle, u8 u16SlvAddr, u16 u16Flags)
{
    return pHandle->pOps->IsDeviceReady(pHandle, u16SlvAddr, u16Flags);
}

status_t I2C_Master_TransmitBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, const u8* cpu8TxData, u16 u16Size, u16 u16Flags)
{
    return pHandle->pOps->TransmitBlock(pHandle, u16SlvAddr, cpu8TxData, u16Size, u16Flags);
}

status_t I2C_Master_ReceiveBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u8* pu8RxData, u16 u16Size, u16 u16Flags)
{
    return pHandle->pOps->ReceiveBlock(pHandle, u16SlvAddr, pu8RxData, u16Size, u16Flags);
}

status_t I2C_Master_ReadBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8* pu8Data, u16 u16Size, u16 u16Flags)
{
    return pHandle->pOps->ReadBlock(pHandle, u16SlvAddr, u16MemAddr, pu8Data, u16Size, u16Flags);
}

status_t I2C_Master_WriteBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, const u8* cpu8Data, u16 u16Size, u16 u16Flags)
{
    return pHandle->pOps->WriteBlock(pHandle, u16SlvAddr, u16MemAddr, cpu8Data, u16Size, u16Flags);
}

status_t I2C_Master_TransmitByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u8 u8TxData, u16 u16Flags)
{
    return I2C_Master_TransmitBlock(pHandle, u16SlvAddr, &u8TxData, 1, u16Flags);
}

status_t I2C_Master_ReceiveByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u8* pu8RxData, u16 u16Flags)
{
    return I2C_Master_ReceiveBlock(pHandle, u16SlvAddr, pu8RxData, 1, u16Flags);
}

status_t I2C_Master_WriteByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8Data, u16 u16Flags)
{
    return I2C_Master_WriteBlock(pHandle, u16SlvAddr, u16MemAddr, &u8Data, 1, u16Flags);
}

status_t I2C_Master_ReadByte(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8* pu8Data, u16 u16Flags)
{
    return I2C_Master_ReadBlock(pHandle, u16SlvAddr, u16MemAddr, pu8Data, 1, u16Flags);
}

status_t I2C_Master_TransmitWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16Data, u16 u16Flags)
{
    u8 au8Data[2];

    switch (u16Flags & I2C_FLAG_WORD_ENDIAN_Msk)
    {
        default:
        case I2C_FLAG_WORD_BIG_ENDIAN:
        {
            au8Data[0] = (u16Data >> 8) & 0xFF;
            au8Data[1] = u16Data & 0xFF;
            break;
        }
        case I2C_FLAG_WORD_LITTLE_ENDIAN:
        {
            au8Data[0] = u16Data & 0xFF;
            au8Data[1] = (u16Data >> 8) & 0xFF;
            break;
        }
    }

    return I2C_Master_TransmitBlock(pHandle, u16SlvAddr, au8Data, ARRAY_SIZE(au8Data), u16Flags);
}

status_t I2C_Master_ReceiveWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16* pu16Data, u16 u16Flags)
{
    u8 au8Data[2];

    ERRCHK_RETURN(I2C_Master_ReceiveBlock(pHandle, u16SlvAddr, au8Data, ARRAY_SIZE(au8Data), u16Flags));

    switch (u16Flags & I2C_FLAG_WORD_ENDIAN_Msk)
    {
        default:
        case I2C_FLAG_WORD_BIG_ENDIAN:
        {
            *pu16Data = ((u16)au8Data[0] << 8) | (u16)au8Data[1];
            break;
        }
        case I2C_FLAG_WORD_LITTLE_ENDIAN:
        {
            *pu16Data = (u16)au8Data[0] | ((u16)au8Data[1] << 8);
            break;
        }
    }

    return ERR_NONE;
}

status_t I2C_Master_WriteWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u16 u16Data, u16 u16Flags)
{
    u8 au8Data[2];

    switch (u16Flags & I2C_FLAG_WORD_ENDIAN_Msk)
    {
        default:
        case I2C_FLAG_WORD_BIG_ENDIAN:
        {
            au8Data[0] = (u16Data >> 8) & 0xFF;
            au8Data[1] = u16Data & 0xFF;
            break;
        }
        case I2C_FLAG_WORD_LITTLE_ENDIAN:
        {
            au8Data[0] = u16Data & 0xFF;
            au8Data[1] = (u16Data >> 8) & 0xFF;
            break;
        }
    }

    return I2C_Master_WriteBlock(pHandle, u16SlvAddr, u16MemAddr, au8Data, ARRAY_SIZE(au8Data), u16Flags);
}

status_t I2C_Master_ReadWord(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u16* pu16Data, u16 u16Flags)
{
    u8 au8Data[2];

    ERRCHK_RETURN(I2C_Master_ReadBlock(pHandle, u16SlvAddr, u16MemAddr, au8Data, ARRAY_SIZE(au8Data), u16Flags));

    switch (u16Flags & I2C_FLAG_WORD_ENDIAN_Msk)
    {
        default:
        case I2C_FLAG_WORD_BIG_ENDIAN:
        {
            *pu16Data = ((u16)au8Data[0] << 8) | (u16)au8Data[1];
            break;
        }
        case I2C_FLAG_WORD_LITTLE_ENDIAN:
        {
            *pu16Data = (u16)au8Data[0] | ((u16)au8Data[1] << 8);
            break;
        }
    }

    return ERR_NONE;
}

status_t I2C_Master_WriteByteBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u8 u8BitsValue, u16 u16Flags)
{
    u8 u8Data    = 0x00;
    u8 u8BitMask = BITMASK8(u8StartBit, u8BitsCount);

    if ((u8StartBit + u8BitsCount) > 8)
    {
        return ERR_OVERFLOW;  // termination bit overflow
    }

    ERRCHK_RETURN(I2C_Master_ReadBlock(pHandle, u16SlvAddr, u16MemAddr, &u8Data, sizeof(u8Data), u16Flags));

    u8BitsValue <<= u8StartBit;
    u8Data &= ~u8BitMask;
    u8Data |= u8BitMask & u8BitsValue;

    ERRCHK_RETURN(I2C_Master_WriteBlock(pHandle, u16SlvAddr, u16MemAddr, &u8Data, sizeof(u8Data), u16Flags));

    return ERR_NONE;
}

status_t I2C_Master_ReadByteBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u8* pu8BitsValue, u16 u16Flags)
{
    u8 u8Data    = 0x00;
    u8 u8BitMask = BITMASK8(u8StartBit, u8BitsCount);

    if ((u8StartBit + u8BitsCount) > 8)
    {
        return ERR_OVERFLOW;  // termination bit overflow
    }

    ERRCHK_RETURN(I2C_Master_ReadBlock(pHandle, u16SlvAddr, u16MemAddr, &u8Data, sizeof(u8Data), u16Flags));

    *pu8BitsValue = (u8BitMask & u8Data) >> u8StartBit;

    return ERR_NONE;
}

status_t I2C_Master_ReadWordBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u16* pu16BitsValue, u16 u16Flags)
{
    u16 u16Data    = 0x0000;
    u16 u16BitMask = BITMASK16(u8StartBit, u8BitsCount);

    if ((u8StartBit + u8BitsCount) > 16)
    {
        return ERR_OVERFLOW;  // termination bit overflow
    }

    ERRCHK_RETURN(I2C_Master_ReadWord(pHandle, u16SlvAddr, u16MemAddr, &u16Data, u16Flags));

    *pu16BitsValue = (u16BitMask & u16Data) >> u8StartBit;

    return ERR_NONE;
}

status_t I2C_Master_WriteWordBits(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8 u8StartBit, u8 u8BitsCount, u16 u16BitsValue, u16 u16Flags)
{
    u16 u16Data    = 0x0000;
    u16 u16BitMask = BITMASK16(u8StartBit, u8BitsCount);

    if ((u8StartBit + u8BitsCount) > 16)
    {
        return ERR_OVERFLOW;  // termination bit overflow
    }

    ERRCHK_RETURN(I2C_Master_ReadWord(pHandle, u16SlvAddr, u16MemAddr, &u16Data, u16Flags));

    u16BitsValue <<= u8StartBit;
    u16Data &= ~u16BitMask;
    u16Data |= u16BitMask & u16BitsValue;

    ERRCHK_RETURN(I2C_Master_WriteWord(pHandle, u16SlvAddr, u16MemAddr, u16Data, u16Flags));

    return ERR_NONE;
}

/**
 * @return count of detected slaves
 */
u8 I2C_Master_ScanAddress(i2c_mst_t* pHandle)
{
    u8  u8SlvAddr = 0, u8Step = 0, u8Count = 0;
    u16 u16Flags = 0;

    PRINTLN("i2c 7-bit slave address detector:");
    PRINTLN("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");

    while (u8SlvAddr < 0x7F)
    {
        PRINTF("%02X: ", u8SlvAddr);

        u8Step = 0x10;

        do
        {
            if (I2C_Master_IsDeviceReady(pHandle, u8SlvAddr, u16Flags) == true)
            {
                PRINTF("%02X ", u8SlvAddr);
                u8Count++;
            }
            else
            {
                PRINTF("-- ");  // none
            }

            ++u8SlvAddr;

        } while (--u8Step);

        PRINTLN();
    }

    PRINTLN(">> %d slaves are detected in this scan", u8Count);

    return u8Count;
}

status_t I2C_Master_Hexdump(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16Flags)
{
    u8 u8MemAddr = 0;
    u8 u8Step    = 0;

    PRINTLN("i2c 7-bit slave memory hexdump:");

    do
    {
        PRINTF("%02X: ", u8MemAddr);

        u8Step = 0x10;

        do
        {
            // 有些设备不支持连读，这里就单个单个读

            switch (u16Flags & I2C_FLAG_MEMUNIT_SIZE_Msk)
            {
                default:
                case I2C_FLAG_MEMUNIT_8BIT:
                {
                    u8 u8Data;

                    if (I2C_Master_ReadByte(pHandle, u16SlvAddr, u8MemAddr, &u8Data, u16Flags) == ERR_NONE)
                    {
                        PRINTF("%02X ", u8Data);
                    }
                    else
                    {
                        PRINTF("-- ");  // none
                    }

                    break;
                }
                case I2C_FLAG_MEMUNIT_16BIT:
                {
                    u16 u16Data;

                    if (I2C_Master_ReadWord(pHandle, u16SlvAddr, u8MemAddr, &u16Data, u16Flags) == ERR_NONE)
                    {
                        PRINTF("%04X ", u16Data);
                    }
                    else
                    {
                        PRINTF("-- ");  // none
                    }

                    break;
                }
            }

            ++u8MemAddr;

        } while (--u8Step);

        PRINTLN();

    } while (u8MemAddr);

    return ERR_NONE;
}
