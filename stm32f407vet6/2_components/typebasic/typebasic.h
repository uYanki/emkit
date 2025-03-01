#ifndef __TYPE_BASIC_H__
#define __TYPE_BASIC_H__

#ifndef __WIN32
#include "board.h"
#include "./arch/default.h"
#endif

#include <stdio.h>
#include <assert.h>

#ifndef __WIN32
#define inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef __cplusplus
#ifndef nullptr
#define nullptr (void*)0
#endif
#endif

/**
 * @brief Modifiers
 * @{
 */

#define RW         /*!< read write */
#define RO   const /*!< read only */
#define WO         /*!< write only */

#define __R  volatile const /* Define "read-only" permission */
#define __RW volatile       /* Define "read-write" permission */
#define __W  volatile       /* Define "write-only" permission */

#ifndef __I
#define __I __R
#endif

#ifndef __IO
#define __IO __RW
#endif

#ifndef __O
#define __O __W
#endif

/*!< variables attribute */
#define __IN
#define __OUT
#define __INOUT

/*!< function interface */
#define __IMPL

/*!< unused variable */
#ifndef UNUSED
#define UNUSED(...) (void)(__VA_ARGS__)
#endif

/*!< c declare */
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

/**
 * @}
 */

/**
 * @brief Basic Types
 * @{
 */

/**
 * @brief 布尔型
 */

#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>  // for C99 and later
#else
#define bool  int
#define true  1
#define false 0
#endif

typedef enum {
    B_OFF = 0, /*!< least significant bit */
    B_ON  = 1, /*!< most significant bit */
} switch_e;

/**
 * @brief 整数型
 */

#ifndef CONFIG_NOT_INCLUDE_STDINT

#include <stdint.h>

#endif

#ifndef CONFIG_NOT_DECLARE_INT_ABBR

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef volatile uint8_t  vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;
typedef volatile uint64_t vu64;

typedef volatile int8_t  vs8;
typedef volatile int16_t vs16;
typedef volatile int32_t vs32;
typedef volatile int64_t vs64;

#endif

/**
 * @brief 浮点型
 */

typedef float  float32_t;
typedef double float64_t;

#ifndef CONFIG_NOT_DECLARE_FLOAT_ABBR

typedef float32_t f32;
typedef float64_t f64;

typedef volatile float32_t vf32;
typedef volatile float64_t vf64;

#endif

/**
 * @}
 */

/**
 * @brief Numeric Range
 * @{
 */

#ifndef CONFIG_NOT_DECLARE_INT_RANGE

#define S8_MIN  0x80                  /*!< -128 */
#define S16_MIN 0x8000                /*!< -32678 */
#define S32_MIN 0x80000000            /*!< -2147483648 */
#define S64_MIN 0x8000000000000000ull /*!< -9223372036854775808 */

#define S8_MAX  0x7F               /*!< +127 */
#define S16_MAX 0x7FFF             /*!< +32767 */
#define S32_MAX 0x7FFFFFFF         /*!< +2147483647 */
#define S64_MAX 0x7FFFFFFFFFFFFFFF /*!< +9223372036854775807 */

#define U8_MIN  0x00               /*!< 0 */
#define U16_MIN 0x0000             /*!< 0 */
#define U32_MIN 0x00000000         /*!< 0 */
#define U64_MIN 0x0000000000000000 /*!< 0 */

#define U8_MAX  0xFFU                 /*!< +255 */
#define U16_MAX 0xFFFFU               /*!< +65535 */
#define U32_MAX 0xFFFFFFFFUL          /*!< +4294967295 */
#define U64_MAX 0xFFFFFFFFFFFFFFFFULL /*!< +18446744073709551615 */

#endif

#ifndef CONFIG_NOT_DECLARE_FLOAT_RANGE

#define F16_MIN
#define F32_MIN 3.40282347e+38F
#define F64_MIN 1.79769313486231571e+308

#define F16_MAX
#define F32_MAX 1.175494351e-38F
#define F64_MAX 2.22507385850720138e-308

#endif

/**
 * @}
 */

/**
 * @brief Math Constant
 * @{
 */

// clang-format off

#ifndef M_PI
#define M_PI       3.1415926f  /*!< 圆周率   π = 3.14159265358979323846 */
#endif
#define M_E        2.7182818f  /*!< 自然常数 e = 2.71828182845904523536 = (1+1/n)^n */
#define M_PHI      1.618033f   /*!< 黄金比例 φ = 1.61803398874989484820 = (1+sqrt(5))/2 */

#define M_PI_X2    6.2831852f  /*!< 2π */
#define M_PI_X100  314         /*!< 100π */

