#ifndef __MBSLAVE_H__
#define __MBSLAVE_H__

#include "paratbl/tbl.h"

#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

/******************************************************/

// 输入寄存器 INPUT
#define REG_INPUT_START    1000ul
#define REG_INPUT_NREGS    100ul

// 离散量输入 DISCRETE
#define DISC_INPUT_START   0ul
#define DISC_INPUT_NDISCS  16ul

// 线圈状态 COIL
#define COIL_START         0ul
#define COIL_NCOILS        64ul

#define _DISC_INPUT_NDISCS ((DISC_INPUT_NDISCS >> 3) + !!(DISC_INPUT_NDISCS % 8))
#define _COIL_NCOILS       ((COIL_NCOILS >> 3) + !!(COIL_NCOILS % 8))

extern USHORT usRegInputBuf[REG_INPUT_NREGS];
extern UCHAR  ucDiscInBuf[_DISC_INPUT_NDISCS];
extern UCHAR  ucCoilBuf[_COIL_NCOILS];

#endif
