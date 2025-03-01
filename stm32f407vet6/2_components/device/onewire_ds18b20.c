#include "onewire_ds18b20.h"

#define LOG_LOCAL_TAG       "ds18b20"
#define LOG_LOCAL_LEVEL     LOG_LEVEL_INFO

#define DS18B20_FAMILY_CODE 0x28

/**
 * @brief Commands
 */
#define DS18B20_CMD_SEARCH_ROM        0xF0  // search rom         搜索ROM
#define DS18B20_CMD_READ_ROM          0x33  // read rom           读取ROM
#define DS18B20_CMD_MATCH_ROM         0x55  // match rom          匹配ROM
#define DS18B20_CMD_SKIP_ROM          0xCC  // skip rom           忽略ROM
#define DS18B20_CMD_ALARM_SEARCH      0xEC  // alarm search       报警索索
#define DS18B20_CMD_CONVERT_T         0x44  // convert            温度转换
#define DS18B20_CMD_WRITE_SCRATCHPAD  0x4E  // write scrachpad    写暂存器
#define DS18B20_CMD_READ_SCRATCHPAD   0xBE  // read scrachpad     读取暂存器
#define DS18B20_CMD_COPY_SCRATCHPAD   0x48  // copy scrachpad     拷贝暂存器
#define DS18B20_CMD_RECALL_EE         0xB8  // recall ee          召回EEPROM
#define DS18B20_CMD_READ_POWER_SUPPLY 0xB4  // read power supply  读取电源模式

/**
 * @brief private function declarations
 */

static bool DS18B20_Reset(onewire_ds18b20_t* pHandle);
static bool DS18B20_SelectSlave(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode);

static void    OneWire_ResetSearch(onewire_ds18b20_t* pHandle);
static uint8_t OneWire_Search(onewire_ds18b20_t* pHandle, uint8_t u8Command);
static int     OneWire_Verify(onewire_ds18b20_t* pHandle);
static void    OneWire_TargetSetup(onewire_ds18b20_t* pHandle, uint8_t u8FamilyCode);
static void    OneWire_FamilySkipSetup(onewire_ds18b20_t* pHandle);

/**
 * @brief private function definitions
 */

static inline pin_level_e DS18B20_ReadBit(onewire_ds18b20_t* pHandle)
{
    pin_level_e eLevel;

    PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_LOW);

    PIN_SetMode(&pHandle->DQ, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);
    DelayBlockUs(3);
    PIN_SetMode(&pHandle->DQ, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

    DelayBlockUs(12);
    eLevel = PIN_ReadLevel(&pHandle->DQ);
    DelayBlockUs(50);

    return eLevel;
}

static inline uint8_t DS18B20_ReadByte(onewire_ds18b20_t* pHandle)
{
    uint8_t u8Data = 0x00, u8Mask;

    for (u8Mask = 0x01; u8Mask != 0; u8Mask <<= 1)
    {
        if (DS18B20_ReadBit(pHandle) == PIN_LEVEL_HIGH)
        {
            u8Data |= u8Mask;
        }
    }

    return u8Data;
}

static inline void DS18B20_WriteBit(onewire_ds18b20_t* pHandle, pin_level_e eLevel)
{
    PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_LOW);
    PIN_SetMode(&pHandle->DQ, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);

    if (eLevel == PIN_LEVEL_LOW)
    {
        // write 0
        PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_LOW);
        DelayBlockUs(60);  // 拉低总线 ≥60us
        PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_HIGH);
        DelayBlockUs(3);  // 恢复时间 >1us
    }
    else
    {
        // write 1
        PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_LOW);
        DelayBlockUs(3);  // 拉低总线 1~15us
        PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_HIGH);
        DelayBlockUs(60);  // 拉高总线 ≥60us
    }
}

static inline void DS18B20_WriteByte(onewire_ds18b20_t* pHandle, uint8_t u8Data)
{
    for (uint8_t u8Mask = 0x01; u8Mask != 0; u8Mask <<= 1)
    {
        DS18B20_WriteBit(pHandle, (u8Data & u8Mask) ? PIN_LEVEL_HIGH : PIN_LEVEL_LOW);
    }
}

