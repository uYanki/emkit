#include "main.h"
#include "system/sleep.h"
#include "system/autoinit.h"
#include "bsp/uart.h"

#include "pinctrl.h"
#include "mbslave.h"

#include "pinmap.h"
#include "paratbl/tbl.h"

// #include "pulse/pwm.h"
#include "pulse/wave.h"

#include "enc/incEnc.h"
#include "enc/pulse.h"

#include "bsp/adc.h"

#include "sensor/ds18b20/ds18b20.h"

#include "math.h"

#define CONFIG_MODULE_WAVEGEN     1  // 波形发生器模块
#define CONFIG_MODULE_TEMPERATURE 1  // 环境温度采集模块
#define CONFIG_MODULE_ENCODER     1  // 编码器测速模块
#define CONFIG_MODULE_FREQDIVOUT  1  // 分频输出模块

//-----------------------------------------------------------------------------
//

static void BspTempSensorCycle(void);
static void BspEncCycle(void);

//-----------------------------------------------------------------------------
//

void usdk_preinit(void)
{
    // sleep
    sleep_init();
    // delay
    DelayInit();
    // hw_uart
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    UART_Config(&USART_InitStructure);
    UART_DMA_Config();
    // paratbl
    SystemParaConfig();
}

//

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    eMBInit(MB_RTU, 0x01, 1, 115200, MB_PAR_EVEN);

    // const UCHAR ucSlaveID[] = {0xAA, 0xBB, 0xCC};
    // eMBSetSlaveID(0x33, FALSE, ucSlaveID, 3);

    eMBEnable();

    WaveGenInit();

    P(SysPara).u16BlinkPeriod = 1000;

    while (1)
    {
        eMBPoll();

        //------------------------------------------------

        WriteBit(ucDiscInBuf[0], 0, !PEin(10));  // key1
        WriteBit(ucDiscInBuf[0], 1, !PEin(11));  // key2
        WriteBit(ucDiscInBuf[0], 2, !PEin(12));  // key3

        PEout(13) = !GETBIT(ucCoilBuf[0], 0);  // led1
        PEout(14) = !GETBIT(ucCoilBuf[0], 1);  // led2

#if 0
        PEout(15) = !GETBIT(ucCoilBuf[0], 2);  // led3
#else
        static tick_t t_blink = 0;
        if (DelayNonBlockMS(t_blink, P(SysPara).u16BlinkPeriod))
        {
            PEout(15) ^= 1;  // toggle
            t_blink = HAL_GetTick();
        }
#endif

        //------------------------------------------------

        WaveGenCycle();

#if CONFIG_MODULE_TEMPERATURE
        BspTempSensorCycle();
#endif  // CONFIG_MODULE_TEMPERATURE

#if CONFIG_MODULE_ENCODER
        BspEncCycle();
#endif  // CONFIG_MODULE_ENCODER
    }
}

//-----------------------------------------------------------------------------
// temperature sensor

#if CONFIG_MODULE_TEMPERATURE

static int BspTempSensorInit(void)
{
    ds18b20_init();
    return INIT_RESULT_SUCCESS;
}

USDK_INIT_EXPORT(BspTempSensorInit, INIT_LEVEL_BOARD)

static void BspTempSensorCycle(void)
{
    static tick_t t = 0;

    if (DelayNonBlockS(t, 1))
    {
        s16 s16EnvTemp;

        __disable_irq();

        if (ds18b20_read_temp(&s16EnvTemp))
        {
            P(MotSta).s16EnvTemp = ds18b20_convert_temp(s16EnvTemp) * 100;
        }
        else
        {
            P(MotSta).s16EnvTemp = 0;
        }

        __enable_irq();

        t = HAL_GetTick();
    }
}

#endif

//-----------------------------------------------------------------------------
// encoder: position recording, speed calculation, freq-div output

#define CONFIG_ENC_PPS 60  // 编码器单圈分辨率
#define CONFIF_FRQ_DIV 4   // 分频输出的分频因子

