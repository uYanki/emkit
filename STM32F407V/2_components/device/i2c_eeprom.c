#include "i2c_eeprom.h"
#include "hexdump.h"
#include "delay.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "eeprom"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE  // LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

#define EEPROM_TIMEOUT_MS 40

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t EEPROM_Init(i2c_eeprom_t* pHandle)
{
    return ERR_NONE;
}

static bool EEPROM_WaitReady(i2c_eeprom_t* pHandle, uint16_t u16SlvAddr, uint8_t u8TimeoutMs)
{
    while (u8TimeoutMs > 0)
    {
        if (I2C_Master_IsDeviceReady(pHandle->hI2C, u16SlvAddr, I2C_FLAG_SLVADDR_7BIT) == true)
        {
            return true;
        }

        DelayBlockMs(1);

        u8TimeoutMs--;
    }

    LOGD("timeout");

    return false;
}

status_t EEPROM_WriteBlock(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, const uint8_t* cpu8Buffer, uint16_t u16Size)
{
#if CONFIG_EEPROM_ADDRESS_OVERFLOW_CHECK_SW

    if ((u32MemAddr + (uint32_t)u16Size) > (uint32_t)(pHandle->eCapacity))
    {
        return ERR_OVERFLOW;  // memory address out of capacity
    }

#endif

    uint16_t u16PageSize;

    // clang-format off
    switch (pHandle->eCapacity)
    {
        case AT24C01:  u16PageSize = 8  ; break;
        case AT24C02:  u16PageSize = 8  ; break;
        case AT24C04:  u16PageSize = 16 ; break;
        case AT24C08:  u16PageSize = 16 ; break;
        case AT24C16:  u16PageSize = 16 ; break;
        case AT24C32:  u16PageSize = 32 ; break;
        case AT24C64:  u16PageSize = 32 ; break;
        case AT24C128: u16PageSize = 64 ; break;
        case AT24C256: u16PageSize = 64 ; break;
        case AT24C512: u16PageSize = 128; break;
        case AT24CM01: u16PageSize = 256; break;
        case AT24CM02: u16PageSize = 256; break;
        default:
        {
            return ERR_INVALID_VALUE; // capacity is unsupported
        }
    }

    // clang-format on

    uint16_t u16SlvAddr;
    uint16_t u16MemAddr;
    uint16_t u16Flags;

    uint16_t u16XferSize = u16PageSize - (u32MemAddr % u16PageSize);  // front

    if (u16XferSize > u16Size)
    {
        u16XferSize = u16Size;
    }

    while (u16Size > 0)
    {
        if (pHandle->eCapacity < AT24C16)
        {
            u16SlvAddr = pHandle->u8SlvAddr;
            u16SlvAddr |= (u32MemAddr >> 8) & 0x07;
            u16MemAddr = u32MemAddr & 0x000000FF;
            u16Flags   = I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT;
        }
        else  // u8MemAddrSize == 16
        {
            u16SlvAddr = pHandle->u8SlvAddr;
            u16SlvAddr |= (u32MemAddr >> 16) & 0x07;
            u16MemAddr = u32MemAddr & 0x0000FFFF;
            u16Flags   = I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_16BIT;
        }

        // wait for ready
        if (EEPROM_WaitReady(pHandle, u16SlvAddr, EEPROM_TIMEOUT_MS) == false)
        {
            return ERR_TIMEOUT;  // eeprom doesn't ready
        }

        ERRCHK_RETURN(I2C_Master_WriteBlock(pHandle->hI2C, u16SlvAddr, u16MemAddr, cpu8Buffer, u16XferSize, u16Flags));

        u32MemAddr += (uint32_t)u16XferSize;
        cpu8Buffer += u16XferSize;
        u16Size -= u16XferSize;

        if (u16Size >= u16PageSize)  // middle
        {
            u16XferSize = u16PageSize;
        }
        else  // back
        {
            u16XferSize = u16Size;
        }
    }

    return ERR_NONE;
}

status_t EEPROM_ReadBlock(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, uint8_t* pu8Buffer, uint16_t u16Size)
{
#if CONFIG_EEPROM_ADDRESS_OVERFLOW_CHECK_SW

    if ((u32MemAddr + (uint32_t)u16Size) > (uint32_t)(pHandle->eCapacity))
    {
        return ERR_OVERFLOW;  // emory address out of capacity
    }

#endif

    uint16_t u16SlvAddr;
    uint16_t u16MemAddr;
    uint16_t u16Flags;

    if (pHandle->eCapacity < AT24C16)
    {
        u16SlvAddr = pHandle->u8SlvAddr;
        u16SlvAddr |= (u32MemAddr >> 8) & 0x07;
        u16MemAddr = u32MemAddr & 0x000000FF;
        u16Flags   = I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT;
    }
    else
    {
        u16SlvAddr = pHandle->u8SlvAddr;
        u16SlvAddr |= (u32MemAddr >> 16) & 0x07;
        u16MemAddr = u32MemAddr & 0x0000FFFF;
        u16Flags   = I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_16BIT;
    }

    // wait for ready
    if (EEPROM_WaitReady(pHandle, u16SlvAddr, EEPROM_TIMEOUT_MS) == false)
    {
        return ERR_TIMEOUT;  // eeprom doesn't ready
    }

    return I2C_Master_ReadBlock(pHandle->hI2C, u16SlvAddr, u16MemAddr, pu8Buffer, u16Size, u16Flags);
}

