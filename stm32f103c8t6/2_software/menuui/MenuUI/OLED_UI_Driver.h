#ifndef __OLED_UI_DRIVER_H
#define __OLED_UI_DRIVER_H

#include "main.h"

// 获取确认，取消，上，下按键状态的函数(【Q：为什么使用宏定义而不是函数？A：因为这样可以提高效率，减少代码量】)
#define Key_GetEnterStatus() HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define Key_GetBackStatus()  HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define Key_GetUpStatus()    HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY4_Pin)
#define Key_GetDownStatus()  HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY3_Pin)

// 定时器中断初始化函数
void Timer_Init(void);

// 编码器使能函数
void Encoder_Enable(void);

// 编码器失能函数
void Encoder_Disable(void);

// 读取编码器的增量值
int16_t Encoder_Get(void);

// 延时函数
void Delay_ms(uint32_t xms);
void Delay_s(uint32_t xs);

#endif
