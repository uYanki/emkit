#ifndef __OLED_DRIVER_H__
#define __OLED_DRIVER_H__

#include "main.h"
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdarg.h>
#include "stdio.h"

#define OLED_IF_SWI2C 1
#define OLED_IF_HWI2C 0
#define OLED_IF_SWSPI 2
#define OLED_IF_HWSPI 3

#define OLED_IF_MODE  OLED_IF_HWI2C

#if OLED_IF_MODE == OLED_IF_SWSPI

// 使用宏定义，速度更快（寄存器方式）
#define OLED_SCL_Clr() (GPIOB->BRR = GPIO_Pin_1)
#define OLED_SCL_Set() (GPIOB->BSRR = GPIO_Pin_1)

#define OLED_SDA_Clr() (GPIOB->BRR = GPIO_Pin_3)
#define OLED_SDA_Set() (GPIOB->BSRR = GPIO_Pin_3)

#define OLED_RES_Clr() (GPIOB->BRR = GPIO_Pin_5)
#define OLED_RES_Set() (GPIOB->BSRR = GPIO_Pin_5)

#define OLED_DC_Clr()  (GPIOB->BRR = GPIO_Pin_7)
#define OLED_DC_Set()  (GPIOB->BSRR = GPIO_Pin_7)

#define OLED_CS_Clr()  (GPIOB->BRR = GPIO_Pin_9)
#define OLED_CS_Set()  (GPIOB->BSRR = GPIO_Pin_9)

#endif

#define OLED_CMD  0  // 写命令
#define OLED_DATA 1  // 写数据

//	oled初始化函数
void OLED_Init(void);

//	oled全局刷新函数
void OLED_Update(void);

//	oled局部刷新函数
void OLED_UpdateArea(uint8_t X, uint8_t Y, uint8_t Width, uint8_t Height);

// 设置颜色模式
void OLED_SetColorMode(bool colormode);

// OLED 设置亮度函数
void OLED_Brightness(int16_t Brightness);

#endif
