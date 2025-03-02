#ifndef __SWI2C_H__
#define __SWI2C_H__

#include "i2c.h"

//-----------------------------------------------------------------------------------
// Configuration
//-----------------------------------------------------------------------------------

typedef struct {
    const char* name;
    int8_t      sda;
    int8_t      scl;
} swi2c_cfg_t;

typedef uint64_t i2c_time_t;

typedef struct {
    void* port;  // swi2c_cfg_t

    void (*cfg_sda)(void* port, pin_dir_e dir);
    void (*cfg_scl)(void* port, pin_dir_e dir);

    void (*set_sda)(void* port, pin_lvl_e level);
    void (*set_scl)(void* port, pin_lvl_e level);

    pin_lvl_e (*get_sda)(void* port);
    pin_lvl_e (*get_scl)(void* port);

    void (*delay)(i2c_time_t n);

    uint32_t interval;  // scl and sda line delay
    uint32_t timeout;

} swi2c_drv_t;

//-----------------------------------------------------------------------------------
// Interface
//-----------------------------------------------------------------------------------

extern i2c_ops_t g_swi2c_ops;

#endif  // !__SWI2C_H__
