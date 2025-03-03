#include "timestamp.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "timestamp"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#define DayToSecond     86400 /* 一天等于86400秒 */
#define HourToSecond    3600  /* 一小时等于3600秒 */
#define MinuteToSecond  60    /* 一分钟等于60秒 */
#define LeapYear        366   /* 闰年有366天 */
#define CommonYear      365   /* 平年有365天 */
#define LeapFeb         29    /* 闰年的2月 */
#define CommonFeb       28    /* 平年的2月 */

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static const uint8_t m_cau8MonthDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* 平年的月份日期表 */

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static bool IsLeapYear(uint32_t year)
{
    if (((0 == year % 4) && (year % 100 != 0)) || (0 == year % 400))
    {
        return true;
    }
    else
    {
        return false;
    }
}

static time_t GetTimezoneOffsetInMinute(timezone_e eTimezone)
{
    // clang-format off
    switch (eTimezone)
    {
        case TIMEZONE_AFRICA_ABIDJAN: return 0;
        case TIMEZONE_AFRICA_ACCRA: return 0;
        case TIMEZONE_AFRICA_ADDIS_ABABA: return 180;
        case TIMEZONE_AFRICA_ALGIERS: return 0;
        case TIMEZONE_AFRICA_ASMERA: return 180;
        case TIMEZONE_AFRICA_BAMAKO: return 0;
        case TIMEZONE_AFRICA_BANGUI: return 60;
        case TIMEZONE_AFRICA_BANJUL: return 0;
        case TIMEZONE_AFRICA_BISSAU: return -60;
        case TIMEZONE_AFRICA_BLANTYRE: return 120;
        case TIMEZONE_AFRICA_BRAZZAVILLE: return 60;
        case TIMEZONE_AFRICA_BUJUMBURA: return 120;
        case TIMEZONE_AFRICA_CAIRO: return 120;
        case TIMEZONE_AFRICA_CASABLANCA: return 0;
        case TIMEZONE_AFRICA_CEUTA: return 0;
        case TIMEZONE_AFRICA_CONAKRY: return 0;
        case TIMEZONE_AFRICA_DAKAR: return 0;
        case TIMEZONE_AFRICA_DAR_ES_SALAAM: return 180;
        case TIMEZONE_AFRICA_DJIBOUTI: return 180;
        case TIMEZONE_AFRICA_DOUALA: return 60;
        case TIMEZONE_AFRICA_EL_AAIUN: return -60;
        case TIMEZONE_AFRICA_FREETOWN: return 0;
        case TIMEZONE_AFRICA_GABORONE: return 120;
        case TIMEZONE_AFRICA_HARARE: return 120;
        case TIMEZONE_AFRICA_JOHANNESBURG: return 120;
        case TIMEZONE_AFRICA_JUBA: return 120;
        case TIMEZONE_AFRICA_KAMPALA: return 180;
        case TIMEZONE_AFRICA_KHARTOUM: return 120;
        case TIMEZONE_AFRICA_KIGALI: return 120;
        case TIMEZONE_AFRICA_KINSHASA: return 60;
        case TIMEZONE_AFRICA_LAGOS: return 60;
        case TIMEZONE_AFRICA_LIBREVILLE: return 60;
        case TIMEZONE_AFRICA_LOME: return 0;
        case TIMEZONE_AFRICA_LUANDA: return 60;
        case TIMEZONE_AFRICA_LUBUMBASHI: return 120;
        case TIMEZONE_AFRICA_LUSAKA: return 120;
        case TIMEZONE_AFRICA_MALABO: return 60;
        case TIMEZONE_AFRICA_MAPUTO: return 120;
        case TIMEZONE_AFRICA_MASERU: return 120;
        case TIMEZONE_AFRICA_MBABANE: return 120;
        case TIMEZONE_AFRICA_MOGADISHU: return 180;
        case TIMEZONE_AFRICA_MONROVIA: return -45;
        case TIMEZONE_AFRICA_NAIROBI: return 180;
        case TIMEZONE_AFRICA_NDJAMENA: return 60;
        case TIMEZONE_AFRICA_NIAMEY: return 60;
        case TIMEZONE_AFRICA_NOUAKCHOTT: return 0;
        case TIMEZONE_AFRICA_OUAGADOUGOU: return 0;
        case TIMEZONE_AFRICA_PORTO_NOVO: return 60;
        case TIMEZONE_AFRICA_SAO_TOME: return 0;
        case TIMEZONE_AFRICA_TRIPOLI: return 120;
        case TIMEZONE_AFRICA_TUNIS: return 60;
        case TIMEZONE_AFRICA_WINDHOEK: return 120;
        case TIMEZONE_AMERICA_ADAK: return -660;
        case TIMEZONE_AMERICA_ANCHORAGE: return -600;
        case TIMEZONE_AMERICA_ANGUILLA: return -240;
        case TIMEZONE_AMERICA_ANTIGUA: return -240;
        case TIMEZONE_AMERICA_ARAGUAINA: return -180;
        case TIMEZONE_AMERICA_ARGENTINA_LA_RIOJA: return -180;
        case TIMEZONE_AMERICA_ARGENTINA_RIO_GALLEGOS: return -180;
        case TIMEZONE_AMERICA_ARGENTINA_SALTA: return -180;
        case TIMEZONE_AMERICA_ARGENTINA_SAN_JUAN: return -180;
        case TIMEZONE_AMERICA_ARGENTINA_SAN_LUIS: return -180;
        case TIMEZONE_AMERICA_ARGENTINA_TUCUMAN: return -180;
        case TIMEZONE_AMERICA_ARGENTINA_USHUAIA: return -180;
        case TIMEZONE_AMERICA_ARUBA: return -240;
        case TIMEZONE_AMERICA_ASUNCION: return -240;
        case TIMEZONE_AMERICA_BAHIA: return -180;
        case TIMEZONE_AMERICA_BAHIA_BANDERAS: return -480;
        case TIMEZONE_AMERICA_BARBADOS: return -240;
        case TIMEZONE_AMERICA_BELEM: return -180;
        case TIMEZONE_AMERICA_BELIZE: return -360;
        case TIMEZONE_AMERICA_BLANC_SABLON: return -240;
        case TIMEZONE_AMERICA_BOA_VISTA: return -240;
        case TIMEZONE_AMERICA_BOGOTA: return -300;
        case TIMEZONE_AMERICA_BOISE: return -420;
        case TIMEZONE_AMERICA_BUENOS_AIRES: return -180;
        case TIMEZONE_AMERICA_CAMBRIDGE_BAY: return -420;
        case TIMEZONE_AMERICA_CAMPO_GRANDE: return -240;
        case TIMEZONE_AMERICA_CANCUN: return -360;
        case TIMEZONE_AMERICA_CARACAS: return -240;
        case TIMEZONE_AMERICA_CATAMARCA: return -180;
        case TIMEZONE_AMERICA_CAYENNE: return -180;
        case TIMEZONE_AMERICA_CAYMAN: return -300;
        case TIMEZONE_AMERICA_CHICAGO: return -360;
        case TIMEZONE_AMERICA_CHIHUAHUA: return -360;
        // case TIMEZONE_AMERICA_CIUDAD_JUAREZ: return 0;
        case TIMEZONE_AMERICA_CORAL_HARBOUR: return -300;
        case TIMEZONE_AMERICA_CORDOBA: return -180;
        case TIMEZONE_AMERICA_COSTA_RICA: return -360;
        case TIMEZONE_AMERICA_CRESTON: return -420;
        case TIMEZONE_AMERICA_CUIABA: return -240;
        case TIMEZONE_AMERICA_CURACAO: return -240;
        case TIMEZONE_AMERICA_DANMARKSHAVN: return -180;
        case TIMEZONE_AMERICA_DAWSON: return -540;
        case TIMEZONE_AMERICA_DAWSON_CREEK: return -480;
        case TIMEZONE_AMERICA_DENVER: return -420;
        case TIMEZONE_AMERICA_DETROIT: return -300;
        case TIMEZONE_AMERICA_DOMINICA: return -240;
        case TIMEZONE_AMERICA_EDMONTON: return -420;
        case TIMEZONE_AMERICA_EIRUNEPE: return -300;
        case TIMEZONE_AMERICA_EL_SALVADOR: return -360;
        case TIMEZONE_AMERICA_FORT_NELSON: return -480;
        case TIMEZONE_AMERICA_FORTALEZA: return -180;
        case TIMEZONE_AMERICA_GLACE_BAY: return -240;
        case TIMEZONE_AMERICA_GODTHAB: return -180;
        case TIMEZONE_AMERICA_GOOSE_BAY: return -240;
        case TIMEZONE_AMERICA_GRAND_TURK: return -300;
        case TIMEZONE_AMERICA_GRENADA: return -240;
        case TIMEZONE_AMERICA_GUADELOUPE: return -240;
        case TIMEZONE_AMERICA_GUATEMALA: return -360;
        case TIMEZONE_AMERICA_GUAYAQUIL: return -300;
        case TIMEZONE_AMERICA_GUYANA: return -225;
        case TIMEZONE_AMERICA_HALIFAX: return -240;
        case TIMEZONE_AMERICA_HAVANA: return -300;
        case TIMEZONE_AMERICA_HERMOSILLO: return -480;
        case TIMEZONE_AMERICA_INDIANA_KNOX: return -360;
        case TIMEZONE_AMERICA_INDIANA_MARENGO: return -300;
        case TIMEZONE_AMERICA_INDIANA_PETERSBURG: return -360;
        case TIMEZONE_AMERICA_INDIANA_TELL_CITY: return -300;
        case TIMEZONE_AMERICA_INDIANA_VEVAY: return -300;
        case TIMEZONE_AMERICA_INDIANA_VINCENNES: return -300;
        case TIMEZONE_AMERICA_INDIANA_WINAMAC: return -300;
        case TIMEZONE_AMERICA_INDIANAPOLIS: return -300;
        case TIMEZONE_AMERICA_INUVIK: return -480;
        case TIMEZONE_AMERICA_IQALUIT: return -300;
        case TIMEZONE_AMERICA_JAMAICA: return -300;
        case TIMEZONE_AMERICA_JUJUY: return -180;
        case TIMEZONE_AMERICA_JUNEAU: return -480;
        case TIMEZONE_AMERICA_KENTUCKY_MONTICELLO: return -360;
        case TIMEZONE_AMERICA_KRALENDIJK: return -240;
        case TIMEZONE_AMERICA_LA_PAZ: return -240;
        case TIMEZONE_AMERICA_LIMA: return -300;
        case TIMEZONE_AMERICA_LOS_ANGELES: return -480;
        case TIMEZONE_AMERICA_LOUISVILLE: return -300;
        case TIMEZONE_AMERICA_LOWER_PRINCES: return -240;
        case TIMEZONE_AMERICA_MACEIO: return -180;
        case TIMEZONE_AMERICA_MANAGUA: return -360;
        case TIMEZONE_AMERICA_MANAUS: return -240;
        case TIMEZONE_AMERICA_MARIGOT: return -240;
        case TIMEZONE_AMERICA_MARTINIQUE: return -240;
        case TIMEZONE_AMERICA_MATAMOROS: return -360;
        case TIMEZONE_AMERICA_MAZATLAN: return -480;
        case TIMEZONE_AMERICA_MENDOZA: return -180;
        case TIMEZONE_AMERICA_MENOMINEE: return -300;
        case TIMEZONE_AMERICA_MERIDA: return -360;
        case TIMEZONE_AMERICA_METLAKATLA: return -480;
        case TIMEZONE_AMERICA_MEXICO_CITY: return -360;
        case TIMEZONE_AMERICA_MIQUELON: return -240;
        case TIMEZONE_AMERICA_MONCTON: return -240;
        case TIMEZONE_AMERICA_MONTERREY: return -360;
        case TIMEZONE_AMERICA_MONTEVIDEO: return -180;
        case TIMEZONE_AMERICA_MONTSERRAT: return -240;
        case TIMEZONE_AMERICA_NASSAU: return -300;
        case TIMEZONE_AMERICA_NEW_YORK: return -300;
        case TIMEZONE_AMERICA_NOME: return -660;
        case TIMEZONE_AMERICA_NORONHA: return -120;
        case TIMEZONE_AMERICA_NORTH_DAKOTA_BEULAH: return -420;
        case TIMEZONE_AMERICA_NORTH_DAKOTA_CENTER: return -420;
        case TIMEZONE_AMERICA_NORTH_DAKOTA_NEW_SALEM: return -420;
        case TIMEZONE_AMERICA_OJINAGA: return -360;
        case TIMEZONE_AMERICA_PANAMA: return -300;
        case TIMEZONE_AMERICA_PARAMARIBO: return -210;
        case TIMEZONE_AMERICA_PHOENIX: return -420;
        case TIMEZONE_AMERICA_PORT_AU_PRINCE: return -300;
        case TIMEZONE_AMERICA_PORT_OF_SPAIN: return -240;
        case TIMEZONE_AMERICA_PORTO_VELHO: return -240;
        case TIMEZONE_AMERICA_PUERTO_RICO: return -240;
        case TIMEZONE_AMERICA_PUNTA_ARENAS: return -180;
        case TIMEZONE_AMERICA_RANKIN_INLET: return -360;
        case TIMEZONE_AMERICA_RECIFE: return -180;
        case TIMEZONE_AMERICA_REGINA: return -360;
        case TIMEZONE_AMERICA_RESOLUTE: return -360;
        case TIMEZONE_AMERICA_RIO_BRANCO: return -300;
        case TIMEZONE_AMERICA_SANTAREM: return -240;
        case TIMEZONE_AMERICA_SANTIAGO: return -180;
        case TIMEZONE_AMERICA_SANTO_DOMINGO: return -270;
        case TIMEZONE_AMERICA_SAO_PAULO: return -180;
        case TIMEZONE_AMERICA_SCORESBYSUND: return -120;
        case TIMEZONE_AMERICA_SITKA: return -480;
        case TIMEZONE_AMERICA_ST_BARTHELEMY: return -240;
        case TIMEZONE_AMERICA_ST_JOHNS: return -210;
        case TIMEZONE_AMERICA_ST_KITTS: return -240;
        case TIMEZONE_AMERICA_ST_LUCIA: return -240;
        case TIMEZONE_AMERICA_ST_THOMAS: return -240;
        case TIMEZONE_AMERICA_ST_VINCENT: return -240;
        case TIMEZONE_AMERICA_SWIFT_CURRENT: return -420;
        case TIMEZONE_AMERICA_TEGUCIGALPA: return -360;
        case TIMEZONE_AMERICA_THULE: return -240;
        case TIMEZONE_AMERICA_TIJUANA: return -480;
        case TIMEZONE_AMERICA_TORONTO: return -300;
        case TIMEZONE_AMERICA_TORTOLA: return -240;
        case TIMEZONE_AMERICA_VANCOUVER: return -480;
        case TIMEZONE_AMERICA_WHITEHORSE: return -480;
        case TIMEZONE_AMERICA_WINNIPEG: return -360;
        case TIMEZONE_AMERICA_YAKUTAT: return -540;
        case TIMEZONE_ANTARCTICA_CASEY: return 480;
        case TIMEZONE_ANTARCTICA_DAVIS: return 420;
        case TIMEZONE_ANTARCTICA_DUMONTDURVILLE: return 600;
        case TIMEZONE_ANTARCTICA_MACQUARIE: return 660;
        case TIMEZONE_ANTARCTICA_MAWSON: return 360;
        case TIMEZONE_ANTARCTICA_MCMURDO: return 720;
        case TIMEZONE_ANTARCTICA_PALMER: return -180;
        case TIMEZONE_ANTARCTICA_ROTHERA: return 0;
        case TIMEZONE_ANTARCTICA_SYOWA: return 180;
        case TIMEZONE_ANTARCTICA_TROLL: return 0;
        case TIMEZONE_ANTARCTICA_VOSTOK: return 360;
        case TIMEZONE_ARCTIC_LONGYEARBYEN: return 60;
        case TIMEZONE_ASIA_ADEN: return 180;
        case TIMEZONE_ASIA_ALMATY: return 360;
        case TIMEZONE_ASIA_AMMAN: return 120;
        case TIMEZONE_ASIA_ANADYR: return 780;
        case TIMEZONE_ASIA_AQTAU: return 300;
        case TIMEZONE_ASIA_AQTOBE: return 300;
        case TIMEZONE_ASIA_ASHGABAT: return 300;
        case TIMEZONE_ASIA_ATYRAU: return 300;
        case TIMEZONE_ASIA_BAGHDAD: return 180;
        case TIMEZONE_ASIA_BAHRAIN: return 240;
        case TIMEZONE_ASIA_BAKU: return 240;
        case TIMEZONE_ASIA_BANGKOK: return 420;
        case TIMEZONE_ASIA_BARNAUL: return 420;
        case TIMEZONE_ASIA_BEIRUT: return 120;
        case TIMEZONE_ASIA_BISHKEK: return 360;
        case TIMEZONE_ASIA_BRUNEI: return 480;
        case TIMEZONE_ASIA_CALCUTTA: return 330;
        case TIMEZONE_ASIA_CHITA: return 540;
        case TIMEZONE_ASIA_CHOIBALSAN: return 420;
        case TIMEZONE_ASIA_COLOMBO: return 330;
        case TIMEZONE_ASIA_DAMASCUS: return 120;
        case TIMEZONE_ASIA_DHAKA: return 360;
        case TIMEZONE_ASIA_DILI: return 540;
        case TIMEZONE_ASIA_DUBAI: return 240;
        case TIMEZONE_ASIA_DUSHANBE: return 360;
        case TIMEZONE_ASIA_FAMAGUSTA: return 120;
        case TIMEZONE_ASIA_GAZA: return 120;
        case TIMEZONE_ASIA_HEBRON: return 120;
        case TIMEZONE_ASIA_HONG_KONG: return 480;
        case TIMEZONE_ASIA_HOVD: return 360;
        case TIMEZONE_ASIA_IRKUTSK: return 480;
        case TIMEZONE_ASIA_JAKARTA: return 420;
        case TIMEZONE_ASIA_JAYAPURA: return 540;
        case TIMEZONE_ASIA_JERUSALEM: return 120;
        case TIMEZONE_ASIA_KABUL: return 270;
        case TIMEZONE_ASIA_KAMCHATKA: return 720;
        case TIMEZONE_ASIA_KARACHI: return 300;
        case TIMEZONE_ASIA_KATMANDU: return 330;
        case TIMEZONE_ASIA_KHANDYGA: return 540;
        case TIMEZONE_ASIA_KRASNOYARSK: return 420;
        case TIMEZONE_ASIA_KUALA_LUMPUR: return 450;
        case TIMEZONE_ASIA_KUCHING: return 480;
        case TIMEZONE_ASIA_KUWAIT: return 180;
        case TIMEZONE_ASIA_MACAU: return 480;
        case TIMEZONE_ASIA_MAGADAN: return 660;
        case TIMEZONE_ASIA_MAKASSAR: return 480;
        case TIMEZONE_ASIA_MANILA: return 480;
        case TIMEZONE_ASIA_MUSCAT: return 240;
        case TIMEZONE_ASIA_NICOSIA: return 120;
        case TIMEZONE_ASIA_NOVOKUZNETSK: return 420;
        case TIMEZONE_ASIA_NOVOSIBIRSK: return 420;
        case TIMEZONE_ASIA_OMSK: return 360;
        case TIMEZONE_ASIA_ORAL: return 300;
        case TIMEZONE_ASIA_PHNOM_PENH: return 420;
        case TIMEZONE_ASIA_PONTIANAK: return 480;
        case TIMEZONE_ASIA_PYONGYANG: return 540;
        case TIMEZONE_ASIA_QATAR: return 240;
        case TIMEZONE_ASIA_QOSTANAY: return 300;
        case TIMEZONE_ASIA_QYZYLORDA: return 300;
        case TIMEZONE_ASIA_RANGOON: return 390;
        case TIMEZONE_ASIA_RIYADH: return 180;
        case TIMEZONE_ASIA_SAIGON: return 480;
        case TIMEZONE_ASIA_SAKHALIN: return 660;
        case TIMEZONE_ASIA_SAMARKAND: return 300;
        case TIMEZONE_ASIA_SEOUL: return 540;
        case TIMEZONE_ASIA_SHANGHAI: return 480;
        case TIMEZONE_ASIA_SINGAPORE: return 450;
        case TIMEZONE_ASIA_SREDNEKOLYMSK: return 660;
        case TIMEZONE_ASIA_TAIPEI: return 480;
        case TIMEZONE_ASIA_TASHKENT: return 360;
        case TIMEZONE_ASIA_TBILISI: return 240;
        case TIMEZONE_ASIA_TEHRAN: return 210;
        case TIMEZONE_ASIA_THIMPHU: return 330;
        case TIMEZONE_ASIA_TOKYO: return 540;
        case TIMEZONE_ASIA_TOMSK: return 420;
        case TIMEZONE_ASIA_ULAANBAATAR: return 420;
        case TIMEZONE_ASIA_URUMQI: return 360;
        case TIMEZONE_ASIA_UST_NERA: return 540;
        case TIMEZONE_ASIA_VIENTIANE: return 420;
        case TIMEZONE_ASIA_VLADIVOSTOK: return 600;
        case TIMEZONE_ASIA_YAKUTSK: return 540;
        case TIMEZONE_ASIA_YEKATERINBURG: return 300;
        case TIMEZONE_ASIA_YEREVAN: return 240;
        case TIMEZONE_ATLANTIC_AZORES: return -60;
        case TIMEZONE_ATLANTIC_BERMUDA: return -240;
        case TIMEZONE_ATLANTIC_CANARY: return 0;
        case TIMEZONE_ATLANTIC_CAPE_VERDE: return -120;
        case TIMEZONE_ATLANTIC_FAEROE: return 0;
        case TIMEZONE_ATLANTIC_MADEIRA: return 0;
        case TIMEZONE_ATLANTIC_REYKJAVIK: return 0;
        case TIMEZONE_ATLANTIC_SOUTH_GEORGIA: return -120;
        case TIMEZONE_ATLANTIC_ST_HELENA: return 0;
        case TIMEZONE_ATLANTIC_STANLEY: return -240;
        case TIMEZONE_AUSTRALIA_ADELAIDE: return 570;
        case TIMEZONE_AUSTRALIA_BRISBANE: return 600;
        case TIMEZONE_AUSTRALIA_BROKEN_HILL: return 570;
        case TIMEZONE_AUSTRALIA_DARWIN: return 570;
        case TIMEZONE_AUSTRALIA_EUCLA: return 525;
        case TIMEZONE_AUSTRALIA_HOBART: return 660;
        case TIMEZONE_AUSTRALIA_LINDEMAN: return 600;
        case TIMEZONE_AUSTRALIA_LORD_HOWE: return 600;
        case TIMEZONE_AUSTRALIA_MELBOURNE: return 600;
        case TIMEZONE_AUSTRALIA_PERTH: return 480;
        case TIMEZONE_AUSTRALIA_SYDNEY: return 600;
        case TIMEZONE_EUROPE_AMSTERDAM: return 60;
        case TIMEZONE_EUROPE_ANDORRA: return 60;
        case TIMEZONE_EUROPE_ASTRAKHAN: return 240;
        case TIMEZONE_EUROPE_ATHENS: return 120;
        case TIMEZONE_EUROPE_BELGRADE: return 60;
        case TIMEZONE_EUROPE_BERLIN: return 60;
        case TIMEZONE_EUROPE_BRATISLAVA: return 60;
        case TIMEZONE_EUROPE_BRUSSELS: return 60;
        case TIMEZONE_EUROPE_BUCHAREST: return 120;
        case TIMEZONE_EUROPE_BUDAPEST: return 60;
        case TIMEZONE_EUROPE_BUSINGEN: return 60;
        case TIMEZONE_EUROPE_CHISINAU: return 180;
        case TIMEZONE_EUROPE_COPENHAGEN: return 60;
        case TIMEZONE_EUROPE_DUBLIN: return 60;
        case TIMEZONE_EUROPE_GIBRALTAR: return 60;
        case TIMEZONE_EUROPE_GUERNSEY: return 60;
        case TIMEZONE_EUROPE_HELSINKI: return 120;
        case TIMEZONE_EUROPE_ISLE_OF_MAN: return 60;
        case TIMEZONE_EUROPE_ISTANBUL: return 120;
        case TIMEZONE_EUROPE_JERSEY: return 60;
        case TIMEZONE_EUROPE_KALININGRAD: return 180;
        case TIMEZONE_EUROPE_KIEV: return 180;
        case TIMEZONE_EUROPE_KIROV: return 240;
        case TIMEZONE_EUROPE_LISBON: return 60;
        case TIMEZONE_EUROPE_LJUBLJANA: return 60;
        case TIMEZONE_EUROPE_LONDON: return 60;
        case TIMEZONE_EUROPE_LUXEMBOURG: return 60;
        case TIMEZONE_EUROPE_MADRID: return 60;
        case TIMEZONE_EUROPE_MALTA: return 60;
        case TIMEZONE_EUROPE_MARIEHAMN: return 120;
        case TIMEZONE_EUROPE_MINSK: return 180;
        case TIMEZONE_EUROPE_MONACO: return 60;
        case TIMEZONE_EUROPE_MOSCOW: return 180;
        case TIMEZONE_EUROPE_OSLO: return 60;
        case TIMEZONE_EUROPE_PARIS: return 60;
        case TIMEZONE_EUROPE_PODGORICA: return 60;
        case TIMEZONE_EUROPE_PRAGUE: return 60;
        case TIMEZONE_EUROPE_RIGA: return 180;
        case TIMEZONE_EUROPE_ROME: return 60;
        case TIMEZONE_EUROPE_SAMARA: return 240;
        case TIMEZONE_EUROPE_SAN_MARINO: return 60;
        case TIMEZONE_EUROPE_SARAJEVO: return 60;
        case TIMEZONE_EUROPE_SARATOV: return 240;
        case TIMEZONE_EUROPE_SIMFEROPOL: return 180;
        case TIMEZONE_EUROPE_SKOPJE: return 60;
        case TIMEZONE_EUROPE_SOFIA: return 120;
        case TIMEZONE_EUROPE_STOCKHOLM: return 60;
        case TIMEZONE_EUROPE_TALLINN: return 180;
        case TIMEZONE_EUROPE_TIRANE: return 60;
        case TIMEZONE_EUROPE_ULYANOVSK: return 240;
        case TIMEZONE_EUROPE_VADUZ: return 60;
        case TIMEZONE_EUROPE_VATICAN: return 60;
        case TIMEZONE_EUROPE_VIENNA: return 60;
        case TIMEZONE_EUROPE_VILNIUS: return 180;
        case TIMEZONE_EUROPE_VOLGOGRAD: return 240;
        case TIMEZONE_EUROPE_WARSAW: return 60;
        case TIMEZONE_EUROPE_ZAGREB: return 60;
        case TIMEZONE_EUROPE_ZURICH: return 60;
        case TIMEZONE_INDIAN_ANTANANARIVO: return 180;
        case TIMEZONE_INDIAN_CHAGOS: return 300;
        case TIMEZONE_INDIAN_CHRISTMAS: return 420;
        case TIMEZONE_INDIAN_COCOS: return 390;
        case TIMEZONE_INDIAN_COMORO: return 180;
        case TIMEZONE_INDIAN_KERGUELEN: return 300;
        case TIMEZONE_INDIAN_MAHE: return 240;
        case TIMEZONE_INDIAN_MALDIVES: return 300;
        case TIMEZONE_INDIAN_MAURITIUS: return 240;
        case TIMEZONE_INDIAN_MAYOTTE: return 180;
        case TIMEZONE_INDIAN_REUNION: return 240;
        case TIMEZONE_PACIFIC_APIA: return -660;
        case TIMEZONE_PACIFIC_AUCKLAND: return 720;
        case TIMEZONE_PACIFIC_BOUGAINVILLE: return 600;
        case TIMEZONE_PACIFIC_CHATHAM: return 765;
        case TIMEZONE_PACIFIC_EASTER: return -360;
        case TIMEZONE_PACIFIC_EFATE: return 660;
        case TIMEZONE_PACIFIC_ENDERBURY: return -720;
        case TIMEZONE_PACIFIC_FAKAOFO: return -660;
        case TIMEZONE_PACIFIC_FIJI: return 720;
        case TIMEZONE_PACIFIC_FUNAFUTI: return 720;
        case TIMEZONE_PACIFIC_GALAPAGOS: return -300;
        case TIMEZONE_PACIFIC_GAMBIER: return -540;
        case TIMEZONE_PACIFIC_GUADALCANAL: return 660;
        case TIMEZONE_PACIFIC_GUAM: return 600;
        case TIMEZONE_PACIFIC_HONOLULU: return -600;
        case TIMEZONE_PACIFIC_KIRITIMATI: return -640;
        case TIMEZONE_PACIFIC_KOSRAE: return 720;
        case TIMEZONE_PACIFIC_KWAJALEIN: return -720;
        case TIMEZONE_PACIFIC_MAJURO: return 720;
        case TIMEZONE_PACIFIC_MARQUESAS: return -570;
        case TIMEZONE_PACIFIC_MIDWAY: return -660;
        case TIMEZONE_PACIFIC_NAURU: return 690;
        case TIMEZONE_PACIFIC_NIUE: return -690;
        case TIMEZONE_PACIFIC_NORFOLK: return 690;
        case TIMEZONE_PACIFIC_NOUMEA: return 660;
        case TIMEZONE_PACIFIC_PAGO_PAGO: return -660;
        case TIMEZONE_PACIFIC_PALAU: return 540;
        case TIMEZONE_PACIFIC_PITCAIRN: return -510;
        case TIMEZONE_PACIFIC_PONAPE: return 660;
        case TIMEZONE_PACIFIC_PORT_MORESBY: return 600;
        case TIMEZONE_PACIFIC_RAROTONGA: return -630;
        case TIMEZONE_PACIFIC_SAIPAN: return 600;
        case TIMEZONE_PACIFIC_TAHITI: return -600;
        case TIMEZONE_PACIFIC_TARAWA: return 720;
        case TIMEZONE_PACIFIC_TONGATAPU: return 780;
        case TIMEZONE_PACIFIC_TRUK: return 600;
        case TIMEZONE_PACIFIC_WAKE: return 720;
        case TIMEZONE_PACIFIC_WALLIS: return 720;
        default: return 0;
    }
    // clang-format on
}

