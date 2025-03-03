#include "main.h"
#include "iap_if.h"
#include "flash_if.h"
#include "menu.h"
#include "ymodem.h"

uint32_t FlashProtection = 0;
char     aFileName[FILE_NAME_LENGTH];

/**
 * @brief  Download a file via serial port
 */
void SerialDownload(void)
{
    char              number[11] = {0};
    uint32_t          size       = 0;
    COM_StatusTypeDef result;

    Serial_PutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
    result = Ymodem_Receive(&size);

    if (result == COM_OK)
    {
        Serial_PutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
        Serial_PutString(aFileName);
        _itoa(size, number, 10);
        Serial_PutString("\n\r Size: ");
        Serial_PutString((char*)number);
        Serial_PutString(" Bytes\r\n");
        Serial_PutString("-------------------\n");
    }
    else if (result == COM_LIMIT)
    {
        Serial_PutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
    }
    else if (result == COM_DATA)
    {
        Serial_PutString("\n\n\rVerification failed!\n\r");
    }
    else if (result == COM_ABORT)
    {
        Serial_PutString("\r\n\nAborted by user.\n\r");
    }
    else
    {
        Serial_PutString("\n\rFailed to receive the file!\n\r");
    }
}

/**
 * @brief  Upload a file via serial port.
 */
void SerialUpload(void)
{
    uint8_t status = 0;

    Serial_PutString("\n\n\rSelect Receive File\n\r");

    HAL_UART_Receive(&UartHandle, &status, 1, HAL_MAX_DELAY);

    if (status == CRC16)
    {
        /* Transmit the flash image through ymodem protocol */
        status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

        if (status != 0)
        {
            Serial_PutString("\n\rError Occurred while Transmitting File\n\r");
        }
        else
        {
            Serial_PutString("\n\rFile uploaded successfully \n\r");
        }
    }
}

/**
 * @brief  Display the Main Menu on HyperTerminal
 * @param  None
 * @retval None
 */
void Main_Menu(void)
{
    uint8_t key = 0;

    /* Test if any sector of Flash memory where user application will be loaded is write protected */
    FlashProtection = FLASH_If_GetWriteProtectionStatus();

    while (1)
    {
        Serial_PutString("\r\n");
        Serial_PutString("================= IAP Menu =================\r\n");
        Serial_PutString("  1 > Download image to the internal Flash\r\n");
        Serial_PutString("  2 > Upload image from the internal Flash\r\n");
        Serial_PutString("  3 > Execute the loaded application\r\n");

        if (FlashProtection == FLASHIF_PROTECTION_NONE)
        {
            Serial_PutString("  4 > Enable the write protection\r\n");
        }
        else
        {
            Serial_PutString("  4 > Disable the write protection\r\n");
        }

        Serial_PutString("============================================\r\n");

        /* Clean the input path */
        __HAL_UART_FLUSH_DRREGISTER(&UartHandle);

    __rx_key:
        /* Receive key */
        HAL_UART_Receive(&UartHandle, &key, 1, HAL_MAX_DELAY);

        switch (key)
        {
            case '1':
            {
                SerialDownload(); /* Download user application in the Flash */
                break;
            }
            case '2':
            {
                SerialUpload(); /* Upload user application from the Flash */
                break;
            }
            case '3':
            {
                Serial_PutString("Start program execution......\r\n\n");
                JumpToApp(APPLICATION_ADDRESS);
                Serial_PutString("App Run Fail\n\n");
                break;
            }
            case '4':
            {
                if (FlashProtection == FLASHIF_PROTECTION_NONE)
                {
                    if (FLASH_If_WriteProtectionConfig(OB_WRPSTATE_ENABLE) == FLASHIF_OK)
                    {
                        FlashProtection = FLASHIF_PROTECTION_WRPENABLED;
                        Serial_PutString("Write Protection enabled...\r\n");
                        Serial_PutString("System will now restart...\r\n");
                        /* Launch the option byte loading */
                        HAL_FLASH_OB_Launch();
                    }
                    else
                    {
                        Serial_PutString("Error: Flash write protection failed...\r\n");
                    }
                }
                else
                {
                    /* Disable the write protection */
                    if (FLASH_If_WriteProtectionConfig(OB_WRPSTATE_DISABLE) == FLASHIF_OK)
                    {
                        FlashProtection = FLASHIF_PROTECTION_NONE;
                        Serial_PutString("Write Protection disabled...\r\n");
                        Serial_PutString("System will now restart...\r\n");
                        /* Launch the option byte loading */
                        HAL_FLASH_OB_Launch();
                    }
                    else
                    {
                        Serial_PutString("Error: Flash write un-protection failed...\r\n");
                    }
                }
                break;
            }
            default:
            {
#if 0
                Serial_PutString("Invalid Number ! ==> The number should be either 1, 2, 3 or 4\r\n");
                break;
#else
                goto __rx_key;
#endif
            }
        }
    }
}
