#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "ch32v30x.h"

extern u16 usRegHoldingBuf[100 + 1];
extern u8  usRegCoilBuf[64 / 8 + 1];  // We have 64 coils

#endif /*__CH32V30x_SYSTEM_H */
