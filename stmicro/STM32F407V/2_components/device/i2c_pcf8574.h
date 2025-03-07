#ifndef __I2C_LM75_H__
#define __I2C_LM75_H__

#include "i2cmst.h"
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief pcf8574 address (A2A1A0)
 */
#define PCF8574_ADDRESS_A000 0x20
#define PCF8574_ADDRESS_A001 0x21
#define PCF8574_ADDRESS_A010 0x22
#define PCF8574_ADDRESS_A011 0x23
#define PCF8574_ADDRESS_A100 0x24
#define PCF8574_ADDRESS_A101 0x25
#define PCF8574_ADDRESS_A110 0x26
#define PCF8574_ADDRESS_A111 0x27

/**
 * @note PCF8574 的中断引脚: 在输入模式的时候，如果有引脚的高低电平跳变，则会产生中断
 */
typedef enum {
    PCF8574_PIN_0    = 1 << 0,
    PCF8574_PIN_1    = 1 << 1,
    PCF8574_PIN_2    = 1 << 2,
    PCF8574_PIN_3    = 1 << 3,
    PCF8574_PIN_4    = 1 << 4,
    PCF8574_PIN_5    = 1 << 5,
    PCF8574_PIN_6    = 1 << 6,
    PCF8574_PIN_7    = 1 << 7,
    PCF8574_PIN_NONE = 0x00,
    PCF8574_PIN_ALL  = 0xFF,
} pcf8574_pin_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;

    uint8_t _u8OutputState;
} i2c_pcf8574_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t PCF8574_Init(i2c_pcf8574_t* pHandle);
status_t PCF8574_ReadPort(i2c_pcf8574_t* pHandle, uint8_t* pu8InputState);
status_t PCF8574_WritePort(i2c_pcf8574_t* pHandle, uint8_t u8OutputState);
status_t PCF8574_TogglePort(i2c_pcf8574_t* pHandle);
status_t PCF8574_ReadPins(i2c_pcf8574_t* pHandle, uint8_t u8PinMask, __OUT pin_level_e* peLevel);
status_t PCF8574_WritePins(i2c_pcf8574_t* pHandle, uint8_t u8PinMask, pin_level_e eLevel);
status_t PCF8574_TogglePins(i2c_pcf8574_t* pHandle, uint8_t u8PinMask);
status_t PCF8574_ShiftLeft(i2c_pcf8574_t* pHandle, uint8_t u8Shift);
status_t PCF8574_ShiftRight(i2c_pcf8574_t* pHandle, uint8_t u8Shift);
status_t PCF8574_RotateLeft(i2c_pcf8574_t* pHandle, uint8_t u8Shift);
status_t PCF8574_RotateRight(i2c_pcf8574_t* pHandle, uint8_t u8Shift);
status_t PCF8574_Reverse(i2c_pcf8574_t* pHandle);
status_t PCF8574_ReadKey(i2c_pcf8574_t* pHandle, pcf8574_pin_e ePin, __OUT bool* pbIsPressed);
status_t PCF8574_ScanMatrixkey4x4(i2c_pcf8574_t* pHandle, __OUT uint8_t* pu8KeyIndex);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void PCF8574_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
