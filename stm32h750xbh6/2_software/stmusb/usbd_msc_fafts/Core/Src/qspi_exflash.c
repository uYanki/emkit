#include "qspi_exflash.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "QSPI_FLASH"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE

/* 复位操作 */
#define RESET_ENABLE_CMD   0x66
#define RESET_MEMORY_CMD   0x99

#define ENTER_QPI_MODE_CMD 0x38
#define EXIT_QPI_MODE_CMD  0xFF

/* 地址模式 */
#define ENTER_4BYTE_ADDR_MODE_CMD 0xB7  // 4字节地址模式

/* 识别操作 */
#define READ_ID_CMD       0x90
#define DUAL_READ_ID_CMD  0x92
#define QUAD_READ_ID_CMD  0x94
#define READ_JEDEC_ID_CMD 0x9F

/* 读操作 */
#define READ_CMD                 0x03
#define FAST_READ_CMD            0x0B
#define DUAL_OUT_FAST_READ_CMD   0x3B
#define DUAL_INOUT_FAST_READ_CMD 0xBB
#define QUAD_OUT_FAST_READ_CMD   0x6B
#define QUAD_INOUT_FAST_READ_CMD 0xEB  // 快速读取指令 (1线指令4线地址4线数据)

/* 写操作 */
#define WRITE_ENABLE_CMD  0x06
#define WRITE_DISABLE_CMD 0x04

/* 寄存器操作 */
#define READ_STATUS_REG1_CMD  0x05
#define READ_STATUS_REG2_CMD  0x35
#define READ_STATUS_REG3_CMD  0x15

#define WRITE_STATUS_REG1_CMD 0x01
#define WRITE_STATUS_REG2_CMD 0x31
#define WRITE_STATUS_REG3_CMD 0x11

/* 编程操作 */
#define PAGE_PROG_CMD             0x02
#define QUAD_INPUT_PAGE_PROG_CMD  0x32  // 快速页编程指令 (1线指令1线地址4线数据)
#define EXT_QUAD_IN_FAST_PROG_CMD 0x12

/* 擦除操作 */
#define SECTOR_ERASE_CMD       0x20
#define CHIP_ERASE_CMD         0xC7
#define BLOCK_ERASE_32K_CMD    0x52
#define BLOCK_ERASE_64K_CMD    0xD8

#define PROG_ERASE_RESUME_CMD  0x7A
#define PROG_ERASE_SUSPEND_CMD 0x75

/* 状态寄存器标志 */
#define W25Qx_FSR_BUSY    ((u8)0x01) /*!< fsr1 busy */
#define W25Qx_FSR_WREN    ((u8)0x02) /*!< fsr1 write enable */
#define W25Qx_FSR_QE      ((u8)0x02) /*!< fsr1 quad enable */
#define W25Qx_FSR_4B_ADDR ((u8)0x01) /*!< fsr2 4 addr mode */

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

extern QSPI_HandleTypeDef hqspi;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief 等待空闲
 */
static bool W25Qx_QSPI_AutoPollingMemReady(u32 u32Timeout)
{
    QSPI_CommandTypeDef     sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig  = {0};  // FSR & Mask == Match

    /* 配置自动轮询 */

    sCommand.Instruction       = READ_STATUS_REG1_CMD;  // 读状态寄存器
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.Match           = 0x00;
    sConfig.MatchMode       = QSPI_MATCH_MODE_AND;  // 与运算
    sConfig.Interval        = 0x10;
    sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;
    sConfig.StatusBytesSize = 1;
    sConfig.Mask            = W25Qx_FSR_BUSY;

    /* 等待状态寄存器的BUSY标志位被置0 */

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, u32Timeout) != HAL_OK)  // 轮询模式（可改中断模式）
    {
        return false;
    }

    return true;
}

/**
 * @brief 写使能
 */
