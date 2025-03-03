#include "easymath.h"

/**
 * @brief 单精度浮点开平方
 */
f32 _sqrtf(f32 v)
{
    f32 fv = v;
    u32 uv = *(u32*)&v;

    uv = (uv + 0x3f76cf62) >> 1;
    v  = *(f32*)&uv;
    v  = (v + fv / v) * 0.5;

    return v;
}

/**
 * @brief 单精度浮点开平方的倒数
 */
f32 _invsqrtf(f32 v)
{
    f32 fv = 0.5f * v;
    s32 uv = *(s32*)&v;

    uv = 0x5f3759df - (uv >> 1);
    v  = *(f32*)&uv;
    v  = v * (1.5f - (fv * v * v));

    return v;
}

/**
 * @brief 求最大公约数, Greatest Common Divisor
 */
s32 gcd(s32 a, s32 b)
{
    // 辗转相除法

    s32 r;

    while (b != 0)
    {
        r = a % b;
        a = b;
        b = r;
    }

    return a;
}

/**
 * @brief 求最小公倍数, Least Common Multiple
 */
s32 lcm(s32 a, s32 b)
{
    return a / gcd(a, b) * b;
}