static IncEncArgs_t* pEncArgs;

static int BspEncInit(void)
{
    static IncEncArgs_t EncArgs;

    EncArgs.u32EncRes    = (vu32*)P_ADDR(SysPara.u32EncRes);
    EncArgs.u32EncPos    = (vu32*)P_ADDR(MotSta.u32EncPos);
    EncArgs.s32EncTurns  = (vs32*)P_ADDR(MotSta.s32EncTurns);
    EncArgs.s32UserSpdFb = (vs32*)P_ADDR(MotSta.s32UserSpdFb);
    EncArgs.s64UserPosFb = (vs64*)P_ADDR(MotSta.s64UserPosFb);
    EncArgs.u16SpdCoeff  = (vu16*)P_ADDR(SpdCtl.u16SpdCoeff);

    pEncArgs = &EncArgs;

    P(SysPara.u32EncRes)  = CONFIG_ENC_PPS;
    P(SpdCtl.u16SpdCoeff) = 100;

    IncEncInit(pEncArgs);
    PulseGenInit();

    return INIT_RESULT_SUCCESS;
}

USDK_INIT_EXPORT(BspEncInit, INIT_LEVEL_BOARD)

static void BspEncCycle(void)
{
    static tick_t t = 0;

    if (DelayNonBlockMS(t, 1))
    {
        static u32 pulse = 0;

        t = HAL_GetTick();

        pulse += abs(IncEncCycle(pEncArgs));

        // 每毫秒输出 (s32DeltaPos / CONFIF_FRQ_DIV) 个脉冲

        if (pulse >= CONFIF_FRQ_DIV)
        {
            u32 n = pulse / CONFIF_FRQ_DIV;

            // output n pulse in 1ms
            u32 frq = n * 1000;

            if (PulseGenConfig(frq, 0.333))
            {
                if (PulseGenStart(n))
                {
                    pulse %= CONFIF_FRQ_DIV;
                }
            }
        }
    }
}

//

static int BspAdcInit(void)
{
    AdcInit();
    return INIT_RESULT_SUCCESS;
}

USDK_INIT_EXPORT(BspAdcInit, INIT_LEVEL_BOARD)

#if 0

static PID_t SpdPID = {
    .Kp  = 0.1f,
    .Ki  = 0.2f,
    .Kd  = 0.0f,
    .out = 0,
    .lo  = 0,
    .hi  = 2000,
};

SpdPID.fbk = P(MotSta).s16UserSpdFb;
// SpdPID.ref = R(07, u16ServoOn) ? P(MotSta).s16UserSpdRef : 0;
SpdPID.ref = R(07, u16ServoOn) ? 2000 : 0;
SpdPID.Ts  = 1;  // pEncArgs->f32DeltaTick;

PID_Handler_Base(&SpdPID);

if (SpdPID.out > 2000)
{
    SpdPID.out = 2000;
}

R(07, s16VdRef) = SpdPID.out;
R(07, s16VqRef) = SpdPID.out;

#endif

#if 0

if (AppCheck(APP_OPENLOOP))
{
    P(MotSta.u16ElecAngle) = R(07, u16ElecAngRef);
}
else if (AppCheck(APP_CLOSELOOP))
{
    u32 u32MechAngle;
    U16 u16ElecAngle;

// 0 <= u32EncOffset < u32EncRes
// 0 <  u32EncPos    < u32EncRes
    
// (0,u32EncRes) => (0,U32_MAX) => (0,U16_MAX)
u32MechAngle = (u32)((s32)P(PosCtl.u32EncPos) - (s32)P(SysPara.u32EncOffset) + (s32)P(SysPara.u32EncRes)) % (u32)P(SysPara.u32EncRes);
u32MechAngle = (U32)((u64)U32_MAX * (u64)u32MechAngle / (u64)P(SysPara.u32EncRes)) >> 16;
u16ElecAngle = (U16)u32MechAngle * P(SysPara.u16MotPolePairs) + P(SysPara.u16HallOffset);

P(MotSta.u16ElecAngle) = u16ElecAngle;
}

