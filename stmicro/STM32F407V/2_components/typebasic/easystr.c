#include "easystr.h"

s32 _str2dec(__IN RO u8* str)
{
    u8  sig = 1;
    s32 ret = 0;

    while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
    {
        ++str;
    }

    if (*str == '-')
    {
        sig = 0;
        ++str;
    }
    else if (*str == '+')
    {
        ++str;
    }

    while (*str >= '0' && *str <= '9')
    {
        ret = ret * 10 + (*str++ - '0');
    }

    return sig ? ret : -ret;
}

u32 _str2hex(__IN RO u8* str)
{
    u8  num;
    u32 ret = 0;

    while (*str)
    {
        if ('0' <= *str && *str <= '9')
        {
            num = *str - '0';
        }
        else if ('a' <= *str && *str <= 'f')
        {
            num = *str - 'a' + 10;
        }
        else if ('A' <= *str && *str <= 'F')
        {
            num = *str - 'A' + 10;
        }
        else
        {
            break;
        }
        ret *= 16;
        ret += num;
        ++str;
    }

    return ret;
}

f32 _str2flt(__IN RO u8* str)
{
    u8  sig   = 1;
    f32 ret   = 0.0;
    f32 power = 1.0;

    while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
    {
        ++str;
    }

    if (*str == '-')
    {
        sig = 0;
        ++str;
    }
    else if (*str == '+')
    {
        ++str;
    }

    while (*str >= '0' && *str <= '9')
    {
        ret *= 10.0;
        ret += (*str++ - '0');
    }

    if (*str == '.')
    {
        str++;
        while (*str >= '0' && *str <= '9')
        {
            ret *= 10.0;
            ret += (*str++ - '0');
            power *= 10.0;
        }
    }

    return (sig ? ret : -ret) / power;
}

u8* _int2str(__OUT u8* str, s32 num, u8 base)
{
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }

    u8 *p = str, *q = str;

    u8  tmp;
    u32 rem;  // remainder
    u32 val = (num < 0) ? -num : num;

    do {
        rem = val % base;
#if 0
        // *p++ = "0123456789abcdefghijklmnopqrstuvwxyz"[rem];
        *p++ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rem];
#else
        // *p++ = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
        *p++ = (rem < 10) ? (rem + '0') : (rem - 10 + 'A');
#endif
        val /= base;
    } while (val != 0);

    // negative
    if (num < 0) { *p++ = '-'; }

    // endwith '0'
    *p-- = '\0';

    // reverse string
    while (q < p)
    {
        tmp  = *q;
        *q++ = *p;
        *p-- = tmp;
    }

    return str;
}

u32 _strlen(RO u8* str)
{
    RO u8* p = str;
    while (*++p) {}
    return (p - str);
}

s32 _strcmp(RO u8* str1, RO u8* str2)
{
    for (; *str1 == *str2 && *str1; str1++, str2++) {}
    return (*str1 - *str2);
}

u8* _strcpy(u8* dest, RO u8* src)
{
    u8* p = dest;
    while ((*p++ = *src++)) {}
    return dest;
}

u8* _strcat(u8* dest, RO u8* src)
{
    u8* p = dest;
    while (*dest++) {}
    while ((*dest++ = (u8)*src++)) {}
    return p;
}

u8* _strstr(RO u8* str, RO u8* substr)
{
    for (; *str; ++str)
    {
        RO u8 *p1 = str, *p2 = substr;
        while (*p1 && *p2 && *p1 == *p2) { ++p1, ++p2; }
        if (!*p2)
        {
            return (u8*)str;
        }
    }

    return nullptr;
}

u8* _strncpy(u8* dest, RO u8* src, u32 len)
{
    u8* p = dest;
    while ((len-- > 0) && (*dest++ = *src++)) {}
    while ((len-- > 0)) { *dest++ = '\0'; }
    return p;
}

u8* _substr(u8* dest, RO u8* src, u32 start, u32 len)
{
    RO u8 *p = src + start, *ret = dest;

    while (*p && len--) { *dest++ = *p++; }
    *dest = '\0';

    return (u8*)ret;
}

u8* _strlwr(RO u8* str)
{
    u8* s = (u8*)str;

    while (*s)
    {
        if (*s >= 'A' && *s <= 'Z')
        {
            *s |= ' ';  // 0x20
        }
        ++s;
    }

    return (u8*)str;
}

u8* _strupr(RO u8* str)
{
    u8* s = (u8*)str;

    while (*s != '\0')
    {
        if (*s >= 'a' && *s <= 'z')
        {
            *s &= ~' ';  // 0x20
        }
        ++s;
    }

    return (u8*)str;
}

s32 _atoi(const char* s)
{
    s32 v    = 0;
    int sign = 1;

    while (*s == ' ' || (u16)(*s - 9) < 5u)
    {
        s++;
    }

    switch (*s)
    {
        case '-':
            sign = -1;
        case '+':
            ++s;
    }

    while ((u16)(*s - '0') < 10u)
    {
        v = v * 10 + *s - '0';
        ++s;
    }

    return sign == -1 ? -v : v;
}

s32 _atol(const char* s)
{
    s32 v    = 0;
    int sign = 0;

    while (*s == ' ' || (s16)(*s - 9) < 5u)
    {
        ++s;
    }

    switch (*s)
    {
        case '-': sign = -1;
        case '+': ++s;
    }

    while ((s16)(*s - '0') < 10u)
    {
        v = v * 10 + *s - '0';
        ++s;
    }

    return sign ? -v : v;
}

void _itoa(s32 num, char* str, u8 base)
{
    const char* tbl = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";

    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *str = '\0';
        return;
    }

    char *ptr = str, *ptr1 = str, chr;
    s32   val;

    do {
        val = num;
        num /= base;
        *ptr++ = tbl[35 + (val - num * base)];
    } while (num);

    // negative sign
    if (val < 0)
    {
        *ptr++ = '-';
    }

    *ptr-- = '\0';

    while (ptr1 < ptr)
    {
        chr     = *ptr;
        *ptr--  = *ptr1;
        *ptr1++ = chr;
    }
}
