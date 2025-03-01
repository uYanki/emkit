#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#define LOG_LOCAL_TAG   "log"
#define LOG_LOCAL_LEVEL LOG_LEVEL_VERBOSE
#endif

#ifndef CONFIG_LOGGER_CSI_SW
#define CONFIG_LOGGER_CSI_SW 0
#endif

#ifndef CONFIG_LOGGER_FUNC_NAME_SW
#define CONFIG_LOGGER_FUNC_NAME_SW 1
#endif

#ifndef CONFIG_LOGGER_TIME_FORMAT
#define CONFIG_LOGGER_TIME_FORMAT LOGGER_TIME_FORMAT_NONE
#endif

/**
 * @brief log output interface
 */
#ifndef LOG_IMPL
#define LOG_IMPL(format, ...) printf(format "\n", ##__VA_ARGS__)
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief assert
 */

#ifndef ASSERT
#ifndef NDEBUG

#if 0

#define ASSERT(expr, msg) assert((msg, expr));

#else

#define ASSERT(expr, msg)                                                    \
    do                                                                       \
    {                                                                        \
        if (!(expr))                                                         \
        {                                                                    \
            PRINTLN("\"" #expr "\" assert failed at file: %s, line: %d. %s", \
                    __FILE__, __LINE__, #msg);                               \
            while (1);                                                       \
        }                                                                    \
    } while (0)

#endif

#else /*!< NDEBUG */
#define ASSERT(expr, msg)
#endif /*!< NDEBUG */
#endif /*!< ASSERT */

#if CONFIG_LOGGER_CSI_SW

/**
 * @brief CSI (Control Sequence Introducer/Initiator) sign
 * @note  https://en.wikipedia.org/wiki/ANSI_escape_code
 * @{
 */

#define CSI_SET                                   "\033["
#define CSI_RST                                   "\033[0m"

#define MAKE_CSI(forecolor, backcolor, fontstyle) forecolor backcolor fontstyle

/**
 * @brief foreground color 前景色
 */
#define CSI_FC_BLACK     "30;" /*!< 黑色 */
#define CSI_FC_RED       "31;" /*!< 红色 */
#define CSI_FC_GREEN     "32;" /*!< 绿色 */
#define CSI_FC_YELLOW    "33;" /*!< 黄色 */
#define CSI_FC_BLUE      "34;" /*!< 蓝色 */
#define CSI_FC_MAGENTA   "35;" /*!< 品红 */
#define CSI_FC_CYAN      "36;" /*!< 青色 */
#define CSI_FC_WHITE     "37;" /*!< 白色 */
#define CSI_FC_BLACK_L   "90;" /*!< 亮黑 */
#define CSI_FC_RED_L     "91;" /*!< 亮红 */
#define CSI_FC_GREEN_L   "92;" /*!< 亮绿 */
#define CSI_FC_YELLOW_L  "93;" /*!< 亮黄 */
#define CSI_FC_BLUE_L    "94;" /*!< 亮蓝 */
#define CSI_FC_FUCHSIN_L "95;" /*!< 亮品红 */
#define CSI_FC_CYAN_L    "96;" /*!< 亮青 */
#define CSI_FC_WHITE_L   "97;" /*!< 亮白 */
#define CSI_FC_DEFAULT   "39;" /*!< 默认 */

/**
 * @brief background color 背景色
 */
#define CSI_BC_NULL
#define CSI_BC_BLACK         "40;"
#define CSI_BC_RED           "41;"
#define CSI_BC_GREEN         "42;"
#define CSI_BC_YELLOW        "43;"
#define CSI_BC_BLUE          "44;"
#define CSI_BC_MAGENTA       "45;"
#define CSI_BC_CYAN          "46;"
#define CSI_BC_WHITE         "47;"
#define CSI_BC_DARK_GRAY     "100;"
#define CSI_BC_LIGHT_RED     "101;"
#define CSI_BC_LIGHT_GREEN   "102;"
#define CSI_BC_LIGHT_YELLOW  "103;"
#define CSI_BC_LIGHT_BLUE    "104;"
#define CSI_BC_LIGHT_MAGENTA "105;"
#define CSI_BC_LIGHT_CYAN    "106;"
#define CSI_BC_LIGHT_WHITE   "107;"

/**
 * @brief font style 字体风格
 */
#define CSI_FS_RESET         "0m" /*!< 重置 */
#define CSI_FS_BOLD          "1m" /*!< 加粗 */
#define CSI_FS_FUZZY         "2m" /*!< 模糊 */
#define CSI_FS_ITALIC        "3m" /*!< 斜体 */
#define CSI_FS_UNDERSCORE    "4m" /*!< 下划线 */
#define CSI_FS_BLINK         "5m" /*!< 闪烁 */
#define CSI_FS_FASTBLINK     "6m" /*!< 快闪(快速闪烁) */
#define CSI_FS_REVERSE       "7m" /*!< 反显 */
#define CSI_FS_CONCEALED     "8m" /*!< 消隐 */
#define CSI_FS_STRIKETHROUGH "9m" /*!< 删除线 */
#define CSI_FS_NORMAL        "22m"

#if 0

/**
 * @brief command
 * @note  清屏并将光标移到控制台左上角开始输出 \033[2J;0;0H
 *
 */
#define CSI_CURSOR_MOVE_UP(row)      #row "A"           // 光标上移n行
#define CSI_CURSOR_MOVE_DOWN(row)    #row "B"           // 光标下移n行
#define CSI_CURSOR_MOVE_RIGHT(col)   #col "C"           // 光标右移n列
#define CSI_CURSOR_MOVE_LET(col)     #col "D"           // 光标左移n列
#define CSI_CURSOR_MOVE_TO(row, col) #row ";" #col "H"  // 设置光标位置
#define CSI_CLEAR_SCREEN             "2J"               // 清屏
#define CSI_CURSOR_MOVE_TO_LINEEND   "K"                // 清除从光标到行尾的内容
#define CSI_CURSOR_SAVE_POS          "s"                // 保存光标位置
#define CSI_CURSOR_RECOVER_POS       "u"                // 恢复光标位置
#define CSI_CURSOR_HIDE              "?25l"             // 隐藏光标
#define CSI_CURSOR_SHOW              "?25h"             // 显示光标

#endif

/**
 * @}
 */

#endif  // CONFIG_LOGGER_CSI_SW

/**
 * @brief level
 */

#define LOG_LEVEL_QUIET   0 /*!< 无输出, Quiet, no log output */
#define LOG_LEVEL_ERROR   1 /*!< 错误, Critical errors, software module can not recover on its own */
#define LOG_LEVEL_WRANING 2 /*!< 警告, Error conditions from which recovery measures have been taken */
#define LOG_LEVEL_INFO    3 /*!< 消息, Information messages which describe normal flow of events */
#define LOG_LEVEL_DEBUG   4 /*!< 调试, Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
#define LOG_LEVEL_VERBOSE 5 /*!< 冗余, Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */

/**
 * @brief time-format
 */
#define LOGGER_TIME_FORMAT_NONE    0
#define LOGGER_TIME_FORMAT_INTEGER 1
#define LOGGER_TIME_FORMAT_STRING  2

/**
 * @brief format
 */

#define _LOGE_TAG "E"
#define _LOGW_TAG "W"
#define _LOGI_TAG "I"
#define _LOGD_TAG "D"
#define _LOGV_TAG "V"

#if (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_NONE)
#define _LOG_TIME_FMT "%s"
#elif (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_INTEGER)
#define _LOG_TIME_FMT "(%d)"
#elif (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_STRING)
#define _LOG_TIME_FMT "(%s)"
#endif

#if CONFIG_LOGGER_CSI_SW
#define _LOGE_CSI MAKE_CSI(CSI_FC_RED, CSI_BC_NULL, CSI_FS_NORMAL)
#define _LOGW_CSI MAKE_CSI(CSI_FC_YELLOW, CSI_BC_NULL, CSI_FS_NORMAL)
#define _LOGI_CSI MAKE_CSI(CSI_FC_CYAN, CSI_BC_NULL, CSI_FS_NORMAL)
#define _LOGD_CSI MAKE_CSI(CSI_FC_GREEN, CSI_BC_NULL, CSI_FS_NORMAL)
#define _LOGV_CSI MAKE_CSI(CSI_FC_BLUE, CSI_BC_NULL, CSI_FS_NORMAL)
#endif

#if CONFIG_LOGGER_CSI_SW
#define LOG_FMT(letter, format) CSI_SET _LOG##letter##_CSI _LOG##letter##_TAG _LOG_TIME_FMT " [%s] %s: " format CSI_RST
#else
#define LOG_FMT(letter, format) _LOG##letter##_TAG _LOG_TIME_FMT " [%s] %s: " format
#endif

/**
 * @brief function name
 * @note copy from <assert.h>
 */

#if CONFIG_LOGGER_FUNC_NAME_SW

#ifndef _LOG_FUNC
#if defined __cplusplus && defined __GNUC__
/* Use g++'s demangled names in C++.  */
#define _LOG_FUNC __PRETTY_FUNCTION__
#elif __STDC_VERSION__ >= 199901L
/* C99 requires the use of __func__.  */
#define _LOG_FUNC __func__
#elif __GNUC__ >= 2
/* Older versions of gcc don't have __func__ but can use __FUNCTION__.  */
#define _LOG_FUNC __FUNCTION__
#else
/* failed to detect __func__ support.  */
#define _LOG_FUNC "??"
#endif
#endif

#else

#define _LOG_FUNC ""

#endif

/**
 * @brief output logs at the specified level.
 */

#define LOG_LEVEL(level, tag, format, ...)                                                   \
    do                                                                                       \
    {                                                                                        \
        if (level == LOG_LEVEL_ERROR)                                                        \
        {                                                                                    \
            LOG_IMPL(LOG_FMT(E, format), LOG_GetTimestamp(), tag, _LOG_FUNC, ##__VA_ARGS__); \
        }                                                                                    \
        else if (level == LOG_LEVEL_WRANING)                                                 \
        {                                                                                    \
            LOG_IMPL(LOG_FMT(W, format), LOG_GetTimestamp(), tag, _LOG_FUNC, ##__VA_ARGS__); \
        }                                                                                    \
        else if (level == LOG_LEVEL_INFO)                                                    \
        {                                                                                    \
            LOG_IMPL(LOG_FMT(I, format), LOG_GetTimestamp(), tag, _LOG_FUNC, ##__VA_ARGS__); \
        }                                                                                    \
        else if (level == LOG_LEVEL_DEBUG)                                                   \
        {                                                                                    \
            LOG_IMPL(LOG_FMT(D, format), LOG_GetTimestamp(), tag, _LOG_FUNC, ##__VA_ARGS__); \
        }                                                                                    \
        else if (level == LOG_LEVEL_VERBOSE)                                                 \
        {                                                                                    \
            LOG_IMPL(LOG_FMT(V, format), LOG_GetTimestamp(), tag, _LOG_FUNC, ##__VA_ARGS__); \
        }                                                                                    \
    } while (0)

/**
 * @brief output logs at the specified level. Also check the level with @ref LOG_GLOBAL_LEVEL.
 */

#ifndef LOG_GLOBAL_LEVEL
#define LOG_GLOBAL_LEVEL LOG_LEVEL_VERBOSE
#endif

#define LOG_LEVEL_GLOBAL(level, tag, format, ...)         \
    do                                                    \
    {                                                     \
        if (level <= LOG_GLOBAL_LEVEL)                    \
        {                                                 \
            LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
        }                                                 \
    } while (0)

/**
 * @brief output logs at the specified level. Also check the level with @ref LOG_LOCAL_LEVEL.
 */
#define LOG_LEVEL_LOCAL(level, tag, format, ...)                 \
    do                                                           \
    {                                                            \
        if (level <= LOG_LOCAL_LEVEL)                            \
        {                                                        \
            LOG_LEVEL_GLOBAL(level, tag, format, ##__VA_ARGS__); \
        }                                                        \
    } while (0)

#define LOGE(format, ...) LOG_LEVEL_LOCAL(LOG_LEVEL_ERROR, LOG_LOCAL_TAG, format, ##__VA_ARGS__)
#define LOGW(format, ...) LOG_LEVEL_LOCAL(LOG_LEVEL_WRANING, LOG_LOCAL_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...) LOG_LEVEL_LOCAL(LOG_LEVEL_INFO, LOG_LOCAL_TAG, format, ##__VA_ARGS__)
#define LOGD(format, ...) LOG_LEVEL_LOCAL(LOG_LEVEL_DEBUG, LOG_LOCAL_TAG, format, ##__VA_ARGS__)
#define LOGV(format, ...) LOG_LEVEL_LOCAL(LOG_LEVEL_VERBOSE, LOG_LOCAL_TAG, format, ##__VA_ARGS__)

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#if (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_NONE)

#define LOG_GetTimestamp() ""

#elif (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_INTEGER)

/**
 * @brief the timestamp (in milliseconds) which used in log output
 */
u32 LOG_GetTimestamp(void);

#elif (CONFIG_LOGGER_TIME_FORMAT == LOGGER_TIME_FORMAT_STRING)

/**
 * @brief the timestamp ("HH:MM:SS.sss") which used in log output
 */
char* LOG_GetTimestamp(void);

#endif

#ifdef __cplusplus
}
#endif

#endif  // ! __SDK_LOGGER_H__
