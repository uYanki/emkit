#include "timestamp.h"

#ifdef __WIN32
#include <stdio.h>
#define LOGI(...) printf(__VA_ARGS__)
#else
#include "logger/h"
#endif

#ifdef __WIN32
void main()
#else
void TimestampTest(void)
#endif
{
    // https://tool.lu/timestamp/
    // https://www.timetool.cn/timestamp/

    struct tm sTime = {
        .tm_year = 2024,
        .tm_mon  = 9,
        .tm_mday = 1,
        .tm_hour = 2,
        .tm_min  = 6,
        .tm_sec  = 19,
    };

    uint32_t ts = datatime_to_timestamp(&sTime, TIMEZONE_ASIA_SHANGHAI, TIMESTAMP_UNIT_S);
    LOGI("ts %d (%d)", ts, ts - 1725127579);

    sTime = timestamp_to_datatime(-11236, TIMEZONE_ASIA_SHANGHAI, TIMESTAMP_UNIT_S);  // 1970-01-01 04:52:44
    LOGI("time %02d-%02d-%02d %02d:%02d:%02d (yday %d, wday %d)\n",
         sTime.tm_year,
         sTime.tm_mon,
         sTime.tm_mday,
         sTime.tm_hour,
         sTime.tm_min,
         sTime.tm_sec,
         sTime.tm_yday,
         sTime.tm_wday);
}
