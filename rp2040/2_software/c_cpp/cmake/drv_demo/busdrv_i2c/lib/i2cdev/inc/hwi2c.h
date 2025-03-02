#ifndef __HWI2C_H__
#define __HWI2C_H__

#include "types.h"

#include "hardware/i2c.h"

typedef struct {
    const char* name;

    void*  handle;
    int8_t sda;
    int8_t scl;

    uint32_t bitrate;
} hwi2c_cfg_t;

typedef struct {
    void*    port;  // hwi2c_cfg_t
    uint64_t timeout;
} hwi2c_drv_t;

#endif