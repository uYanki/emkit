

#include "typebasic.h"

#if 1  // Pin Ctrl

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
    gpio_init_type gpio_init_struct;

    switch (eMode)
    {
        case PIN_MODE_INPUT_FLOATING: gpio_init_struct.gpio_mode = GPIO_MODE_INPUT; break;
        case PIN_MODE_OUTPUT_OPEN_DRAIN: gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT, gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN; break;
        case PIN_MODE_OUTPUT_PUSH_PULL: gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT, gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL; break;
        case PIN_MODE_HIGH_RESISTANCE: return ERR_NOT_SUPPORTED;  // unsupported mode
        default: return ERR_INVALID_VALUE;                        // invaild in/out mode
    }

    switch (ePull)
    {
        case PIN_PULL_NONE: gpio_init_struct.gpio_pull = GPIO_PULL_NONE; break;
        case PIN_PULL_DOWN: gpio_init_struct.gpio_pull = GPIO_PULL_DOWN; break;
        case PIN_PULL_UP: gpio_init_struct.gpio_pull = GPIO_PULL_UP; break;
        default: return ERR_INVALID_VALUE;  // invaild pull mode
    }

    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_pins           = pHandle->Pin;

    gpio_init(pHandle->Port, &gpio_init_struct);

    return ERR_NONE;
}

static void PIN_WriteLevel(const pin_t* pHandle, pin_level_e eLevel)
{
    gpio_type* gpio = (gpio_type*)(pHandle->Port);
    uint16_t   pin  = pHandle->Pin;

    (eLevel == PIN_LEVEL_LOW) ? (gpio->clr = pin) : (gpio->scr = pin);
}

static pin_level_e PIN_ReadLevel(const pin_t* pHandle)
{
    gpio_type* gpio = (gpio_type*)(pHandle->Port);
    uint16_t   pin  = pHandle->Pin;

    return (pin != (pin & gpio->idt)) ? PIN_LEVEL_LOW : PIN_LEVEL_HIGH;
}

static void PIN_ToggleLevel(const pin_t* pHandle)
{
    gpio_type* gpio = (gpio_type*)(pHandle->Port);
    uint16_t   pin  = pHandle->Pin;

    gpio->odt ^= pin;
}

#endif

#if CONFIG_HWI2C_MODULE_SW

#include "i2cdefs.h"
#include "i2c_application.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_i2cmst"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#define I2C_TIMEOUT     0xFF

#define MakeError_(eStatus)                     \
    do {                                        \
        switch (eStatus)                        \
        {                                       \
            case I2C_OK: return ERR_NONE;       \
            case I2C_ERR_BUSY: return ERR_BUSY; \
            default: return ERR_TIMEOUT;        \
        }                                       \
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
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);

    if (hwi2c == I2C1)
    {
        crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
    }
    else if (hwi2c == I2C2)
    {
        crm_periph_clock_enable(CRM_I2C2_PERIPH_CLOCK, TRUE);
    }
    else
    {
        return ERR_INVALID_VALUE;  // unknown instance
    }

    if (pHandle->SDA.Port == GPIOA || pHandle->SCL.Port == GPIOA)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    }
    else if (pHandle->SDA.Port == GPIOB || pHandle->SCL.Port == GPIOB)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    }

    gpio_init_type gpio_initstructure = {
        .gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN,
        .gpio_pull           = GPIO_PULL_UP,
        .gpio_mode           = GPIO_MODE_MUX,
        .gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE,
    };

    gpio_initstructure.gpio_pins = pHandle->SDA.Pin,
    gpio_init(pHandle->SDA.Port, &gpio_initstructure);

    gpio_initstructure.gpio_pins = pHandle->SCL.Pin,
    gpio_init(pHandle->SCL.Port, &gpio_initstructure);

    if (u32ClockFreqHz == 0)
    {
        u32ClockFreqHz = 100000;  // 100k
    }

    i2c_reset(hwi2c);
    i2c_init(hwi2c, I2C_FSMODE_DUTY_2_1, u32ClockFreqHz);
    i2c_own_address1_set(hwi2c, I2C_ADDRESS_MODE_7BIT, 0xA0);
    i2c_enable(hwi2c, TRUE);

    return ERR_NONE;
}

