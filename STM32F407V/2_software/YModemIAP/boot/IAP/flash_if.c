/**
 ******************************************************************************
 * @file    IAP/IAP_Main/Src/flash_if.c
 * @author  MCD Application Team
 ******************************************************************************
 */

/** @addtogroup STM32F4xx_IAP_Main
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint32_t GetSector(uint32_t Address);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Unlocks Flash for write access
 * @param  None
 * @retval None
 */
void FLASH_If_Init(void)
{
    HAL_FLASH_Unlock();

    /* Clear pending flags (if any) */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

/**
 * @brief  This function does an erase of all user flash area
 * @param  StartSector: start of user flash area
 * @retval 0: user flash area successfully erased
 *         1: error occurred
 */
uint32_t FLASH_If_Erase(uint32_t StartSector)
{
    uint32_t               UserStartSector;
    uint32_t               SectorError;
    FLASH_EraseInitTypeDef pEraseInit;

    /* Unlock the Flash to enable the flash control register access *************/
    FLASH_If_Init();

    /* Get the sector where start the user flash area */
    UserStartSector = GetSector(StartSector);

    pEraseInit.TypeErase    = TYPEERASE_SECTORS;
    pEraseInit.Sector       = UserStartSector;
    pEraseInit.NbSectors    = 10;
    pEraseInit.VoltageRange = VOLTAGE_RANGE_3;

    if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK)
    {
        /* Error occurred while page erase */
        return (1);
    }

    return (0);
}

/**
 * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
 * @note   After writing data buffer, the flash content is checked.
 * @param  FlashAddress: start address for writing data buffer
 * @param  Data: pointer on data buffer
 * @param  DataLength: length of data buffer (unit is 32-bit word)
 * @retval 0: Data successfully written to Flash memory
 *         1: Error occurred while writing data in Flash memory
 *         2: Written Data in flash memory is different from expected one
 */
uint32_t FLASH_If_Write(uint32_t FlashAddress, uint32_t* Data, uint32_t DataLength)
{
    uint32_t i = 0;

    for (i = 0; (i < DataLength) && (FlashAddress <= (USER_FLASH_END_ADDRESS - 4)); i++)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by word */
        if (HAL_FLASH_Program(TYPEPROGRAM_WORD, FlashAddress, *(uint32_t*)(Data + i)) == HAL_OK)
        {
            /* Check the written value */
            if (*(uint32_t*)FlashAddress != *(uint32_t*)(Data + i))
            {
                /* Flash content doesn't match SRAM content */
                return (FLASHIF_WRITINGCTRL_ERROR);
            }
            /* Increment FLASH destination address */
            FlashAddress += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            return (FLASHIF_WRITING_ERROR);
        }
    }

    return (FLASHIF_OK);
}

/**
 * @brief  Returns the write protection status of user flash area.
 * @param  None
 * @retval 0: No write protected sectors inside the user flash area
 *         1: Some sectors inside the user flash area are write protected
 */
uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
    uint32_t                   ProtectedSECTOR = 0xFFF;
    FLASH_OBProgramInitTypeDef OptionsBytesStruct;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Check if there are write protected sectors inside the user flash area ****/
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    /* Get pages already write protected ****************************************/
    ProtectedSECTOR = ~(OptionsBytesStruct.WRPSector) & FLASH_SECTOR_TO_BE_PROTECTED;

    /* Check if desired pages are already write protected ***********************/
    if (ProtectedSECTOR != 0)
    {
        /* Some sectors inside the user flash area are write protected */
        return FLASHIF_PROTECTION_WRPENABLED;
    }
    else
    {
        /* No write protected sectors inside the user flash area */
        return FLASHIF_PROTECTION_NONE;
    }
}

/**
 * @brief  Gets the sector of a given address
 * @param  Address: Flash address
 * @retval The sector of a given address
 */
static uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = FLASH_SECTOR_0;

    if (Address < ADDR_FLASH_SECTOR_0)
    {
        // use default sector
    }
#ifdef FLASH_SECTOR_0
#ifdef FLASH_SECTOR_1
    else if (Address < ADDR_FLASH_SECTOR_1)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_0;
    }
#endif
#ifdef FLASH_SECTOR_1
#ifdef FLASH_SECTOR_2
    else if (Address < ADDR_FLASH_SECTOR_2)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_1;
    }
#endif
#ifdef FLASH_SECTOR_2
#ifdef FLASH_SECTOR_3
    else if (Address < ADDR_FLASH_SECTOR_3)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_2;
    }
#endif
#ifdef FLASH_SECTOR_3
#ifdef FLASH_SECTOR_4
    else if (Address < ADDR_FLASH_SECTOR_4)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_3;
    }
#endif
#ifdef FLASH_SECTOR_4
#ifdef FLASH_SECTOR_5
    else if (Address < ADDR_FLASH_SECTOR_5)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_4;
    }
