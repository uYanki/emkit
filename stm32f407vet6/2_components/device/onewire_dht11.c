#include "onewire_dht11.h"
#include <math.h>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "dht11"
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static bool DHT11_PollForPinLevel(onewire_dht11_t* pHandle, pin_level_e eLevel, uint8_t u8TimeoutUs, __OUT uint8_t* pu8UsedTimeUs)
{
    bool    bMatched = false;
    uint8_t u8TimeUsed;  // unit:1us, res:10us

    for (u8TimeUsed = 0; u8TimeUsed < u8TimeoutUs; u8TimeUsed += 10)
    {
        if (PIN_ReadLevel(&pHandle->DQ) == eLevel)
        {
            bMatched = true;
            goto __exit;
        }

        DelayBlockUs(10);
    }

__exit:

    if (pu8UsedTimeUs != nullptr)
    {
        *pu8UsedTimeUs = u8TimeUsed;
    }

    return bMatched;
}

static uint8_t DHT11_ReadByte(onewire_dht11_t* pHandle)
{
    uint8_t u8BitIdx;
    uint8_t u8UsedTime;
    uint8_t u8Data = 0;

    for (u8BitIdx = 0; u8BitIdx < 8; u8BitIdx++)
    {
        u8Data <<= 1;

        // 等待DQ电平变高 (超时100us)
        if (DHT11_PollForPinLevel(pHandle, PIN_LEVEL_HIGH, 100, nullptr) == false)
        {
            // 超时无应答，失败
            return 0;
        }

        // 等待DQ电平变低，统计DQ高电平时长 (超时100us)
        DHT11_PollForPinLevel(pHandle, PIN_LEVEL_LOW, 100, &u8UsedTime);

        // 高脉冲持续时间大于40us，认为是1，否则是0
        if (u8UsedTime >= 40)
        {
            u8Data++;
        }
    }

    return u8Data;
}

status_t DHT11_Init(onewire_dht11_t* pHandle)
{
    return ERR_NONE;
}

status_t DHT11_ReadTempHumi(onewire_dht11_t* pHandle, float32_t* pf32Temperature, float32_t* pf32Humidity)
{
    uint8_t u8Index;
    uint8_t au8Data[5];

    PIN_SetMode(&pHandle->DQ, PIN_MODE_OUTPUT_PUSH_PULL, PIN_PULL_UP);

    // 主机发送起始信号, DQ拉低时间至少 18ms
    PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_LOW);  // DQ_L
    DelayBlockMs(20);

    // 拉高, 等待15us
    PIN_WriteLevel(&pHandle->DQ, PIN_LEVEL_HIGH);  // DQ_H
    DelayBlockUs(15);

    PIN_SetMode(&pHandle->DQ, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

    if (DHT11_PollForPinLevel(pHandle, PIN_LEVEL_LOW, 100, nullptr) == false ||   // 等待DQ电平变低 (超时100us)
        DHT11_PollForPinLevel(pHandle, PIN_LEVEL_HIGH, 100, nullptr) == false ||  // 等待DQ电平变高 (超时100us)
        DHT11_PollForPinLevel(pHandle, PIN_LEVEL_LOW, 100, nullptr) == false)     // 等待DQ电平变低 (超时100us)
    {
        // 超时无应答，失败
        return ERR_TIMEOUT;
    }

    // 读 40bit 数据
    for (u8Index = 0; u8Index < 5; u8Index++)
    {
        au8Data[u8Index] = DHT11_ReadByte(pHandle);
    }
    DelayBlockUs(100);

    if ((au8Data[0] + au8Data[1] + au8Data[2] + au8Data[3]) == au8Data[4])
    {
        *pf32Humidity    = au8Data[0] + au8Data[1] / 10.f;
        *pf32Temperature = au8Data[2] + au8Data[3] / 10.f;
    }

    return ERR_NONE;
}

/*!
 *  @brief  Converts Celcius to Fahrenheit
 *  @param  c
 *					value in Celcius
 *	@return float32_t value in Fahrenheit
 */
float32_t Celcius2Fahrenheit(float32_t c)
{
    return c * 1.8f + 32;
}

/*!
 *  @brief  Converts Fahrenheit to Celcius
 *  @param  f
 *					value in Fahrenheit
 *	@return float32_t value in Celcius
 */
float32_t Fahrenheit2Celcius(float32_t f)
{
    return (f - 32) * 0.55555;
}

