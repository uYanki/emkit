#ifndef __I2C_APDS9960_H__
#define __I2C_APDS9960_H__

#include "i2cmst.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define APDS9960_ADDRESS 0x39

#define APDS9960_ID_1    0xAB
#define APDS9960_ID_2    0x9C
#define APDS9960_ID_3    0xA8

typedef struct {
    // power on
    uint8_t PON : 1;

    // ALS enable
    uint8_t AEN : 1;

    // Proximity detect enable
    uint8_t PEN : 1;

    // wait timer enable
    uint8_t WEN : 1;

    // ALS interrupt enable
    uint8_t AIEN : 1;

    // proximity interrupt enable
    uint8_t PIEN : 1;

    // gesture enable
    uint8_t GEN : 1;
} apds9960_register_enable_t;

typedef struct {
    // ALS Interrupt Persistence. Controls rate of Clear channel interrupt to
    // the host processor
    uint8_t APERS : 4;

    // proximity interrupt persistence, controls rate of prox interrupt to host
    // processor
    uint8_t PPERS : 4;
} apds9960_register_pers_t;

typedef struct {
    uint8_t WLONG : 1;
} apds9960_register_config1_t;

typedef struct {
    /*Proximity Pulse Count. Specifies the number of proximity pulses to be
    generated on LDR. Number of pulses is set by PPULSE value plus 1.
    */
    uint8_t PPULSE : 6;

    // Proximity Pulse Length. Sets the LED-ON pulse width during a proximity
    // LDR pulse.
    uint8_t PPLEN : 2;

} apds9960_register_ppulse_t;

typedef struct {
    // ALS and Color gain control
    uint8_t AGAIN : 2;

    // proximity gain control
    uint8_t PGAIN : 2;

    // led drive strength
    uint8_t LDRIVE : 2;
} apds9960_register_control_t;

typedef struct {
    /* Additional LDR current during proximity and gesture LED pulses. Current
    value, set by LDRIVE, is increased by the percentage of LED_BOOST.
    */
    uint8_t LED_BOOST : 2;

    // clear photodiode saturation int enable
    uint8_t CPSIEN : 1;

    // proximity saturation interrupt enable
    uint8_t PSIEN : 1;
} apds9960_register_config2_t;

typedef struct {
    // proximity mask
    uint8_t PMASK_R : 1;
    uint8_t PMASK_L : 1;
    uint8_t PMASK_D : 1;
    uint8_t PMASK_U : 1;

    /* Sleep After Interrupt. When enabled, the device will automatically enter
    low power mode when the INT pin is asserted and the state machine has
    progressed to the SAI decision block. Normal operation is resumed when INT
    pin is cleared over I2C.
    */
    uint8_t SAI : 1;

    /* Proximity Gain Compensation Enable. This bit provides gain compensation
    when proximity photodiode signal is reduced as a result of sensor masking.
    If only one diode of the diode pair is contributing, then only half of the
    signal is available at the ADC; this results in a maximum ADC value of 127.
    Enabling PCMP enables an additional gain of 2X, resulting in a maximum ADC
    value of 255.
    */
    uint8_t PCMP : 1;
} apds9960_register_config3_t;

typedef struct {
    /* Gesture Exit Persistence. When a number of consecutive gesture end
    occurrences become equal or greater to the GEPERS value, the Gesture state
    machine is exited.
    */
    uint8_t GEXPERS : 2;

    /* Gesture Exit Mask. Controls which of the gesture detector photodiodes
    (UDLR) will be included to determine a gesture end and subsequent exit
    of the gesture state machine. Unmasked UDLR data will be compared with the
    value in GTHR_OUT. Field value bits correspond to UDLR detectors.
    */
    uint8_t GEXMSK : 4;

    /* Gesture FIFO Threshold. This value is compared with the FIFO Level (i.e.
    the number of UDLR datasets) to generate an interrupt (if enabled).
    */
    uint8_t GFIFOTH : 2;
} apds9960_register_gconf1_t;

typedef struct {
    /* Gesture Wait Time. The GWTIME controls the amount of time in a low power
    mode between gesture detection cycles.
    */
    uint8_t GWTIME : 3;

    // Gesture LED Drive Strength. Sets LED Drive Strength in gesture mode.
    uint8_t GLDRIVE : 2;

    // Gesture Gain Control. Sets the gain of the proximity receiver in gesture
    // mode.
    uint8_t GGAIN : 2;
} apds9960_register_gconf2_t;

