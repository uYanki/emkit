#include "spi_ds1302.h"
#include "hexdump.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "ds1302"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief access mode
 */
#define CMD_READ  0x81 /* Read command */
#define CMD_WRITE 0x80 /* Write command */

/**
 * @brief registers
 * @note  burst mode: 从地址0开始，单次读/写多个连续的寄存器
 */
#define REG_SEC        0x00 /* second, 秒 0-59 */
#define REG_MIN        0x01 /* minute, 分 0-59 */
#define REG_HOUR       0x02 /* hour, 时 1-12/1-24 */
#define REG_DATE       0x03 /* day of month, 年 0-99 */
#define REG_MON        0x04 /* month, 星期 1-7 */
#define REG_DAY        0x05 /* day of week, 月 1-12 */
#define REG_YEAR       0x06 /* year, 日 1-31 */
#define REG_CTRL       0x07 /* control */
#define REG_TCR        0x08 /* trickle charge, 涓流充电 */
#define REG_CLCK_BURST 0x1F /* clock burst */
#define REG_RAM0       0x20 /* RAM0 */
#define REG_RAM30      0x3F /* RAM31 */
#define REG_RAM_BURST  0x7F /* RAM burst */

/**
 * @brief bitmask for @ref REG_CTRL
 * bit[7] WP: write protect
 */
#define CMD_WRITE_ENABLE  0x00 /* Write enable */
#define CMD_WRITE_DISABLE 0x80 /* Write disable */

/**
 * @brief bitmask for @ref REG_SEC
 * bit[7] CH: clock halt
 */
#define CMD_CLOCK_HALT 0x80  // 时钟停振，进入低功耗态

/**
 * @brief bitmask for @ref REG_TCR
 * bit[7,4] TCS: 1010 慢充电。
 * bit[3,2] DS:  二极管个数选择。    01: 1个二极管; 10: 2个二极管; 11或00，禁止充电。
 * bit[1,0] RS:  二极管串联电阻选择。00，不充电；01，2KΩ电阻；10，4KΩ电阻；11，8KΩ电阻。
 */

/**
 * @brief ram info
 */
#define RAMSIZE (REG_RAM30 - REG_RAM0)

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

uint8_t hex2bcd(uint8_t hex)
{
    // return hex % 10 + hex / 10 * 16;
    return ((hex / 10) << 4) + (hex % 10);
}