static inline bool HWI2C_Master_IsDeviceReady(i2c_mst_t* pHandle, uint8_t u16SlvAddr, uint16_t u16Flags)
{
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);

    /* wait for the busy flag to be reset */
    if (i2c_wait_flag(hwi2c, I2C_BUSYF_FLAG, I2C_EVENT_CHECK_NONE, I2C_TIMEOUT) != I2C_OK)
    {
        return false;
    }

    /* ack acts on the current byte */
    i2c_master_receive_ack_set(hwi2c, I2C_MASTER_ACK_CURRENT);

    /* send slave address */
    if (i2c_master_write_addr(hwi2c, u16SlvAddr << 1, I2C_TIMEOUT) != I2C_OK)
    {
        /* generate stop condtion */
        i2c_stop_generate(hwi2c);

        return false;
    }

    /* clear addr flag */
    i2c_flag_clear(hwi2c, I2C_ADDR7F_FLAG);

    /* generate stop condtion */
    i2c_stop_generate(hwi2c);

    return true;
}

static inline status_t HWI2C_Master_ReadBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type*                  hwi2c        = (i2c_type*)(pHandle->I2Cx);
    i2c_mem_address_width_type eMemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_MEMADDR_16BIT) ? I2C_MEM_ADDR_WIDIH_16 : I2C_MEM_ADDR_WIDIH_8;
    MakeError_(i2c_memory_read(hwi2c, eMemAddrSize, u16SlvAddr << 1, u16MemAddr, pu8Data, u16Size, I2C_TIMEOUT));
}

static inline status_t HWI2C_Master_WriteBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint16_t u16MemAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type*                  hwi2c        = (i2c_type*)(pHandle->I2Cx);
    i2c_mem_address_width_type eMemAddrSize = CHKMSK16(u16Flags, I2C_FLAG_MEMADDR_SIZE_Msk, I2C_FLAG_MEMADDR_16BIT) ? I2C_MEM_ADDR_WIDIH_16 : I2C_MEM_ADDR_WIDIH_8;
    MakeError_(i2c_memory_write(hwi2c, eMemAddrSize, u16SlvAddr << 1, u16MemAddr, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

static inline status_t HWI2C_Master_ReceiveBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, uint8_t* pu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);
    MakeError_(i2c_master_receive(hwi2c, u16SlvAddr << 1, pu8Data, u16Size, I2C_TIMEOUT));
}

static inline status_t HWI2C_Master_TransmitBlock(i2c_mst_t* pHandle, uint16_t u16SlvAddr, const uint8_t* cpu8Data, uint16_t u16Size, uint16_t u16Flags)
{
    i2c_type* hwi2c = (i2c_type*)(pHandle->I2Cx);
    MakeError_(i2c_master_transmit(hwi2c, u16SlvAddr << 1, (uint8_t*)cpu8Data, u16Size, I2C_TIMEOUT));
}

#endif

#if CONFIG_HWSPI_MODULE_SW

#include "spimst.h"

