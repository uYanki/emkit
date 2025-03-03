#if 1

#include "pindefs.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_pin"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static status_t    PIN_SetMode(const pin_t* pHandle, pin_mode_e eMode, pin_pull_e ePull);
static void        PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel);
static pin_level_e PIN_ReadLevel(const pin_t* pHandle);
static void        PIN_ToggleLevel(const pin_t* pHandle);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const pin_ops_t g_hwPinOps = {
    .SetMode     = PIN_SetMode,
    .ReadLevel   = PIN_ReadLevel,
    .WriteLevel  = PIN_WriteLevel,
    .ToggleLevel = PIN_ToggleLevel,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static status_t PIN_SetMode(const pin_t* pHandle, pin_mode_e eMode, pin_pull_e ePull)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    switch (eMode)
    {
        case PIN_MODE_INPUT_FLOATING: GPIO_InitStruct.Mode = GPIO_MODE_INPUT; break;
        case PIN_MODE_OUTPUT_OPEN_DRAIN: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; break;
        case PIN_MODE_OUTPUT_PUSH_PULL: GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; break;
        case PIN_MODE_HIGH_RESISTANCE: HAL_GPIO_DeInit(pHandle->Port, pHandle->Pin); return ERR_NONE;
        default: return ERR_INVALID_VALUE;  // invaild in/out mode
    }

    switch (ePull)
    {
        case PIN_PULL_NONE: GPIO_InitStruct.Pull = GPIO_NOPULL; break;
        case PIN_PULL_DOWN: GPIO_InitStruct.Pull = GPIO_PULLDOWN; break;
        case PIN_PULL_UP: GPIO_InitStruct.Pull = GPIO_PULLUP; break;
        default: return ERR_INVALID_VALUE;  // invaild pull mode
    }

    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pin   = pHandle->Pin;
    HAL_GPIO_Init(pHandle->Port, &GPIO_InitStruct);

    return ERR_NONE;
}

static void PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel)
{
    HAL_GPIO_WritePin(pHandle->Port, pHandle->Pin, (eLevel == PIN_LEVEL_LOW) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

static pin_level_e PIN_ReadLevel(const pin_t* pHandle)
{
    return (HAL_GPIO_ReadPin(pHandle->Port, pHandle->Pin) == GPIO_PIN_RESET) ? PIN_LEVEL_LOW : PIN_LEVEL_HIGH;
}

static void PIN_ToggleLevel(const pin_t* pHandle)
{
    HAL_GPIO_TogglePin(pHandle->Port, pHandle->Pin);
}

#endif

#if 1  // timebase source from systick

#include "delay.h"

tick_t GetTickUs(void)
{
    /**
     * @note SysTick 为 24-bit 递减计数器，当 SysTick->VAL 减到 0 时会将 SysTick->LOAD 里的值赋到 SysTick->VAL 里.
     */

    tick_t TimeUs   = 0;
    tick_t PeriodUs = 1000 / uwTickFreq;

    TimeUs += PeriodUs * HAL_GetTick();
    TimeUs += PeriodUs * (SysTick->LOAD - SysTick->VAL) / SysTick->LOAD;

    return TimeUs;
}

tick_t GetTickMs(void)
{
    return HAL_GetTick();
}

#endif

#if CONFIG_HWUART_MODULE_SW

/**
 * @note UART1 has IdleIsr (used in modbus), UART2 does't have IdleIsr
 */

#ifndef DBGUART_HANDLE
#define DBGUART_HANDLE huart1
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "dbguart"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

extern UART_HandleTypeDef DBGUART_HANDLE;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

int fputc(int ch, FILE* f)
{
    if (ch == '\n')
    {
        HAL_UART_Transmit(&DBGUART_HANDLE, (uint8_t*)"\r", 1, HAL_MAX_DELAY);
    }

    HAL_UART_Transmit(&DBGUART_HANDLE, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

    return (ch);
}

int fgetc(FILE* f)
{
    uint8_t ch = '\0';
    HAL_UART_Receive(&DBGUART_HANDLE, &ch, 1, HAL_MAX_DELAY);
    return ch;
}

#if defined(__CC_ARM) && 0

// auto switch uart direction when call printf

#include <stdarg.h>

// precall
int $Sub$$__2printf(const char* fmt, ...)
{
    extern int $Super$$__2printf(const char* fmt, ...);

    int len;

    // set tx dir
    Uart_SetWorkDir(UART_DIR_TX);

    // $Super$$__2printf("<*>");

    va_list args;
    va_start(args, fmt);
    len = vprintf(fmt, args);  // print
    va_end(args);

    // set rx dir
    Uart_SetWorkDir(UART_DIR_RX);

    return len;
}

#endif

#endif

#if CONFIG_HWI2C_MODULE_SW

#include "i2cdefs.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_i2cmst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

#define I2C_TIMEOUT     0xFF

#define MakeError_(eStatus)                       \
    do {                                          \
        switch (eStatus)                          \
        {                                         \
            case HAL_OK: return ERR_NONE;         \
            case HAL_BUSY: return ERR_BUSY;       \
            case HAL_TIMEOUT: return ERR_TIMEOUT; \
            default: return ERR_FAIL;             \
        }                                         \
    } while (0)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static status_t HWI2C_Master_Init(i2c_mst_t* pHandle, uint32_t u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle);
static bool     HWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags);
static status_t HWI2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags);
static status_t HWI2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags);
static status_t HWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags);
static status_t HWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const i2cmst_ops_t g_hwI2cOps = {
    .Init          = HWI2C_Master_Init,
    .IsDeviceReady = HWI2C_Master_IsDeviceReady,
    .ReadBlock     = HWI2C_Master_ReadBlock,
    .WriteBlock    = HWI2C_Master_WriteBlock,
    .ReceiveBlock  = HWI2C_Master_ReceiveBlock,
    .TransmitBlock = HWI2C_Master_TransmitBlock,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t HWI2C_Master_Init(i2c_mst_t* pHandle, uint32_t u32ClockFreqHz, i2c_duty_cycle_e eDutyCycle)
{
    I2C_HandleTypeDef* hwi2c = (I2C_HandleTypeDef*)(pHandle->I2Cx);

    if (
#ifdef I2C1
        hwi2c->Instance == I2C1 ||
#endif
#ifdef I2C2
        hwi2c->Instance == I2C2 ||
#endif
#ifdef I2C3
        hwi2c->Instance == I2C3 ||
#endif
#ifdef I2C4
        hwi2c->Instance == I2C4 ||
#endif
#ifdef I2C5
        hwi2c->Instance == I2C5 ||
#endif
        0)
    {
        return ERR_NONE;
    }

    // invalid instance
    return ERR_INVALID_VALUE;  // unknown instance
}

static inline bool HWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags)
{
    I2C_HandleTypeDef* hwi2c = (I2C_HandleTypeDef*)(pHandle->I2Cx);
    return HAL_I2C_IsDeviceReady(hwi2c, u16SlvAddr << 1, 5, I2C_TIMEOUT) == HAL_OK;
}

