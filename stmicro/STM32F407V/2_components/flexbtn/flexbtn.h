#ifndef __FLEXBTN_H__
#define __FLEXBTN_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_FLEXBTN_SCAN_FREQ_HZ 50  // How often FlexBtn_Cycle() is called

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define FLEXBTN_MS_TO_SCAN_CNT(ms) (ms / (1000 / CONFIG_FLEXBTN_SCAN_FREQ_HZ))

typedef enum {
    FLEXBTN_EVENT_PRESS,
    FLEXBTN_EVENT_RELEASE,  // Click
    FLEXBTN_EVENT_DOUBLE_CLICK,
    FLEXBTN_EVENT_REPEAT_CLICK,
    FLEXBTN_EVENT_SHORT_PRESS,
    FLEXBTN_EVENT_SHORT_RELEASE,
    FLEXBTN_EVENT_LONG_PRESS,
    FLEXBTN_EVENT_LONG_RELEASE,
    FLEXBTN_EVENT_LONG_HOLD,
    FLEXBTN_EVENT_LONG_HOLD_RELEASE,
    FLEXBTN_EVENT_MAX,
    FLEXBTN_EVENT_NONE,
} flexbtn_event_e;

typedef enum {
    FLEXBTN_STATE_DEFAULT,
    FLEXBTN_STATE_PRESS,
    FLEXBTN_STATE_MULTIPLE_CLICK,
} flexbtn_state_e;

/**
 * @brief Flexible Button
 */
typedef struct flexbtn flexbtn_t;

struct flexbtn {
    uint8_t u8ID; /* [in] button id */

    uint16_t u16ShortPressStartTick;       /* [in] short press start time */
    uint16_t u16LongPressStartTick;        /* [in] long press start time */
    uint16_t u16LongHoldStartTick;         /* [in] long hold start time */
    uint16_t u16MaxMultipleClicksInterval; /* [in] multiple clicks interval */

    void (*pfnEventCb)(flexbtn_t* pHandle, flexbtn_event_e eEvent); /* [in]  */
    bool (*pfnIsPressed)(flexbtn_t* pHandle);                       /* [in]  get button state, true:pressed, false:released */

    flexbtn_state_e _eState; /* state */

    uint16_t _u16ScanCnt;  /* number of scans, counted when the button is pressed, plus one per scan cycle. */
    uint16_t _u16ClickCnt; /* number of button clicks */

    flexbtn_t* _pNext; /* one-way linked list, pointing to the next button. */

    flexbtn_event_e eEvent; /* [out] */
};

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief register flexbtn to scan list
 */
bool FlexBtnAttach(flexbtn_t* pHandle);

/**
 * @brief  scan all key.
 * @note   need to be called cyclically within the specified period. (sample cycle: 5~20ms)
 * @return activated button count
 */
uint8_t FlexBtnCycle(void);

/**
 * @brief covert flexbtn_event_e to string
 */
const char* FlexBtnEventStr(flexbtn_event_e eEvent);

#ifdef __cplusplus
}
#endif

#endif