/**
 * @test 全双工收发(有bug)，半双工接收 待测试
 */

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "drv_hwspi"
#define LOG_LOCAL_LEVEL LOG_LEVEL_QUIET

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
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    // 暂不支持 GPIO REMAP !!!

    if (hwspi == SPI1)
    {
        /* SPI1: PA4/CS, PA5/SCLK, PA6/MISO, PA7/MOSI */
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
        crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
    }
    else if (hwspi == SPI2)
    {
        /* SPI2: PB12/CS, PB13/SCLK, PB14/MISO, PB15/MOSI */
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
    }
    else
    {
        return ERR_INVALID_VALUE;  // unknown instance
    }

    gpio_init_type gpio_init_struct = {
        .gpio_out_type       = GPIO_OUTPUT_PUSH_PULL,
        .gpio_pull           = GPIO_PULL_UP,
        .gpio_mode           = GPIO_MODE_MUX,
        .gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER,
    };

    spi_init_type spi_init_struct = {0};

    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;

    if (u32ClockSpeedHz >= 50000000)
    {
        spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_2;  // 50M
    }
    else if (u32ClockSpeedHz >= 25000000)
    {
        spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_4;  // 25M
    }
    else
    {
        spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;  // 12.5M
    }

    switch (u16Flags & SPI_FLAG_FIRSTBIT_Msk)
    {
        case SPI_FLAG_LSBFIRST: spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_LSB; break;
        case SPI_FLAG_MSBFIRST: spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB; break;
    }

    switch (u16Flags & SPI_FLAG_CPOL_Msk)
    {
        case SPI_FLAG_CPOL_LOW: spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW; break;
        case SPI_FLAG_CPOL_HIGH: spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH; break;
    }

    switch (u16Flags & SPI_FLAG_CPHA_Msk)
    {
        case SPI_FLAG_CPHA_1EDGE: spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE; break;
        case SPI_FLAG_CPHA_2EDGE: spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE; break;
    }

    switch (u16Flags & SPI_FLAG_DATAWIDTH_Msk)
    {
        case SPI_FLAG_DATAWIDTH_8B: spi_init_struct.frame_bit_num = SPI_FRAME_8BIT; break;
        case SPI_FLAG_DATAWIDTH_16B: spi_init_struct.frame_bit_num = SPI_FRAME_16BIT; break;
        case SPI_FLAG_DATAWIDTH_32B: return ERR_NOT_SUPPORTED;  // unsupported datawidth
    }

    switch (u16Flags & SPI_FLAG_CS_MODE_Msk)
    {
        case SPI_FLAG_NONE_CS:
        case SPI_FLAG_SOFT_CS:
        {
            spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
            break;
        }
        case SPI_FLAG_HADR_CS:
        {
            spi_init_struct.cs_mode_selection = SPI_CS_HARDWARE_MODE;

            gpio_init_struct.gpio_pins = pHandle->CS.Pin;
            gpio_init(pHandle->CS.Pin, &gpio_init_struct);

            break;
        }
    }

    switch (u16Flags & SPI_FLAG_WIRE_Msk)
    {
        case SPI_FLAG_3WIRE:
        {
            gpio_init_struct.gpio_pins = pHandle->MOSI.Pin;
            gpio_init(pHandle->MOSI.Pin, &gpio_init_struct);

            gpio_init_struct.gpio_pins = pHandle->MISO.Pin;
            gpio_init(pHandle->MISO.Port, &gpio_init_struct);

            spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;

            break;
        }

        case SPI_FLAG_4WIRE:
        {
            gpio_init_struct.gpio_pins = pHandle->MOSI.Pin;
            gpio_init(pHandle->MOSI.Pin, &gpio_init_struct);

            gpio_init_struct.gpio_pins = pHandle->MISO.Pin;
            gpio_init(pHandle->MISO.Port, &gpio_init_struct);
            spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;

            break;
        }
    }

    gpio_init_struct.gpio_pins = pHandle->SCLK.Pin;
    gpio_init(pHandle->SCLK.Port, &gpio_init_struct);

    spi_init(hwspi, &spi_init_struct);
    spi_enable(hwspi, TRUE);

    return ERR_NONE;
}

static status_t HWSPI_Master_TransmitBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint16_t u16Size)
{
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    if (CHKMSK16(pHandle->u16TimingConfig, SPI_FLAG_WIRE_Msk, SPI_FLAG_3WIRE))
    {
        spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_TX);
    }

    while (u16Size--)
    {
        while (spi_i2s_flag_get(hwspi, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(hwspi, *cpu8TxData++);
    }

    return ERR_NONE;
}

static status_t HWSPI_Master_ReceiveBlock(spi_mst_t* pHandle, uint8_t* pu8RxData, uint16_t u16Size)
{
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    if (CHKMSK16(pHandle->u16TimingConfig, SPI_FLAG_WIRE_Msk, SPI_FLAG_3WIRE))
    {
        spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_RX);
    }

    while (u16Size--)
    {
        while (spi_i2s_flag_get(hwspi, SPI_I2S_RDBF_FLAG) == RESET);
        *pu8RxData++ = spi_i2s_data_receive(hwspi);
    }

    return ERR_NONE;
}