typedef struct {
    /* Gesture Dimension Select. Selects which gesture photodiode pairs are
    enabled to gather results during gesture.
    */
    uint8_t GDIMS : 2;
} apds9960_register_gconf3_t;

typedef struct {
    /* Gesture Mode. Reading this bit reports if the gesture state machine is
    actively running, 1 = Gesture, 0= ALS, Proximity, Color. Writing a 1 to this
    bit causes immediate entry in to the gesture state machine (as if GPENTH had
    been exceeded). Writing a 0 to this bit causes exit of gesture when current
    analog conversion has finished (as if GEXTH had been exceeded).
    */
    uint8_t GMODE : 1;

    /* Gesture interrupt enable. Gesture Interrupt Enable. When asserted, all
    gesture related interrupts are unmasked.
    */
    uint8_t GIEN : 2;
} apds9960_register_gconf4_t;

typedef struct {
    /* Number of Gesture Pulses. Specifies the number of pulses to be generated
    on LDR. Number of pulses is set by GPULSE value plus 1.
    */
    uint8_t GPULSE : 6;

    // Gesture Pulse Length. Sets the LED_ON pulse width during a Gesture LDR
    // Pulse.
    uint8_t GPLEN : 2;
} apds9960_register_gpulse_t;

typedef struct {
    /* ALS Valid. Indicates that an ALS cycle has completed since AEN was
    asserted or since a read from any of the ALS/Color data registers.
    */
    uint8_t AVALID : 1;

    /* Proximity Valid. Indicates that a proximity cycle has completed since PEN
    was asserted or since PDATA was last read. A read of PDATA automatically
    clears PVALID.
    */
    uint8_t PVALID : 1;

    /* Gesture Interrupt. GINT is asserted when GFVLV becomes greater than
    GFIFOTH or if GVALID has become asserted when GMODE transitioned to zero.
    The bit is reset when FIFO is completely emptied (read).
    */
    uint8_t GINT : 1;

    // ALS Interrupt. This bit triggers an interrupt if AIEN in ENABLE is set.
    uint8_t AINT : 1;

    // Proximity Interrupt. This bit triggers an interrupt if PIEN in ENABLE is
    // set.
    uint8_t PINT : 1;

    /* Indicates that an analog saturation event occurred during a previous
    proximity or gesture cycle. Once set, this bit remains set until cleared by
    clear proximity interrupt special function command (0xE5 PICLEAR) or by
    disabling Prox (PEN=0). This bit triggers an interrupt if PSIEN is set.
    */
    uint8_t PGSAT : 1;

    /* Clear Photodiode Saturation. When asserted, the analog sensor was at the
    upper end of its dynamic range. The bit can be de-asserted by sending a
    Clear channel interrupt command (0xE6 CICLEAR) or by disabling the ADC
    (AEN=0). This bit triggers an interrupt if CPSIEN is set.
    */
    uint8_t CPSAT : 1;
} apds9960_register_status_t;

typedef struct
{
    /* Gesture FIFO Data. GVALID bit is sent when GFLVL becomes greater than
    GFIFOTH (i.e. FIFO has enough data to set GINT). GFIFOD is reset when GMODE
    = 0 and the GFLVL=0 (i.e. All FIFO data has been read).
    */
    uint8_t GVALID : 1;

    /* Gesture FIFO Overflow. A setting of 1 indicates that the FIFO has filled
    to capacity and that new gesture detector data has been lost.
    */
    uint8_t GFOV : 1;
} apds9960_register_gstatus_t;

/** ALS gain */
typedef enum {
    APDS9960_AGAIN_1X  = 0x00, /**< No gain */
    APDS9960_AGAIN_4X  = 0x01, /**< 2x gain */
    APDS9960_AGAIN_16X = 0x02, /**< 16x gain */
    APDS9960_AGAIN_64X = 0x03  /**< 64x gain */
} apds9960_als_gain_e;

/** Proxmity gain */
typedef enum {
    APDS9960_PGAIN_1X = 0x00, /**< 1x gain */
    APDS9960_PGAIN_2X = 0x01, /**< 2x gain */
    APDS9960_PGAIN_4X = 0x02, /**< 4x gain */
    APDS9960_PGAIN_8X = 0x03  /**< 8x gain */
} apds9960_proxmity_gain_e;

/** Pulse length */
typedef enum {
    APDS9960_PPULSELEN_4US  = 0x00, /**< 4uS */
    APDS9960_PPULSELEN_8US  = 0x01, /**< 8uS */
    APDS9960_PPULSELEN_16US = 0x02, /**< 16uS */
    APDS9960_PPULSELEN_32US = 0x03  /**< 32uS */
} apds9960_proxmity_pulse_length_e;

