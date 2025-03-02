#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef unsigned int size_t;

#define println(fmt, ...) printf(fmt "\r\n", ##__VA_ARGS__)

#define BV(n)             (1u << (n))

// pin level
typedef enum {
    LOW = 0,
    HIGH,
} pin_lvl_e;

typedef enum {
    OUT = 0,
    IN,
} pin_dir_e;

typedef enum {
    BYTE_ORDER_BIG_ENDIAN,     // 大端 (高字节在低地址)
    BYTE_ORDER_LITTLE_ENDIAN,  // 小端 (低字节在)
} endian_e;

typedef enum {
    UINT_FORMAT_BIN = 0,  // binary
    UINT_FORMAT_DEC = 1,  // decimal
    UINT_FORMAT_HEX = 2,  // hexadecimal
    UINT_FORMAT_BCD = 3,
} uint_fmt_e;

#define INLINE inline

#ifndef nullptr
#define nullptr ((void*)0)
#endif

#define __IN
#define __OUT

typedef enum {
    // ERROR
    _err_begin = -128,
    ERR_UNKOWUN,
    ERR_BUSY,
    ERR_TIMEOUT,
    _err_end,
    // SUCCESS
    SUCCESS = 0,
} err_t;

static inline bool IS_OK(err_t state) { return state == SUCCESS; }
static inline bool IS_ERR(err_t state) { return (_err_begin < (state)) && ((state) < _err_end); }

// static inline DelayBlockUS(uint32_t n) {}