/*!
 *  @brief  Compute Heat Index
 *  		Using both Rothfusz and Steadman's equations
 *			(http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml)
 *  @param  temperature temperature in selected scale
 *  @param  percentHumidity humidity in percent
 *  @param  isFahrenheit true if fahrenheit, false if celcius
 *	@return heat index
 */
float64_t ComputeHeatIndex(float64_t f64Temperature, float64_t f64PercentHumidity, bool bIsFahrenheit)
{
    /*

        The computation of the heat index is a refinement of a result obtained by multiple regression analysis carried out by Lans P. Rothfusz and described in a 1990 National Weather Service (NWS) Technical Attachment (SR 90-23).  The regression equation of Rothfusz is

            HI = -42.379 + 2.04901523*T + 10.14333127*RH - .22475541*T*RH - .00683783*T*T - .05481717*RH*RH + .00122874*T*T*RH + .00085282*T*RH*RH - .00000199*T*T*RH*RH

        where T is temperature in degrees F and RH is relative humidity in percent.  HI is the heat index expressed as an apparent temperature in degrees F.  If the RH is less than 13% and the temperature is between 80 and 112 degrees F, then the following adjustment is subtracted from HI:

            ADJUSTMENT = [(13-RH)/4]*SQRT{[17-ABS(T-95.)]/17}

        where ABS and SQRT are the absolute value and square root functions, respectively.  On the other hand, if the RH is greater than 85% and the temperature is between 80 and 87 degrees F, then the following adjustment is added to HI:

            ADJUSTMENT = [(RH-85)/10] * [(87-T)/5]

        The Rothfusz regression is not appropriate when conditions of temperature and humidity warrant a heat index value below about 80 degrees F. In those cases, a simpler formula is applied to calculate values consistent with Steadman's results:

            HI = 0.5 * {T + 61.0 + [(T-68.0)*1.2] + (RH*0.094)}

        In practice, the simple formula is computed first and the result averaged with the temperature.
        If pHandle heat index value is 80 degrees F or higher, the full regression equation along with any adjustment as described above is applied.
        The Rothfusz regression is not valid for extreme temperature and relative humidity conditions beyond the range of data considered by Steadman.

    */
    float32_t hi;

    if (!bIsFahrenheit)
    {
        f64Temperature = Celcius2Fahrenheit(f64Temperature);
    }

    hi = 0.5 * (f64Temperature + 61.0 + ((f64Temperature - 68.0) * 1.2) +
                (f64PercentHumidity * 0.094));

    if (hi > 79)
    {
        hi = -42.379 + 2.04901523 * f64Temperature + 10.14333127 * f64PercentHumidity +
             -0.22475541 * f64Temperature * f64PercentHumidity +
             -0.00683783 * pow(f64Temperature, 2) +
             -0.05481717 * pow(f64PercentHumidity, 2) +
             0.00122874 * pow(f64Temperature, 2) * f64PercentHumidity +
             0.00085282 * f64Temperature * pow(f64PercentHumidity, 2) +
             -0.00000199 * pow(f64Temperature, 2) * pow(f64PercentHumidity, 2);

        if ((f64PercentHumidity < 13) && (f64Temperature >= 80.0) &&
            (f64Temperature <= 112.0))
        {
            hi -= ((13.0 - f64PercentHumidity) * 0.25) *
                  sqrt((17.0 - fabs(f64Temperature - 95.0)) * 0.05882);
        }

        else if ((f64PercentHumidity > 85.0) && (f64Temperature >= 80.0) &&
                 (f64Temperature <= 87.0))
        {
            hi += ((f64PercentHumidity - 85.0) * 0.1) * ((87.0 - f64Temperature) * 0.2);
        }
    }

    return bIsFahrenheit ? hi : Fahrenheit2Celcius(hi);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void DHT11_Test(void)
{
    onewire_dht11_t dht11 = {
        .DQ = {GPIOA, GPIO_PIN_5},
    };

    float32_t f32Temperature, f32Humidity;

    while (1)
    {
        DelayBlockS(1);

        if (DHT11_ReadTempHumi(&dht11, &f32Temperature, &f32Humidity) == ERR_NONE)
        {
            LOGI("T = %.1f ℃, H = %.1f %%, %.5f", f32Temperature, f32Humidity, ComputeHeatIndex(f32Temperature, f32Humidity, false));
        }
    }
}

#endif
