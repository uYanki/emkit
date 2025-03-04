#pragma once

#include "config.h"
#if CONFIG_PID_MODULE
#include "pid.h"
#endif

#if CONFIG_EXT_MODULE
#define EXT_MODULE_COUNT   2
#define EXT_MODULE_BUFSIZE 64  // 单次最大映射64个寄存器
#endif

///< @note mbpoll -> foarmat > 32-bit float > little-endian byte swap

typedef struct {
    // modbus slave id (this device)
    u16 salveID;

#if CONFIG_LED_MODULE
    // blink
    struct {
        // toggle period
        u16 period;
    } blink;
#endif

#if CONFIG_PID_MODULE
    // pid
    struct {
        f32 Kp;
        f32 Ki;
        f32 Kd;
        f32 ref;
        f32 fbk;
        f32 ramp;
        f32 lower;
        f32 upper;
        f32 out;
        f32 Ts;  // unit: second
    } PID;
#endif

#if CONFIG_EXT_MODULE
    // modbus slave id (extension module)
    struct {
        u16 salveID;
        u16 regStart;
        u16 regCount;
        u16 scanPeriod;  // ms
        u16 error;
    } EXT[EXT_MODULE_COUNT];
#endif

    // command word, 命令字 (unsaved)
    union {
        uint16_t value : 16;
        struct {
            // system
            bool system_reset : 1;
            // params table
            bool paratbl_reset  : 1;
            bool paratbl_save   : 1;
            bool paratbl_reload : 1;
        };
    } cmdword;

} paragrp_t;

/**
 * @defgroup modbus extension module memory
 * {
 */

extern paragrp_t g_paragrp;

#if CONFIG_EXT_MODULE
extern uint16_t g_extgrp[EXT_MODULE_COUNT][EXT_MODULE_BUFSIZE];
#endif

/**
 * }
 */

/**
 * @defgroup modbus salve holding registers
 * {
 */

// @note offsetof / sizoef -> byte, need to convert it to word

#define MBREG_SLAVE_ID 0
#define MBREG_CMDWORD  1

#if CONFIG_LED_MODULE
#define MBREG_LED 2
#endif

#if CONFIG_PID_MODULE
#define MBREG_PID_START 100  // (offsetof(paragrp_t, PID) / 2)
#define MBREG_PID_SIZE  (sizeof(g_paragrp.PID) / 2)
#define MBREG_PID_END   (MBREG_PID_START + MBREG_PID_SIZE - 1)
#endif

#if CONFIG_EXT_MODULE
// config
#define MBREG_EXT_START     (200u)
#define MBREG_EXT_SIZE      (sizeof(g_paragrp.EXT) / 2)
#define MBREG_EXT_END       (MBREG_EXT_START + MBREG_EXT_SIZE - 1)
// memmap
#define MBREG_EXT_MAP_START (1000u)
#define MBREG_EXT_MAP_SIZE  (EXT_MODULE_COUNT * EXT_MODULE_BUFSIZE)
#define MBREG_EXT_MAP_END   (MBREG_EXT_MAP_START + MBREG_EXT_MAP_SIZE - 1)
#endif

/**
 * }
 */

void paragrp_reset(void);
