#include "i2c_apds9960.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "apds9960"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief register
 */
#define APDS9960_RAM        0x00
#define APDS9960_ENABLE     0x80
#define APDS9960_ATIME      0x81
#define APDS9960_WTIME      0x83
#define APDS9960_AILTIL     0x84
#define APDS9960_AILTH      0x85
#define APDS9960_AIHTL      0x86
#define APDS9960_AIHTH      0x87
#define APDS9960_PILT       0x89
#define APDS9960_PIHT       0x8B
#define APDS9960_PERS       0x8C
#define APDS9960_CONFIG1    0x8D
#define APDS9960_PPULSE     0x8E
#define APDS9960_CONTROL    0x8F
#define APDS9960_CONFIG2    0x90
#define APDS9960_ID         0x92
#define APDS9960_STATUS     0x93
#define APDS9960_CDATAL     0x94
#define APDS9960_CDATAH     0x95
#define APDS9960_RDATAL     0x96
#define APDS9960_RDATAH     0x97
#define APDS9960_GDATAL     0x98
#define APDS9960_GDATAH     0x99
#define APDS9960_BDATAL     0x9A
#define APDS9960_BDATAH     0x9B
#define APDS9960_PDATA      0x9C
#define APDS9960_POFFSET_UR 0x9D
#define APDS9960_POFFSET_DL 0x9E
#define APDS9960_CONFIG3    0x9F
#define APDS9960_GPENTH     0xA0
#define APDS9960_GEXTH      0xA1
#define APDS9960_GCONF1     0xA2
#define APDS9960_GCONF2     0xA3
#define APDS9960_GOFFSET_U  0xA4
#define APDS9960_GOFFSET_D  0xA5
#define APDS9960_GOFFSET_L  0xA7
#define APDS9960_GOFFSET_R  0xA9
#define APDS9960_GPULSE     0xA6
#define APDS9960_GCONF3     0xAA
#define APDS9960_GCONF4     0xAB
#define APDS9960_GFLVL      0xAE
#define APDS9960_GSTATUS    0xAF
#define APDS9960_IFORCE     0xE4
#define APDS9960_PICLEAR    0xE5
#define APDS9960_CICLEAR    0xE6
#define APDS9960_AICLEAR    0xE7
#define APDS9960_GFIFO_U    0xFC
#define APDS9960_GFIFO_D    0xFD
#define APDS9960_GFIFO_L    0xFE
#define APDS9960_GFIFO_R    0xFF