uint8_t bcd2hex(uint8_t bcd)
{
    // return bcd % 16 + bcd / 16 * 10;
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static void DS1302_WriteMem(spi_ds1302_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data, uint8_t u8Size)
{
    SPI_Master_Select(pHandle->hSPI);

    u8MemAddr <<= 1;
    u8MemAddr |= CMD_WRITE;

    SPI_Master_TransmitBlock(pHandle->hSPI, &u8MemAddr, 1);
    SPI_Master_TransmitBlock(pHandle->hSPI, pu8Data, u8Size);

    SPI_Master_Deselect(pHandle->hSPI);
    SPI_Master_Deselect(pHandle->hSPI);
}

static uint8_t DS1302_ReadMem(spi_ds1302_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data, uint8_t u8Size)
{
    uint8_t u8Data = 0;

    SPI_Master_Select(pHandle->hSPI);

    u8MemAddr <<= 1;
    u8MemAddr |= CMD_READ;

    SPI_Master_TransmitBlock(pHandle->hSPI, &u8MemAddr, 1);
    SPI_Master_ReceiveBlock(pHandle->hSPI, pu8Data, u8Size);

    SPI_Master_Deselect(pHandle->hSPI);

    return u8Data;
}

static void DS1302_WriteByte(spi_ds1302_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data)
{
    DS1302_WriteMem(pHandle, u8MemAddr, &u8Data, 1);
}

static uint8_t DS1302_ReadByte(spi_ds1302_t* pHandle, uint8_t u8MemAddr)
{
    uint8_t u8Data;
    DS1302_ReadMem(pHandle, u8MemAddr, &u8Data, 1);
    return u8Data;
}

static void DS1302_WriteMemUnlock(spi_ds1302_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data, uint8_t u8Size)
{
    // Disable write protection
    DS1302_WriteByte(pHandle, REG_CTRL, CMD_WRITE_ENABLE);

    // Write User Data
    DS1302_WriteMem(pHandle, u8MemAddr, pu8Data, u8Size);

    // Enable write protection
    DS1302_WriteByte(pHandle, REG_CTRL, CMD_WRITE_DISABLE);
}

void DS1302_Init(spi_ds1302_t* pHandle)
{}

void DS1302_StartTimer(spi_ds1302_t* pHandle)  // 开始计时
{
    uint8_t u8Data = DS1302_ReadByte(pHandle, REG_SEC);

    if ((u8Data & CMD_CLOCK_HALT) != CMD_CLOCK_HALT)
    {
        return;
    }

    u8Data &= ~CMD_CLOCK_HALT;
    DS1302_WriteMemUnlock(pHandle, REG_CLCK_BURST, &u8Data, 1);
}

void DS1302_StopTimer(spi_ds1302_t* pHandle)  // 停止计时
{
    uint8_t u8Data = DS1302_ReadByte(pHandle, REG_SEC);

    if ((u8Data & CMD_CLOCK_HALT) == CMD_CLOCK_HALT)
    {
        return;
    }

    u8Data |= CMD_CLOCK_HALT;
    DS1302_WriteMemUnlock(pHandle, REG_CLCK_BURST, &u8Data, 1);
}

void DS1302_GetTime(spi_ds1302_t* pHandle, rtc_time_t* pTime)
{
    uint8_t au8Data[7] = {0};

    DS1302_ReadMem(pHandle, REG_CLCK_BURST, &au8Data[0], ARRAY_SIZE(au8Data));

    /* Decode the registers */
    pTime->u8Second   = bcd2hex(au8Data[REG_SEC] & 0x7F);
    pTime->u8Minute   = bcd2hex(au8Data[REG_MIN]);
    pTime->u8Hour     = bcd2hex(au8Data[REG_HOUR]);
    pTime->u8WeekDay  = bcd2hex(au8Data[REG_DAY]) - 1;
    pTime->u8MonthDay = bcd2hex(au8Data[REG_DATE]);
    pTime->u8Month    = bcd2hex(au8Data[REG_MON]) - 1;
    pTime->u8Year     = bcd2hex(au8Data[REG_YEAR]);
}

void DS1302_SetTime(spi_ds1302_t* pHandle, rtc_time_t* pTime)
{
    uint8_t au8Data[7];

    au8Data[REG_SEC]  = hex2bcd(pTime->u8Second);
    au8Data[REG_MIN]  = hex2bcd(pTime->u8Minute);
    au8Data[REG_HOUR] = hex2bcd(pTime->u8Hour);  // bit[7] 0:24小时制 1:12小时制
    au8Data[REG_DAY]  = hex2bcd(pTime->u8WeekDay);
    au8Data[REG_DATE] = hex2bcd(pTime->u8MonthDay + 1);
    au8Data[REG_MON]  = hex2bcd(pTime->u8Month + 1);
    au8Data[REG_YEAR] = hex2bcd(pTime->u8Year % 100);

    DS1302_WriteMemUnlock(pHandle, REG_CLCK_BURST, &au8Data[0], 7);
}

void DS1302_Hexdump(spi_ds1302_t* pHandle)
{
    uint8_t au8Data[RAMSIZE] = {0};
    DS1302_ReadMem(pHandle, REG_RAM_BURST, &au8Data[0], RAMSIZE);
    hexdump(&au8Data[0], RAMSIZE, 16, 1, true, nullptr, 0);
}

uint8_t DS1302_ReadRam(spi_ds1302_t* pHandle, uint8_t u8MemAddr)
{
    if (u8MemAddr >= RAMSIZE)
    {
        return 0x00;  // false
    }

    return DS1302_ReadByte(pHandle, u8MemAddr + REG_RAM0);
}

bool DS1302_WriteRam(spi_ds1302_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data)
{
    if (u8MemAddr > RAMSIZE)
    {
        return false;
    }

    DS1302_WriteMemUnlock(pHandle, REG_RAM0 + u8MemAddr, &u8Data, 1);

    return true;
}

void DS1302_ClearRam(spi_ds1302_t* pHandle)
{
    // RAM 无法使用 brust 模式写

    // Disable write protection
    DS1302_WriteByte(pHandle, REG_CTRL, CMD_WRITE_ENABLE);

    for (uint8_t u8MemAddr = 0; u8MemAddr < RAMSIZE; ++u8MemAddr)
    {
        // Clear Data
        DS1302_WriteByte(pHandle, REG_RAM0 + u8MemAddr, 0x00);
    }

    // Enable write protection
    DS1302_WriteByte(pHandle, REG_CTRL, CMD_WRITE_DISABLE);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#define PRINT_RTC_TIME(time) LOGI("%d/%d/%d %d:%d:%d", time.u8Year, time.u8Month, time.u8MonthDay, time.u8Hour, time.u8Minute, time.u8Second)
#define ADDR_RST_TIME        0x13
#define CMD_RST_TIME         0xF0

void DS1302_Test(void)
{
    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_5}, /* DAT */
        .MOSI = {GPIOA, GPIO_PIN_5},
        .SCLK = {GPIOA, GPIO_PIN_6}, /* CLK */
        .CS   = {GPIOA, GPIO_PIN_3}, /* RST */
    };

    spi_ds1302_t ds1302 = {
        .hSPI = &spi,
    };

    rtc_time_t time = {
        .u8Year     = 10,
        .u8Month    = 2,
        .u8MonthDay = 14,
        .u8WeekDay  = 1,
        .u8Hour     = 12,
        .u8Minute   = 34,
        .u8Second   = 56,
        .u8YearDay  = 0,  // unused
    };

    SPI_Master_Init(&spi, 50000, SPI_DUTYCYCLE_50_50, DS1302_SPI_TIMING | SPI_FLAG_SOFT_CS);

    DS1302_Init(&ds1302);

    DS1302_Hexdump(&ds1302);

    bool bResetTime = DS1302_ReadRam(&ds1302, ADDR_RST_TIME) != CMD_RST_TIME;

    if (bResetTime)  // 值不匹配时重置时间
    {
        DS1302_ClearRam(&ds1302);
        DS1302_WriteRam(&ds1302, ADDR_RST_TIME, CMD_RST_TIME);
        DS1302_Hexdump(&ds1302);

        LOGI("set time");
        PRINT_RTC_TIME(time);
        DS1302_SetTime(&ds1302, &time);  // 初始化时间
    }

    while (1)
    {
        static uint8_t i = 0;

        switch (i)
        {
            case 0:
            {
                LOGI("> stop timer");
                DS1302_StopTimer(&ds1302);
                ++i;
                break;
            }

            case 1 ... 3:
            {
                LOGI("> wait...");
                ++i;
                break;
            }

            case 4:
            {
                LOGI("> start timer");
                DS1302_StartTimer(&ds1302);
                ++i;
                break;
            }

            default:
            {
                break;
            }
        }

        DelayBlockMs(1000);
        DS1302_GetTime(&ds1302, &time);
        PRINT_RTC_TIME(time);
    }
}

#endif /* CONFIG_DEMOS_SW */
