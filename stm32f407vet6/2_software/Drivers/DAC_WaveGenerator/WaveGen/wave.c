#include "wave.h"

#define TIM_FRQ_HZ 84000000ul  // 84e6

extern DAC_HandleTypeDef hdac;
extern DMA_HandleTypeDef hdma_dac1;
extern DMA_HandleTypeDef hdma_dac2;
extern TIM_HandleTypeDef htim6;

/**
 * @brief
 * @param[in]  u32Freq unit: 0.1hz
 * @param[out] pu16Clkdiv
 * @param[out] pu16Reload
 */
void GetTimParaByFreq(uint32_t u32Freq, uint16_t* pu16Clkdiv, uint16_t* pu16Reload)
{
    uint32_t u32Mid = 10 * TIM_FRQ_HZ / u32Freq;  // clkdiv x relaod
    uint32_t u32Err = 1;

    uint32_t u32Ref = u32Mid;

    uint32_t u32Clkdiv, u32Reload;
    bool     bSymbolOpt = false;

    while (1)
    {
        //
        // 1<PSC<=65535
        // 0<ARR<=65535
        // TIMCLK=FREQ*(PSC+1)*(ARR+1)
        //

        // 0xFFFFFFFF / 0x10000 = 0xFFFF < 65536

        for (u32Clkdiv = (u32Ref / 65536) + 1; u32Clkdiv <= 0x10000; u32Clkdiv++)
        {
            if ((u32Ref % u32Clkdiv) == 0)
            {
                u32Reload = u32Ref / u32Clkdiv;

                *pu16Clkdiv = u32Clkdiv - 1;
                *pu16Reload = u32Reload - 1;

                return;
            }
        }

        //
        // 调整偏差, 直到找出可被整除的数值
        //

        if ((u32Mid <= (UINT32_MAX - u32Err)) && (bSymbolOpt == false))
        {
            u32Ref     = u32Mid + u32Err;  // 增大
            bSymbolOpt = true;
        }
        else if ((u32Mid >= u32Err) && (bSymbolOpt == true))
        {
            u32Ref     = u32Mid - u32Err;  // 减小
            bSymbolOpt = false;
            u32Err++;
        }
    }
}

void WaveStart(uint32_t freq)  // DAC 相同触发源
{
    uint16_t psc, arr;

    HAL_TIM_Base_Stop(&htim6);

    GetTimParaByFreq(freq, &psc, &arr);

    __HAL_TIM_SetCounter(&htim6, 0);
    __HAL_TIM_SetClockDivision(&htim6, psc);  // Prescaler
    __HAL_TIM_SetAutoreload(&htim6, arr);     // Period

    HAL_TIM_Base_Start(&htim6);
}

void WaveConfig(uint32_t channel, WaveType_e type, uint16_t* user_src, uint16_t user_size)
{
    switch (type)
    {
        default:
        case WaveType_None:
        {
            return;
        }
        case WaveType_Noise:
        {
            HAL_DACEx_NoiseWaveGenerate(&hdac, channel, DAC_LFSRUNMASK_BITS11_0);
            break;
        }
        case WaveType_Triangle:
        {
            HAL_DACEx_TriangleWaveGenerate(&hdac, channel, DAC_TRIANGLEAMPLITUDE_4095);
            break;
        }
        case WaveType_Custom:
        {
            if (user_src != NULL && user_size > 0)
            {
                HAL_DAC_Start_DMA(&hdac, channel, (uint32_t*)user_src, user_size, DAC_ALIGN_12B_R);
            }
            else
            {
                HAL_DAC_Stop_DMA(&hdac, channel);
            }

            break;
        }
    }
}

void WaveEnableOutput(uint32_t channel)
{
    HAL_DAC_Start(&hdac, channel);
}

void WaveDisableOutput(uint32_t channel)
{
    HAL_DAC_Stop(&hdac, channel);
}