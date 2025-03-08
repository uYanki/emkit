#ifndef __TYEPS_H__
#define __TYEPS_H__

#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#define RO const

typedef unsigned char uint8_t;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

#endif