static status_t HWSPI_Master_TransmitReceiveBlock(spi_mst_t* pHandle, const uint8_t* cpu8TxData, uint8_t* pu8RxData, uint16_t u16Size)
{
    spi_type* hwspi = (spi_type*)(pHandle->SPIx);

    if (CHKMSK16(pHandle->u16TimingConfig, SPI_FLAG_WIRE_Msk, SPI_FLAG_3WIRE))
    {
        while (u16Size--)
        {
            spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_TX);
            spi_i2s_data_transmit(hwspi, *cpu8TxData++);
            while (spi_i2s_flag_get(hwspi, SPI_I2S_TDBE_FLAG) == RESET);

            spi_half_duplex_direction_set(hwspi, SPI_TRANSMIT_HALF_DUPLEX_RX);
            while (spi_i2s_flag_get(hwspi, SPI_I2S_RDBF_FLAG) == RESET);
            *pu8RxData++ = spi_i2s_data_receive(hwspi);
        }
    }
    else
    {
        while (u16Size--)
        {
            spi_i2s_data_transmit(hwspi, *cpu8TxData++);
            while (spi_i2s_flag_get(hwspi, SPI_I2S_TDBE_FLAG) == RESET);

            while (spi_i2s_flag_get(hwspi, SPI_I2S_RDBF_FLAG) == RESET);
            *pu8RxData++ = spi_i2s_data_receive(hwspi);
        }
    }

    return ERR_NONE;
}

#endif

#if CONFIG_HWUART_MODULE_SW

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG           "drv_hwuart"
#define LOG_LOCAL_LEVEL         LOG_LEVEL_INFO

#define DBGUART_BASE            USART1
#define DBGUART_CRM_CLK         CRM_USART1_PERIPH_CLOCK
#define DBGUART_TX_PIN          GPIO_PINS_9
#define DBGUART_TX_GPIO         GPIOA
#define DBGUART_TX_GPIO_CRM_CLK CRM_GPIOA_PERIPH_CLOCK

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void DbgUartInit(uint32_t u32Baudrate)
{
    gpio_init_type gpio_init_struct;

#if defined(__GNUC__) && !defined(__clang__)
    setvbuf(stdout, NULL, _IONBF, 0);
#endif

    /* enable the uart and gpio clock */
    crm_periph_clock_enable(DBGUART_CRM_CLK, TRUE);
    crm_periph_clock_enable(DBGUART_TX_GPIO_CRM_CLK, TRUE);

    gpio_default_para_init(&gpio_init_struct);

    /* configure the uart tx pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins           = DBGUART_TX_PIN;
    gpio_init_struct.gpio_pull           = GPIO_PULL_NONE;
    gpio_init(DBGUART_TX_GPIO, &gpio_init_struct);

    /* configure uart param */
    usart_init(DBGUART_BASE, u32Baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
    usart_transmitter_enable(DBGUART_BASE, TRUE);
    usart_enable(DBGUART_BASE, TRUE);
}

/* support printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
__asm(".global __use_no_semihosting\n\t");
void _sys_exit(int x)
{
    x = x;
}
/* __use_no_semihosting was requested, but _ttywrch was */
void _ttywrch(int ch)
{
    ch = ch;
}
FILE __stdout;
#else
#ifdef __CC_ARM
#pragma import(__use_no_semihosting)
struct __FILE {
    int handle;
};
FILE __stdout;
void _sys_exit(int x)
{
    x = x;
}
/* __use_no_semihosting was requested, but _ttywrch was */
void _ttywrch(int ch)
{
    ch = ch;
}
#endif
#endif

#if defined(__GNUC__) && !defined(__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE* f)
#endif

/**
 * @brief  retargets the c library printf function to the usart.
 * @param  none
 * @retval none
 */
PUTCHAR_PROTOTYPE
{
    while (usart_flag_get(DBGUART_BASE, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(DBGUART_BASE, (uint16_t)ch);
    while (usart_flag_get(DBGUART_BASE, USART_TDC_FLAG) == RESET);
    return ch;
}

#if (defined(__GNUC__) && !defined(__clang__)) || (defined(__ICCARM__))
#if defined(__GNUC__) && !defined(__clang__)
int _write(int fd, char* pbuffer, int size)
#elif defined(__ICCARM__)
#pragma module_name = "?__write"
int __write(int fd, char* pbuffer, int size)
#endif
{
    for (int i = 0; i < size; i++)
    {
        while (usart_flag_get(DBGUART_BASE, USART_TDBE_FLAG) == RESET);
        usart_data_transmit(DBGUART_BASE, (uint16_t)(*pbuffer++));
        while (usart_flag_get(DBGUART_BASE, USART_TDC_FLAG) == RESET);
    }

    return size;
}
#endif

#endif
