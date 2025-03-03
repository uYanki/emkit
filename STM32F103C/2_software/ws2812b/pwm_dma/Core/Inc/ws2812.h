#ifndef __RGB_H__
#define __RGB_H__

#include "main.h"

// 重装值 89
// 周期 800k => 1.25us
// code_1: (58+1)/(89+1) * 1.25us = 0.82us
// code_0: (25+1)/(89+1) * 1.25us = 0.36us

#define CODE_1 (58)  // 1码定时器计数次数
#define CODE_0 (25)  // 0码定时器计数次数

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} rgb_t;

#define Pixel_NUM 64  // LED数量

void RGB_SetColor(uint8_t LedId, rgb_t Color);  // 给一个LED装载24个颜色数据码（0码和1码）
void Reset_Load(void);                          // 该函数用于将数组最后24个数据变为0，代表RESET_code
void RGB_SendArray(void);                       // 发送最终数组

void RGB_RED(uint16_t Pixel_Len);    // 显示红灯
void RGB_GREEN(uint16_t Pixel_Len);  // 显示绿灯
void RGB_BLUE(uint16_t Pixel_Len);   // 显示蓝灯
void RGB_WHITE(uint16_t Pixel_Len);  // 显示白灯

#endif
