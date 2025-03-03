#ifndef __EASYSORT_H__
#define __EASYSORT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "typebasic.h"

typedef s32 (*sort_cmp_t)(RO void*, RO void*);

///< swap elements when function ('cmp') return value is greater than 0
void _bsort(void* array, u32 count, u32 size, sort_cmp_t cmp);  // bubble
void _qsort(void* array, u32 count, u32 size, sort_cmp_t cmp);  // quick

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