#define M_LOG2E    1.4426950f  /*!<  log2(e) = 1.44269504088896340736 */
#define M_LOG10E   0.4342944f  /*!<  log10(e) = 0.434294481903251827651 */
#define M_LN2      0.6931471f  /*!<  ln(2) = 0.693147180559945309417 */
#define M_LN10     2.3025850f  /*!<  ln(10) = 2.30258509299404568402 */

#define M_RAD2DEG  57.295779f  /*!< 1 radians to 57.295780 degress */
#define M_DEG2RAD  0.017453f   /*!< 1 degress to  0.017453 radians */

#define M_SIN15    0.258819f
#define M_SIN30    0.5f
#define M_SIN45    0.707106f  /*!< sqrt(2)/2 */
#define M_SIN60    0.866025f  /*!< sqrt(3)/2 */
#define M_SIN75    0.965925f

#define M_COS15    SIN75
#define M_COS30    SIN60
#define M_COS45    SIN45
#define M_COS60    SIN30
#define M_COS75    SIN15

#define M_SQRT2    1.414213f
#define M_SQRT3    1.732050f
#define M_SQRT3_2  0.866025f
#define M_SQRT5    2.236067f
#define M_INVSQRT2 0.707106f  /*!< 1/sqrt(2) */
#define M_INVSQRT3 0.577350f  /*!< 1/sqrt(3) */
#define M_INVSQRT5 0.447213f  /*!< 1/sqrt(5) */

// clang-format on

/**
 * @}
 */

/**
 * @brief Limit Marcos
 * @{
 */

/**
 * @param lhs: left-hand side
 * @param rhs: right-hand side
 */
#ifndef MIN
#define MIN(lhs, rhs) (((lhs) < (rhs)) ? (lhs) : (rhs))
#endif
#ifndef MAX
#define MAX(lhs, rhs) (((lhs) > (rhs)) ? (lhs) : (rhs))
#endif

#define MIN3(a, b, c) (MIN(a, MIN(b, c)))
#define MAX3(a, b, c) (MAX(a, MAX(b, c)))

/**
 * @param LO: lowest allowable value
 * @param HI: highest allowable value
 */
#define CLAMP(DAT, LO, HI)   (MAX(LO, MIN(DAT, HI)))
#define INOPEN(DAT, LO, HI)  (((LO) < (DAT)) && ((DAT) < (HI)))   /*!< check if the data is within the open range */
#define INCLOSE(DAT, LO, HI) (((LO) <= (DAT)) && ((DAT) <= (HI))) /*!< check if the data is within the closed range */

/**
 *              outMax - outMin
 *  outVal  = -------------------  x  ( inVal - inMin ) + outMin
 *               inMax - inMin
 */
#define MapTo(inVal, inMin, inMax, outMin, outMax) ((((outMax) - (outMin)) / (f32)((inMax) - (inMin))) * ((inVal) - (inMin)) + (outMin))

/**
 * @}
 */

#define AbsDelta(lhs, rhs) ((lhs) > (rhs)) ? ((lhs) - (rhs)) : ((rhs) - (lhs))

#define ABS(n)             (((n) > 0) ? (n) : (-n))

/*!< count of elements in an array */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(*(array)))
/*!< byte offset of member in structure (OFFSETOF) */
#define MEMBER_OFFSET(structure, member) ((u32)(char*)&(((structure*)0)->member))
/*!< size of a member of a structure */
#define MEMBER_SIZE(structure, member) (sizeof(((structure*)0)->member))

#define ALIGN_DOWN(address, align)     ((address) & ~((align) - 1))
#define ALIGN_UP(address, align)       (((address) + ((align) - 1)) & ~((align) - 1))

/**
 * @brief Bitwise Operations
 * @{
 */

typedef enum {
    LSBFirst, /*!< least significant bit */
    MSBFirst, /*!< most significant bit */
} bit_order_e;

#ifndef BV
#define BV(n) (1UL << (n))
#endif

#define MASK8(LEN)               (~(U8_MAX << (LEN)))
#define MASK16(LEN)              (~(U16_MAX << (LEN)))
#define MASK32(LEN)              (~(U32_MAX << (LEN)))
#define MASK64(LEN)              (~(U64_MAX << (LEN)))

#define BITMASK8(STB, LEN)       (MASK8(LEN) << (STB))
#define BITMASK16(STB, LEN)      (MASK16(LEN) << (STB))
#define BITMASK32(STB, LEN)      (MASK32(LEN) << (STB))
#define BITMASK64(STB, LEN)      (MASK64(LEN) << (STB))

#define SETBIT(DAT, BIT)         ((DAT) |= (1U << (BIT)))    /*!< set a bit */
#define CLRBIT(DAT, BIT)         ((DAT) &= (~(1U << (BIT)))) /*!< clear a bit */
#define CHKBIT(DAT, BIT)         (((DAT) >> (BIT)) & 1U)     /*!< get a bit */

