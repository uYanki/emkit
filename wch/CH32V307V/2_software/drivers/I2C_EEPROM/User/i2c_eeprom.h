#ifndef __I2C_EEPROM_H__
#define __I2C_EEPROM_H__

#include <stdint.h>
#include "debug.h"

void     AT24Cxx_Init(void);
uint8_t  AT24Cxx_ReadOneByte(uint16_t ReadAddr);
void     AT24Cxx_WriteOneByte(uint16_t WriteAddr, uint8_t DataToWrite);
void     AT24Cxx_Read(uint16_t ReadAddr, uint8_t* pBuffer, uint16_t NumToRead);
void     AT24Cxx_Write(uint16_t WriteAddr, uint8_t* pBuffer, uint16_t NumToWrite);

#endif /* __I2C_EEPROM_H__ */
