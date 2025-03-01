#include "spi_w25qxx.h"
#include "logger.h"

/**
 * @note register description: https://shequ.stmicroelectronics.cn/thread-636692-1-1.html
 */

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "w25qxx"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE

/**
 * @brief W25Q128
 * 包含256个块、每个块（64KB）有16个扇区（4096个扇区）、每个扇区（4KB）有16页、每一页有256个字节（Byte
 * 该书有256个章节，每个章节有16个小节，每个小节有16页，每页有256个字。
 */
#define W25Q128_FLASH_SIZE              0x1000000 /* 128 MBits => 16MBytes */
#define W25QXX_BLOCK_SIZE               0x10000   /* 256 sectors of 64KBytes */
#define W25QXX_SECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25QXX_PAGE_SIZE                0x100     /* 65536 pages of 256 bytes */

#define W25QXX_DUMMY_CYCLES_READ        4
#define W25QXX_DUMMY_CYCLES_READ_QUAD   10

#define W25QXX_BULK_ERASE_MAX_TIME      250000
#define W25QXX_SECTOR_ERASE_MAX_TIME    3000
#define W25QXX_SUBSECTOR_ERASE_MAX_TIME 800
#define W25QXX_TIMEOUT_VALUE            3000  // ms

/**
 * @brief Commands
 */
/* Reset Operations */
#define ENABLE_RESET_CMD 0x66
#define RESET_DEVICE_CMD 0x99

/* Register Operations */
#define READ_STATUS_REG1_CMD  0x05
#define READ_STATUS_REG2_CMD  0x35
#define READ_STATUS_REG3_CMD  0x15

#define WRITE_STATUS_REG1_CMD 0x01
#define WRITE_STATUS_REG2_CMD 0x31
#define WRITE_STATUS_REG3_CMD 0x11

/* Identification Operations */
#define READ_UID_CMD           0x4B  // unique id
#define READ_MAN_ID_CMD        0x90  // manufacturer device id
#define DUAL_READ_ID_CMD       0x92
#define QUAD_READ_ID_CMD       0x94
#define READ_JEDEC_ID_CMD      0x9F

#define POWER_DOWN_CMD         0xB9
#define RELEASE_POWER_DOWN_CMD 0xAB

/* Read Operations */
#define READ_CMD                 0x03
#define FAST_READ_CMD            0x0B
#define DUAL_OUT_FAST_READ_CMD   0x3B
#define DUAL_INOUT_FAST_READ_CMD 0xBB
#define QUAD_OUT_FAST_READ_CMD   0x6B
#define QUAD_INOUT_FAST_READ_CMD 0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD  0x06
#define WRITE_DISABLE_CMD 0x04

/* Program Operations */
#define PAGE_PROG_CMD            0x02
#define QUAD_INPUT_PAGE_PROG_CMD 0x32

/* Erase Operations */
#define SECTOR_ERASE_CMD             0x20
#define BLOCK_ERASE_32KB_CMD         0x52  // block earse
#define BLOCK_ERASE_64KB_CMD         0xD8  // block earse
#define CHIP_ERASE_CMD               0xC7
#define PROG_ERASE_RESUME_CMD        0x7A
#define PROG_ERASE_SUSPEND_CMD       0x75

#define ENTER_QPI_MODE_CMD           0x38
#define EXIT_QPI_MODE_CMD            0xFF

#define READ_SFDP_REG_CMD            0x5A
#define ERASE_SECURITY_REG_CMD       0x44
#define PROGRAM_SECURITY_REG_CMD     0x42
#define READ_SECURITY_REG_CMD        0x48

#define GLOBAL_BLOCK_LOCK_CMD        0x7E
#define GLOBAL_BLOCK_UNLOCK_CMD      0x98
#define READ_BLOCK_LOCK_CMD          0x3D
#define INDIVIDUAL_BLOCK_LOCK_CMD    0x36
#define INDIVIDUAL_BLOCK_UNLOCK_CMD  0x39
#define VOLATILE_SR_WRITE_ENABLE_CMD 0x50

#define ENABLE_4BYTE_ADDR_CMD        0xB7
#define EXIT_4BYTE_ADDR_CMD          0xE9