status_t EEPROM_FillByte(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, uint16_t u16Size, uint8_t u8Data)
{
    uint8_t  au8Buff[16];  // 16x
    uint16_t u16XferSize;

    ASSERT(u16Size > 0, "illegal param");

    LOGI("address = 0x%08X, size = %d bytes", u32MemAddr, u16Size);

    for (uint8_t i = 0; i < ARRAY_SIZE(au8Buff); i++)
    {
        au8Buff[i] = u8Data;
    }

    while (u16Size > 0)
    {
        u16XferSize = MIN(u16Size, ARRAY_SIZE(au8Buff));

        ERRCHK_RETURN(EEPROM_WriteBlock(pHandle, u32MemAddr, &au8Buff[0], u16XferSize));

        u32MemAddr += u16XferSize;
        u16Size -= u16XferSize;
    }

    return ERR_NONE;
}

status_t EEPROM_Hexdump(i2c_eeprom_t* pHandle, uint32_t u32MemAddr, uint16_t u16Size)
{
    uint8_t  au8Buff[16] = {0};  // 16x
    uint16_t u16XferSize;

    ASSERT(u16Size > 0, "illegal param");

    LOGI("address = 0x%08X, size = %d bytes", u32MemAddr, u16Size);

    while (u16Size > 0)
    {
        u16XferSize = MIN(u16Size, ARRAY_SIZE(au8Buff));

        ERRCHK_RETURN(EEPROM_ReadBlock(pHandle, u32MemAddr, &au8Buff[0], u16XferSize));
        hexdump(&au8Buff[0], u16XferSize, u16XferSize, 1, true, nullptr, u32MemAddr);

        u32MemAddr += u16XferSize;
        u16Size -= u16XferSize;
    }

    return ERR_NONE;
}

status_t EEPROM_DetectCapacity(i2c_eeprom_t* pHandle)
{
    uint16_t u16SlvAddr = pHandle->u8SlvAddr;

    if (EEPROM_WaitReady(pHandle, u16SlvAddr, EEPROM_TIMEOUT_MS) == false)
    {
        return ERR_NO_DEVICE;  // eeprom doesn't exist
    }

    bool bAckPre = true;

    for (uint32_t u32MemAddr = AT24C01; u32MemAddr <= AT24CM02 /* 2Mbit*/; u32MemAddr <<= 1)
    {
        if (u32MemAddr < AT24C16)
        {
            u16SlvAddr = pHandle->u8SlvAddr;
            u16SlvAddr |= (u32MemAddr >> 8) & 0x07;
        }
        else
        {
            u16SlvAddr = pHandle->u8SlvAddr;
            u16SlvAddr |= (u32MemAddr >> 16) & 0x07;
        }

        if (EEPROM_WaitReady(pHandle, u16SlvAddr, EEPROM_TIMEOUT_MS) == true)
        {
            bAckPre = true;
        }
        else
        {
            if (bAckPre)
            {
                LOGI("eeprom capacity maybe %d kbit / %d bytes", u32MemAddr >> 7, u32MemAddr);
            }

            bAckPre = false;
        }
    }

    return ERR_NONE;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW  // 仅 AC5-O0&O1优化/AC6-O0优化 通过

#define BUFFSIZE 256

void EEPROM_Test(i2c_mst_t* hI2C)
{
    i2c_eeprom_t eeprom = {
        .hI2C      = hI2C,
        .u8SlvAddr = AT24CXX_ADDRESS_A000,
        .eCapacity = AT24CM01,  // 根据实际容量调整（影响页大小）
    };

    static uint8_t au8WrBuff[BUFFSIZE] = {0};  // 若卡住了就调大堆栈
    static uint8_t au8RdBuff[BUFFSIZE] = {0};

    for (uint16_t i = 0; i < BUFFSIZE; ++i)
    {
        au8WrBuff[i] = i;
    }

    EEPROM_Init(&eeprom);
    EEPROM_DetectCapacity(&eeprom);

    EEPROM_WriteBlock(&eeprom, 0, au8WrBuff, BUFFSIZE);
    EEPROM_ReadBlock(&eeprom, 0, au8RdBuff, BUFFSIZE);

    if (memcmp(au8WrBuff, au8RdBuff, BUFFSIZE) == 0)
    {
        LOGI("success");
    }
    else
    {
        LOGI("fail");
        HEXDUMP1(au8WrBuff, BUFFSIZE);
        HEXDUMP1(au8RdBuff, BUFFSIZE);
    }
}

#endif