#define SETBIT64(DAT, BIT)       ((DAT) |= (1ULL << (BIT)))    /*!< set a bit */
#define CLRBIT64(DAT, BIT)       ((DAT) &= (~(1ULL << (BIT)))) /*!< clear a bit */
#define CHKBIT64(DAT, BIT)       (((DAT) >> (BIT)) & 1ULL)     /*!< get a bit */

#define SETBITS(DAT, STB, LEN)   ((DAT) |= (MASK32(LEN) << (STB)))  /*!< set bits (bit-32) */
#define CLRBITS(DAT, STB, LEN)   ((DAT) &= ~(MASK32(LEN) << (STB))) /*!< clear bits (bit-32) */
#define GETBITS(DAT, STB, LEN)   (((DAT) >> (STB)) & MASK32(LEN))   /*!< get bits (bit-32) */

#define SETBITS64(DAT, STB, LEN) ((DAT) |= (MASK64(LEN) << (STB)))  /*!< set bits (bit-32) */
#define CLRBITS64(DAT, STB, LEN) ((DAT) &= ~(MASK64(LEN) << (STB))) /*!< clear bits (bit-32) */
#define GETBITS64(DAT, STB, LEN) (((DAT) >> (STB)) & MASK64(LEN))   /*!< get bits (bit-32) */

#define CHKMSK8(DAT, MSK, VAL)   (((u8)(DAT) & (u8)(MSK)) == (u8)(VAL))
#define CHKMSK16(DAT, MSK, VAL)  (((u16)(DAT) & (u16)(MSK)) == (u16)(VAL))
#define CHKMSK32(DAT, MSK, VAL)  (((u32)(DAT) & (u32)(MSK)) == (u32)(VAL))
#define CHKMSK64(DAT, MSK, VAL)  (((u64)(DAT) & (u64)(MSK)) == (u64)(VAL))

#define LOBYTE(DAT)              (u8)(0xFF & (DAT))
#define HIBYTE(DAT)              (u8)(0xFF & ((DAT) >> 8))
#define LOWORD(DAT)              (u16)(0xFFFF & (DAT))
#define HIWORD(DAT)              (u16)(0xFFFF & ((DAT) >> 16))

/*!< link two 16bits data to a 32bits data */
#define LINK32(HI, LO) (((u32)(HI) << 16) | ((u32)(LO)))
/*!< link two 32bits data to a 64bits data */
#define LINK64_2(HI, LO) (((u64)(HI) << 32) | ((u64)(LO)))
/*!< link four 16bits data to a 64bits data */
#define LINK64_4(W3, W2, W1, W0)             (((u64)(W3) << 48) | ((u64)(W2) << 32) | ((u64)(W1) << 16) | ((u64)(W0)))

#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define LINK64(...)                          GET_MACRO(__VA_ARGS__, LINK64_4, LINK64_2, LINK64_2)(__VA_ARGS__)

/*!
 * Count the number of set bits in a 32-bit word.
 * Algorithm comes from Hacker's Delight by Henry S. Warren, Jr
 */
static inline int bitcnt(uint32_t x)
{
    uint32_t n;

    n = (x >> 1) & 0x77777777;
    x = x - n;
    n = (n >> 1) & 0x77777777;
    x = x - n;
    n = (n >> 1) & 0x77777777;
    x = x - n;
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x * 0x01010101;

    return x >> 24;
}

/*!
 * Count leading zeros.
 */
static inline int clz(uint32_t x)
{
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >> 16);
    return bitcnt(~x);
}

/*!
 * Count trailing zeros.
 */
static inline int ctz(uint32_t x)
{
    return bitcnt(~x & (x - 1));
}

/*!
 * Integer logarithm in base 2.
 */
static inline int ilog2(uint32_t x)
{
    assert(x);
    return 31 - clz(x);
}

/**
 * @}
 */

/**
 * @brief byte order
 *
 *  LE: little endian, low byte at low address
 *  BE: big endian, high byte at low address
 *  HE: host endian
 *
 * @{
 *
 */

typedef uint16_t byteorder_t, endian_t;

#if defined(__ORDER_LITTLE_ENDIAN__)
#define LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__
#else
#define LITTLE_ENDIAN 1234
#endif

#if defined(__ORDER_BIG_ENDIAN__)
#define BIG_ENDIAN __ORDER_BIG_ENDIAN__
#else
#define BIG_ENDIAN 4321
#endif

