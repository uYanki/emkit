#ifndef __HEXDUMP_H__
#define __HEXDUMP_H__

#include <stdbool.h>
#include "logger.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define HEXDUMP1(buff, size) hexdump(buff, size, 16, 1, true, nullptr, (u32)buff);
#define HEXDUMP2(buff, size) hexdump(buff, size, 16, 2, true, nullptr, (u32)buff);
#define HEXDUMP4(buff, size) hexdump(buff, size, 16, 4, true, nullptr, (u32)buff);
#define HEXDUMP8(buff, size) hexdump(buff, size, 16, 8, true, nullptr, (u32)buff);

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool hexdump(const uint8_t* cpu8Buffer, uint32_t u32BytesDumped, uint8_t u8BytesPerLine, uint8_t u8BytesOncePrint, bool bShowAscii, const char* szPrefixFormat, uint32_t u32ShownStartingAddress);

#ifdef __cplusplus
}
#endif

#endif
