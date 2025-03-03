#include "buildtime.h"
#include <string.h>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "version"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static const char* m_szBuildData = __DATE__;
static const char* m_szBuildTime = __TIME__;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

build_time_t GetBuildTime(void)
{
    build_time_t sBuildTime;

    {
        const char* aszMonths[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        uint8_t u8Month = 0;

        while (strcmp(m_szBuildData, aszMonths[u8Month++]) != m_szBuildData[3]);

        sBuildTime.sDate.u8Month = u8Month;

        sBuildTime.sDate.u8Day = (m_szBuildData[5] - '0');

        if (m_szBuildData[4] != ' ')
        {
            sBuildTime.sDate.u8Day += (m_szBuildData[4] - '0') * 10;
        }

        sBuildTime.sDate.u16Year = ((m_szBuildData[7] - '0') * 1000 + (m_szBuildData[8] - '0') * 100 + (m_szBuildData[9] - '0') * 10 + (m_szBuildData[10] - '0'));
    }

    {
        sBuildTime.sTime.u8Hour   = (m_szBuildTime[0] - '0') * 10 + (m_szBuildTime[1] - '0');
        sBuildTime.sTime.u8Minute = (m_szBuildTime[3] - '0') * 10 + (m_szBuildTime[4] - '0');
        sBuildTime.sTime.u8Second = (m_szBuildTime[6] - '0') * 10 + (m_szBuildTime[7] - '0');
    }

    return sBuildTime;
}
