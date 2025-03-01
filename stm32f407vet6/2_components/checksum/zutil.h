/**
 * @file zutil.h
 *
 * Copyright (c) 2021 Semidrive Semiconductor.
 * All rights reserved.
 *
 * Description:type define
 *
 * Revision History:
 * -----------------
 */
#ifndef __ZUTIL_H
#define __ZUTIL_H

/* necessary stuff to transplant crc32 and adler32 from zlib */
#include <inttypes.h>
#include <types.h>

typedef unsigned long uLong;
typedef unsigned int uInt;
typedef uint8_t Byte;
typedef Byte Bytef;
typedef int32_t z_off_t;
typedef int64_t z_off64_t;
typedef unsigned long z_crc_t;

#define Z_NULL NULL
#define OF(args) args
#define local static
#define ZEXPORT
#define FAR

#endif
