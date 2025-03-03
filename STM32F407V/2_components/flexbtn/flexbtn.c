#include "flexbtn.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO
#define LOG_LOCAL_TAG   "flexbtn"

#ifndef nullptr
#define nullptr NULL
#endif

#ifndef ASSERT
#define ASSERT(cond, msg)
#endif

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

/**
 * @note linked list head pointer
 *
 *  first registered button is at the end of the 'linked list'.
 *  this member variable points to the head of the 'linked list'.
 *
 */
static flexbtn_t* m_pLinkedList = nullptr;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

bool FlexBtnAttach(flexbtn_t* pHandle)
{
    // check

    ASSERT(pHandle, "pointer is nullptr");
    ASSERT(pHandle->pfnEventCb, "callback function needs to be set");
    ASSERT(pHandle->pfnIsPressed, "callback function needs to be set");

    pHandle->eEvent  = FLEXBTN_EVENT_NONE;
    pHandle->_eState = FLEXBTN_STATE_DEFAULT;

    pHandle->_u16ScanCnt  = 0;
    pHandle->_u16ClickCnt = 0;
    pHandle->_pNext       = nullptr;

    // register

    flexbtn_t* pCurr = m_pLinkedList;

    while (pCurr != nullptr)
    {
        if (pCurr == pHandle)
        {
            /* already exist. */
            return false;
        }

        pCurr = pCurr->_pNext;
    }

    pHandle->_pNext = m_pLinkedList;
    m_pLinkedList   = pHandle;

    return true;
}

/**
 * @brief  handle all key events in one scan cycle.
 * @return activated button count
 */