#define FIFO_PAUSE_TIME     30  // Wait period (ms) between FIFO reads

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static inline uint8_t _APDS9960_GetCONTROL(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetENABLE(i2c_apds9960_t* pHandle);
static inline void    _APDS9960_SetSTATUS(i2c_apds9960_t* pHandle, uint8_t u8Data);
static inline uint8_t _APDS9960_GetGCONF1(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetGCONF2(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetGCONF3(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetGCONF4(i2c_apds9960_t* pHandle);
static inline void    _APDS9960_SetGCONF4(i2c_apds9960_t* pHandle, uint8_t u8Data);
static inline uint8_t _APDS9960_GetCONFIG1(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetCONFIG2(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetCONFIG3(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetGPULSE(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetPERS(i2c_apds9960_t* pHandle);
static inline uint8_t _APDS9960_GetPPULSE(i2c_apds9960_t* pHandle);
static inline void    _APDS9960_SetGSTATUS(i2c_apds9960_t* pHandle, uint8_t u8Data);

static inline status_t APDS9960_WriteByte(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data);
static inline uint8_t  APDS9960_ReadByte(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data);
static inline status_t APDS9960_ReadWord(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data);
static inline status_t APDS9960_WriteWord(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data);
static inline status_t APDS9960_ReadBlock(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data, uint8_t u8Count);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

static inline status_t APDS9960_WriteByte(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint8_t u8Data)
{
    return I2C_Master_WriteByte(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u8Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline uint8_t APDS9960_ReadByte(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data)
{
    return I2C_Master_ReadByte(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu8Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline status_t APDS9960_ReadWord(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint16_t* pu16Data)
{
    return I2C_Master_ReadWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_LITTLE_ENDIAN);
}

static inline status_t APDS9960_WriteWord(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint16_t u16Data)
{
    return I2C_Master_WriteWord(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, u16Data, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT | I2C_FLAG_WORD_LITTLE_ENDIAN);
}

static inline status_t APDS9960_ReadBlock(i2c_apds9960_t* pHandle, uint8_t u8MemAddr, uint8_t* pu8Data, uint8_t u8Count)
{
    return I2C_Master_ReadBlock(pHandle->hI2C, pHandle->u8SlvAddr, u8MemAddr, pu8Data, u8Count, I2C_FLAG_SLVADDR_7BIT | I2C_FLAG_MEMADDR_8BIT);
}

static inline uint8_t _APDS9960_GetCONTROL(i2c_apds9960_t* pHandle)
{
    return (pHandle->_control.LDRIVE << 6) | (pHandle->_control.PGAIN << 2) | pHandle->_control.AGAIN;
}

static inline uint8_t _APDS9960_GetENABLE(i2c_apds9960_t* pHandle)
{
    return (pHandle->_enable.GEN << 6) | (pHandle->_enable.PIEN << 5) | (pHandle->_enable.AIEN << 4) | (pHandle->_enable.WEN << 3) | (pHandle->_enable.PEN << 2) | (pHandle->_enable.AEN << 1) | pHandle->_enable.PON;
};

static inline void _APDS9960_SetSTATUS(i2c_apds9960_t* pHandle, uint8_t u8Data)
{
    pHandle->_status.AVALID = u8Data & 0x01;
    pHandle->_status.PVALID = (u8Data >> 1) & 0x01;
    pHandle->_status.GINT   = (u8Data >> 2) & 0x01;
    pHandle->_status.AINT   = (u8Data >> 4) & 0x01;
    pHandle->_status.PINT   = (u8Data >> 5) & 0x01;
    pHandle->_status.PGSAT  = (u8Data >> 6) & 0x01;
    pHandle->_status.CPSAT  = (u8Data >> 7) & 0x01;
}

static inline uint8_t _APDS9960_GetGCONF1(i2c_apds9960_t* pHandle)
{
    return (pHandle->_gconf1.GFIFOTH << 6) | (pHandle->_gconf1.GEXMSK << 2) | pHandle->_gconf1.GEXPERS;
}

static inline uint8_t _APDS9960_GetGCONF2(i2c_apds9960_t* pHandle)
{
    return (pHandle->_gconf2.GGAIN << 5) | (pHandle->_gconf2.GLDRIVE << 3) | pHandle->_gconf2.GWTIME;
}

static inline uint8_t _APDS9960_GetGCONF3(i2c_apds9960_t* pHandle)
{
    return pHandle->_gconf3.GDIMS;
}

static inline uint8_t _APDS9960_GetGCONF4(i2c_apds9960_t* pHandle)
{
    return (pHandle->_gconf4.GIEN << 1) | pHandle->_gconf4.GMODE;
}

static inline void _APDS9960_SetGCONF4(i2c_apds9960_t* pHandle, uint8_t u8Data)
{
    pHandle->_gconf4.GIEN  = (u8Data >> 1) & 0x01;
    pHandle->_gconf4.GMODE = u8Data & 0x01;
}

static inline uint8_t APDS9960_GetCONFIG1(i2c_apds9960_t* pHandle)
{
    return pHandle->_config1.WLONG << 1;
};

static inline uint8_t _APDS9960_GetCONFIG2(i2c_apds9960_t* pHandle)
{
    return (pHandle->_config2.PSIEN << 7) | (pHandle->_config2.CPSIEN << 6) | (pHandle->_config2.LED_BOOST << 4) | 1;
}

static inline uint8_t _APDS9960_GetCONFIG3(i2c_apds9960_t* pHandle)
{
    return (pHandle->_config3.PCMP << 5) | (pHandle->_config3.SAI << 4) | (pHandle->_config3.PMASK_U << 3) | (pHandle->_config3.PMASK_D << 2) | (pHandle->_config3.PMASK_L << 1) | pHandle->_config3.PMASK_R;
}

static inline uint8_t _APDS9960_GetGPULSE(i2c_apds9960_t* pHandle)
{
    return (pHandle->_gpulse.GPLEN << 6) | pHandle->_gpulse.GPULSE;
}

static inline uint8_t _APDS9960_GetPERS(i2c_apds9960_t* pHandle)
{
    return (pHandle->_pers.PPERS << 4) | pHandle->_pers.APERS;
};

static inline uint8_t _APDS9960_GetPPULSE(i2c_apds9960_t* pHandle)
{
    return (pHandle->_ppulse.PPLEN << 6) | pHandle->_ppulse.PPULSE;
}

static inline void _APDS9960_SetGSTATUS(i2c_apds9960_t* pHandle, uint8_t u8Data)
{
    pHandle->_gstatus.GFOV   = (u8Data >> 1) & 0x01;
    pHandle->_gstatus.GVALID = u8Data & 0x01;
}

/**
 *  @brief  Enables the device Disables the device (putting it in lower power sleep mode)
 *  @param  bEnable Enable (true/false)
 */
void APDS9960_Enable(i2c_apds9960_t* pHandle, bool bEnable)
{
    pHandle->_enable.PON = bEnable;
    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
}

/**
 *  @brief  Initializes I2C and configures the sensor
 *  @param  u16TimeMs Integration time
 *  @param  eAGain Gain
 */
status_t APDS9960_Init(i2c_apds9960_t* pHandle, uint16_t u16TimeMs, apds9960_als_gain_e eAGain)
{
    if (I2C_Master_IsDeviceReady(pHandle->hI2C, pHandle->u8SlvAddr, I2C_FLAG_SLVADDR_7BIT) == false)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    /* Make sure we're actually connected */
    uint8_t x;

    APDS9960_ReadByte(pHandle, APDS9960_ID, &x);

    if (x != APDS9960_ID_1)
    {
        return ERR_NO_DEVICE;  // device doesn't exist
    }

    /* Set default integration time and gain */
    APDS9960_SetADCIntegrationTime(pHandle, u16TimeMs);
    APDS9960_SetAlsGain(pHandle, eAGain);

    // disable everything to start
    APDS9960_EnableGesture(pHandle, false);
    APDS9960_EnableProximity(pHandle, false);
    APDS9960_EnableColor(pHandle, false);

    APDS9960_DisableColorInterrupt(pHandle);
    APDS9960_DisableProximityInterrupt(pHandle);
    APDS9960_ClearInterrupt(pHandle);

    /* Note: by default, the device is in power down mode on bootup */
    APDS9960_Enable(pHandle, false);
    DelayBlockMs(10);
    APDS9960_Enable(pHandle, true);
    DelayBlockMs(10);

    // default to all gesture dimensions
    APDS9960_SetGestureDimensions(pHandle, APDS9960_DIMENSIONS_ALL);
    APDS9960_SetGestureFifoThreshold(pHandle, APDS9960_GFIFO_4);
    APDS9960_SetGestureGain(pHandle, APDS9960_GGAIN_4);
    APDS9960_SetGestureProximityThreshold(pHandle, 50);
    APDS9960_ResetCounts(pHandle);

    pHandle->_gpulse.GPLEN  = APDS9960_GPULSE_32US;
    pHandle->_gpulse.GPULSE = 9;  // 10 u8Pulses
    APDS9960_WriteByte(pHandle, APDS9960_GPULSE, _APDS9960_GetGPULSE(pHandle));

    return ERR_NONE;
}

/**
 *  @brief  Sets the integration time for the ADC of the APDS9960, in millis
 *  @param  u16TimeMs Integration time
 */
void APDS9960_SetADCIntegrationTime(i2c_apds9960_t* pHandle, uint16_t u16TimeMs)
{
    float32_t f32Tmp;

    // convert ms into 2.78ms increments
    f32Tmp = 256 - u16TimeMs / 2.78f;
    f32Tmp = CLAMP(f32Tmp, 0.f, 255.f);

    /* Update the timing register */
    APDS9960_WriteByte(pHandle, APDS9960_ATIME, (uint8_t)f32Tmp);
}

/**
 *  @brief  Returns the integration time for the ADC of the APDS9960, in millis
 *  @return Integration time
 */
float32_t APDS9960_GetADCIntegrationTime(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;

    APDS9960_ReadByte(pHandle, APDS9960_ATIME, &u8Data);

    // convert to units of 2.78 ms
    return (256 - u8Data) * 2.78f;
}

/**
 *  @brief  Adjusts the color/ALS gain on the APDS9960 (adjusts the sensitivity to light)
 *  @param  eAGain Gain
 */
void APDS9960_SetAlsGain(i2c_apds9960_t* pHandle, apds9960_als_gain_e eAGain)
{
    pHandle->_control.AGAIN = eAGain;

    /* Update the timing register */
    APDS9960_WriteByte(pHandle, APDS9960_CONTROL, _APDS9960_GetCONTROL(pHandle));
}

/**
 *  @brief  Returns the ADC gain
 *  @return ADC gain
 */
apds9960_als_gain_e APDS9960_GetAlsGain(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;

    APDS9960_ReadByte(pHandle, APDS9960_CONTROL, &u8Data);

    return (apds9960_als_gain_e)(u8Data & 0x03);
}

/**
 *  @brief  Adjusts the Proximity gain on the APDS9960
 *  @param  ePGain Gain
 */
void APDS9960_SetProxGain(i2c_apds9960_t* pHandle, apds9960_proxmity_gain_e ePGain)
{
    pHandle->_control.PGAIN = ePGain;

    /* Update the timing register */
    APDS9960_WriteByte(pHandle, APDS9960_CONTROL, _APDS9960_GetCONTROL(pHandle));
}

/**
 *  @brief  Returns the Proximity gain on the APDS9960
 *  @return Proxmity gain
 */
apds9960_proxmity_gain_e APDS9960_GetProxGain(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;

    APDS9960_ReadByte(pHandle, APDS9960_CONTROL, &u8Data);

    return (apds9960_proxmity_gain_e)((u8Data & 0x0C) >> 2);
}

/**
 *  @brief  Sets number of proxmity u8Pulses
 *  @param  ePulseLen Pulse length
 *  @param  u8Pulses Number of pulses
 */
void APDS9960_SetProxPulse(i2c_apds9960_t* pHandle, apds9960_proxmity_pulse_length_e ePulseLen, uint8_t u8Pulses)
{
    pHandle->_ppulse.PPLEN  = ePulseLen;
    pHandle->_ppulse.PPULSE = CLAMP(u8Pulses, 1, 64) - 1;

    APDS9960_WriteByte(pHandle, APDS9960_PPULSE, _APDS9960_GetPPULSE(pHandle));
}

/**
 *  @brief  Enable proximity readings on APDS9960
 *  @param  bEnable Enable (true/false)
 */
void APDS9960_EnableProximity(i2c_apds9960_t* pHandle, bool bEnable)
{
    pHandle->_enable.PEN = bEnable;

    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
}

/**
 *  @brief  Enable proximity interrupts
 */
void APDS9960_EnableProximityInterrupt(i2c_apds9960_t* pHandle)
{
    pHandle->_enable.PIEN = 1;
    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
    APDS9960_ClearInterrupt(pHandle);
}

/**
 *  @brief  Disable proximity interrupts
 */
void APDS9960_DisableProximityInterrupt(i2c_apds9960_t* pHandle)
{
    pHandle->_enable.PIEN = 0;
    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
}

/**
 *  @brief  Set proxmity interrupt thresholds
 *  @param  u8LowThresh Low threshold
 *  @param  u8HighThresh High threshold
 *  @param  u8Persistence Persistence
 */
void APDS9960_SetProximityInterruptThreshold(i2c_apds9960_t* pHandle, uint8_t u8LowThresh, uint8_t u8HighThresh, uint8_t u8Persistence)
{
    APDS9960_WriteByte(pHandle, APDS9960_PILT, u8LowThresh);
    APDS9960_WriteByte(pHandle, APDS9960_PIHT, u8HighThresh);

    if (u8Persistence > 7)
    {
        u8Persistence = 7;
    }

    pHandle->_pers.PPERS = u8Persistence;
    APDS9960_WriteByte(pHandle, APDS9960_PERS, _APDS9960_GetPERS(pHandle));
}

/**
 *  @brief  Returns proximity interrupt status
 *  @return true if enabled, false otherwise.
 */
bool APDS9960_GetProximityInterrupt(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;
    APDS9960_ReadByte(pHandle, APDS9960_STATUS, &u8Data);
    _APDS9960_SetSTATUS(pHandle, u8Data);
    return pHandle->_status.PINT;
};

/**
 *  @brief  Read proximity data
 *  @return Proximity
 */
uint8_t APDS9960_ReadProximity(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;
    APDS9960_ReadByte(pHandle, APDS9960_PDATA, &u8Data);
    return u8Data;
}

/**
 *  @brief  Returns validity status of a gesture
 *  @return Status (true/false)
 */
bool APDS9960_GestureValid(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;
    APDS9960_ReadByte(pHandle, APDS9960_GSTATUS, &u8Data);
    _APDS9960_SetGSTATUS(pHandle, u8Data);
    return pHandle->_gstatus.GVALID;
}

/**
 *  @brief  Sets gesture dimensions
 *  @param  u8Dims Dimensions (APDS9960_DIMENSIONS_ALL, APDS9960_DIMENSIONS_UP_DOWN, APGS9960_DIMENSIONS_LEFT_RIGHT)
 */
void APDS9960_SetGestureDimensions(i2c_apds9960_t* pHandle, uint8_t u8Dims)
{
    pHandle->_gconf3.GDIMS = u8Dims;
    APDS9960_WriteByte(pHandle, APDS9960_GCONF3, _APDS9960_GetGCONF3(pHandle));
}

/**
 *  @brief  Sets gesture FIFO Threshold
 *  @param  u8Thresh Threshold (APDS9960_GFIFO_1, APDS9960_GFIFO_4, APDS9960_GFIFO_8, APDS9960_GFIFO_16)
 */
void APDS9960_SetGestureFifoThreshold(i2c_apds9960_t* pHandle, uint8_t u8Thresh)
{
    pHandle->_gconf1.GFIFOTH = u8Thresh;
    APDS9960_WriteByte(pHandle, APDS9960_GCONF1, _APDS9960_GetGCONF1(pHandle));
}

/**
 *  @brief  Sets gesture sensor gain
 *  @param  u8Gain Gain (APDS9960_GAIN_1, APDS9960_GAIN_2, APDS9960_GAIN_4, APDS9960_GAIN_8)
 */
void APDS9960_SetGestureGain(i2c_apds9960_t* pHandle, uint8_t u8Gain)
{
    pHandle->_gconf2.GGAIN = u8Gain;
    APDS9960_WriteByte(pHandle, APDS9960_GCONF2, _APDS9960_GetGCONF2(pHandle));
}

/**
 *  @brief  Sets gesture sensor threshold
 *  @param  u8Thresh Threshold
 */
void APDS9960_SetGestureProximityThreshold(i2c_apds9960_t* pHandle, uint8_t u8Thresh)
{
    APDS9960_WriteByte(pHandle, APDS9960_GPENTH, u8Thresh);
}

/**
 *  @brief  Sets gesture sensor offset
 *  @param  u8OffsetUp Up offset
 *  @param  u8OffsetDown Down offset
 *  @param  u8OffsetLeft Left offset
 *  @param  u8OffsetRight Right offset
 */
void APDS9960_SetGestureOffset(i2c_apds9960_t* pHandle, uint8_t u8OffsetUp, uint8_t u8OffsetDown, uint8_t u8OffsetLeft, uint8_t u8OffsetRight)
{
    APDS9960_WriteByte(pHandle, APDS9960_GOFFSET_U, u8OffsetUp);
    APDS9960_WriteByte(pHandle, APDS9960_GOFFSET_D, u8OffsetDown);
    APDS9960_WriteByte(pHandle, APDS9960_GOFFSET_L, u8OffsetLeft);
    APDS9960_WriteByte(pHandle, APDS9960_GOFFSET_R, u8OffsetRight);
}

/**
 *  @brief  Enable gesture readings on APDS9960
 *  @param  bEnable Enable (true/false)
 */
void APDS9960_EnableGesture(i2c_apds9960_t* pHandle, bool bEnable)
{
    if (!bEnable)
    {
        pHandle->_gconf4.GMODE = 0;
        APDS9960_WriteByte(pHandle, APDS9960_GCONF4, _APDS9960_GetGCONF4(pHandle));
    }
    pHandle->_enable.GEN = bEnable;
    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
    APDS9960_ResetCounts(pHandle);
    // update to enable gesture sensing without proximity sensing
    if (bEnable)
    {  // start the gesture engine here without a prox. gesture
        APDS9960_WriteByte(pHandle, APDS9960_GCONF4, 0x01);
    }
}

/**
 *  @brief  Resets gesture counts
 */
void APDS9960_ResetCounts(i2c_apds9960_t* pHandle)
{
    pHandle->_u8GestCnt = 0;
    pHandle->_u8UCount  = 0;
    pHandle->_u8DCount  = 0;
    pHandle->_u8LCount  = 0;
    pHandle->_u8RCount  = 0;
}

/**
 *  @brief  Reads gesture
 *  @return Received gesture (APDS9960_DOWN APDS9960_UP, APDS9960_LEFT APDS9960_RIGHT)
 */
apds9960_gesture_e APDS9960_ReadGesture(i2c_apds9960_t* pHandle)
{
    uint8_t u8BytesToRead;
    uint8_t au8Data[256];
    tick_t  t = GetTickUs();

    uint8_t eGestureReceived = APDS9960_GESTURE_NONE;

    while (1)
    {
        int32_t up_down_diff    = 0;
        int32_t left_right_diff = 0;

        if (!APDS9960_GestureValid(pHandle))
        {
            break;
        }

        DelayBlockMs(FIFO_PAUSE_TIME);
        APDS9960_ReadByte(pHandle, APDS9960_GFLVL, &u8BytesToRead);

        if (u8BytesToRead > 0)
        {
            // produces sideffects needed for readGesture to work
            APDS9960_ReadBlock(pHandle, APDS9960_GFIFO_U, au8Data, u8BytesToRead);

            if (abs((int32_t)au8Data[0] - (int32_t)au8Data[1]) > 13)
            {
                up_down_diff += (int32_t)au8Data[0] - (int32_t)au8Data[1];
            }

            if (abs((int32_t)au8Data[2] - (int32_t)au8Data[3]) > 13)
            {
                left_right_diff += (int32_t)au8Data[2] - (int32_t)au8Data[3];
            }

            if (up_down_diff != 0)
            {
                if (up_down_diff < 0)
                {
                    if (pHandle->_u8DCount > 0)
                    {
                        eGestureReceived = APDS9960_GESTURE_UP;
                    }
                    else
                    {
                        pHandle->_u8UCount++;
                    }
                }
                else if (up_down_diff > 0)
                {
                    if (pHandle->_u8UCount > 0)
                    {
                        eGestureReceived = APDS9960_GESTURE_DOWN;
                    }
                    else
                    {
                        pHandle->_u8DCount++;
                    }
                }
            }

            if (left_right_diff != 0)
            {
                if (left_right_diff < 0)
                {
                    if (pHandle->_u8RCount > 0)
                    {
                        eGestureReceived = APDS9960_GESTURE_LEFT;
                    }
                    else
                    {
                        pHandle->_u8LCount++;
                    }
                }
                else if (left_right_diff > 0)
                {
                    if (pHandle->_u8LCount > 0)
                    {
                        eGestureReceived = APDS9960_GESTURE_RIGHT;
                    }
                    else
                    {
                        pHandle->_u8RCount++;
                    }
                }
            }

            if (eGestureReceived != APDS9960_GESTURE_NONE)
            {
                APDS9960_ResetCounts(pHandle);
                break;
            }

            if (up_down_diff != 0 || left_right_diff != 0)
            {
                t = GetTickUs();
            }
        }

        if (DelayNonBlockMs(t, 300))
        {
            APDS9960_ResetCounts(pHandle);
            break;
        }
    }

    return eGestureReceived;
}
/**
 *  @brief  Set LED brightness for proximity/gesture
 *  @param  eLedDrive LED Drive
 *  @param  eLedBoost LED Boost
 */
void APDS9960_SetLED(i2c_apds9960_t* pHandle, apds9960_led_drive_e eLedDrive, apds9960_led_boost_e eLedBoost)
{
    // set BOOST
    pHandle->_config2.LED_BOOST = eLedBoost;
    APDS9960_WriteByte(pHandle, APDS9960_CONFIG2, _APDS9960_GetCONFIG2(pHandle));

    pHandle->_control.LDRIVE = eLedDrive;
    APDS9960_WriteByte(pHandle, APDS9960_CONTROL, _APDS9960_GetCONTROL(pHandle));
}

/**
 *  @brief  Enable color readings on APDS9960
 *  @param  bEnable Enable (true/false)
 */
void APDS9960_EnableColor(i2c_apds9960_t* pHandle, bool bEnable)
{
    pHandle->_enable.AEN = bEnable;
    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
}

/**
 *  @brief  Returns status of color data
 *  @return true if color data ready, false otherwise
 */
bool APDS9960_ColorDataReady(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;
    APDS9960_ReadByte(pHandle, APDS9960_STATUS, &u8Data);
    _APDS9960_SetSTATUS(pHandle, u8Data);
    return pHandle->_status.AVALID;
}

/**
 *  @brief  Reads the raw red, green, blue and clear channel values
 *  @param  *r Red value
 *  @param  *g Green value
 *  @param  *b Blue value
 *  @param  *c Clear channel value
 */
void APDS9960_GetColorData(i2c_apds9960_t* pHandle, uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c)
{
    APDS9960_ReadWord(pHandle, APDS9960_CDATAL, c);
    APDS9960_ReadWord(pHandle, APDS9960_RDATAL, r);
    APDS9960_ReadWord(pHandle, APDS9960_GDATAL, g);
    APDS9960_ReadWord(pHandle, APDS9960_BDATAL, b);
}

/**
 *  @brief  Converts the raw R/G/B values to color temperature in degrees Kelvin
 *  @param  r Red value
 *  @param  g Green value
 *  @param  b Blue value
 *  @return Color temperature
 */
uint16_t APDS9960_CalculateColorTemperature(i2c_apds9960_t* pHandle, uint16_t r, uint16_t g, uint16_t b)
{
    float32_t X, Y, Z; /* RGB to XYZ correlation      */
    float32_t xc, yc;  /* Chromaticity co-ordinates   */
    float32_t n;       /* McCamy's formula            */
    float32_t cct;

    /* 1. Map RGB values to their XYZ counterparts.    */
    /* Based on 6500K fluorescent, 3000K fluorescent   */
    /* and 60W incandescent values for a wide range.   */
    /* Note: Y = Illuminance or lux                    */
    X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
    Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    Z = (-0.68202F * r) + (0.77073F * g) + (0.56332F * b);

    /* 2. Calculate the chromaticity co-ordinates      */
    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);

    /* 3. Use McCamy's formula to determine the CCT    */
    n = (xc - 0.3320F) / (0.1858F - yc);

    /* Calculate the final CCT */
    cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

    /* Return the results in degrees Kelvin */
    return (uint16_t)cct;
}

/**
 *  @brief  Calculate ambient light values
 *  @param  r Red value
 *  @param  g Green value
 *  @param  b Blue value
 *  @return LUX value
 */
uint16_t APDS9960_CalculateLux(i2c_apds9960_t* pHandle, uint16_t r, uint16_t g, uint16_t b)
{
    float32_t illuminance;

    /* This only uses RGB ... how can we integrate clear or calculate lux */
    /* based exclusively on clear since this might be more reliable?      */
    illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

    return (uint16_t)illuminance;
}

/**
 *  @brief  Enables color interrupt
 */
void APDS9960_EnableColorInterrupt(i2c_apds9960_t* pHandle)
{
    pHandle->_enable.AIEN = 1;
    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
}

/**
 *  @brief  Disables color interrupt
 */
void APDS9960_DisableColorInterrupt(i2c_apds9960_t* pHandle)
{
    pHandle->_enable.AIEN = 0;
    APDS9960_WriteByte(pHandle, APDS9960_ENABLE, _APDS9960_GetENABLE(pHandle));
}

/**
 *  @brief  Clears interrupt
 */
void APDS9960_ClearInterrupt(i2c_apds9960_t* pHandle)
{
    uint8_t u8Data;
    APDS9960_ReadByte(pHandle, APDS9960_AICLEAR, &u8Data);
}

/**
 *  @brief  Sets interrupt limits
 *  @param  u16Low Low limit
 *  @param  u16High High limit
 */
void APDS9960_SetIntLimits(i2c_apds9960_t* pHandle, uint16_t u16Low, uint16_t u16High)
{
    APDS9960_WriteWord(pHandle, APDS9960_AILTIL, u16Low);
    APDS9960_WriteWord(pHandle, APDS9960_AIHTL, u16High);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

#define DEMO 2

void APDS9960_Test(i2c_mst_t* hI2C)
{
    i2c_apds9960_t apds9960 = {
        .hI2C      = hI2C,
        .u8SlvAddr = APDS9960_ADDRESS,
    };

#if DEMO == 0  // gesture_sensor

    APDS9960_Init(&apds9960, 10, APDS9960_AGAIN_4X);
    APDS9960_EnableProximity(&apds9960, true);
    APDS9960_EnableGesture(&apds9960, true);

    LOGI("Detecting gestures ...");

    while (1)
    {
        apds9960_gesture_e eGesture = APDS9960_ReadGesture(&apds9960);

        switch (eGesture)
        {
            case APDS9960_GESTURE_DOWN: LOGI("v"); break;
            case APDS9960_GESTURE_UP: LOGI("^"); break;
            case APDS9960_GESTURE_LEFT: LOGI("<"); break;
            case APDS9960_GESTURE_RIGHT: LOGI(">"); break;
            default: break;
        }
    }

#elif DEMO == 1  // proximity_sensor

    const pin_t INT = {
#if defined(BOARD_STM32F407VET6_XWS)
        GPIOA, GPIO_PIN_4
#endif
    };

    PIN_SetMode(&INT, PIN_MODE_INPUT_FLOATING, PIN_PULL_UP);

    APDS9960_Init(&apds9960, 10, APDS9960_AGAIN_4X);

    // enable proximity mode
    APDS9960_EnableProximity(&apds9960, true);

    // set the interrupt threshold to fire when proximity reading goes above 30
    APDS9960_SetProximityInterruptThreshold(&apds9960, 0, 30, 4);

    // enable the proximity interrupt
    APDS9960_EnableProximityInterrupt(&apds9960);

    while (1)
    {
        // print the proximity reading when the interrupt pin goes low
        if (PIN_ReadLevel(&INT) == PIN_LEVEL_LOW)
        {
            LOGI("%d", APDS9960_ReadProximity(&apds9960));

            // clear the interrupt
            APDS9960_ClearInterrupt(&apds9960);
        }
    }

#elif DEMO == 2  // color_sensor

    APDS9960_Init(&apds9960, 10, APDS9960_AGAIN_64X);

    // enable color sensign mode
    APDS9960_EnableColor(&apds9960, true);

    while (1)
    {
        // create some variables to store the color data in
        uint16_t r, g, b, c;

        // wait for color data to be ready
        while (!APDS9960_ColorDataReady(&apds9960))
        {
            DelayBlockMs(5);
        }

        // get the data and print the different channels
        APDS9960_GetColorData(&apds9960, &r, &g, &b, &c);
        r /= 16, g /= 16, b /= 16, c /= 16;  // 4096->256
        LOGI("rgbc: %d,%d,%d,%d", r, g, b, c);

        DelayBlockMs(500);
    }

#endif
}

#endif
