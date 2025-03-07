
/*
 *@Note
 I2C接口操作EEPROM外设例程：
 I2C1_SCL(PB10)、I2C1_SDA(PB11)。
 本例程使用 EEPROM 为 AT24Cxx系列。
 操作步骤：
 READ EEPROM：Start + EEPROM_DEV_ADDR + 8bit Data Address + Start + 0xA1 + Read Data + Stop.
 WRITE EERPOM：Start + EEPROM_DEV_ADDR + 8bit Data Address + Write Data + Stop.

*/

#include "i2c_eeprom.h"

/**********************************************************************
 * @Note:
 *
 *  READ EEPROM：Start + EEPROM_DEV_ADDR + 8bit Data Address + Start + 0xA1 + Read Data + Stop.
 *  WRITE EERPOM：Start + EEPROM_DEV_ADDR + 8bit Data Address + Write Data + Stop.
 *
 *******************************************************************************/

/* EERPOM DATA ADDRESS Length Definition */
#define EEPROM_MEM_ADDSIZE_8BIT  0
#define EEPROM_MEM_ADDSIZE_16BIT 1

/* EERPOM DATA ADDRESS Length Selection */
#define EEPROM_MEM_ADDSIZE EEPROM_MEM_ADDSIZE_16BIT

#define EEPROM_DEV_ADDR    0XA0
#define EEPROM_I2C_BASE    I2C2

/*********************************************************************
 * @fn      AT24Cxx_ReadOneByte
 *
 * @brief   Read one data from EEPROM.
 *
 * @param   ReadAddr - Read frist address.
 *
 * @return  temp - Read data.
 */
uint8_t AT24Cxx_ReadOneByte(uint16_t ReadAddr)
{
    uint8_t temp = 0;

    while (I2C_GetFlagStatus(EEPROM_I2C_BASE, I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(EEPROM_I2C_BASE, ENABLE);

    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(EEPROM_I2C_BASE, EEPROM_DEV_ADDR, I2C_Direction_Transmitter);

    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

#if (EEPROM_MEM_ADDSIZE == EEPROM_MEM_ADDSIZE_8BIT)

    I2C_SendData(EEPROM_I2C_BASE, (u8)(ReadAddr & 0x00FF));
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#elif (EEPROM_MEM_ADDSIZE == EEPROM_MEM_ADDSIZE_16BIT)

    I2C_SendData(EEPROM_I2C_BASE, (u8)(ReadAddr >> 8));
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(EEPROM_I2C_BASE, (u8)(ReadAddr & 0x00FF));
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#endif

    I2C_GenerateSTART(EEPROM_I2C_BASE, ENABLE);

    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(EEPROM_I2C_BASE, EEPROM_DEV_ADDR, I2C_Direction_Receiver);

    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    while (I2C_GetFlagStatus(EEPROM_I2C_BASE, I2C_FLAG_RXNE) == RESET)
    {
        I2C_AcknowledgeConfig(EEPROM_I2C_BASE, DISABLE);
    }

    temp = I2C_ReceiveData(EEPROM_I2C_BASE);
    I2C_GenerateSTOP(EEPROM_I2C_BASE, ENABLE);

    return temp;
}

/*********************************************************************
 * @fn      AT24Cxx_WriteOneByte
 *
 * @brief   Write one data to EEPROM.
 *
 * @param   WriteAddr - Write frist address.
 *
 * @return  DataToWrite - Write data.
 */
void AT24Cxx_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite)
{
    Delay_Ms(1);
    while (I2C_GetFlagStatus(EEPROM_I2C_BASE, I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(EEPROM_I2C_BASE, ENABLE);
    Delay_Ms(1);
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(EEPROM_I2C_BASE, EEPROM_DEV_ADDR, I2C_Direction_Transmitter);
    Delay_Ms(1);
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

#if (EEPROM_MEM_ADDSIZE == EEPROM_MEM_ADDSIZE_8BIT)

    I2C_SendData(EEPROM_I2C_BASE, (u8)(WriteAddr & 0x00FF));
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#elif (EEPROM_MEM_ADDSIZE == EEPROM_MEM_ADDSIZE_16BIT)

    I2C_SendData(EEPROM_I2C_BASE, (u8)(WriteAddr >> 8));
    Delay_Ms(1);
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(EEPROM_I2C_BASE, (u8)(WriteAddr & 0x00FF));
    Delay_Ms(1);
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#endif

    if (I2C_GetFlagStatus(EEPROM_I2C_BASE, I2C_FLAG_TXE) != RESET)
    {
        I2C_SendData(EEPROM_I2C_BASE, DataToWrite);
    }
    Delay_Ms(1);
    while (!I2C_CheckEvent(EEPROM_I2C_BASE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(EEPROM_I2C_BASE, ENABLE);
}

/*********************************************************************
 * @fn      AT24Cxx_Read
 *
 * @brief   Read multiple data from EEPROM.
 *
 * @param   ReadAddr - Read frist address. (AT24c02: 0~255)
 *          pBuffer - Read data.
 *          NumToRead - Data number.
 *
 * @return  none
 */
void AT24Cxx_Read(uint16_t ReadAddr, uint8_t* pBuffer, uint16_t NumToRead)
{
    while (NumToRead)
    {
        *pBuffer++ = AT24Cxx_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}

/*********************************************************************
 * @fn      AT24Cxx_Write
 *
 * @brief   Write multiple data to EEPROM.
 *
 * @param   WriteAddr - Write frist address. (AT24c02: 0~255)
 *          pBuffer - Write data.
 *          NumToWrite - Data number.
 *
 * @return  none
 */
void AT24Cxx_Write(uint16_t WriteAddr, uint8_t* pBuffer, uint16_t NumToWrite)
{
    while (NumToWrite--)
    {
        AT24Cxx_WriteOneByte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
        Delay_Ms(2);
    }
}
