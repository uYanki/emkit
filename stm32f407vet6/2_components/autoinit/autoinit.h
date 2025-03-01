#ifndef __AUTO_INIT_H__
#define __AUTO_INIT_H__

#include "sdkinc.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#if defined(__ARMCC_VERSION) || defined(__GNUC__)
/* ARM Compiler */
/* GCC Compiler */
#define __SECTION(x) __attribute__((section(x)))
#define __USED       __attribute__((used))
#define __ALIGN(n)   __attribute__((aligned(n)))
#define __WEAK       __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
/* for IAR Compiler */
#define __SECTION(x) @x
#define __USED       __root
#define __ALIGN(n)   _Pragma("data_alignment=" #n)
#define __WEAK       __weak
#elif defined(__TI_COMPILER_VERSION__)
#define __SECTION(x)
#define __USED
#define __ALIGN(n)
#define __WEAK __attribute__((weak))
#else
#error "unsupported toolchain..."
#endif

//-----------------------------------------------------------------------------

typedef int (*initcall_t)(void);
typedef void (*exitcall_t)(void);

// the compiler automatically sorts based on the to of ascii codes:
// small ones have higher priority, while large ones have lower priority.

#define __autoinit(func, level)        \
    __SECTION(".core.initcall." level) \
    __USED static const autoinit_t __core_init_##func

#if CONFIG_USDK_INIT_DEBUG

#include <stdio.h>

typedef struct {
    initcall_t  func;
    const char* name;
} autoinit_t;

#define INIT_EXPORT(func, level) \
    __autoinit(func, level) = {func, #func};

#else

typedef initcall_t autoinit_t;

#define INIT_EXPORT(func, level) \
    __autoinit(func, level) = func;

#endif

//-----------------------------------------------------------------------------

#define INIT_LEVEL_BOARD       "1"  // board support package
#define INIT_LEVEL_DEVICE      "2"  // device
#define INIT_LEVEL_COMPONENT   "3"  // components
#define INIT_LEVEL_ENVIRONMENT "4"  // environment
#define INIT_LEVEL_APPLICATION "5"  // application

#define INIT_RESULT_SUCCESS    0
#define INIT_RESULT_FAILURE    1

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

extern void core_preinit(void);
extern void core_postinit(void);

#ifdef __cplusplus
}
#endif

#endif