/* Flag Status Register */
#define W25QXX_FSR_BUSY ((uint8_t)0x01) /*!< busy */
#define W25QXX_FSR_WREN ((uint8_t)0x02) /*!< write enable */
#define W25QXX_FSR_QE   ((uint8_t)0x02) /*!< quad enable */

/**
 * @brief w25qxx burst wrap enumeration definition
 */
typedef enum {
    W25QXX_BURST_WRAP_NONE    = 0x10, /**< no burst wrap */
    W25QXX_BURST_WRAP_8_BYTE  = 0x00, /**< 8 byte burst wrap */
    W25QXX_BURST_WRAP_16_BYTE = 0x20, /**< 16 byte burst wrap */
    W25QXX_BURST_WRAP_32_BYTE = 0x40, /**< 32 byte burst wrap */
    W25QXX_BURST_WRAP_64_BYTE = 0x60, /**< 64 byte burst wrap */
} w25qxx_burst_wrap_e;

/**
 * @brief w25qxx status 1 enumeration definition
 */
typedef enum {
    W25QXX_STATUS1_STATUS_REGISTER_PROTECT_0             = (1 << 7), /**< status register protect 0 */
    W25QXX_STATUS1_SECTOR_PROTECT_OR_TOP_BOTTOM_PROTECT  = (1 << 6), /**< sector protect bit or top / bottom protect bit */
    W25QXX_STATUS1_TOP_BOTTOM_PROTECT_OR_BLOCK_PROTECT_3 = (1 << 5), /**< top / bottom protect bit or block 3 protect bit */
    W25QXX_STATUS1_BLOCK_PROTECT_2                       = (1 << 4), /**< block 2 protect bit */
    W25QXX_STATUS1_BLOCK_PROTECT_1                       = (1 << 3), /**< block 1 protect bit */
    W25QXX_STATUS1_BLOCK_PROTECT_0                       = (1 << 2), /**< block 0 protect bit */
    W25QXX_STATUS1_WRITE_ENABLE_LATCH                    = (1 << 1), /**< write enable latch */
    W25QXX_STATUS1_ERASE_WRITE_PROGRESS                  = (1 << 0), /**< erase / write in progress */
} w25qxx_status1_e;

/**
 * @brief w25qxx status 2 enumeration definition
 */
typedef enum {
    W25QXX_STATUS2_SUSPEND_STATUS                = (1 << 7), /**< suspend status */
    W25QXX_STATUS2_COMPLEMENT_PROTECT            = (1 << 6), /**< complement protect */
    W25QXX_STATUS2_SECURITY_REGISTER_3_LOCK_BITS = (1 << 5), /**< security register 3 lock bits */
    W25QXX_STATUS2_SECURITY_REGISTER_2_LOCK_BITS = (1 << 4), /**< security register 2 lock bits */
    W25QXX_STATUS2_SECURITY_REGISTER_1_LOCK_BITS = (1 << 3), /**< security register 1 lock bits */
    W25QXX_STATUS2_QUAD_ENABLE                   = (1 << 1), /**< quad enable */
    W25QXX_STATUS2_STATUS_REGISTER_PROTECT_1     = (1 << 0), /**< status register protect 1 */
} w25qxx_status2_e;

/**
 * @brief w25qxx status 3 enumeration definition
 */
