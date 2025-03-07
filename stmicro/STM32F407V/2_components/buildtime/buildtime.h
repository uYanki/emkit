#ifndef __BUILDTIME_H__
#define __BUILDTIME_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/**
 * @brief firmware build time
 */
typedef struct {
    struct {
        uint16_t u16Year;
        uint8_t  u8Month;
        uint8_t  u8Day;
    } sDate;
    struct {
        uint8_t u8Hour;
        uint8_t u8Minute;
        uint8_t u8Second;
    } sTime;
} build_time_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

build_time_t GetBuildTime(void);

#ifdef __cplusplus
}
#endif

#endif
