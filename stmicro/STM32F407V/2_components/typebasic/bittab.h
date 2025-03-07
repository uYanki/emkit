#ifndef __BITTAB_H__
#define __BITTAB_H__

#include "typebasic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_REVBIT_TABLE_SW 0  // 位反转表

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

u8  revbit8(u8 u8Data);
u16 revbit16(u16 u16Data);
u32 revbit32(u32 u32Data);

void setbits(u8* pu8Data, u16 u16StartBit, u8 u8BitWidth, u8 u8Value);
u8   getbits(u8* pu8Data, u16 u16StartBit, u8 u8BitWidth);

#ifdef __cplusplus
}
#endif

#endif
