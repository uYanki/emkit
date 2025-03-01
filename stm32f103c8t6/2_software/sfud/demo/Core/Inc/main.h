/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define UART_WIFI huart2
#define UART_BLE huart2
#define SPI1_CS_FLASH_Pin GPIO_PIN_13
#define SPI1_CS_FLASH_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_1
#define LED_GPIO_Port GPIOA
#define SPI1_CS_TFCARD_Pin GPIO_PIN_4
#define SPI1_CS_TFCARD_GPIO_Port GPIOA
#define NRF24L01_CE_Pin GPIO_PIN_0
#define NRF24L01_CE_GPIO_Port GPIOB
#define SPI1_CS_NRF24L01_Pin GPIO_PIN_2
#define SPI1_CS_NRF24L01_GPIO_Port GPIOB
#define KEY_Pin GPIO_PIN_8
#define KEY_GPIO_Port GPIOA
#define NRF24L01_IRQ_Pin GPIO_PIN_15
#define NRF24L01_IRQ_GPIO_Port GPIOA
#define SWI2C_SCL_EEPROM_Pin GPIO_PIN_4
#define SWI2C_SCL_EEPROM_GPIO_Port GPIOB
#define SWI2C_SDA_EEPROM_Pin GPIO_PIN_6
#define SWI2C_SDA_EEPROM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
