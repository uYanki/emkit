#include "i2cdefs.h"

#if CONFIG_SWI2C_MODULE_SW

#include "delay.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "swi2cmst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

#if CONFIG_SWI2C_FAST_MODE_SW  // no-delay
#undef DelayBlockUs
#define DelayBlockUs(...)
#endif

#define I2C_MODE_WRITE 0U
#define I2C_MODE_READ  1U

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static void SWI2C_Master_GenerateStartSingal(i2c_mst_t* pHandle);
static void SWI2C_Master_GenerateStopSingal(i2c_mst_t* pHandle);
static void SWI2C_Master_GenerateAck(i2c_mst_t* pHandle);
static void SWI2C_Master_GenerateNack(i2c_mst_t* pHandle);
static bool SWI2C_Master_PollForAck(i2c_mst_t* pHandle);
static void SWI2C_Master_TransmitByte(i2c_mst_t* pHandle, u8 u8TxData);
static u8   SWI2C_Master_ReceiveByte(i2c_mst_t* pHandle);

static status_t SWI2C_Master_Init(i2c_mst_t* pHandle, u32 u32ClockSpeedHz, i2c_duty_cycle_e eDutyCycle);
static status_t SWI2C_Master_SetClock(i2c_mst_t* pHandle, u32 u32ClockSpeedHz, i2c_duty_cycle_e eDutyCycle);
static bool     SWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, u8 u16SlvAddr, u16 u16Flags);
static status_t SWI2C_Master_ReadBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8* pu8RxData, u16 u16Size, u16 u16Flags);
static status_t SWI2C_Master_WriteBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, const u8* pu8TxData, u16 u16Size, u16 u16Flags);
static status_t SWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u8* pu8RxData, u16 u16Size, u16 u16Flags);
static status_t SWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, const u8* pu8TxData, u16 u16Size, u16 u16Flags);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const i2cmst_ops_t g_swI2cOps = {
    .Init          = SWI2C_Master_Init,
    .IsDeviceReady = SWI2C_Master_IsDeviceReady,
    .ReadBlock     = SWI2C_Master_ReadBlock,
    .WriteBlock    = SWI2C_Master_WriteBlock,
    .ReceiveBlock  = SWI2C_Master_ReceiveBlock,
    .TransmitBlock = SWI2C_Master_TransmitBlock,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static status_t SWI2C_Master_Init(i2c_mst_t* pHandle, u32 u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle)
{
    PIN_SetMode(&pHandle->SDA, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);
    PIN_SetMode(&pHandle->SCL, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);

    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_HIGH);
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);

    ERRCHK_RETURN(SWI2C_Master_SetClock(pHandle, u32ClockFreqHz, eDutyCycle));

    return ERR_NONE;
}

static status_t SWI2C_Master_SetClock(i2c_mst_t* pHandle, u32 u32ClockSpeedHz, i2c_duty_cycle_e eDutyCycle)
{
    u32 u32ClockCycleUs;

    ASSERT(u32ClockSpeedHz, "clock freq must greater than 0");

    u32ClockCycleUs              = 1000000UL / u32ClockSpeedHz;
    pHandle->u32ClockLowCycleUs  = (u32ClockCycleUs * ((u32)eDutyCycle + 1)) >> 8;
    pHandle->u32ClockHighCycleUs = u32ClockCycleUs - pHandle->u32ClockLowCycleUs;

    return ERR_NONE;
}

static status_t SWI2C_Master_BusReset(i2c_mst_t* pHandle, u8 u8PulseCount)  // I2C死锁复位，待测试
{
    status_t eRet = ERR_FAIL;

    u8PulseCount = 9;  // 9脉冲复位总线

    PIN_SetMode(&pHandle->SDA, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);
    PIN_SetMode(&pHandle->SCL, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);

    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_HIGH);
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);

    while (u8PulseCount--)
    {
        PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_LOW);  // sda=0
        DelayBlockUs(pHandle->u32ClockHighCycleUs / 2);
        PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1
        DelayBlockUs(pHandle->u32ClockHighCycleUs / 2);
    }

    PIN_SetMode(&pHandle->SDA, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
    PIN_SetMode(&pHandle->SCL, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

    DelayBlockUs(pHandle->u32ClockHighCycleUs);

    // 检查是否已回到空闲电平
    if (PIN_ReadLevel(&pHandle->SDA) == PIN_LEVEL_HIGH &&  //
        PIN_ReadLevel(&pHandle->SCL) == PIN_LEVEL_HIGH)
    {
        eRet = ERR_NONE;
    }

    PIN_SetMode(&pHandle->SDA, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);
    PIN_SetMode(&pHandle->SCL, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);

    return eRet;
}

static void SWI2C_Master_GenerateStartSingal(i2c_mst_t* pHandle)
{
    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_HIGH);  // sda=1

    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1
    DelayBlockUs(pHandle->u32ClockHighCycleUs / 2);
    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_LOW);  // sda=0
    DelayBlockUs(pHandle->u32ClockHighCycleUs / 2);

    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_LOW);  // scl=0
    DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);
}

