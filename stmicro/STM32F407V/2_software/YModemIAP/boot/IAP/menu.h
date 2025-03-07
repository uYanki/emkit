/**
 ******************************************************************************
 * @file    IAP_Main/Inc/menu.h
 * @author  MCD Application Team
 * @version V1.4.0
 * @date    29-April-2016
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "ymodem.h"

/* Imported variables --------------------------------------------------------*/
extern char aFileName[FILE_NAME_LENGTH];

/* Private variables ---------------------------------------------------------*/
typedef void (*pFunction)(void);

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Main_Menu(void);

#endif /* __MENU_H */
