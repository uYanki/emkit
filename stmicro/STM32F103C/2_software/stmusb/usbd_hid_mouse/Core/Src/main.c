/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usbd_hid.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

// 0: button (bit2 - middle button, bit1 - right, bit0 - left),
// 1: x
// 2: y
// 3: wheel

static uint8_t            Mouse_Buffer[4] = {0, 0, 0, 0};
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void        SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define CURSOR_STEP 2
#define ROLLER_STEP 1

typedef enum {
    BUTTON_CLICK_L   = 0,
    BUTTON_CLICK_R   = 1,
    BUTTON_CLICK_M   = 2,
    BUTTON_RIGHT     = 3,
    BUTTON_LEFT      = 4,
    BUTTON_UP        = 5,
    BUTTON_DOWN      = 6,
    BUTTON_ROLL_UP   = 7,
    BUTTON_ROLL_DOWN = 8
} button_e;

uint32_t GetKeyState(button_e button)
{
    switch (button)
    {
#if 0
        case BUTTON_LEFT: return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 0;
        case BUTTON_UP: return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == 0;
        case BUTTON_DOWN: return HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == 0;
        case BUTTON_RIGHT: return HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == 0;
#else
        case BUTTON_ROLL_UP: return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == 0;
        case BUTTON_ROLL_DOWN: return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == 0;

        case BUTTON_CLICK_L: return HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == 0;
        case BUTTON_CLICK_R: return HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == 0;
        case BUTTON_CLICK_M: return HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == 0;
#endif
        default: return 0;
    }
}

/**
 * Function Name : JoyState.
 * Description   : Decodes the Joystick direction.
 * Input         : None.
 * Output        : None.
 * Return value  : The direction value.
 */
uint8_t JoyState(void)
{
    uint8_t t;

    for (t = 1; t < 4; t++)
    {
        Mouse_Buffer[t] = 0;
    }

    t = 0;  // use as flag to show if any key was pressed

    /**
     * @brief Mouse move
     **/

    /* "right" key is pressed */
    if (GetKeyState(BUTTON_RIGHT))
    {
        Mouse_Buffer[1] += CURSOR_STEP;
        t |= 1;
    }

    /* "left" key is pressed */
    if (GetKeyState(BUTTON_LEFT))
    {
        Mouse_Buffer[1] -= CURSOR_STEP;
        t |= 1;
    }

    /* "up" key is pressed */
    if (GetKeyState(BUTTON_UP))
    {
        Mouse_Buffer[2] -= CURSOR_STEP;
        t |= 1;
    }

    /* "down" key is pressed */
    if (GetKeyState(BUTTON_DOWN))
    {
        Mouse_Buffer[2] += CURSOR_STEP;
        t |= 1;
    }

    /**
     * @brief Mouse Roll
     **/
    /* "Roll_up" key is pressed */
    if (GetKeyState(BUTTON_ROLL_UP))
    {
        Mouse_Buffer[3] += ROLLER_STEP;
        t |= 1;
    }
    /* "Roll_up" key is pressed */
    if (GetKeyState(BUTTON_ROLL_DOWN))
    {
        Mouse_Buffer[3] -= ROLLER_STEP;
        t |= 1;
    }

    /**
     * @brief Mouse CLICK
     **/

    /*  "Left Click" key is pressed */
    if (GetKeyState(BUTTON_CLICK_L))
    {
        if (!(Mouse_Buffer[0] & 1))
        {
            Mouse_Buffer[0] |= 1;
            t |= 1;
        }
    }
    else
    {
        if (Mouse_Buffer[0] & 1)
        {
            Mouse_Buffer[0] &= (~1);
            t |= 1;
        }
    }

    /*  "Right Click" key is pressed */
    if (GetKeyState(BUTTON_CLICK_R))
    {
        if (!(Mouse_Buffer[0] & 2))
        {
            Mouse_Buffer[0] |= 2;
            t |= 1;
        }
    }
    else
    {
        if (Mouse_Buffer[0] & 2)
        {
            Mouse_Buffer[0] &= (~2);
            t |= 1;
        }
    }

    /*  "Middle Click" key is pressed */
    if (GetKeyState(BUTTON_CLICK_M))
    {
        if (!(Mouse_Buffer[0] & 4))
        {
            Mouse_Buffer[0] |= 4;
            t |= 1;
        }
    }
    else
    {
        if (Mouse_Buffer[0] & 4)
        {
            Mouse_Buffer[0] &= (~4);
            t |= 1;
        }
    }

    return t;
}

/**
 * Function Name : Joystick_Send.
 * Description   : prepares buffer to be sent containing Joystick event infos.
 * Input         : Keys: keys received from terminal.
 * Output        : None.
 * Return value  : None.
 */
void Joystick_Send(uint8_t Keys)
{
    printf("report %02X %02X %02X %02X\n", Mouse_Buffer[0], Mouse_Buffer[1], Mouse_Buffer[2], Mouse_Buffer[3]);

    USBD_HID_SendReport(&hUsbDeviceFS, Mouse_Buffer, sizeof(Mouse_Buffer));
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_USB_DEVICE_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1)
    {
        if (JoyState() != 0)
        {
            Joystick_Send(JoyState());
            HAL_Delay(10);
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef       RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef       RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit     = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection    = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{
    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */

    /* USER CODE END USART1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LED_Pin */
    GPIO_InitStruct.Pin   = LED_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : KEY1_Pin KEY2_Pin KEY3_Pin KEY4_Pin */
    GPIO_InitStruct.Pin  = KEY1_Pin | KEY2_Pin | KEY3_Pin | KEY4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : KEY0_Pin */
    GPIO_InitStruct.Pin  = KEY0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY0_GPIO_Port, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int fputc(int ch, FILE* f)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xFF);
    return ch;
}

// usb_printf("hello world\r\n");

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