static bool W25Qx_QSPI_WriteEnable(void)
{
    QSPI_CommandTypeDef     sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig  = {0};

    /* 发送写使能命令 */

    sCommand.Instruction = WRITE_ENABLE_CMD;
    sCommand.DataMode    = QSPI_DATA_NONE;
    sCommand.NbData      = 0;

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    /* 配置自动轮询 */

    sCommand.Instruction = READ_STATUS_REG1_CMD;  // 读状态寄存器
    sCommand.DataMode    = QSPI_DATA_1_LINE;
    sCommand.NbData      = 1;

    sConfig.Match           = W25Qx_FSR_WREN;
    sConfig.Mask            = W25Qx_FSR_WREN;
    sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval        = 0x10;
    sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    /* 等待状态寄存器的WREN标志位被置1 */

    if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    return true;
}

/**
 * @brief 复位器件
 */
bool W25Qx_QSPI_Reset(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    /* 初始化指令 */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.DataMode          = QSPI_DATA_NONE;

    /* 使能复位 */

    sCommand.Instruction = RESET_ENABLE_CMD;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    /* 复位器件 */

    sCommand.Instruction = RESET_MEMORY_CMD;

    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    if (W25Qx_QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != true)
    {
        return false;
    }

    return true;
}

/**
 * @brief 读取器件ID
 */
u32 W25Qx_QSPI_ReadJedecID(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    u8  au8RxBuff[3] = {0};
    u32 u32JedecID   = 0;

    /* 初始化指令 */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = READ_JEDEC_ID_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.NbData            = 3;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;  // 读取 JEDEC ID

    /* 发送指令 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return 0;
    }

    /* 接收数据 */
    if (HAL_QSPI_Receive(&hqspi, au8RxBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return 0;
    }

    u32JedecID = (au8RxBuff[0] << 16) | (au8RxBuff[1] << 8) | au8RxBuff[2];

    return u32JedecID;
}

/**
 * @brief 读取器件ID
 */
u16 W25Qx_QSPI_ReadDeviceID(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    u8  au8RxBuff[2] = {0};
    u16 u16DeviceID  = 0;

    /* 初始化指令 */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = READ_ID_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.Address           = 0x000000;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_1_LINE;
    sCommand.NbData            = 2;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* 发送指令 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return 0;
    }

    /* 接收数据 */
    if (HAL_QSPI_Receive(&hqspi, au8RxBuff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return 0;
    }

    u16DeviceID = (au8RxBuff[0] << 8) | au8RxBuff[1];

    return u16DeviceID;
}

/**
 * @brief 进入内存映射模式
 * @note 该模式需关闭MPU，且只能读不能写
 */
bool W25Qx_QSPI_EnterMemoryMappedMode(void)
{
    QSPI_CommandTypeDef      sCommand = {0};
    QSPI_MemoryMappedTypeDef sConfig  = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_4_LINES;
    sCommand.DataMode          = QSPI_DATA_4_LINES;
    sCommand.DummyCycles       = 6;
    sCommand.Instruction       = QUAD_INOUT_FAST_READ_CMD;  // 1-4-4 模式

    sConfig.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;  // 禁用超时计数器
    sConfig.TimeOutPeriod     = 0;

    // W25Qx_QSPI_Reset();

    if (HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sConfig) != HAL_OK)
    {
        return false;
    }

    return true;
}

/**
 * @brief 擦除扇区 (4K)
 */
bool W25Qx_QSPI_EraseSector(u32 u32SectorAddress)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.Address           = u32SectorAddress;
    sCommand.Instruction       = SECTOR_ERASE_CMD;

    /* 启用写操作 */
    if (W25Qx_QSPI_WriteEnable() != true)
    {
        return false;
    }

    /* 发送指令 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    /* 等待擦除结束 */
    if (W25Qx_QSPI_AutoPollingMemReady(CONFIG_W25Qxx_SUBSECTOR_ERASE_MAX_TIME) != true)
    {
        return false;
    }

    return true;
}

/**
 * @brief 擦除区块32K
 */
bool W25Qx_QSPI_EraseBlock_32K(u32 u32BlockAddress)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.Address           = u32BlockAddress;
    sCommand.Instruction       = BLOCK_ERASE_32K_CMD;

    /* 启用写操作 */
    if (W25Qx_QSPI_WriteEnable() != true)
    {
        return false;
    }

    /* 发送指令 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    /* 等待擦除结束 */
    if (W25Qx_QSPI_AutoPollingMemReady(CONFIG_W25Qxx_SECTOR_ERASE_MAX_TIME) != true)
    {
        return false;
    }

    return true;
}

