#ifndef __IAP_IF_H__
#define __IAP_IF_H__

#include <stdbool.h>
#include "stm32f4xx.h"

#define UartHandle huart1

extern UART_HandleTypeDef UartHandle;

void              Serial_PutString(char* p_string);
HAL_StatusTypeDef Serial_PutByte(uint8_t param);

bool IsAppExist(uint32_t AppAddr);
void JumpToApp(uint32_t AppAddr);
void JumpToSysboot(void);

#endif /* __IAP_IF_H__ */
