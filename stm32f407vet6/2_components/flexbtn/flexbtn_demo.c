#include "flexbtn.h"

#if CONFIG_DEMOS_SW

#include "pinctrl.h"
#include "delay.h"

#define CONFIG_COMBINED_EVENTS_SW 1

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "flexbtn-demo"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

typedef enum {
    BUTTON_PREV,
    BUTTON_OKAY,
    BUTTON_NEXT,
} button_id_e;

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static void EventCb(flexbtn_t* pHandle, flexbtn_event_e eEvent);
static bool IsPressed(flexbtn_t* pHandle);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static pin_t keys[] = {
    {KEY1_PIN},
    {KEY2_PIN},
    {KEY3_PIN},
};

static flexbtn_t flexbtn[3] = {
    {
     .u8ID                         = BUTTON_PREV,
     .u16ShortPressStartTick       = FLEXBTN_MS_TO_SCAN_CNT(1500),
     .u16LongPressStartTick        = FLEXBTN_MS_TO_SCAN_CNT(3000),
     .u16LongHoldStartTick         = FLEXBTN_MS_TO_SCAN_CNT(4500),
     .u16MaxMultipleClicksInterval = FLEXBTN_MS_TO_SCAN_CNT(300),
     .pfnIsPressed                 = IsPressed,
     .pfnEventCb                   = EventCb,
     },

    {
     .u8ID                         = BUTTON_OKAY,
     .u16ShortPressStartTick       = FLEXBTN_MS_TO_SCAN_CNT(1000),
     .u16LongPressStartTick        = FLEXBTN_MS_TO_SCAN_CNT(2000),
     .u16LongHoldStartTick         = FLEXBTN_MS_TO_SCAN_CNT(3000),
     .u16MaxMultipleClicksInterval = FLEXBTN_MS_TO_SCAN_CNT(300),
     .pfnIsPressed                 = IsPressed,
     .pfnEventCb                   = EventCb,
     },

    {
     .u8ID                         = BUTTON_NEXT,
     .u16ShortPressStartTick       = FLEXBTN_MS_TO_SCAN_CNT(1500),
     .u16LongPressStartTick        = FLEXBTN_MS_TO_SCAN_CNT(3000),
     .u16LongHoldStartTick         = FLEXBTN_MS_TO_SCAN_CNT(4500),
     .u16MaxMultipleClicksInterval = FLEXBTN_MS_TO_SCAN_CNT(300),
     .pfnIsPressed                 = IsPressed,
     .pfnEventCb                   = EventCb,
     },
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static bool IsPressed(flexbtn_t* pHandle)
{
    switch ((button_id_e)(pHandle->u8ID))
    {
        case BUTTON_PREV:
        case BUTTON_OKAY:
        case BUTTON_NEXT:
        {
            return PIN_ReadLevel(&keys[pHandle->u8ID]) == PIN_LEVEL_LOW;
        }

        default:
        {
            LOGW("unknown button id");
            break;
        }
    }

    return false;
}

static void EventCb(flexbtn_t* pHandle, flexbtn_event_e eEvent)
{
    if (flexbtn[BUTTON_NEXT].eEvent == FLEXBTN_EVENT_LONG_HOLD && flexbtn[BUTTON_OKAY].eEvent == FLEXBTN_EVENT_RELEASE)
    {
        LOGI("combined key event = %s + %s", FlexBtnEventStr(flexbtn[BUTTON_NEXT].eEvent), FlexBtnEventStr(flexbtn[BUTTON_OKAY].eEvent));
    }
    else
    {
        LOGI("key%d event = %s", pHandle->u8ID, FlexBtnEventStr(eEvent));
    }
}

void FlexBtn_Test(void)
{
    for (int i = 0; i < ARRAY_SIZE(flexbtn); ++i)
    {
        PIN_SetMode(&keys[i], PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

        FlexBtnAttach(&flexbtn[i]);
    }

    while (1)
    {
        static tick_t t = 0;

        if (DelayNonBlockMs(t, 1000 / CONFIG_FLEXBTN_SCAN_FREQ_HZ))
        {
            FlexBtnCycle();
            t = GetTickUs();
        }
    }
}

#endif