/**
 * @brief 擦除区块64K
 */
bool W25Qx_QSPI_EraseBlock_64K(uint32_t u32BlockAddress)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.Address           = u32BlockAddress;
    sCommand.Instruction       = BLOCK_ERASE_64K_CMD;

    /* 启用写操作 */
    if (W25Qx_QSPI_WriteEnable() != true)
    {
        return false;
    }

    /* 发送指令 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    /* 等待擦除结束 */
    if (W25Qx_QSPI_AutoPollingMemReady(CONFIG_W25Qxx_SECTOR_ERASE_MAX_TIME) != true)
    {
        return false;
    }

    return true;
}

/**
 * @brief 整片擦除
 */
bool W25Qx_QSPI_EraseChip(void)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = CHIP_ERASE_CMD;  // 整片擦除
    sCommand.AddressMode       = QSPI_ADDRESS_NONE;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* 启用写操作 */
    if (W25Qx_QSPI_WriteEnable() != true)
    {
        return false;
    }

    /* 发送指令 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    /* 等待擦除结束 */
    if (W25Qx_QSPI_AutoPollingMemReady(CONFIG_W25Qxx_BULK_ERASE_MAX_TIME) != true)
    {
        return false;
    }

    return true;
}

/**
 * @brief 写入数据
 */
bool W25Qx_QSPI_WriteBuffer(u8* pu8Data, u32 u32WriteAddr, u32 u32Size)
{
    QSPI_CommandTypeDef sCommand = {0};

    u32 u32EndAddr, u32XferSize, u32CurrAddr;

    /* 计算写入地址和页面末尾之间的大小 */
    u32XferSize = CONFIG_W25Qxx_PAGE_SIZE - (u32WriteAddr % CONFIG_W25Qxx_PAGE_SIZE);

    /* 检查数据的大小是否小于页面中的剩余位置 */
    if (u32XferSize > u32Size)
    {
        u32XferSize = u32Size;
    }

    /* 初始化地址变量 */
    u32CurrAddr = u32WriteAddr;
    u32EndAddr  = u32WriteAddr + u32Size;

    /* 初始化程序命令 */
    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.Instruction       = QUAD_INPUT_PAGE_PROG_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode          = QSPI_DATA_4_LINES;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* 逐页执行写入 */
    do
    {
        sCommand.Address = u32CurrAddr;
        sCommand.NbData  = u32XferSize;

        /* 启用写操作 */
        if (W25Qx_QSPI_WriteEnable() != true)
        {
            return false;
        }

        /* 配置命令 */
        if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return false;
        }

        /* 传输数据 */
        if (HAL_QSPI_Transmit(&hqspi, pu8Data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return false;
        }

        /* 配置自动轮询模式等待程序结束 */
        if (W25Qx_QSPI_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != true)
        {
            return false;
        }

        /* 更新下一页编程的地址和大小变量 */
        u32CurrAddr += u32XferSize;
        pu8Data += u32XferSize;

        if ((u32CurrAddr + CONFIG_W25Qxx_PAGE_SIZE) > u32EndAddr)
        {
            u32XferSize = u32EndAddr - u32CurrAddr;
        }
        else
        {
            u32XferSize = CONFIG_W25Qxx_PAGE_SIZE;
        }

    } while (u32CurrAddr < u32EndAddr);

    return true;
}

/**
 * @brief 读取数据
 */