static inline status_t HWI2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    I2C_HandleTypeDef* hwi2c          = (I2C_HandleTypeDef*)(pHandle->I2Cx);
    uint16_t           u16MemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_MEMADDR_16BIT) ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT;
    MakeError_(HAL_I2C_Mem_Read(hwi2c, u16SlvAddr << 1, u16MemAddr, u16MemAddrSize, pu8Data, u16Size, I2C_TIMEOUT));
}

static inline status_t HWI2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    I2C_HandleTypeDef* hwi2c          = (I2C_HandleTypeDef*)(pHandle->I2Cx);
    uint16_t           u16MemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_MEMADDR_16BIT) ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT;
    MakeError_(HAL_I2C_Mem_Write(hwi2c, u16SlvAddr << 1, u16MemAddr, u16MemAddrSize, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

static inline status_t HWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    I2C_HandleTypeDef* hwi2c = (I2C_HandleTypeDef*)(pHandle->I2Cx);
    MakeError_(HAL_I2C_Master_Transmit(hwi2c, u16SlvAddr << 1, (uint8_t*)pu8Data, u16Size, I2C_TIMEOUT));
}

static inline status_t HWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    I2C_HandleTypeDef* hwi2c = (I2C_HandleTypeDef*)(pHandle->I2Cx);
    MakeError_(HAL_I2C_Master_Transmit(hwi2c, u16SlvAddr << 1, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

#endif

#if CONFIG_HWSPI_MODULE_SW

#include "spimst.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_hwspi"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

#define SPI_TIMEOUT     0xFF

#define MakeError_(eStatus)                       \
    do {                                          \
        switch (eStatus)                          \
        {                                         \
            case HAL_OK: return ERR_NONE;         \
            case HAL_BUSY: return ERR_BUSY;       \
            case HAL_TIMEOUT: return ERR_TIMEOUT; \
            default: return ERR_FAIL;             \
        }                                         \
    } while (0)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static status_t HWSPI_Master_Init(spi_mst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags);
static status_t HWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size);
static status_t HWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size);
static status_t HWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

const spimst_ops_t g_hwSpiOps = {
    .Init                 = HWSPI_Master_Init,
    .TransmitBlock        = HWSPI_Master_TransmitBlock,
    .ReceiveBlock         = HWSPI_Master_ReceiveBlock,
    .TransmitReceiveBlock = HWSPI_Master_TransmitReceiveBlock,
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static status_t HWSPI_Master_Init(spi_mst_t* pHandle, uint32_t u32ClockSpeedHz, spi_duty_cycle_e eDutyCycle, uint16_t u16Flags)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);

    if (
#ifdef SPI1
        hwspi->Instance == SPI1 ||
#endif
#ifdef SPI2
        hwspi->Instance == SPI2 ||
#endif
#ifdef SPI3
        hwspi->Instance == SPI3 ||
#endif
#ifdef SPI4
        hwspi->Instance == SPI4 ||
#endif
#ifdef SPI5
        hwspi->Instance == SPI5 ||
#endif
        0)
    {
        return ERR_NONE;
    }

    // invalid instance
    return ERR_INVALID_VALUE;  // unknown instance
}

static status_t HWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);
    MakeError_(HAL_SPI_Transmit(hwspi, (uint8_t*)cpu8TxData, u16Size, SPI_TIMEOUT));
}

static status_t HWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);
    MakeError_(HAL_SPI_Receive(hwspi, pu8RxData, u16Size, SPI_TIMEOUT));
}

static status_t HWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size)
{
    SPI_HandleTypeDef* hwspi = (SPI_HandleTypeDef*)(pHandle->SPIx);
    MakeError_(HAL_SPI_TransmitReceive(hwspi, (uint8_t*)cpu8TxData, pu8RxData, u16Size, SPI_TIMEOUT));
}

#endif
