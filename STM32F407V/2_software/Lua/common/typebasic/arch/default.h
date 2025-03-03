#ifndef __ARCH_DEFAULT_H__
#define __ARCH_DEFAULT_H__

/**
 * @brief 32位内核计数器 (DWT, Data Watchpoint and Trace)
 */

#ifndef DWT_SUPPORT

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM3) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM4) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM33) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM35P) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef DWT_SUPPORT
#define DWT_SUPPORT 1
#endif

#endif

/**
 * @brief 64位内核计数器 (TSG, Timestamp generator)
 * @link https://www.armbbs.cn/forum.php?mod=viewthread&tid=120715
 */

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM7)

#ifndef TSG_SUPPORT
#define TSG_SUPPORT 1
#endif

#endif

/**
 * @brief 指令/数据缓存 (Instruction/Data Cache)
 */

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM35P) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef ICACHE_SUPPORT
#define ICACHE_SUPPORT 1
#endif

#endif

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef DCACHE_SUPPORT
#define DCACHE_SUPPORT 1
#endif

#endif

/**
 * @brief 零等待存储器 (Instruction/Data Tightly Coupled Memory)
 */

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM1) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef ITCM_SUPPORT
#define ITCM_SUPPORT 1
#endif

#endif

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM1) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef DTCM_SUPPORT
#define DTCM_SUPPORT 1
#endif

#endif

/**
 * @brief 浮点单元 (Floating-Point Unit)
 *
 *   FPU16: HP, Half-Precision
 *   FPU32: SP, Single-Precision
 *   FPU64: DP, Double-Precision
 *
 */

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef FPU16_SUPPORT
#define FPU16_SUPPORT 1
#endif

#endif

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM4) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM33) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM35P) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef FPU32_SUPPORT
#define FPU32_SUPPORT 1
#endif

#endif

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef FPU64_SUPPORT
#define FPU64_SUPPORT 1
#endif

#endif

/**
 * @brief 硬件除法 (Hardware Divide)
 */

#if (CONFIG_ARCH_TYPE == ARCH_ARM_CM23) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM3) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM4) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM33) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM35P) || \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM52) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM55) ||  \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM7) ||   \
    (CONFIG_ARCH_TYPE == ARCH_ARM_CM85)

#ifndef HWDIV_SUPPORT
#define HWDIV_SUPPORT 1
#endif

#endif

/**
 * @brief 内存单元大小
 */

#ifndef CONFIG_MEM_UNIT_SIZE

#if (CONFIG_ARCH_TYPE == ARCH_DSP_C2000)
#define CONFIG_MEM_UNIT_SIZE MEM_UNIT_16BIT
#else
#define CONFIG_MEM_UNIT_SIZE MEM_UNIT_8BIT
#endif

#endif

/**
 * @brief 32位/64位非对齐访问
 * @related UNALIGNED_ACCESS_32BIT_SUPPORT
 * @related UNALIGNED_ACCESS_64BIT_SUPPORT
 */

#if (CONFIG_ARCH_TYPE != ARCH_ARM_CM0) && \
    (CONFIG_ARCH_TYPE != ARCH_ARM_CM0P)

#define UNALIGNED_ACCESS_32BIT_SUPPORT

#endif

#if 0

#define UNALIGNED_ACCESS_32BIT_SUPPORT

#endif

#endif

#endif  // ! __ARCH_CONF_H__