/**
 * @brief 时间戳转日期时间
 */
struct tm timestamp_to_datatime(time_t timestamp, timezone_e eTimezone, timestamp_unit_e eUnit)
{
    // 1970-01-01: 星期四

    struct tm sTime = {0};

    if (eUnit == TIMESTAMP_UNIT_MS)
    {
        eUnit /= 1000;
    }

    time_t sec = timestamp + GetTimezoneOffsetInMinute(eTimezone) * MinuteToSecond;
    time_t day = sec / DayToSecond; /* 总天数 */

    sTime.tm_year = 1970;              /* 年份 */
    sTime.tm_wday = (3 + day) % 7 + 1; /* 星期 */

    while (day >= CommonYear)
    {
        if (IsLeapYear(sTime.tm_year))
        {
            /* 闰年 */
            if (day >= LeapYear)
            {
                day -= LeapYear;
            }
            else
            {
                break;
            }
        }
        else
        {
            /* 平年 */
            day -= CommonYear;
        }

        sTime.tm_year++; /* 年份 */
    }

    while (day >= CommonFeb)
    {
        if ((sTime.tm_mon == 1) && IsLeapYear(sTime.tm_year))
        {
            /* 闰年2月 */
            if (day >= LeapFeb)
            {
                sTime.tm_yday += LeapFeb;
                day -= LeapFeb;
            }
            else
            {
                break;
            }
        }
        else
        {
            /* 平年 */
            if (day >= m_cau8MonthDay[sTime.tm_mon])
            {
                sTime.tm_yday += m_cau8MonthDay[sTime.tm_mon];
                day -= m_cau8MonthDay[sTime.tm_mon];
            }
            else
            {
                break;
            }
        }

        sTime.tm_mon++;
    }

