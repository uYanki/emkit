#ifndef __MEMOPS_H__
#define __MEMOPS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "typebasic.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @param dest: destination
 * @param src:  source
 */

void* _memset(void* src, u8 chr, u32 len);
void* _memcpy(void* dest, RO void* src, u32 len);
void* _memmove(void* dest, RO void* src, u32 len);
void  _memswap(void* src1, void* src2, u32 len);

s32 _memcmp(RO void* src1, RO void* src2, u32 len);

void* _memxor(void* src, u8 chr, u32 len);      ///< 异或
void* _memlchr(RO void* src, u8 chr, u32 len);  ///< 顺找
void* _memrchr(RO void* src, u8 chr, u32 len);  ///< 倒找

void _mem_lshift(void* ptr, u16 len, u16 shift);
void _mem_rshift(void* ptr, u16 len, u16 shift);

#ifdef __cplusplus
}
#endif

#endif