#if defined(__BYTE_ORDER__)
#define HOST_ENDIAN __BYTE_ORDER__
#elif defined(__ARMEL__)  // arm le
#define HOST_ENDIAN LITTLE_ENDIAN
#elif defined(__ARMEB__)  // arm be
#define HOST_ENDIAN BIG_ENDIAN
#endif

#if !defined(HOST_ENDIAN)
// #warning "set `HOST_ENDIAN` as `LITTLE_ENDIAN`"
#define HOST_ENDIAN LITTLE_ENDIAN
#endif

/**
 * @}
 */

/**
 * @brief byte swap
 * @{
 */

#if (defined(__GNUC__) && !defined(__CC_ARM))

#define bswap16(x) __builtin_bswap16(x)
#define bswap32(x) __builtin_bswap32(x)
#define bswap64(x) __builtin_bswap64(x)

#else

// clang-format off

static inline uint16_t bswap16(uint16_t x)
{
    return ((x & 0x00FF) << 8) |
           ((x & 0xFF00) >> 8);
}

static inline uint32_t bswap32(uint32_t x)
{
    return ((x & 0x000000FF) << 24) |
           ((x & 0x0000FF00) << 8) |
           ((x & 0x00FF0000) >> 8) |
           ((x & 0xFF000000) >> 24);
}

static inline uint64_t bswap64(uint64_t x)
{
    return ((x & 0x00000000000000FFULL) << 56) |
           ((x & 0x000000000000FF00ULL) << 40) |
           ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x00000000FF000000ULL) << 8) |
           ((x & 0x000000FF00000000ULL) >> 8) |
           ((x & 0x0000FF0000000000ULL) >> 24) |
           ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0xFF00000000000000ULL) >> 56);
}

// clang-format on

#endif

/**
 * @}
 */

/**
 * @brief number from byte array
 * @{
 */

#if (HOST_ENDIAN == LITTLE_ENDIAN)

#define he16(x) le16((void*)(x))
#define he32(x) le32((void*)(x))
#define he64(x) le64((void*)(x))

#elif (HOST_ENDIAN == BIG_ENDIAN)

#define he16(x) be16((void*)(x))
#define he32(x) be32((void*)(x))
#define he64(x) be64((void*)(x))

#endif

static inline u16 le16(void* p)
{
    u8* _p = p;

    return ((u16)_p[1] << 8) |
           ((u16)_p[0] << 0);
}

static inline u32 le32(void* p)
{
    u16* _p = p;

    return ((u32)le16(_p + 1) << 16) |
           ((u32)le16(_p + 0) << 0);
}

static inline u64 le64(void* p)
{
    u32* _p = p;

    return ((u64)le32(_p + 1) << 32) |
           ((u64)le32(_p + 0) << 0);
}

static inline u16 be16(void* p)
{
    u8* _p = p;

    return ((u16)_p[0] << 8) |
           ((u16)_p[1] << 0);
}

static inline u32 be32(void* p)
{
    u16* _p = p;

    return ((u32)be16(_p + 0) << 16) |
           ((u32)be16(_p + 1) << 0);
}

static inline u64 be64(void* p)
{
    u32* _p = p;

    return ((u64)be32(_p + 0) << 32) |
           ((u64)be32(_p + 1) << 0);
}

/**
 * @}
 */

/**
 * @brief endian convert
 *
 * - host endian to big endian.
 * - host endian to little endian.
 * - big endian to host endian.
 * - little endian to host endian.
 *
 * @{
 */

#if (HOST_ENDIAN == LITTLE_ENDIAN)

#define htobe16(x) bswap16(x)
#define htobe32(x) bswap32(x)
#define htobe64(x) bswap64(x)
#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)

#define be16toh(x) bswap16(x)
#define be32toh(x) bswap32(x)
#define be64toh(x) bswap64(x)
#define le16toh(x) (x)
#define le32toh(x) (x)
#define le64toh(x) (x)

#elif (HOST_ENDIAN == BIG_ENDIAN)

#define htobe16(x) (x)
#define htobe32(x) (x)
#define htobe64(x) (x)
#define htole16(x) bswap16(x)
#define htole32(x) bswap32(x)
#define htole64(x) bswap64(x)

#define be16toh(x) (x)
#define be32toh(x) (x)
#define be64toh(x) (x)
#define le16toh(x) bswap16(x)
#define le32toh(x) bswap32(x)
#define le64toh(x) bswap64(x)

#endif

/**
 * @}
 */

/**
 * @brief printf
 * @{
 */

#ifndef PRINTF
#define PRINTF(...) printf(__VA_ARGS__)
#endif

#ifndef PRINTLN
#define PRINTLN(format, ...) PRINTF(format "\n", ##__VA_ARGS__)
#endif

#ifndef ASSERT
#define ASSERT(cond, msg) assert((msg, cond))
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  // __TYPES_H__