typedef enum {
    W25QXX_STATUS3_HOLD_RESET_FUNCTION                   = (1 << 7), /**< HOLD or RESET function */
    W25QXX_STATUS3_OUTPUT_DRIVER_STRENGTH_100_PERCENTAGE = (0 << 5), /**< output driver strength 100% */
    W25QXX_STATUS3_OUTPUT_DRIVER_STRENGTH_75_PERCENTAGE  = (1 << 5), /**< output driver strength 75% */
    W25QXX_STATUS3_OUTPUT_DRIVER_STRENGTH_50_PERCENTAGE  = (2 << 5), /**< output driver strength 50% */
    W25QXX_STATUS3_OUTPUT_DRIVER_STRENGTH_25_PERCENTAGE  = (3 << 5), /**< output driver strength 25% */
    W25QXX_STATUS3_WRITE_PROTECT_SELECTION               = (1 << 2), /**< write protect selection */
    W25QXX_STATUS3_POWER_UP_ADDRESS_MODE                 = (1 << 1), /**< power up address mode */
    W25QXX_STATUS3_CURRENT_ADDRESS_MODE                  = (1 << 0), /**< current address mode */
} w25qxx_status3_e;

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static inline status_t _W25Qxx_Reset(spi_w25qxx_t* pHandle);
static inline status_t _W25Qxx_GetStatus(spi_w25qxx_t* pHandle);
static inline status_t _W25Qxx_WriteEnable(spi_w25qxx_t* pHandle);
static inline status_t _W25Qxx_WriteDisable(spi_w25qxx_t* pHandle);
static inline status_t _W25Qxx_PollForIdle(spi_w25qxx_t* pHandle, uint16_t u16Timeout);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t _W25Qxx_WriteAddr(spi_w25qxx_t* pHandle, uint32_t u32Address)
{
    uint8_t au8Cmd[4];

    au8Cmd[1] = (uint8_t)(u32Address >> 16);
    au8Cmd[2] = (uint8_t)(u32Address >> 8);
    au8Cmd[3] = (uint8_t)(u32Address);

    switch (pHandle->eAddrMode)
    {
        case W25QXX_ADDRESS_MODE_3_BYTE:
        {
            SPI_Master_TransmitBlock(pHandle->hSPI, &au8Cmd[1], 3);

            break;
        }
        case W25QXX_ADDRESS_MODE_4_BYTE:
        {
            au8Cmd[0] = (uint8_t)(u32Address >> 24);
            SPI_Master_TransmitBlock(pHandle->hSPI, &au8Cmd[0], 4);
            break;
        }
        default:
        {
            return ERR_INVALID_VALUE;
        }
    }

    return ERR_NONE;
}

/**
 * @brief Reset
 */
static inline status_t _W25Qxx_Reset(spi_w25qxx_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, ENABLE_RESET_CMD);
    SPI_Master_TransmitByte(pHandle->hSPI, RESET_DEVICE_CMD);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

/**
 * @brief Read current status
 */
static inline status_t _W25Qxx_GetStatus(spi_w25qxx_t* pHandle)
{
    uint8_t u8Status;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, READ_STATUS_REG1_CMD);
    SPI_Master_ReceiveByte(pHandle->hSPI, &u8Status);
    SPI_Master_Deselect(pHandle->hSPI);

    /* Check the value of the register */
    return (u8Status & W25QXX_FSR_BUSY) ? ERR_BUSY : ERR_NONE;
}

/**
 * @brief Send a Write Enable and wait it is effective.
 */
static inline status_t _W25Qxx_WriteEnable(spi_w25qxx_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, WRITE_ENABLE_CMD);
    SPI_Master_Deselect(pHandle->hSPI);

    return _W25Qxx_PollForIdle(pHandle, W25QXX_TIMEOUT_VALUE);
}

/**
 * @brief Send a Write Disable and wait it is effective.
 */
static inline status_t _W25Qxx_WriteDisable(spi_w25qxx_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, WRITE_DISABLE_CMD);
    SPI_Master_Deselect(pHandle->hSPI);

    return _W25Qxx_PollForIdle(pHandle, W25QXX_TIMEOUT_VALUE);
}

static inline status_t _W25Qxx_PollForIdle(spi_w25qxx_t* pHandle, uint16_t u16Timeout)
{
    uint32_t Tickstart = GetTickUs();

    /* Wait the end of Flash writing */
    while (_W25Qxx_GetStatus(pHandle) == ERR_BUSY)
    {
        /* Check for the Timeout */
        if (DelayNonBlockMs(Tickstart, u16Timeout))
        {
            LOGW("operation timeout");
            return ERR_TIMEOUT;
        }

        DelayBlockMs(5);
    }

    return ERR_NONE;
}

/**
 * @brief Initializes
 */
