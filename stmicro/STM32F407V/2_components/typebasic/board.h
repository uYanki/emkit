#ifndef __BOARD_H__
#define __BOARD_H__

#include "./arch/opts.h"

#if defined BOARD_STM32F407VET6_XWS
#include "./variants/board_stm32f407vet6_xws.h"
#elif defined BOARD_AT32F415CBT6_DEV
#include "./variants/board_at32f415cbt6_dev.h"
#elif defined BOARD_USER_DEFINED
#include "board_info.h"

#elif 0

#define CONFIG_ARCH_TYPE ARCH_ARM_CM4

/**
 * @brief Type abbreviations
 */
#define CONFIG_NOT_DECLARE_INT_ABBR     // 不声明整数类型缩写
#define CONFIG_NOT_DECLARE_FLOAT_ABBR   // 不声明浮点类型缩写

/**
 * @brief Constant
 */
#define CONFIG_NOT_DECLARE_INT_RANGE    // 不声明整数范围
#define CONFIG_NOT_DECLARE_FLOAT_RANGE  // 不声明浮点范围

#endif

/**
 * @}
 */

#endif  // !__BOARD_H__
