#ifndef __I2C_APPLICATION_H
#define __I2C_APPLICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at32f415.h"

#define I2C_EVENT_CHECK_NONE    ((uint32_t)0x00000000) /*!< check flag none */
#define I2C_EVENT_CHECK_ACKFAIL ((uint32_t)0x00000001) /*!< check flag ackfail */
#define I2C_EVENT_CHECK_STOP    ((uint32_t)0x00000002) /*!< check flag stop */

typedef enum {
    I2C_MEM_ADDR_WIDIH_8  = 0x01, /*!< memory address is 8 bit */
    I2C_MEM_ADDR_WIDIH_16 = 0x02, /*!< memory address is 16 bit */
} i2c_mem_address_width_type;

typedef enum {
    I2C_OK = 0,         /*!< no error */
    I2C_ERR_BUSY,       /*!< step 1 error */
    I2C_ERR_STEP_2,     /*!< step 2 error */
    I2C_ERR_STEP_3,     /*!< step 3 error */
    I2C_ERR_STEP_4,     /*!< step 4 error */
    I2C_ERR_STEP_5,     /*!< step 5 error */
    I2C_ERR_STEP_6,     /*!< step 6 error */
    I2C_ERR_STEP_7,     /*!< step 7 error */
    I2C_ERR_STEP_8,     /*!< step 8 error */
    I2C_ERR_STEP_9,     /*!< step 9 error */
    I2C_ERR_STEP_10,    /*!< step 10 error */
    I2C_ERR_STEP_11,    /*!< step 11 error */
    I2C_ERR_STEP_12,    /*!< step 12 error */
    I2C_ERR_START,      /*!< start error */
    I2C_ERR_ADDR10,     /*!< addr10 error */
    I2C_ERR_ADDR,       /*!< addr error */
    I2C_ERR_STOP,       /*!< stop error */
    I2C_ERR_ACKFAIL,    /*!< ackfail error */
    I2C_status_tIMEOUT, /*!< timeout error */
    I2C_ERR_INTERRUPT,  /*!< interrupt error */
} i2c_status_type;

i2c_status_type i2c_master_transmit(i2c_type* hi2c, uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout);
i2c_status_type i2c_master_receive(i2c_type* hi2c, uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout);

i2c_status_type i2c_memory_write(i2c_type* hi2c, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout);
i2c_status_type i2c_memory_read(i2c_type* hi2c, i2c_mem_address_width_type mem_address_width, uint16_t address, uint16_t mem_address, uint8_t* pdata, uint16_t size, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif
