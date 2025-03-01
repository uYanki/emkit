#ifndef __ONEWIRE_DS18B20_H__
#define __ONEWIRE_DS18B20_H__

#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef enum {
    DS18B20_RESOLUTION_9BIT  = 0x00, /*!< 9 bit resolution */
    DS18B20_RESOLUTION_10BIT = 0x01, /*!< 10 bit resolution */
    DS18B20_RESOLUTION_11BIT = 0x02, /*!< 11 bit resolution */
    DS18B20_RESOLUTION_12BIT = 0x03, /*!< 12 bit resolution */
} ds18b20_resolution_e;

typedef union {
    uint8_t  au8Data[8];
    uint64_t u64RomCode;
    struct {
        uint8_t u8FamilyCode;       // 编码
        uint8_t u8SerialNumber[6];  // 序列号SN
        uint8_t u8CRC;              // 校验码
    };
} ds18b20_romcode_t;

typedef struct {
    __IN const pin_t DQ;

    uint8_t u8LastDiscrepancy;       /*!< Search private */
    uint8_t u8LastFamilyDiscrepancy; /*!< Search private */
    uint8_t u8LastDeviceFlag;        /*!< Search private */

    ds18b20_romcode_t romcode; /*!< 8-bytes address of last search device */
} onewire_ds18b20_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool DS18B20_StartConv(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode);
void DS18B20_PollForConversion(onewire_ds18b20_t* pHandle, uint16_t u16TimeoutUs);

bool DS18B20_SetResolution(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, ds18b20_resolution_e eResolution);
bool DS18B20_GetResolution(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, __OUT ds18b20_resolution_e* eResolution);

uint8_t DS18B20_ReadTemp(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, __OUT float32_t* f32Temperature);

bool DS18B20_ReadROM(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode);

bool DS18B20_SetAlarmTemperature(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode, int8_t s8LowTemp, int8_t s8HighTemp);
bool DS18B20_DisableAlarmTemperature(onewire_ds18b20_t* pHandle, ds18b20_romcode_t* romcode);

uint8_t DS18B20_SearchFirst(onewire_ds18b20_t* pHandle);
uint8_t DS18B20_SearchNext(onewire_ds18b20_t* pHandle);
uint8_t DS18B20_AlarmSearch(onewire_ds18b20_t* pHandle);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
