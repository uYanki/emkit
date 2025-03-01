#ifndef __OLED_PORT_H__
#define __OLED_PORT_H__

#include "main.h"

void OLED_Init(void);                      // 初始化驱动
void OLED_SendBuff(uint8_t buff[8][128]);  // 将8*128字节的buff一次性全部发送的函数

#endif