/**
 * @brief dallas crc = x8 + x5 + x4 + 1
 */
static inline uint8_t DS18B20_CRC8(uint8_t* pData, uint8_t u8Size)
{
    bool    bMix;
    uint8_t u8Crc = 0;
    uint8_t u8Data;
    uint8_t j;

    while (u8Size--)
    {
        u8Data = *pData++;

        for (j = 0; j < 8; ++j)
        {
            bMix = (u8Crc ^ u8Data) & 0x01;
            u8Crc >>= 1;

            if (bMix)
            {
                // 与 0b10001100 作异或
                u8Crc ^= 0x8C;
            }

            u8Data >>= 1;
        }
    }

    return u8Crc;
}

/**
 * @return true  器件存在
 * @return false 器件不存在
 */
static bool DS18B20_Reset(onewire_ds18b20_t* pHandle)
{
    // 总线复位

    PIN_SetMode(&pHandle->DQ, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);

    PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_LOW);
    DelayBlockUs(500);

    PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_HIGH);
    DelayBlockUs(20);

    // 检查器件是否应答

    PIN_SetMode(&pHandle->DQ, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

    uint8_t u8Retries;

    for (u8Retries = 0; PIN_ReadLevel(&pHandle->DQ) == PIN_LEVEL_HIGH;)
    {
        if (++u8Retries == 200)
        {
            return false;
        }

        DelayBlockUs(1);
    }

    for (u8Retries = 0; PIN_ReadLevel(&pHandle->DQ) == PIN_LEVEL_LOW;)
    {
        if (++u8Retries == 240)
        {
            return false;
        }

        DelayBlockUs(1);
    }

    return true;
}

static bool DS18B20_SelectSlave(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode)
{
    if (romcode == nullptr)  // all slave
    {
        if (DS18B20_Reset(pHandle) == false)
        {
            return false;
        }

        DS18B20_WriteByte(pHandle, DS18B20_CMD_SKIP_ROM);
    }
    else  // target slave
    {
        uint8_t i;

        /**
         * @brief ROM
         *
         *    = 8 bits 固定标识 ( 1-Wire family code = 0x28 ) +
         *      48 bits 唯一序列号 +
         *      8 bits 的前面 56 bits 的 CRC 校验码.
         *
         */

        if (romcode->u8FamilyCode != DS18B20_FAMILY_CODE)
        {
            return false;
        }

        if (DS18B20_Reset(pHandle) == false)
        {
            return false;
        }

        DS18B20_WriteByte(pHandle, DS18B20_CMD_MATCH_ROM);

        for (i = 0; i < 8; i++)
        {
            DS18B20_WriteByte(pHandle, romcode->au8Data[i]);
        }
    }

    return true;
}

static void OneWire_ResetSearch(onewire_ds18b20_t* pHandle)
{
    /* Reset the search state */
    pHandle->u8LastDiscrepancy       = 0;
    pHandle->u8LastDeviceFlag        = 0;
    pHandle->u8LastFamilyDiscrepancy = 0;
}