/** LED drive */
typedef enum {
    APDS9960_LEDDRIVE_100MA = 0x00, /**< 100mA */
    APDS9960_LEDDRIVE_50MA  = 0x01, /**< 50mA */
    APDS9960_LEDDRIVE_25MA  = 0x02, /**< 25mA */
    APDS9960_LEDDRIVE_12MA  = 0x03  /**< 12.5mA */
} apds9960_led_drive_e;

/** LED boost */
typedef enum {
    APDS9960_LEDBOOST_100PCNT = 0x00, /**< 100% */
    APDS9960_LEDBOOST_150PCNT = 0x01, /**< 150% */
    APDS9960_LEDBOOST_200PCNT = 0x02, /**< 200% */
    APDS9960_LEDBOOST_300PCNT = 0x03  /**< 300% */
} apds9960_led_boost_e;

/** Dimensions */
typedef enum {
    APDS9960_DIMENSIONS_ALL        = 0x00,  // All dimensions
    APDS9960_DIMENSIONS_UP_DOWN    = 0x01,  // Up/Down dimensions
    APDS9960_DIMENSIONS_LEFT_RIGHT = 0x02,  // Left/Right dimensions
} apds9960_dimensions_e;

/** FIFO Interrupts */
typedef enum {
    APDS9960_GFIFO_1  = 0x00,  // Generate interrupt after 1 dataset in FIFO
    APDS9960_GFIFO_4  = 0x01,  // Generate interrupt after 2 datasets in FIFO
    APDS9960_GFIFO_8  = 0x02,  // Generate interrupt after 3 datasets in FIFO
    APDS9960_GFIFO_16 = 0x03,  // Generate interrupt after 4 datasets in FIFO
} apds9960_fifo_e;

/** Gesture Gain */
typedef enum {
    APDS9960_GGAIN_1 = 0x00,  // Gain 1x
    APDS9960_GGAIN_2 = 0x01,  // Gain 2x
    APDS9960_GGAIN_4 = 0x02,  // Gain 4x
    APDS9960_GGAIN_8 = 0x03,  // Gain 8x
} apds9960_gesture_gain_e;

/** Gesture Pulse Lenghts */
typedef enum {
    APDS9960_GPULSE_4US  = 0x00,  // Pulse 4us
    APDS9960_GPULSE_8US  = 0x01,  // Pulse 8us
    APDS9960_GPULSE_16US = 0x02,  // Pulse 16us
    APDS9960_GPULSE_32US = 0x03,  // Pulse 32us
} apds9960_gpulse_e;

/* Gesture wait time */
typedef enum {
    APDS9960_GWTIME_0MS    = 0,
    APDS9960_GWTIME_2_8MS  = 1,
    APDS9960_GWTIME_5_6MS  = 2,
    APDS9960_GWTIME_8_4MS  = 3,
    APDS9960_GWTIME_14_0MS = 4,
    APDS9960_GWTIME_22_4MS = 5,
    APDS9960_GWTIME_30_8MS = 6,
    APDS9960_GWTIME_39_2MS = 7,
} apds9960_gwtime_e;

typedef enum {
    APDS9960_GESTURE_NONE  = 0x00,
    APDS9960_GESTURE_UP    = 0x01, /**< Gesture Up */
    APDS9960_GESTURE_DOWN  = 0x02, /**< Gesture Down */
    APDS9960_GESTURE_LEFT  = 0x03, /**< Gesture Left */
    APDS9960_GESTURE_RIGHT = 0x04, /**< Gesture Right */
} apds9960_gesture_e;

typedef struct {
    __IN i2c_mst_t* hI2C;
    __IN uint8_t    u8SlvAddr;

    // registers
    apds9960_register_enable_t  _enable;
    apds9960_register_pers_t    _pers;
    apds9960_register_config1_t _config1;
    apds9960_register_ppulse_t  _ppulse;
    apds9960_register_control_t _control;
    apds9960_register_config2_t _config2;
    apds9960_register_config3_t _config3;
    apds9960_register_status_t  _status;
    apds9960_register_gstatus_t _gstatus;
    apds9960_register_gconf1_t  _gconf1;
    apds9960_register_gconf4_t  _gconf4;
    apds9960_register_gconf2_t  _gconf2;
    apds9960_register_gconf3_t  _gconf3;
    apds9960_register_gpulse_t  _gpulse;

    // gesture
    uint8_t _u8UCount;  // up
    uint8_t _u8DCount;  // down
    uint8_t _u8LCount;  // left
    uint8_t _u8RCount;  // right
    uint8_t _u8GestCnt;
} i2c_apds9960_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

