#ifndef __ONEWIRE_DHT11_H__
#define __ONEWIRE_DHT11_H__

#include "typebasic.h"
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    __IN const pin_t DQ;
    __OUT float32_t  f32Temperature;  // 温度(范围:0~50 ℃)
    __OUT float32_t  f32Humidity;     // 湿度(范围:20~90 %RH)
} onewire_dht11_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t DHT11_Init(onewire_dht11_t* pHandle);

/**
 * @param[out] pf32Temperature 温度(范围:0~50 ℃)
 * @param[out] pf32Humidity 湿度(范围:20~90 %RH)
 */
status_t DHT11_ReadTempHumi(onewire_dht11_t* pHandle, float32_t* pf32Temperature, float32_t* pf32Humidity);

/**
 * @brief 摄氏度和华氏度互转
 */
float32_t Celcius2Fahrenheit(float32_t c);
float32_t Fahrenheit2Celcius(float32_t f);

/**
 * @brief 计算热量指数
 */
float64_t ComputeHeatIndex(float64_t f64Temperature, float64_t f64PercentHumidity, bool bIsFahrenheit /* true */);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void DHT11_Test(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