    sTime.tm_mon += 1;              /* 月份 */
    sTime.tm_mday = day + 1;        /* 日期(月) */
    sTime.tm_yday += sTime.tm_mday; /* 日期(年) */

    day           = sec % DayToSecond;                     /* 秒 */
    sTime.tm_hour = day / HourToSecond;                    /* 小时 */
    sTime.tm_min  = (day % HourToSecond) / MinuteToSecond; /* 分钟 */
    sTime.tm_sec  = (day % HourToSecond) % MinuteToSecond; /* 秒钟 */

    return sTime;
}

time_t datatime_to_timestamp(struct tm* pDataTime, timezone_e eTimezone, timestamp_unit_e eUnit)
{
    time_t   day = 0;
    time_t   hour;
    time_t   minute;
    time_t   second;
    time_t   timestamp;
    uint32_t i;

    for (i = 1970; i < pDataTime->tm_year; i++)
    {
        if (IsLeapYear(i))
        {
            day += 366;
        }
        else
        {
            day += 365;
        }
    }

    for (i = 0; i < pDataTime->tm_mon - 1; i++)
    {
        day += m_cau8MonthDay[i];
    }

    if (IsLeapYear(pDataTime->tm_year))
    {
        day += 1;  // 闰年
    }

    day += pDataTime->tm_mday - 1;
    hour   = day * 24 + pDataTime->tm_hour;
    minute = hour * 60 + pDataTime->tm_min;
    second = minute * 60 + pDataTime->tm_sec;

    timestamp = second - GetTimezoneOffsetInMinute(eTimezone) * MinuteToSecond;

    if (eUnit == TIMESTAMP_UNIT_MS)
    {
        timestamp *= 1000;
    }

    return timestamp;
}