bool W25Qx_QSPI_ReadBuffer(uint8_t* pu8Buffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
    QSPI_CommandTypeDef sCommand = {0};

    sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
    sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    sCommand.AddressMode       = QSPI_ADDRESS_4_LINES;
    sCommand.DataMode          = QSPI_DATA_4_LINES;
    sCommand.DummyCycles       = 6;
    sCommand.NbData            = NumByteToRead;
    sCommand.Address           = ReadAddr;
    sCommand.Instruction       = QUAD_INOUT_FAST_READ_CMD;  // 快速读取指令

    /* 配置命令 */
    if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    /* 接收数据 */
    if (HAL_QSPI_Receive(&hqspi, pu8Buffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return false;
    }

    return true;
}

#if 0

#include "logger.h"

#define W25Qxx_NumByteToTest 32 * 1024

u32 W25Qxx_TestAddr = 0;
u8  W25Qxx_WriteBuffer[W25Qxx_NumByteToTest];
u8  W25Qxx_ReadBuffer[W25Qxx_NumByteToTest];

bool W25Qx_QSPI_Test(void)
{
    bool bStatus;

    uint32_t i = 0;  // 计数变量
    uint32_t u32StartTime;
    uint32_t u32EndTime;
    uint32_t u32DeltaTime;  // 执行时间
    float    f32Speed;      // 执行速度
    u32      u32FlashID;

    W25Qx_QSPI_Reset();

    u32FlashID = W25Qx_QSPI_ReadJedecID();
    LOGD("flash ID:%X", u32FlashID);

    //
    // 擦除
    //

    u32StartTime = HAL_GetTick();
    bStatus      = W25Qx_QSPI_EraseBlock_32K(W25Qxx_TestAddr);
    u32EndTime   = HAL_GetTick();

    if (bStatus == false)
    {
        LOGE("erase fail");
        while (1);
    }

    u32DeltaTime = u32EndTime - u32StartTime;
    LOGD("erase 32KB: %d ms", u32DeltaTime);

    //
    // 写入
    //

    for (i = 0; i < W25Qxx_NumByteToTest; i++)  // 先将数据写入数组
    {
        W25Qxx_WriteBuffer[i] = i;
    }

    u32StartTime = HAL_GetTick();
    bStatus      = W25Qx_QSPI_WriteBuffer(W25Qxx_WriteBuffer, W25Qxx_TestAddr, W25Qxx_NumByteToTest);
    u32EndTime   = HAL_GetTick();

    if (bStatus == false)
    {
        LOGE("fail to write");
        while (1);
    }

    u32DeltaTime = u32EndTime - u32StartTime;
    f32Speed     = (float)W25Qxx_NumByteToTest / u32DeltaTime;

    LOGD("success to write %d KB, total time: %d ms, speed %.2f KB/s", W25Qxx_NumByteToTest / 1024, u32DeltaTime, f32Speed);

    //
    // 读取
    //

    W25Qx_QSPI_EnterMemoryMappedMode();

    if (bStatus == false)
    {
        LOGE("fail to enter memory mapped mode");
        while (1);
    }

    memcpy(W25Qxx_ReadBuffer, (uint8_t*)W25Qxx_MEM_ADDR + W25Qxx_TestAddr, W25Qxx_NumByteToTest);

    if (memcmp(W25Qxx_WriteBuffer, W25Qxx_ReadBuffer, W25Qxx_NumByteToTest) != 0)  // 数据校验
    {
        LOGE("fail to check");
        while (1);
    }

    LOGD("check pass");

    //
    // 整片读取 测速
    //

    u32StartTime = HAL_GetTick();
    for (i = 0; i < CONFIG_W25Qxx_FLASH_SIZE / (W25Qxx_NumByteToTest); i++)
    {
        memcpy(W25Qxx_ReadBuffer, (u8*)W25Qxx_MEM_ADDR + W25Qxx_TestAddr, W25Qxx_NumByteToTest);
        W25Qxx_TestAddr = W25Qxx_TestAddr + W25Qxx_NumByteToTest;
    }
    u32EndTime = HAL_GetTick();

    u32DeltaTime = u32EndTime - u32StartTime;                                            // 计算擦除时间,单位ms
    f32Speed     = (float)CONFIG_W25Qxx_FLASH_SIZE / 1024 / 1024 / u32DeltaTime * 1000;  // 计算读取速度,单位 MB/S

    if (bStatus != true)
    {
        LOGE("fail to read");
        while (1);
    }

    LOGD("success to read %d MB, total time: %d ms, speed %.2f MB/s", CONFIG_W25Qxx_FLASH_SIZE / 1024 / 1024, u32DeltaTime, f32Speed);

    return true;
}

#endif