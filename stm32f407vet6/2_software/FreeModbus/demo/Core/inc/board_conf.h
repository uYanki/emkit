#ifndef __BOADR_CONF_H__
#define __BOADR_CONF_H__

#include "stm32f4xx.h"

// bsp/uart
#define CONFIG_UART_MODE            UART_MODE_RS232
#define CONFIG_UART_REDIRECT_PRINTF 1
#define CONFIG_UART_REDIRECT_SCANF  0

extern void uart_txcbk(void);
#define CONFIG_UART_TXCBK() uart_txcbk()

extern void uart_rxcbk(const uint8_t* buffer, uint16_t length);
#define CONFIG_UART_RXCBK(buffer, length) uart_rxcbk(buffer, length)

// system/autoinit
#define CONFIG_USDK_INIT_DEBUG            0

#endif