uint8_t FlexBtnCycle(void)
{
    uint8_t u8ActivedCount = 0;  // count of actived buttons

    for (flexbtn_t* pCurr = m_pLinkedList; pCurr != nullptr; pCurr = pCurr->_pNext)
    {
        bool bIsPressed = pCurr->pfnIsPressed(pCurr);

        if (pCurr->_eState > FLEXBTN_STATE_DEFAULT)
        {
            pCurr->_u16ScanCnt++;

            if (pCurr->_u16ScanCnt >= ((1 << (sizeof(pCurr->_u16ScanCnt) * 8)) - 1))
            {
                pCurr->_u16ScanCnt = pCurr->u16LongHoldStartTick;
            }
        }

        switch (pCurr->_eState)
        {
            case FLEXBTN_STATE_DEFAULT: /* stage: default ( button is released ) */
            {
                if (bIsPressed) /* is pressed */
                {
                    pCurr->_u16ScanCnt  = 0;
                    pCurr->_u16ClickCnt = 0;

                    pCurr->pfnEventCb(pCurr, pCurr->eEvent = FLEXBTN_EVENT_PRESS);

                    /* swtich to button down stage */
                    pCurr->_eState = FLEXBTN_STATE_PRESS;
                }
                else
                {
                    pCurr->eEvent = FLEXBTN_EVENT_NONE;
                }

                break;
            }

            case FLEXBTN_STATE_PRESS: /* stage: down ( button is pressed ) */
            {
                if (bIsPressed) /* is pressed */
                {
                    if (pCurr->_u16ClickCnt > 0) /* multiple click */
                    {
                        if (pCurr->_u16ScanCnt > pCurr->u16MaxMultipleClicksInterval)
                        {
                            pCurr->pfnEventCb(pCurr, pCurr->eEvent = (flexbtn_event_e)MIN(pCurr->_u16ClickCnt, FLEXBTN_EVENT_REPEAT_CLICK));

                            /* swtich to button down stage */
                            pCurr->_eState      = FLEXBTN_STATE_PRESS;
                            pCurr->_u16ScanCnt  = 0;
                            pCurr->_u16ClickCnt = 0;
                        }
                    }
                    else if (pCurr->_u16ScanCnt >= pCurr->u16LongHoldStartTick)
                    {
                        if (pCurr->eEvent != FLEXBTN_EVENT_LONG_HOLD)
                        {
                            pCurr->pfnEventCb(pCurr, pCurr->eEvent = FLEXBTN_EVENT_LONG_HOLD);
                        }
                    }
                    else if (pCurr->_u16ScanCnt >= pCurr->u16LongPressStartTick)
                    {
                        if (pCurr->eEvent != FLEXBTN_EVENT_LONG_PRESS)
                        {
                            pCurr->pfnEventCb(pCurr, pCurr->eEvent = FLEXBTN_EVENT_LONG_PRESS);
                        }
                    }
                    else if (pCurr->_u16ScanCnt >= pCurr->u16ShortPressStartTick)
                    {
                        if (pCurr->eEvent != FLEXBTN_EVENT_SHORT_PRESS)
                        {
                            pCurr->pfnEventCb(pCurr, pCurr->eEvent = FLEXBTN_EVENT_SHORT_PRESS);
                        }
                    }
                }
                else /* button up */
                {
                    if (pCurr->_u16ScanCnt >= pCurr->u16LongHoldStartTick)
                    {
                        pCurr->pfnEventCb(pCurr, pCurr->eEvent = FLEXBTN_EVENT_LONG_HOLD_RELEASE);
                        pCurr->_eState = FLEXBTN_STATE_DEFAULT;
                    }
                    else if (pCurr->_u16ScanCnt >= pCurr->u16LongPressStartTick)
                    {
                        pCurr->pfnEventCb(pCurr, pCurr->eEvent = FLEXBTN_EVENT_LONG_RELEASE);
                        pCurr->_eState = FLEXBTN_STATE_DEFAULT;
                    }
                    else if (pCurr->_u16ScanCnt >= pCurr->u16ShortPressStartTick)
                    {
                        pCurr->pfnEventCb(pCurr, pCurr->eEvent = FLEXBTN_EVENT_SHORT_RELEASE);
                        pCurr->_eState = FLEXBTN_STATE_DEFAULT;
                    }
                    else
                    {
                        /* swtich to multiple click stage */
                        pCurr->_eState = FLEXBTN_STATE_MULTIPLE_CLICK;
                        pCurr->_u16ClickCnt++;
                    }
                }

                break;
            }

            case FLEXBTN_STATE_MULTIPLE_CLICK:
            {
                if (bIsPressed) /* is pressed */
                {
                    /* swtich to button down stage */
                    pCurr->_eState     = FLEXBTN_STATE_PRESS;
                    pCurr->_u16ScanCnt = 0;
                }
                else
                {
                    if (pCurr->_u16ScanCnt > pCurr->u16MaxMultipleClicksInterval)
                    {
                        pCurr->pfnEventCb(pCurr, pCurr->eEvent = (flexbtn_event_e)MIN(pCurr->_u16ClickCnt, FLEXBTN_EVENT_REPEAT_CLICK));

                        /* swtich to default stage */
                        pCurr->_eState = FLEXBTN_STATE_DEFAULT;
                    }
                }

                break;
            }
        }

        if (pCurr->_eState > FLEXBTN_STATE_DEFAULT)
        {
            u8ActivedCount++;
        }
    }

    return u8ActivedCount;
}

const char* FlexBtnEventStr(flexbtn_event_e eEvent)
{
    switch (eEvent)
    {
        case FLEXBTN_EVENT_PRESS:
        {
            return "Press";
        }
        case FLEXBTN_EVENT_RELEASE:
        {
            return "Release";
        }
        case FLEXBTN_EVENT_DOUBLE_CLICK:
        {
            return "DoubleClick";
        }
        case FLEXBTN_EVENT_REPEAT_CLICK:
        {
            return "RepeatClick";
        }
        case FLEXBTN_EVENT_SHORT_PRESS:
        {
            return "ShortPress";
        }
        case FLEXBTN_EVENT_SHORT_RELEASE:
        {
            return "ShortRelease";
        }
        case FLEXBTN_EVENT_LONG_PRESS:
        {
            return "LongPress";
        }
        case FLEXBTN_EVENT_LONG_RELEASE:
        {
            return "LongRelease";
        }
        case FLEXBTN_EVENT_LONG_HOLD:
        {
            return "LongHold";
        }
        case FLEXBTN_EVENT_LONG_HOLD_RELEASE:
        {
            return "LongHoldRelease";
        }
        default:
        case FLEXBTN_EVENT_NONE:
        {
            return "";
        }
    }
}