status_t W25Qxx_Init(spi_w25qxx_t* pHandle)
{
    uint8_t au8ID[2];

    _W25Qxx_Reset(pHandle);
    ERRCHK_RETURN(_W25Qxx_PollForIdle(pHandle, W25QXX_TIMEOUT_VALUE));

    W25Qxx_ReadDeviceID(pHandle, au8ID);
    uint16_t u16ID = be16(au8ID);

    switch (u16ID)
    {
        case W25Q80_MAN_ID:
        case W25Q16_MAN_ID:
        case W25Q32_MAN_ID:
        case W25Q64_MAN_ID:
        {
            // 3字节地址模式
            pHandle->eAddrMode = W25QXX_ADDRESS_MODE_3_BYTE;
            SPI_Master_Select(pHandle->hSPI);
            SPI_Master_TransmitByte(pHandle->hSPI, EXIT_4BYTE_ADDR_CMD);
            SPI_Master_Deselect(pHandle->hSPI);
            break;
        }
        case W25Q128_MAN_ID:
        case W25Q256_MAN_ID:
        case W25Q512_MAN_ID:
        {
            // 4字节地址模式(待测试)
            pHandle->eAddrMode = W25QXX_ADDRESS_MODE_4_BYTE;
            SPI_Master_Select(pHandle->hSPI);
            SPI_Master_TransmitByte(pHandle->hSPI, ENABLE_4BYTE_ADDR_CMD);
            SPI_Master_Deselect(pHandle->hSPI);
            break;
        }
        default:
        {
            return ERR_NO_DEVICE;
        }
    }

    return ERR_NONE;
}

/**
 * @brief Read manufacture/device ID.
 */
status_t W25Qxx_ReadDeviceID(spi_w25qxx_t* pHandle, uint8_t ID[2])
{
    uint8_t au8Dummy[] = {0x00, 0x00, 0x00};

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, READ_MAN_ID_CMD);
    SPI_Master_TransmitBlock(pHandle->hSPI, au8Dummy, 3);
    SPI_Master_ReceiveBlock(pHandle->hSPI, ID, 2);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

/**
 * @brief Read unique ID.
 */
status_t W25Qxx_ReadUniqueID(spi_w25qxx_t* pHandle, uint8_t ID[8])
{
    uint8_t au8Dummy[] = {0x00, 0x00, 0x00, 0x00};

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, READ_UID_CMD);
    SPI_Master_TransmitBlock(pHandle->hSPI, au8Dummy, 4);
    SPI_Master_ReceiveBlock(pHandle->hSPI, ID, 8);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

/**
 * @brief Read jedec ID.
 */
status_t W25Qxx_ReadJedecID(spi_w25qxx_t* pHandle, uint8_t ID[3])
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, READ_JEDEC_ID_CMD);
    SPI_Master_ReceiveBlock(pHandle->hSPI, ID, 3);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

/**
 * @brief Power down.
 */
status_t W25Qxx_PowerDown(spi_w25qxx_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, POWER_DOWN_CMD);
    SPI_Master_Deselect(pHandle->hSPI);
    return ERR_NONE;
}

/**
 * @brief Release power down.
 */
status_t W25Qxx_WakeUp(spi_w25qxx_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, RELEASE_POWER_DOWN_CMD);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

/**
 * @brief Reads an amount of data from the memory.
 * @param[in]  u32ReadAddr: Read start address
 * @param[in]  u32Size: Size of data to read
 * @param[out] pu8Data: Pointer to data to be read
 */
status_t W25Qxx_ReadData(spi_w25qxx_t* pHandle, uint32_t u32ReadAddr, uint32_t u32Size, uint8_t* pu8Data)
{
    LOGV("read %4d bytes at 0x%08X", u32Size, u32ReadAddr);

    ERRCHK_RETURN(_W25Qxx_PollForIdle(pHandle, W25QXX_TIMEOUT_VALUE));

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, READ_CMD);
    _W25Qxx_WriteAddr(pHandle, u32ReadAddr);
    SPI_Master_ReceiveBlock(pHandle->hSPI, pu8Data, u32Size);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