u16 u16HallOffset;  ///< P00.016 霍尔安装偏置(电角度偏置)

#endif

void SystemParaConfig(void)
{
    u8 u8idx;

    // System
    {
        P(SysPara).u32DrvScheme = 0;

        P(SysPara).u16McuSwVerMajor  = 0;
        P(SysPara).u16McuSwVerMinor  = 1;
        P(SysPara).u16McuSwVerPatch  = 1;
        P(SysPara).u32McuSwBuildDate = 0;

        P(SysPara).u16RunTimeLim = 0;

        P(SysPara).u16ExtDiNbr = 0;
        P(SysPara).u16ExtDoNbr = 0;
        P(SysPara).u16ExtAiNbr = 2;
        P(SysPara).u16TempNbr  = 2;  // ChipTemp + EnvTemp

        P(SysPara).u16BlinkPeriod = 500;
    }

    // MotPara
    {
        P(SysPara).u16MotType       = 0;
        P(SysPara).u16MotVoltInRate = 0;
        P(SysPara).u16MotVdMax      = 0;
        P(SysPara).u16MotVqMax      = 0;
        P(SysPara).u16MotPowerRate  = 0;
        P(SysPara).u16MotCurRate    = 0;
        P(SysPara).u16MotCurMax     = 0;
        P(SysPara).u16MotTrqRate    = 0;
        P(SysPara).u16MotTrqMax     = 0;
        P(SysPara).u16MotSpdRate    = 0;
        P(SysPara).u16MotSpdMax     = 0;
        P(SysPara).u16MotAccMax     = 0;
        P(SysPara).u16MotPolePairs  = 0;
        P(SysPara).u32MotInertia    = 0;
        P(SysPara).u16MotRs         = 0;
        P(SysPara).u16MotLds        = 0;
        P(SysPara).u16MotLqs        = 0;
        P(SysPara).u16MotEmfCoeff   = 0;
        P(SysPara).u16MotTrqCoeff   = 0;
        P(SysPara).u16MotTm         = 0;
    }

    // Encoder
    {
        P(SysPara).u16EncType    = ENC_INC;
        P(SysPara).u32EncRes     = 60;
        P(SysPara).s32EncOffset  = 0;
        P(SysPara).u16HallOffset = 0;
    }

    // AdcSampler
    {
        P(SysPara).u16IphZoom = 1;
        P(SysPara).s16IaBias  = 0;
        P(SysPara).s16IbBias  = 0;
        P(SysPara).s16IcBias  = 0;

        P(SysPara).u16UmdcZoom = 1;
        P(SysPara).s16UmdcBias = 0;
        P(SysPara).u16UcdcZoom = 1;
        P(SysPara).s16UcdcBias = 0;

        P(SysPara).u16TempZoom = 1;
        P(SysPara).s16TempBias = 0;

        for (u8idx = 0; u8idx < P(SysPara).u16ExtAiNbr; ++u8idx)
        {
            P(SysPara).u16AiZoom[u8idx]     = 1;
            P(SysPara).s16AiBias[u8idx]     = 0;
            P(SysPara).u16AiDeadZone[u8idx] = 0;
            P(SysPara).u16AiFltrTime[u8idx] = 0;
        }
    }
    // SlvCom
    {
        P(SysPara).u16CommSlaveAddr = 1;

        P(SysPara).u16ModBaudrate       = 0; // 配对应的比特率表
        P(SysPara).u16ModDataFmt        = (u16)MODBUS_DATFMR_8N1;
        P(SysPara).u16ModMasterEndian   = 0;
        P(SysPara).u16ModDisconnectTime = 0;
        P(SysPara).u16ModAckDelay       = 0;

        P(SysPara).u16CopBitrate    = 0;  // 配对应的比特率表
        P(SysPara).u16CopPdoInhTime = 0;
    }
}
