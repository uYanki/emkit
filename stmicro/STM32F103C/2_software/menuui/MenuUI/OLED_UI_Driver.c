#include "OLED_UI_Driver.h"

/*
【文件说明】：[硬件抽象层]
此文件包含按键与编码器的驱动程序，如果需要移植此项目，请根据实际情况修改相关代码。
当你确保oled屏幕能够正常点亮，并且能够正确地运行基础功能时（如显示字符串等），就可以开始移植
有关按键与编码器等的驱动程序了。
*/

extern TIM_HandleTypeDef ENC_TIM;

/**
 * @brief 编码器使能函数
 * @param 无
 * @return 无
 */
void Encoder_Enable(void)
{
    HAL_TIM_Encoder_Start(&ENC_TIM, TIM_CHANNEL_ALL);
}

/**
 * @brief 编码器失能函数
 * @param 无
 * @return 无
 */
void Encoder_Disable(void)
{
    HAL_TIM_Encoder_Stop(&ENC_TIM, TIM_CHANNEL_ALL);
}

/**
 * @brief 获取编码器的增量计数值（四倍频解码）
 *
 * @details 该函数通过读取定时器TIM1的计数值，对编码器信号进行四倍频解码处理。
 *          使用静态变量累积计数，并通过除法和取模运算去除多余的增量部分，
 *          确保返回精确的增量值。主要用于电机控制、位置检测等应用场景。
 *
 * @note   函数内部会自动清零定时器计数值，确保下次读取的准确性
 *
 * @return int16_t 返回解码后的编码器增量值
 */
int16_t Encoder_Get(void)
{
    // 静态变量，用于在函数调用间保存未被4整除的余数
    static int32_t encoderAccumulator = 0;

    // 读取当前定时器计数值
    int16_t temp = __HAL_TIM_GET_COUNTER(&ENC_TIM);

    // 清零定时器计数器，为下次读取做准备
    __HAL_TIM_SET_COUNTER(&ENC_TIM, 0);

    // 将当前读取值累加到累加器中
    encoderAccumulator += temp;

    // 计算四倍频解码后的增量值（去除未完成的部分）
    int16_t result = encoderAccumulator / 4;

    // 保存未被4整除的余数，保证精度
    encoderAccumulator %= 4;

    // 返回解码后的增量值
    return result;
}

/**
 * @brief  毫秒级延时
 * @param  xms 延时时长，范围：0~4294967295
 * @retval 无
 */
void Delay_ms(uint32_t xms)
{
    HAL_Delay(xms);
}

/**
 * @brief  秒级延时
 * @param  xs 延时时长，范围：0~4294967295
 * @retval 无
 */
void Delay_s(uint32_t xs)
{
    while (xs--)
    {
        Delay_ms(1000);
    }
}