static bool _W25Qxx_IsNeedErase(const uint8_t* cpu8Data, uint16_t u16Size)
{
    while (u16Size--)
    {
        if (*cpu8Data != 0xFF)
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief Writes an amount of data to the memory.
 * @param[in] u32WriteAddr: Write start address
 * @param[in] u32Size: Size of data to write,No more than 256byte.
 * @param[in] cpu8Data: Pointer to data to be written
 */
status_t W25Qxx_WriteDataNoCheck(spi_w25qxx_t* pHandle, uint32_t u32WriteAddr, uint32_t u32Size, const uint8_t* cpu8Data)
{
    uint32_t u32EndAddr, u32CurrSize, u32CurrAddr;

    /* Calculation of the size between the write address and the end of the page */
    u32CurrAddr = 0;  // in page

    while (u32CurrAddr <= u32WriteAddr)
    {
        u32CurrAddr += W25QXX_PAGE_SIZE;
    }

    u32CurrSize = u32CurrAddr - u32WriteAddr;

    /* Check if the size of the data is less than the remaining place in the page */
    if (u32CurrSize > u32Size)
    {
        u32CurrSize = u32Size;
    }

    /* Initialize the adress variables */
    u32CurrAddr = u32WriteAddr;
    u32EndAddr  = u32WriteAddr + u32Size;

    /* Perform the write page by page */
    do
    {
        ERRCHK_RETURN(_W25Qxx_PollForIdle(pHandle, W25QXX_TIMEOUT_VALUE));

        /* Enable write operations */
        _W25Qxx_WriteEnable(pHandle);

        LOGV("write %4d bytes at 0x%08X", u32CurrSize, u32CurrAddr);

        SPI_Master_Select(pHandle->hSPI);
        SPI_Master_TransmitByte(pHandle->hSPI, PAGE_PROG_CMD);
        _W25Qxx_WriteAddr(pHandle, u32CurrAddr);
        SPI_Master_TransmitBlock(pHandle->hSPI, cpu8Data, u32CurrSize);
        SPI_Master_Deselect(pHandle->hSPI);

        /* Update the address and size variables for next page programming */
        u32CurrAddr += u32CurrSize;
        cpu8Data += u32CurrSize;
        u32CurrSize = MIN(W25QXX_PAGE_SIZE, u32EndAddr - u32CurrAddr);

    } while (u32CurrAddr < u32EndAddr);

    return ERR_NONE;
}

/**
 * @brief Writes an amount of data to the memory.
 * @param[in] u32WriteAddr: Write start address
 * @param[in] u32Size: Size of data to write,No more than 256byte.
 * @param[in] cpu8Data: Pointer to data to be written
 */
status_t W25Qxx_WriteData(spi_w25qxx_t* pHandle, uint32_t u32WriteAddr, uint32_t u32Size, const uint8_t* cpu8Data)
{
    uint8_t au8OldData[W25QXX_SECTOR_SIZE];

    /* Calculation of the size between the write address and the end of the page */

    uint32_t u32StartAddr = u32WriteAddr;
    uint32_t u32EndAddr   = u32StartAddr + u32Size;

    uint32_t u32CurrAddr = (u32StartAddr / W25QXX_SECTOR_SIZE) * W25QXX_SECTOR_SIZE;
    uint32_t u32CurrSize;

    /* Perform the write page by page */
    do
    {
        W25Qxx_ReadData(pHandle, u32CurrAddr, W25QXX_SECTOR_SIZE, au8OldData);

        uint16_t u16Offset;  // sector offset

        if (u32CurrAddr < u32StartAddr)
        {
            u16Offset   = u32StartAddr - u32CurrAddr;
            u32CurrSize = MIN(W25QXX_SECTOR_SIZE - u16Offset, u32Size);
        }
        else if ((u32CurrAddr + W25QXX_SECTOR_SIZE) > u32EndAddr)
        {
            u16Offset   = 0;
            u32CurrSize = u32EndAddr - u32CurrAddr;
        }
        else
        {
            u16Offset   = 0;
            u32CurrSize = W25QXX_SECTOR_SIZE;
        }

        LOGV("write %4d bytes at 0x%08X", u32CurrSize, u32CurrAddr + u16Offset);

        for (uint16_t i = u16Offset; i < u32CurrSize; i++)
        {
            if (au8OldData[i] != 0xFF)
            {
                LOGV("erase block at 0x%08X", u32CurrAddr);

                W25Qxx_EraseSector(pHandle, u32CurrAddr);

                /* Waiting for erasure to complete */
                ERRCHK_RETURN(_W25Qxx_PollForIdle(pHandle, W25QXX_TIMEOUT_VALUE));

                break;
            }
        }

        if (u16Offset > 0)  // head
        {
            W25Qxx_WriteDataNoCheck(pHandle, u32CurrAddr, u16Offset, &au8OldData[0]);
        }

        W25Qxx_WriteDataNoCheck(pHandle, u32CurrAddr + u16Offset, u32CurrSize, cpu8Data);

        if (u16Offset == 0 && u32CurrSize < W25QXX_SECTOR_SIZE)  // tail
        {
            W25Qxx_WriteDataNoCheck(pHandle, u32CurrAddr + u32CurrSize, W25QXX_SECTOR_SIZE - u32CurrSize, &au8OldData[u32CurrSize]);
        }

        u32CurrAddr += W25QXX_SECTOR_SIZE;
        cpu8Data += u32CurrSize;
        u32CurrSize = MIN(W25QXX_SECTOR_SIZE, u32EndAddr - u32CurrAddr);

    } while (u32CurrAddr < u32EndAddr);

    return ERR_NONE;
}

/**
 * @brief Erases the specified block of the memory.
 * @param[in] u32Address: Block address to erase
 */
status_t W25Qxx_EraseSector(spi_w25qxx_t* pHandle, uint32_t u32Address)
{
    _W25Qxx_WriteEnable(pHandle);

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, SECTOR_ERASE_CMD);
    _W25Qxx_WriteAddr(pHandle, u32Address);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

/**
 * @brief Erases the entire memory.will take a very long time.
 */
status_t W25Qxx_EraseChip(spi_w25qxx_t* pHandle)
{
    /* Enable write operations */
    _W25Qxx_WriteEnable(pHandle);

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, CHIP_ERASE_CMD);
    SPI_Master_Deselect(pHandle->hSPI);

    return ERR_NONE;
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#include "hexdump.h"

#define CONFIG_DEMOS_READ_ID_ONLY_SW 0

void W25Qxx_Test(void)
{
#if defined(BOARD_STM32F407VET6_XWS)

    spi_mst_t spi = {
        .MISO = {W25Q128_MISO_PIN},
        .MOSI = {W25Q128_MOSI_PIN},
        .SCLK = {W25Q128_SCLK_PIN},
        .CS   = {W25Q128_CS_PIN},
    };

#elif defined(BOARD_CS32F103C8T6_QG)

    extern SPI_HandleTypeDef hspi1;

    spi_mst_t spi = {
        // .SPIx = &hspi1,
        .MISO = {SPI_MISO_PIN},
        .MOSI = {SPI_MOSI_PIN},
        .SCLK = {SPI_SCLK_PIN},
        .CS   = {FLASH_CS_PIN},
    };

#endif

    spi_w25qxx_t w25qxx = {
        .hSPI = &spi,
    };

    SPI_Master_Init(&spi, 500000, SPI_DUTYCYCLE_50_50, W25QXX_SPI_TIMING | SPI_FLAG_SOFT_CS);

    W25Qxx_Init(&w25qxx);

    static uint8_t au8Data[W25QXX_SECTOR_SIZE * 2] = {0};
    uint32_t       u32Address                      = 88;

    W25Qxx_ReadDeviceID(&w25qxx, au8Data);

    hexdump(au8Data, 2, 16, 1, false, "[DeviceID]:", 0);

    W25Qxx_ReadUniqueID(&w25qxx, au8Data);
    hexdump(au8Data, 8, 16, 1, false, "[UniqueID]:", 0);

    W25Qxx_ReadJedecID(&w25qxx, au8Data);
    hexdump(au8Data, 3, 16, 1, false, "[JedecID]:", 0);

#if !CONFIG_DEMOS_READ_ID_ONLY_SW

    // 1. read

    W25Qxx_ReadData(&w25qxx, u32Address, ARRAY_SIZE(au8Data), au8Data);

    PRINTLN("[first read]");
    hexdump(au8Data, ARRAY_SIZE(au8Data), 16, 4, true, nullptr, u32Address);

    // 2. write

    for (uint16_t i = 0; i < ARRAY_SIZE(au8Data); ++i)
    {
        // au8Data[i] += 1;
        au8Data[i] = 'A';
    }

    PRINTLN("[to write]");
    hexdump(au8Data, ARRAY_SIZE(au8Data), 16, 4, true, nullptr, u32Address);

    W25Qxx_WriteData(&w25qxx, u32Address, ARRAY_SIZE(au8Data) - W25QXX_PAGE_SIZE, au8Data);

    // 3. read again

    memset(au8Data, 0, sizeof(au8Data));
    W25Qxx_ReadData(&w25qxx, u32Address, ARRAY_SIZE(au8Data), au8Data);

    PRINTLN("[second read]");
    hexdump(au8Data, ARRAY_SIZE(au8Data), 16, 4, true, nullptr, u32Address);

#endif

    while (1)
    {
    }
}

#endif /* CONFIG_DEMOS_SW */