static void SWI2C_Master_GenerateStopSingal(i2c_mst_t* pHandle)
{
    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_LOW);  // sda=0
    DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1
    DelayBlockUs(pHandle->u32ClockHighCycleUs / 2);
    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_HIGH);  // sda=1
}

/**
 * @note it is called after ReceiveByte(pHandle)
 */
static void SWI2C_Master_GenerateAck(i2c_mst_t* pHandle)
{
    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_LOW);   // sda=0, ack
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1
    DelayBlockUs(pHandle->u32ClockHighCycleUs);
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_LOW);  // scl=0
    DelayBlockUs(pHandle->u32ClockLowCycleUs);
}

/**
 * @note it is called after ReceiveByte(pHandle)
 */
static void SWI2C_Master_GenerateNack(i2c_mst_t* pHandle)
{
    PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_HIGH);  // sda=1, nack
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1
    DelayBlockUs(pHandle->u32ClockHighCycleUs);
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_LOW);  // scl=0
    DelayBlockUs(pHandle->u32ClockLowCycleUs);
}

/**
 * @brief it is called after TransmitByte(pHandle)
 * @return true ACK
 * @return false NACK
 */
static bool SWI2C_Master_PollForAck(i2c_mst_t* pHandle)
{
    u8       u8TimeoutUs = pHandle->u32ClockHighCycleUs;
    const u8 u8CycleUs   = 5;

    status_t eAck = true;  // ack

    DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1

    PIN_SetMode(&pHandle->SDA, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);
    while (PIN_ReadLevel(&pHandle->SDA) == PIN_LEVEL_HIGH)
    {
        if (u8TimeoutUs < u8CycleUs)
        {
            eAck = false;  // nack
            goto __exit;
        }

        DelayBlockUs(u8CycleUs);
        u8TimeoutUs -= u8CycleUs;
    }

__exit:
    PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_LOW);  // scl=0
    DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);
    PIN_SetMode(&pHandle->SDA, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);

    return eAck;
}

static void SWI2C_Master_TransmitByte(i2c_mst_t* pHandle, u8 u8TxData)
{
    u8 u8Mask = 0x80;

    do
    {
        if (u8TxData & u8Mask)
        {
            PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_HIGH);  // sda = 1
        }
        else
        {
            PIN_WriteLevel(&pHandle->SDA, PIN_LEVEL_LOW);  // sda = 0
        }

        DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);
        PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1
        DelayBlockUs(pHandle->u32ClockHighCycleUs);
        PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_LOW);  // scl=0
        DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);

    } while (u8Mask >>= 1);
}

static u8 SWI2C_Master_ReceiveByte(i2c_mst_t* pHandle)
{
    u8 u8Mask = 0x80, u8RxData = 0x00;

    PIN_SetMode(&pHandle->SDA, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

    do
    {
        DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);

        PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_HIGH);  // scl=1
        DelayBlockUs(pHandle->u32ClockHighCycleUs);

        if (PIN_ReadLevel(&pHandle->SDA) == PIN_LEVEL_HIGH)  // sda=x
        {
            u8RxData |= u8Mask;
        }

        PIN_WriteLevel(&pHandle->SCL, PIN_LEVEL_LOW);  // scl=0
        DelayBlockUs(pHandle->u32ClockLowCycleUs / 2);

    } while (u8Mask >>= 1);

    PIN_SetMode(&pHandle->SDA, PIN_MODE_OUTPUT_OPEN_DRAIN, PIN_PULL_UP);

    return u8RxData;
}

static bool SWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, u8 u16SlvAddr, u16 u16Flags)
{
    bool eAck;
    SWI2C_Master_GenerateStartSingal(pHandle);
    SWI2C_Master_TransmitByte(pHandle, u16SlvAddr << 1);
    eAck = SWI2C_Master_PollForAck(pHandle);
    SWI2C_Master_GenerateStopSingal(pHandle);
    return eAck ? true : false;
}

static status_t SWI2C_Master_ReadBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, u8* pu8RxData, u16 u16Size, u16 u16Flags)
{
    status_t eRet = ERR_TIMEOUT;

    SWI2C_Master_GenerateStartSingal(pHandle);

    SWI2C_Master_TransmitByte(pHandle, (u16SlvAddr << 1) | I2C_MODE_WRITE);

    if (SWI2C_Master_PollForAck(pHandle) == false)
    {
        goto __exit;
    }

    if (u16Flags & I2C_FLAG_MEMADDR_16BIT)
    {
        SWI2C_Master_TransmitByte(pHandle, u16MemAddr >> 8);

        if (SWI2C_Master_PollForAck(pHandle) == false)
        {
            goto __exit;
        }
    }

    SWI2C_Master_TransmitByte(pHandle, u16MemAddr & 0xFF);

    if (SWI2C_Master_PollForAck(pHandle) == false)
    {
        goto __exit;
    }

    SWI2C_Master_GenerateStartSingal(pHandle);

    SWI2C_Master_TransmitByte(pHandle, (u16SlvAddr << 1) | I2C_MODE_READ);

    if (SWI2C_Master_PollForAck(pHandle) == false)
    {
        goto __exit;
    }

    while (u16Size--)
    {
        *pu8RxData++ = SWI2C_Master_ReceiveByte(pHandle);
        SWI2C_Master_GenerateAck(pHandle);
    }

    *pu8RxData = SWI2C_Master_ReceiveByte(pHandle);
    SWI2C_Master_GenerateNack(pHandle);

    eRet = ERR_NONE;

__exit:
    SWI2C_Master_GenerateStopSingal(pHandle);
    return eRet;
}

