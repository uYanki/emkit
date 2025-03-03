#ifndef __EASYMATH_H__
#define __EASYMATH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "typebasic.h"

///< 向下取整
static inline f32 _floor(f32 x)
{
    return (f32)((s32)x - (x < 0.0f && x != (s32)x));
}

///< 向上取整
static inline f32 _ceil(f32 x)
{
    return (f32)((s32)x + (x > 0.0f && x != (s32)x));
}

///< 四舍五入
static inline f32 _round(f32 x)
{
    return (x >= 0.0f) ? _floor(x + 0.5f) : _ceil(x - 0.5f);
}

inline f32 _sqrtf(f32 v);
inline f32 _invsqrtf(f32 v);

s32 gcd(s32 a, s32 b);
s32 lcm(s32 a, s32 b);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