static uint8_t OneWire_Search(onewire_ds18b20_t* pHandle, uint8_t u8Command)
{
    uint8_t id_bit_number;
    uint8_t last_zero, rom_byte_number, search_result;
    uint8_t id_bit, cmp_id_bit;
    uint8_t rom_byte_mask, search_direction;

    /* Initialize for search */
    id_bit_number   = 1;
    last_zero       = 0;
    rom_byte_number = 0;
    rom_byte_mask   = 1;
    search_result   = 0;

    // if the last call was not the last one
    if (!pHandle->u8LastDeviceFlag)
    {
        // 1-Wire reset
        if (DS18B20_Reset(pHandle) == false)
        {
            /* Reset the search */
            pHandle->u8LastDiscrepancy       = 0;
            pHandle->u8LastDeviceFlag        = 0;
            pHandle->u8LastFamilyDiscrepancy = 0;
            return 0;
        }

        // issue the search u8Command
        DS18B20_WriteByte(pHandle, u8Command);

        // loop to do the search
        do {
            // read a bit and its complement
            id_bit     = DS18B20_ReadBit(pHandle);
            cmp_id_bit = DS18B20_ReadBit(pHandle);

            // check for no devices on 1-wire
            if ((id_bit == 1) && (cmp_id_bit == 1))
            {
                break;
            }
            else
            {
                // all devices coupled have 0 or 1
                if (id_bit != cmp_id_bit)
                {
                    search_direction = id_bit;  // bit write value for search
                }
                else
                {
                    // if pHandle discrepancy if before the Last Discrepancy
                    // on a previous next then pick the same as last time
                    if (id_bit_number < pHandle->u8LastDiscrepancy)
                    {
                        search_direction = ((pHandle->romcode.au8Data[rom_byte_number] & rom_byte_mask) > 0);
                    }
                    else
                    {
                        // if equal to last pick 1, if not then pick 0
                        search_direction = (id_bit_number == pHandle->u8LastDiscrepancy);
                    }

                    // if 0 was picked then record its position in LastZero
                    if (search_direction == 0)
                    {
                        last_zero = id_bit_number;

                        // check for Last discrepancy in family
                        if (last_zero < 9)
                        {
                            pHandle->u8LastFamilyDiscrepancy = last_zero;
                        }
                    }
                }

                // set or clear the bit in the ROM byte rom_byte_number
                // with mask rom_byte_mask
                if (search_direction == 1)
                {
                    pHandle->romcode.au8Data[rom_byte_number] |= rom_byte_mask;
                }
                else
                {
                    pHandle->romcode.au8Data[rom_byte_number] &= ~rom_byte_mask;
                }

                // serial number search direction write bit
                DS18B20_WriteBit(pHandle, search_direction);

                // increment the byte counter id_bit_number
                // and shift the mask rom_byte_mask
                id_bit_number++;
                rom_byte_mask <<= 1;

                // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
                if (rom_byte_mask == 0)
                {
                    // docrc8(romcode.au8Data[rom_byte_number]);  // accumulate the CRC
                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            }
        } while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

        // if the search was successful then
        if (!(id_bit_number < 65))
        {
            // search successful so set u8LastDiscrepancy,u8LastDeviceFlag,search_result
            pHandle->u8LastDiscrepancy = last_zero;

            // check for last device
            if (pHandle->u8LastDiscrepancy == 0)
            {
                pHandle->u8LastDeviceFlag = 1;
            }

            search_result = 1;
        }
    }

    // if no device found then reset counters so next 'search' will be like a first
    if (!search_result || !pHandle->romcode.au8Data[0])
    {
        pHandle->u8LastDiscrepancy       = 0;
        pHandle->u8LastDeviceFlag        = 0;
        pHandle->u8LastFamilyDiscrepancy = 0;
        search_result                    = 0;
    }

    return search_result;
}

static int OneWire_Verify(onewire_ds18b20_t* pHandle)
{
    unsigned char rom_backup[8];
    int           i, rslt, ld_backup, ldf_backup, lfd_backup;

    // keep a backup copy of the current state
    for (i = 0; i < 8; i++)
    {
        rom_backup[i] = pHandle->romcode.au8Data[i];
    }
    ld_backup  = pHandle->u8LastDiscrepancy;
    ldf_backup = pHandle->u8LastDeviceFlag;
    lfd_backup = pHandle->u8LastFamilyDiscrepancy;

    // set search to find the same device
    pHandle->u8LastDiscrepancy = 64;
    pHandle->u8LastDeviceFlag  = 0;

    if (OneWire_Search(pHandle, DS18B20_CMD_SEARCH_ROM))
    {
        // check if same device found
        rslt = 1;
        for (i = 0; i < 8; i++)
        {
            if (rom_backup[i] != pHandle->romcode.au8Data[i])
            {
                rslt = 1;
                break;
            }
        }
    }
    else
    {
        rslt = 0;
    }

    // restore the search state
    for (i = 0; i < 8; i++)
    {
        pHandle->romcode.au8Data[i] = rom_backup[i];
    }
    pHandle->u8LastDiscrepancy       = ld_backup;
    pHandle->u8LastDeviceFlag        = ldf_backup;
    pHandle->u8LastFamilyDiscrepancy = lfd_backup;

    // return the result of the verify
    return rslt;
}

static void OneWire_TargetSetup(onewire_ds18b20_t* pHandle, uint8_t u8FamilyCode)
{
    uint8_t i;

    // set the search state to find SearchFamily type devices
    pHandle->romcode.au8Data[0] = u8FamilyCode;

    for (i = 1; i < 8; i++)
    {
        pHandle->romcode.au8Data[i] = 0;
    }

    pHandle->u8LastDiscrepancy       = 64;
    pHandle->u8LastFamilyDiscrepancy = 0;
    pHandle->u8LastDeviceFlag        = 0;
}

static void OneWire_FamilySkipSetup(onewire_ds18b20_t* pHandle)
{
    // set the Last discrepancy to last family discrepancy
    pHandle->u8LastDiscrepancy       = pHandle->u8LastFamilyDiscrepancy;
    pHandle->u8LastFamilyDiscrepancy = 0;

    // check for end of list
    if (pHandle->u8LastDiscrepancy == 0)
    {
        pHandle->u8LastDeviceFlag = 1;
    }
}

/**
 * @brief public function definitions
 */

uint8_t DS18B20_SearchFirst(onewire_ds18b20_t* pHandle)
{
    /* Reset search values */
    OneWire_ResetSearch(pHandle);

    /* Start with searching */
    return OneWire_Search(pHandle, DS18B20_CMD_SEARCH_ROM);
}

uint8_t DS18B20_SearchNext(onewire_ds18b20_t* pHandle)
{
    /* Leave the search state alone */
    return OneWire_Search(pHandle, DS18B20_CMD_SEARCH_ROM);
}

bool DS18B20_StartConv(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode)
{
    if (DS18B20_SelectSlave(pHandle, romcode) == false)
    {
        return false;
    }

    DS18B20_WriteByte(pHandle, DS18B20_CMD_CONVERT_T);

    return true;
}

/**
 * @param u16TimeoutUs 0: forever
 */
void DS18B20_PollForConversion(onewire_ds18b20_t* pHandle, uint16_t u16TimeoutUs)
{
    while (DS18B20_ReadBit(pHandle) == PIN_LEVEL_LOW)
    {
        DelayBlockUs(1);

        if (u16TimeoutUs > 0)
        {
            if (--u16TimeoutUs)
            {
                return;
            }
        }
    }
}

bool DS18B20_SetResolution(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, ds18b20_resolution_e eResolution)
{
    /* Read scratchpad u8Command by onewire protocol */

    if (DS18B20_SelectSlave(pHandle, romcode) == false)
    {
        return false;
    }

    DS18B20_WriteByte(pHandle, DS18B20_CMD_READ_SCRATCHPAD);
    DS18B20_ReadByte(pHandle); /* Ignore first 2 bytes */
    DS18B20_ReadByte(pHandle);

    uint8_t TH   = DS18B20_ReadByte(pHandle);
    uint8_t TL   = DS18B20_ReadByte(pHandle);
    uint8_t CONF = DS18B20_ReadByte(pHandle);

    switch (eResolution)
    {
        case DS18B20_RESOLUTION_9BIT:
        case DS18B20_RESOLUTION_10BIT:
        case DS18B20_RESOLUTION_11BIT:
        case DS18B20_RESOLUTION_12BIT:
        {
            CONF &= 0x9F;  // 0b10011111
            CONF |= eResolution << 5;
            break;
        }
        default:
        {
            return false;
        }
    }

    /* Write scratchpad u8Command by onewire protocol, only th, tl and conf register can be written */

    DS18B20_SelectSlave(pHandle, romcode);
    DS18B20_WriteByte(pHandle, DS18B20_CMD_WRITE_SCRATCHPAD);
    DS18B20_WriteByte(pHandle, TH);
    DS18B20_WriteByte(pHandle, TL);
    DS18B20_WriteByte(pHandle, CONF);

    /* Copy scratchpad to EEPROM of DS18B20 */

    DS18B20_SelectSlave(pHandle, romcode);
    DS18B20_WriteByte(pHandle, DS18B20_CMD_COPY_SCRATCHPAD);

    return true;
}

bool DS18B20_GetResolution(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, __OUT ds18b20_resolution_e* eResolution)
{
    if (DS18B20_SelectSlave(pHandle, romcode) == false)
    {
        return false;
    }

    DS18B20_WriteByte(pHandle, DS18B20_CMD_READ_SCRATCHPAD);

    DS18B20_ReadByte(pHandle);
    DS18B20_ReadByte(pHandle);
    DS18B20_ReadByte(pHandle);
    DS18B20_ReadByte(pHandle);

    /* 5th byte of scratchpad is configuration register */
    *eResolution = (DS18B20_ReadByte(pHandle) & 0x60) >> 5;

    return true;
}

uint8_t DS18B20_ReadTemp(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, __OUT float32_t* f32Temperature)
{
    float32_t f32DecimalPart;
    uint8_t   au8Data[8];
    uint8_t   u8Crc;

#if 1
    /* Check if line is released, if it is, then conversion is complete */
    if (!DS18B20_ReadBit(pHandle))
    {
        /* Conversion is not finished yet */
        return 0;
    }
#endif

    if (DS18B20_SelectSlave(pHandle, romcode) == false)
    {
        return false;
    }

    DS18B20_WriteByte(pHandle, DS18B20_CMD_READ_SCRATCHPAD);

    /* Read data */
    au8Data[0] = DS18B20_ReadByte(pHandle); /* Temperature LSB */
    au8Data[1] = DS18B20_ReadByte(pHandle); /* Temperature MSB */
    au8Data[2] = DS18B20_ReadByte(pHandle); /* Alarm High Byte    */
    au8Data[3] = DS18B20_ReadByte(pHandle); /* Alarm Low Byte     */
    au8Data[4] = DS18B20_ReadByte(pHandle); /* Reserved Byte 1    */
    au8Data[5] = DS18B20_ReadByte(pHandle); /* Reserved Byte 2    */
    au8Data[6] = DS18B20_ReadByte(pHandle); /* Count Remain Byte  */
    au8Data[7] = DS18B20_ReadByte(pHandle); /* Count Per degree C */
    u8Crc      = DS18B20_ReadByte(pHandle); /* CRC */

    DS18B20_Reset(pHandle);

    if (u8Crc != DS18B20_CRC8(au8Data, 8))
    {
        // PRINTLN(" crc error");
        return false;  // CRC 校验失败
    }

    /* First two bytes of scratchpad are temperature values */
    uint16_t u16RawTemp = au8Data[0] | (au8Data[1] << 8);

    ds18b20_resolution_e eResolution = (au8Data[4] & 0x60) >> 5;  // 分辨率

    bool bMinus = (u16RawTemp & 0xF000) != 0;  // 符号位: 正数0b0000, 负数0b1111

    if (bMinus)
    {
        u16RawTemp = ~u16RawTemp + 1;
    }

    uint8_t s8DigitPart   = (u16RawTemp & 0x0FF0) >> 4;  // 整数部分
    uint8_t s8DecimalPart = u16RawTemp & 0x000F;         // 小数部分

    switch (eResolution)
    {
        case DS18B20_RESOLUTION_9BIT:
        {
            f32DecimalPart = ((s8DecimalPart >> 3) & 0x01) * 0.5f;
            break;
        }
        case DS18B20_RESOLUTION_10BIT:
        {
            f32DecimalPart = ((s8DecimalPart >> 2) & 0x03) * 0.25f;
            break;
        }
        case DS18B20_RESOLUTION_11BIT:
        {
            f32DecimalPart = ((s8DecimalPart >> 1) & 0x07) * 0.125f;
            break;
        }
        case DS18B20_RESOLUTION_12BIT:
        {
            f32DecimalPart = ((s8DecimalPart >> 0) & 0x0F) * 0.0625f;
            break;
        }
        default:
        {
            f32DecimalPart = 0xFF;
            s8DigitPart    = 0;
            break;
        }
    }

    *f32Temperature = s8DigitPart + f32DecimalPart;

    if (bMinus)
    {
        *f32Temperature = -*f32Temperature;
    }

    return true;
}

/**
 * @brief 总线上单个DS18B20时适用
 */
bool DS18B20_ReadROM(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode)
{
    uint8_t i;

    if (DS18B20_Reset(pHandle) == false)
    {
        return false;
    }

    DS18B20_WriteByte(pHandle, DS18B20_CMD_READ_ROM);

    for (i = 0; i < 8; i++)
    {
        romcode->au8Data[i] = DS18B20_ReadByte(pHandle);
    }

    DS18B20_Reset(pHandle);

#if 0  // log it
    PRINTF("ROM:");
    for (i = 0; i < 8; i++)
    {
        PRINTF(" %02X", romcode->au8Data[i]);
    }
    PRINTF(", CRC %s", (DS18B20_CRC8(romcode->au8Data, 7) == romcode->au8Data[7]) ? "ok" : "err");
    PRINTLN();
#endif

    return true;
}

bool DS18B20_SetAlarmTemperature(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, int8_t s8LowTemp, int8_t s8HighTemp)
{
    /* Read scratchpad u8Command by onewire protocol */

    if (DS18B20_SelectSlave(pHandle, romcode) == false)
    {
        return false;
    }

    DS18B20_WriteByte(pHandle, DS18B20_CMD_READ_SCRATCHPAD);

    /* Ignore first 2 bytes */
    DS18B20_ReadByte(pHandle);
    DS18B20_ReadByte(pHandle);

    uint8_t TH   = DS18B20_ReadByte(pHandle);
    uint8_t TL   = DS18B20_ReadByte(pHandle);
    uint8_t CONF = DS18B20_ReadByte(pHandle);

    if (s8LowTemp > 125)
    {
        s8LowTemp = 125;
    }
    else if (s8LowTemp < -55)
    {
        s8LowTemp = -55;
    }

    if (s8HighTemp > 125)
    {
        s8HighTemp = 125;
    }
    else if (s8HighTemp < -55)
    {
        s8HighTemp = -55;
    }

    TL = (uint8_t)s8LowTemp;
    TH = (uint8_t)s8HighTemp;

    /* Write scratchpad u8Command by onewire protocol, only th, tl and conf register can be written */

    DS18B20_SelectSlave(pHandle, romcode);
    DS18B20_WriteByte(pHandle, DS18B20_CMD_WRITE_SCRATCHPAD);
    DS18B20_WriteByte(pHandle, TH);
    DS18B20_WriteByte(pHandle, TL);
    DS18B20_WriteByte(pHandle, CONF);

    /* Copy scratchpad to EEPROM of DS18B20 */

    DS18B20_SelectSlave(pHandle, romcode);
    DS18B20_WriteByte(pHandle, DS18B20_CMD_COPY_SCRATCHPAD);

    return true;
}

bool DS18B20_DisableAlarmTemperature(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode)
{
    return DS18B20_SetAlarmTemperature(pHandle, romcode, -55, 125);
}

uint8_t DS18B20_AlarmSearch(onewire_ds18b20_t* pHandle)
{
    /* Start alarm search */
    return OneWire_Search(pHandle, DS18B20_CMD_ALARM_SEARCH);
}

#if CONFIG_DEMOS_SW

void DS18B20_Test()
{
#if 0  // 单设备

    onewire_ds18b20_t ds18b20 = {
        .DQ = &PA6,
        // .DQ = &DS18B20_DQ_PIN,
    };

    float32_t f32Temperature;

    ds18b20_romcode_t ROM = {0};
    DS18B20_ReadROM(&ds18b20, &ROM);

    while (1)
    {
        if (DS18B20_StartConv(&ds18b20, nullptr))
        {
            DS18B20_PollForConversion(&ds18b20, 0);

            if (DS18B20_ReadTemp(&ds18b20, nullptr, &f32Temperature))
            {
                PRINTLN("T = %.5f", f32Temperature);
            }
        }
    }

#else  // 多设备

#define DS18B20_MAX_SEARCH_SIZE 10  // max 64 devices

    uint8_t           devices, i, j, u8SlaveCount, u8AlarmedCount;
    ds18b20_romcode_t aSlave[DS18B20_MAX_SEARCH_SIZE];
    ds18b20_romcode_t aAlarmedSlave[DS18B20_MAX_SEARCH_SIZE];
    float32_t         af32SlaveTemps[DS18B20_MAX_SEARCH_SIZE];

    /* OneWire working struct */

    onewire_ds18b20_t onewire = {
        .DQ = {GPIOA, GPIO_PIN_6},
    };

    /* Checks for any device on 1-wire */
    u8SlaveCount = 0;
    devices      = DS18B20_SearchFirst(&onewire);
    while (devices)
    {
        /* Increase counter */
        u8SlaveCount++;

        /* Get full ROM value, 8 bytes, give location of first byte where to save */
        aSlave[u8SlaveCount - 1].u64RomCode = onewire.romcode.u64RomCode;

        /* Get next device */
        devices = DS18B20_SearchNext(&onewire);
    }

    /* If any devices on 1wire */
    if (u8SlaveCount > 0)
    {
        PRINTF("Devices found on 1-wire: %d\n", u8SlaveCount);

        /* Display 64bit rom code for each device */

        for (j = 0; j < u8SlaveCount; j++)
        {
            for (i = 0; i < 8; i++)
            {
                PRINTF("0x%02X ", aSlave[j].au8Data[i]);
            }

            PRINTF("\n");
        }
    }
    else
    {
        PRINTF("No devices on OneWire.\n");
    }

    /* Go through all connected devices and set resolution to 12bits */
    for (i = 0; i < u8SlaveCount; i++)
    {
        /* Set resolution to 12bits */
        DS18B20_SetResolution(&onewire, &aSlave[i], DS18B20_RESOLUTION_12BIT);
        DS18B20_DisableAlarmTemperature(&onewire, &aSlave[i]);
    }

    /* Set high temperature alarm on device number 0, 25 degrees celcius */
    DS18B20_SetAlarmTemperature(&onewire, &aSlave[0], 0, 35);

    while (1)
    {
        /* Start temperature conversion on all devices on one bus */
        DS18B20_StartConv(&onewire, nullptr);

        /* Wait until all are done on one onewire port */
        DS18B20_PollForConversion(&onewire, 0);

        /* Read temperature from each device separatelly */
        for (i = 0; i < u8SlaveCount; i++)
        {
            /* Read temperature from ROM address and store it to temps variable */
            if (DS18B20_ReadTemp(&onewire, &aSlave[i], &af32SlaveTemps[i]))
            {
                /* PRINT temperature */
                PRINTF("Temp %d: %3.5f; \n", i, af32SlaveTemps[i]);
            }
            else
            {
                /* Reading error */
                PRINTF("Reading error;\n");
            }
        }

        /* Reset alarm count */
        u8AlarmedCount = 0;

        /* Check if any device has alarm flag set */
        while (DS18B20_AlarmSearch(&onewire))
        {
            /* Store ROM of device which has alarm flag set */
            aAlarmedSlave[u8AlarmedCount].u64RomCode = onewire.romcode.u64RomCode;
            /* Increase count */
            u8AlarmedCount++;
        }

        /* Format string and send over USART for debug */
        PRINTF("Devices with alarm: %d\n", u8AlarmedCount);

        /* Any device has alarm flag set? */
        if (u8AlarmedCount > 0)
        {
            /* Show romcode of pHandle devices */
            for (j = 0; j < u8AlarmedCount; j++)
            {
                PRINTF("Device with alarm: ");
                for (i = 0; i < 8; i++)
                {
                    PRINTF("0x%02X ", aAlarmedSlave[j].au8Data[i]);
                }
                PRINTF("\n    ");
            }
            PRINTF("ALARM devices recognized!\n\r");
        }

        /* PRINT separator */
        PRINTLN("----------");

        /* Some delay */
        DelayBlockMs(1000);
    }
#endif
}

#endif