#endif
#ifdef FLASH_SECTOR_5
#ifdef FLASH_SECTOR_6
    else if (Address < ADDR_FLASH_SECTOR_6)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_5;
    }
#endif
#ifdef FLASH_SECTOR_6
#ifdef FLASH_SECTOR_7
    else if (Address < ADDR_FLASH_SECTOR_7)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_6;
    }
#endif
#ifdef FLASH_SECTOR_7
#ifdef FLASH_SECTOR_8
    else if (Address < ADDR_FLASH_SECTOR_8)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_7;
    }
#endif
#ifdef FLASH_SECTOR_8
#ifdef FLASH_SECTOR_9
    else if (Address < ADDR_FLASH_SECTOR_9)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_8;
    }
#endif
#ifdef FLASH_SECTOR_9
#ifdef FLASH_SECTOR_10
    else if (Address < ADDR_FLASH_SECTOR_10)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_9;
    }
#endif
#ifdef FLASH_SECTOR_10
#ifdef FLASH_SECTOR_11
    else if (Address < ADDR_FLASH_SECTOR_11)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_10;
    }
#endif
#ifdef FLASH_SECTOR_11
#ifdef FLASH_SECTOR_12
    else if (Address < ADDR_FLASH_SECTOR_12)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_11;
    }
#endif
#ifdef FLASH_SECTOR_12
#ifdef FLASH_SECTOR_13
    else if (Address < ADDR_FLASH_SECTOR_13)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_12;
    }
#endif
#ifdef FLASH_SECTOR_13
#ifdef FLASH_SECTOR_14
    else if (Address < ADDR_FLASH_SECTOR_14)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_13;
    }
#endif
#ifdef FLASH_SECTOR_14
#ifdef FLASH_SECTOR_15
    else if (Address < ADDR_FLASH_SECTOR_15)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_14;
    }
#endif
#ifdef FLASH_SECTOR_15
#ifdef FLASH_SECTOR_16
    else if (Address < ADDR_FLASH_SECTOR_16)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_15;
    }
#endif
#ifdef FLASH_SECTOR_16
#ifdef FLASH_SECTOR_17
    else if (Address < ADDR_FLASH_SECTOR_17)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_16;
    }
#endif
#ifdef FLASH_SECTOR_17
#ifdef FLASH_SECTOR_18
    else if (Address < ADDR_FLASH_SECTOR_18)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_17;
    }
#endif
#ifdef FLASH_SECTOR_18
#ifdef FLASH_SECTOR_19
    else if (Address < ADDR_FLASH_SECTOR_19)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_18;
    }
#endif
#ifdef FLASH_SECTOR_19
#ifdef FLASH_SECTOR_20
    else if ((Address < ADDR_FLASH_SECTOR_20)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_19;
    }
#endif
#ifdef FLASH_SECTOR_20
#ifdef FLASH_SECTOR_21
    else if (Address < ADDR_FLASH_SECTOR_21)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_20;
    }
#endif
#ifdef FLASH_SECTOR_21
#ifdef FLASH_SECTOR_22
    else if (Address < ADDR_FLASH_SECTOR_22)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_21;
    }
#endif
#ifdef FLASH_SECTOR_22
#ifdef FLASH_SECTOR_23
    else if (Address < ADDR_FLASH_SECTOR_23)
#else
    else
#endif
    {
        sector = FLASH_SECTOR_22;
    }
#endif
#ifdef FLASH_SECTOR_23
#ifdef FLASH_SECTOR_24
    else if (Address < ADDR_FLASH_SECTOR_24)
#else
    else
#endif
    else
    {
        sector = FLASH_SECTOR_23;
    }
#endif

    return sector;
}

/**
 * @brief  Configure the write protection status of user flash area.
 * @param  modifier DISABLE or ENABLE the protection
 * @retval HAL_StatusTypeDef HAL_OK if change is applied.
 */
HAL_StatusTypeDef FLASH_If_WriteProtectionConfig(uint32_t modifier)
{
    uint32_t                   ProtectedSECTOR = 0xFFF;
    FLASH_OBProgramInitTypeDef config_new, config_old;
    HAL_StatusTypeDef          result = HAL_OK;

    /* Get pages write protection status ****************************************/
    HAL_FLASHEx_OBGetConfig(&config_old);

    /* The parameter says whether we turn the protection on or off */
    config_new.WRPState = modifier;

    /* We want to modify only the Write protection */
    config_new.OptionType = OPTIONBYTE_WRP;

    /* No read protection, keep BOR and reset settings */
    config_new.RDPLevel   = OB_RDP_LEVEL_0;
    config_new.USERConfig = config_old.USERConfig;
    /* Get pages already write protected ****************************************/
    ProtectedSECTOR       = config_old.WRPSector | FLASH_SECTOR_TO_BE_PROTECTED;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();

    config_new.WRPSector = ProtectedSECTOR;
    result               = HAL_FLASHEx_OBProgram(&config_new);

    return result;
}

/**
 * @}
 */
