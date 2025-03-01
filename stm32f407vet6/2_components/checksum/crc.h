#ifndef __CRC_H__
#define __CRC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

///////////////////////////////////////////////////////////////////////////////

#include "types.h"

u8  crc8(RO u8* dat, u16 len);
u16 crc16(RO u8* dat, u16 len);

///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