status_t            APDS9960_Init(i2c_apds9960_t* pHandle, uint16_t u16TimeMs, apds9960_als_gain_e eAGain);
void                APDS9960_SetADCIntegrationTime(i2c_apds9960_t* pHandle, uint16_t u16TimeMs);
float32_t           APDS9960_GetADCIntegrationTime(i2c_apds9960_t* pHandle);
void                APDS9960_SetAlsGain(i2c_apds9960_t* pHandle, apds9960_als_gain_e eAGain);
apds9960_als_gain_e APDS9960_GetAlsGain(i2c_apds9960_t* pHandle);
void                APDS9960_SetLED(i2c_apds9960_t* pHandle, apds9960_led_drive_e eLedDrive, apds9960_led_boost_e eLedBoost);

// proximity
void                     APDS9960_EnableProximity(i2c_apds9960_t* pHandle, bool bEnable);
void                     APDS9960_SetProxGain(i2c_apds9960_t* pHandle, apds9960_proxmity_gain_e ePGain);
apds9960_proxmity_gain_e APDS9960_GetProxGain(i2c_apds9960_t* pHandle);
void                     APDS9960_SetProxPulse(i2c_apds9960_t* pHandle, apds9960_proxmity_pulse_length_e ePulseLen, uint8_t u8Pulses);
void                     APDS9960_EnableProximityInterrupt(i2c_apds9960_t* pHandle);
void                     APDS9960_DisableProximityInterrupt(i2c_apds9960_t* pHandle);
uint8_t                  APDS9960_ReadProximity(i2c_apds9960_t* pHandle);
void                     APDS9960_SetProximityInterruptThreshold(i2c_apds9960_t* pHandle, uint8_t u8LowThresh, uint8_t u8HighThresh, uint8_t u8Persistence);
bool                     APDS9960_GetProximityInterrupt(i2c_apds9960_t* pHandle);

// gesture
void               APDS9960_EnableGesture(i2c_apds9960_t* pHandle, bool bEnable);
bool               APDS9960_GestureValid(i2c_apds9960_t* pHandle);
void               APDS9960_SetGestureDimensions(i2c_apds9960_t* pHandle, uint8_t u8Dims);
void               APDS9960_SetGestureFifoThreshold(i2c_apds9960_t* pHandle, uint8_t u8Thresh);
void               APDS9960_SetGestureGain(i2c_apds9960_t* pHandle, uint8_t u8Gain);
void               APDS9960_SetGestureProximityThreshold(i2c_apds9960_t* pHandle, uint8_t u8Thresh);
void               APDS9960_SetGestureOffset(i2c_apds9960_t* pHandle, uint8_t u8OffsetUp, uint8_t u8OffsetDown, uint8_t u8OffsetLeft, uint8_t u8OffsetRight);
apds9960_gesture_e APDS9960_ReadGesture(i2c_apds9960_t* pHandle);
void               APDS9960_ResetCounts(i2c_apds9960_t* pHandle);

// light & color
void     APDS9960_EnableColor(i2c_apds9960_t* pHandle, bool bEnable);
bool     APDS9960_ColorDataReady(i2c_apds9960_t* pHandle);
void     APDS9960_GetColorData(i2c_apds9960_t* pHandle, uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c);
uint16_t APDS9960_CalculateColorTemperature(i2c_apds9960_t* pHandle, uint16_t r, uint16_t g, uint16_t b);
uint16_t APDS9960_CalculateLux(i2c_apds9960_t* pHandle, uint16_t r, uint16_t g, uint16_t b);
void     APDS9960_EnableColorInterrupt(i2c_apds9960_t* pHandle);
void     APDS9960_DisableColorInterrupt(i2c_apds9960_t* pHandle);
void     APDS9960_ClearInterrupt(i2c_apds9960_t* pHandle);
void     APDS9960_SetIntLimits(i2c_apds9960_t* pHandle, uint16_t u16Low, uint16_t u16High);

// turn on/off elements
void APDS9960_Enable(i2c_apds9960_t* pHandle, bool bEnable);

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW
void APDS9960_Test(i2c_mst_t* hI2C);
#endif

#ifdef __cplusplus
}
#endif

#endif