static status_t SWI2C_Master_WriteBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u16 u16MemAddr, const u8* pu8TxData, u16 u16Size, u16 u16Flags)
{
    status_t eRet = ERR_TIMEOUT;

    SWI2C_Master_GenerateStartSingal(pHandle);

    SWI2C_Master_TransmitByte(pHandle, (u16SlvAddr << 1) | I2C_MODE_WRITE);

    if (SWI2C_Master_PollForAck(pHandle) == false)
    {
        goto __exit;
    }

    if (u16Flags & I2C_FLAG_MEMADDR_16BIT)
    {
        SWI2C_Master_TransmitByte(pHandle, u16MemAddr >> 8);  // high address

        if (SWI2C_Master_PollForAck(pHandle) == false)
        {
            if ((u16Flags & I2C_FLAG_SIGNAL_IGNORE_NACK) == 0)
            {
                goto __exit;
            }
        }
    }

    SWI2C_Master_TransmitByte(pHandle, u16MemAddr & 0xFF);  // low address

    if (SWI2C_Master_PollForAck(pHandle) == false)
    {
        if ((u16Flags & I2C_FLAG_SIGNAL_IGNORE_NACK) == 0)
        {
            goto __exit;
        }
    }

    while (u16Size--)
    {
        SWI2C_Master_TransmitByte(pHandle, *pu8TxData++);

        if (SWI2C_Master_PollForAck(pHandle) == false)
        {
            if ((u16Flags & I2C_FLAG_SIGNAL_IGNORE_NACK) == 0)
            {
                goto __exit;
            }
        }
    }

    eRet = ERR_NONE;

__exit:

    SWI2C_Master_GenerateStopSingal(pHandle);

    return eRet;
}

static status_t SWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, u8* pu8RxData, u16 u16Size, u16 u16Flags)
{
    status_t eRet = ERR_TIMEOUT;

    if ((u16Flags & I2C_FLAG_SIGNAL_NOSTART) == 0)
    {
        SWI2C_Master_GenerateStartSingal(pHandle);

        SWI2C_Master_TransmitByte(pHandle, (u16SlvAddr << 1) | I2C_MODE_READ);

        if (SWI2C_Master_PollForAck(pHandle) == false)
        {
            if ((u16Flags & I2C_FLAG_SIGNAL_IGNORE_NACK) == 0)
            {
                goto __exit;
            }
        }
    }

    while (u16Size--)
    {
        *pu8RxData++ = SWI2C_Master_ReceiveByte(pHandle);
        SWI2C_Master_GenerateAck(pHandle);
    }

    *pu8RxData = SWI2C_Master_ReceiveByte(pHandle);
    SWI2C_Master_GenerateNack(pHandle);

    eRet = ERR_NONE;

__exit:

    if ((u16Flags & I2C_FLAG_SIGNAL_NOSTOP) == 0)
    {
        SWI2C_Master_GenerateStopSingal(pHandle);
    }

    return eRet;
}

static status_t SWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, u16 u16SlvAddr, const u8* pu8TxData, u16 u16Size, u16 u16Flags)
{
    status_t eRet = ERR_TIMEOUT;

    if ((u16Flags & I2C_FLAG_SIGNAL_NOSTART) == 0)
    {
        SWI2C_Master_GenerateStartSingal(pHandle);

        SWI2C_Master_TransmitByte(pHandle, (u16SlvAddr << 1) | I2C_MODE_WRITE);

        if (SWI2C_Master_PollForAck(pHandle) == false)
        {
            if ((u16Flags & I2C_FLAG_SIGNAL_IGNORE_NACK) == 0)
            {
                goto __exit;
            }
        }
    }

    while (u16Size--)
    {
        SWI2C_Master_TransmitByte(pHandle, *pu8TxData++);

        if (SWI2C_Master_PollForAck(pHandle) == false)
        {
            if ((u16Flags & I2C_FLAG_SIGNAL_IGNORE_NACK) == 0)
            {
                goto __exit;
            }
        }
    }

    eRet = ERR_NONE;

__exit:

    if ((u16Flags & I2C_FLAG_SIGNAL_NOSTOP) == 0)
    {
        SWI2C_Master_GenerateStopSingal(pHandle);
    }

    return eRet;
}

#endif
