#ifndef __PARA_TABLE_H__
#define __PARA_TABLE_H__

#include "attr.h"

#include "pv_motdrv.h"
#include "pv_trqctl.h"
#include "pv_spdctl.h"
#include "pv_posctl.h"
#include "pv_wave.h"

//-----------------------------------------------------------------------------
//

#define ADDR_FMT_DEC 1
#define ADDR_FMT_HEX 2

#ifndef ADDR_FMT
#define ADDR_FMT ADDR_FMT_DEC
#endif

#if ADDR_FMT == ADDR_FMT_DEC
#define GROUP_SIZE 100
#elif ADDR_FMT == ADDR_FMT_HEX
#define GROUP_SIZE 255
#endif

//-----------------------------------------------------------------------------
//

// windows
// #pragma pack(2)
// #pragma pack()

#define EXPORT_PARA_GROUP

//-----------------------------------------------------------------------------
//

// 旋转正方向
typedef enum {
    ROT_FWD_DIR_CCW = 0,  ///< 逆时针为正转（编码器计数增加）
    ROT_FWD_DIR_CW,       ///< 顺时针为正转
} RotFwdDir_e;

// 旋转方向
typedef enum {
    ROT_DIR_FWD = 0,  ///< 正转
    ROT_DIR_REV,      ///< 反转
} RotDir_e;

typedef enum {
    ENC_ABS,
    ENC_INC,
    ENC_HALL,
} EncType_e;

typedef enum {
    // databits - parity - stopbit
    MODBUS_DATFMR_8N1,
    MODBUS_DATFMR_8E1,
    MODBUS_DATFMR_8O1,
} ModDataFmt_e;

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16___System___;       ///< P00.000 NO 保留
        u32 u32DrvScheme;          ///< P00.001 RO 驱动器方案
        u16 u16McuSwVerMajor;      ///< P00.003 RO 软件基线版本号
        u16 u16McuSwVerMinor;      ///< P00.004 RO 软件分支版本号
        u16 u16McuSwVerPatch;      ///< P00.005 RO 软件补丁版本号
        u32 u32McuSwBuildDate;     ///< P00.006 RO 软件构建日期
        u16 u16RunTimeLim;         ///< P00.008 RO 运行时间限制
        u16 u16ExtDiNbr;           ///< P00.009 RO 外部DI端子个数
        u16 u16ExtDoNbr;           ///< P00.010 RO 外部DO端子个数
        u16 u16ExtAiNbr;           ///< P00.011 RO 外部AI端子个数
        u16 u16TempNbr;            ///< P00.012 RO 温度采样个数
        u16 u16BlinkPeriod;        ///< P00.013 RW 指示灯闪烁周期
        u16 u16___Motor___;        ///< P00.014 NO 保留
        u16 u16MotType;            ///< P00.015 RW 电机类型
        u16 u16MotVoltInRate;      ///< P00.016 RW 电机额定输入电压 0.1V
        u16 u16MotVdMax;           ///< P00.017 RW 电机最大Vd
        u16 u16MotVqMax;           ///< P00.018 RW 电机最大Vq
        u16 u16MotPowerRate;       ///< P00.019 RW 电机额定功率 0.01kW
        u16 u16MotCurRate;         ///< P00.020 RW 电机额定电流 0.01A
        u16 u16MotCurMax;          ///< P00.021 RW 电机最大电流 0.01A
        u16 u16MotTrqRate;         ///< P00.022 RW 电机额定转矩 0.01N.m
        u16 u16MotTrqMax;          ///< P00.023 RW 电机最大转矩 0.01N.m
        u16 u16MotSpdRate;         ///< P00.024 RW 电机额定转速 rpm
        u16 u16MotSpdMax;          ///< P00.025 RW 电机最大转速 rpm
        u16 u16MotAccMax;          ///< P00.026 RW 电机最大加速度
        u16 u16MotPolePairs;       ///< P00.027 RW 电机极对数 pair
        u32 u32MotInertia;         ///< P00.028 RW 电机转子转动惯量 0.001kg.cm^2
        u16 u16MotRs;              ///< P00.030 RW 电机定子电阻 mOh
        u16 u16MotLds;             ///< P00.031 RW 电机定子D轴电感 0.01mH
        u16 u16MotLqs;             ///< P00.032 RW 电机定子Q轴电感 0.01mH
        u16 u16MotEmfCoeff;        ///< P00.033 RW 电机反电动势常数 0.1Vrms/Krpm
        u16 u16MotTrqCoeff;        ///< P00.034 RW 电机转矩系数
        u16 u16MotTm;              ///< P00.035 RW 电机机械时间常数 0.01ms
        u16 u16___Encoder___;      ///< P00.036 NO 保留
        u16 u16EncType;            ///< P00.037 RW 编码器类型
        u32 u32EncRes;             ///< P00.038 RW 编码器分辨率 pulse
        s32 s32EncOffset;          ///< P00.040 RW 编码器安装偏置: 编码器"0"位置与FOC坐标系0电角度之间的偏置角,标定到编码器分辨率
        u16 u16HallOffset;         ///< P00.042 RW 霍尔安装偏置: 霍尔"0"位置与FOC坐标系0电角度之间的偏置角,标定到2^16
        u16 u16___AdcSampler___;   ///< P00.043 NO 保留
        u16 u16IphZoom;            ///< P00.044 RW 相电流采样缩放系数
        s16 s16IaBias;             ///< P00.045 RW A相电流采样偏置
        s16 s16IbBias;             ///< P00.046 RW B相电流采样偏置
        s16 s16IcBias;             ///< P00.047 RW C相电流采样偏置
        u16 u16UmdcZoom;           ///< P00.048 RW 供电母线采样缩放系数
        s16 s16UmdcBias;           ///< P00.049 RW 供电母线采样偏置 0.1V
        u16 u16UcdcZoom;           ///< P00.050 RW 控制母线采样缩放系数
        s16 s16UcdcBias;           ///< P00.051 RW 控制母线采样偏置 0.1V
        u16 u16TempZoom;           ///< P00.052 RW 温度采样缩放系数
        s16 s16TempBias;           ///< P00.053 RW 温度采样偏置
        u16 u16AiZoom[3];          ///< P00.054 RW 模拟量采样缩放系数
        s16 s16AiBias[3];          ///< P00.057 RW 模拟量采样偏置 0.1V
        u16 u16AiDeadZone[3];      ///< P00.060 RW 模拟量输入死区
        u16 u16AiFltrTime[3];      ///< P00.063 RW 模拟量输入滤波时间常数
        u16 u16___SlvCom___;       ///< P00.066 NO 保留
        u16 u16CommSlaveAddr;      ///< P00.067 RW 通讯从站地址
        u16 u16ModBaudrate;        ///< P00.068 RW Modbus 波特率
        u16 u16ModDataFmt;         ///< P00.069 RW Modbus 数据格式
        u16 u16ModMasterEndian;    ///< P00.070 RW Modbus 主站大小端
        u16 u16ModDisconnectTime;  ///< P00.071 RW Modbus 通讯断开检测时间
        u16 u16ModAckDelay;        ///< P00.072 RW Modbus 命令响应延时
        u16 u16CopBitrate;         ///< P00.073 RW CANopen 比特率
        u16 u16CopPdoInhTime;      ///< P00.074 RW CANopen PDO禁止时间
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P00.074
} SysPara_u;

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16FocMode;          ///< P01.000 RW FOC模式配置
        u16 u16___MotDrvRef___;  ///< P01.001 NO 保留
        u16 u16ElecAngRef;       ///< P01.002 RW 电角度指令
        s16 s16VqRef;            ///< P01.003 RW Q轴电压指令
        s16 s16VdRef;            ///< P01.004 RW D轴电压指令
        s16 s16ValphaRef;        ///< P01.005 RW Alpha轴电压指令
        s16 s16VbetaRef;         ///< P01.006 RW Beta轴电压指令
        u16 u16SvpwmTaRef;       ///< P01.007 RW A相占空比指令
        u16 u16SvpwmTbRef;       ///< P01.008 RW B相占空比指令
        u16 u16SvpwmTcRef;       ///< P01.009 RW C相占空比指令
        u16 u16___MotDrvOut___;  ///< P01.010 NO 保留
        u16 u16SvpwmSector;      ///< P01.011 RW 电压矢量扇区
        u16 u16ElecAngOut;       ///< P01.012 RW 电角度指令
        s16 s16VqOut;            ///< P01.013 RW Q轴电压指令
        s16 s16VdOut;            ///< P01.014 RW D轴电压指令
        s16 s16ValphaOut;        ///< P01.015 RW Alpha轴电压指令
        s16 s16VbetaOut;         ///< P01.016 RW Beta轴电压指令
        u16 u16SvpwmTaOut;       ///< P01.017 RW A相占空比指令
        u16 u16SvpwmTbOut;       ///< P01.018 RW B相占空比指令
        u16 u16SvpwmTcOut;       ///< P01.019 RW C相占空比指令
        u16 u16___MotDrvFb___;   ///< P01.020 NO 保留
        s16 s16IaFbSi;           ///< P01.021 RO A相反馈电流
        s16 s16IbFbSi;           ///< P01.022 RO B相反馈电流
        s16 s16IcFbSi;           ///< P01.023 RO C相反馈电流
        s16 s16IalphaFb;         ///< P01.024 RW Alpha轴反馈电流
        s16 s16IbetaFb;          ///< P01.025 RW Beta轴反馈电流
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P01.025
} MotDrv_u;

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16___System___;    ///< P02.000 NO 保留
        u16 u32SysRunTime;      ///< P02.001 RO 系统运行时间, minute
        u16 u32AppRunTime;      ///< P02.002 RO 应用运行时间, minute
        u16 u16___DiDo___;      ///< P02.003 NO 保留
        u16 u16VirDiState;      ///< P02.004 RO 虚拟DI输入电平状态
        u16 u16VirDoState;      ///< P02.005 RO 虚拟DO输出电平状态
        u16 u16ExtDiState;      ///< P02.006 RO 外部DI输入电平状态
        u16 u16ExtDoState;      ///< P02.007 RO 外部DO输出电平状态
        u16 u16___AiAo___;      ///< P02.008 NO 保留
        u16 u16UmdcSi;          ///< P02.009 RO 供电母线电压, 0.01V
        u16 u16UcdcSi;          ///< P02.010 RO 控制母线电压, 0.01V
        s16 s16UaiSi[3];        ///< P02.011 RO AI输入电压, 0.01V
        u16 u16UaoSi[3];        ///< P02.014 RO AO输出电压, 0.01V
        s16 s16EnvTemp;         ///< P02.017 RO 环境温度, 0.01°C
        s16 s16ChipTemp;        ///< P02.018 RO 芯片温度, 0.01°C
        u32 u32AdcSampTime;     ///< P02.019 RO ADC采样耗时, 0.01 us
        u16 u16AdcSampRes[10];  ///< P02.021 RO ADC采样原始结果
        u16 u16___Encoder___;   ///< P02.031 NO 保留
        u16 u16EncState;        ///< P02.032 RO 编码器状态
        s16 s16EncTemp;         ///< P02.033 RO 编码器温度, 0.01°C
        u16 u16EncMode;         ///< P02.034 RW 编码器工作模式
        s64 s64EncPosInit;      ///< P02.035 RO 编码器上电初始位置, pulse
        u32 u32EncPos;          ///< P02.039 RO 编码器单圈位置, pulse
        s32 s32EncTurns;        ///< P02.041 RO 编码器圈数, turn
        u16 u16___MotSta___;    ///< P02.043 NO 保留
        s32 s32UserSpdRef;      ///< P02.044 RO 用户速度指令, rpm
        s32 s32UserSpdFb;       ///< P02.046 RO 用户速度反馈, rpm
        s64 s64UserPosRef;      ///< P02.048 RO 用户位置指令, pulse
        s64 s64UserPosFb;       ///< P02.052 RO 用户位置反馈, pulse
        s16 s16UserTrqRef;      ///< P02.056 RO 用户转矩指令
        s16 s16UserTrqFb;       ///< P02.057 RO 用户转矩反馈
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P02.057
} MotSta_u;

//-----------------------------------------------------------------------------
//

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16TrqRefSrc;       ///< P03.000 RW 转矩指令来源
        s16 s16TrqDigRef;       ///< P03.001 RW 数字转矩指令
        s16 s16TrqDigRefs[16];  ///< P03.002 RW 数字转矩指令
        u16 u16TrqAiCoeff;      ///< P03.018 RW 单位模拟量对应转矩指令
        u16 u16TrqFwdLim;       ///< P03.019 RW 正向转矩限制
        u16 u16TrqRevLim;       ///< P03.020 RW 反向转矩限制
        u16 u16TrqLimRes;       ///< P03.021 RW 转矩限制状态
        u16 u16TrqModePosLim;   ///< P03.022 RW 转矩模式位置限制状态
        u16 u16TrqModeSpdLim;   ///< P03.023 RW 转矩模式速度限制状态
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P03.023
} TrqCtl_u;

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16SpdTgtSrc;       ///< P04.000 RW 速度指令来源
        s16 s16SpdTgtRef;       ///< P04.001 RW 目标速度指令
        u16 u16SpdTgtUnit;      ///< P04.002 RW 速度指令单位
        u16 u16SpdPlanMode;     ///< P04.003 RW 速度规划模式
        u16 u16DecTime;         ///< P04.004 RW 减速时间
        u16 u16AccTime;         ///< P04.005 RW 加速时间
        s16 s16SpdDigRef;       ///< P04.006 RW 数字速度指令
        u16 u16SpdMulRefSel;    ///< P04.007 RW 多段指令选择
        s16 s16SpdDigRefs[16];  ///< P04.008 RW 数字速度指令
        u16 u16SpdAiCoeff;      ///< P04.024 RW 单位模拟量对应转速指令
        u16 u16SpdLimSrc;       ///< P04.025 RW 速度限制来源
        u16 u16SpdLimRes;       ///< P04.026 RW 速度限制状态
        u16 s64SpdFwdLim;       ///< P04.027 RW 正向速度限制
        u16 s64SpdRevLim;       ///< P04.028 RW 反向速度限制
        u16 u16SpdCoeff;        ///< P04.029 RW 速度缩放系数
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P04.029
} SpdCtl_u;

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16PosTgtSrc;       ///< P05.000 RW 位置指令来源
        s64 s16PosTgtRef;       ///< P05.001 RW 目标位置指令
        s64 s64PosDigRef;       ///< P05.005 RW 数字位置指令
        u16 u16SpdMulRefSel;    ///< P05.009 RW 多段指令选择
        s64 s64PosDigRefs[16];  ///< P05.010 RW 数字位置指令
        u16 u16PosLimSrc;       ///< P05.074 RW 位置限制来源
        u16 u16PosLimRes;       ///< P05.075 RO 位置限制状态
        u16 s64PosFwdLim;       ///< P05.076 RW 正向位置限制
        u16 s64PosRevLim;       ///< P05.077 RW 反向位置限制
        s64 s64MechOffset;      ///< P05.078 RW 机械位置偏置
        u32 u32ElecGearNum;     ///< P05.082 RW 电子齿轮比分子 (numerator)
        u32 u32ElecGearDeno;    ///< P05.084 RW 电子齿轮比分母 (denominator)
        u16 u16EncFreqDivDir;   ///< P05.086 RW 编码器分频输出脉冲方向
        u16 u16EncFreqDivNum;   ///< P05.087 RW 编码器分频输出分子
        u16 u16EncFreqDivDeno;  ///< P05.088 RW 编码器分频输出分母
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P05.088
} PosCtl_u;

//-----------------------------------------------------------------------------
//

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16DbgVal[8];  ///< P06.000 RW
        u32 u32DbgVal[8];  ///< P06.008 RW
        u64 u64DbgVal[4];  ///< P06.024 RW
        f32 f32DbgVal[8];  ///< P06.040 RW
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P06.055
} DbgVal_u;

//-----------------------------------------------------------------------------
//

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16WaveConfig;      ///< P07.000 RW 触发配置
        u16 u16WaveType;        ///< P07.001 RW 波形类型
        u32 u32WaveTargetFreq;  ///< P07.002 RW 波形期望频率
        u32 u32WaveActualFreq;  ///< P07.004 RO 波形实际频率
        u16 u16WaveFreqUnit;    ///< P07.006 RW 波形频率单位
        u16 u16WaveAmplitude;   ///< P07.007 RW 波形振幅
        u16 u16WaveAlign;       ///< P07.008 RW 数据点对齐方式
        u16 u16WaveSize;        ///< P07.009 RW 数据点数量
        u16 u16WaveData[80];    ///< P07.010 RW 数据点数值
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P07.089
} WaveGen_u;

//-----------------------------------------------------------------------------
//

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u32 u32ErrMask;         ///< P08.000 RW 故障屏蔽字
        u32 u32WrnMask;         ///< P08.002 RW 警告屏蔽字
        u32 u32FaultMask;       ///< P08.004 RW 内部异常屏蔽字
        u16 u16AlmLog[16];      ///< P08.006 RO 报警历史
        u32 u32AlmLogTime[16];  ///< P08.022 RO 报警历史时间戳
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P08.053
} Alarm_u;

//-----------------------------------------------------------------------------
// 数据记录

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16LogMode;           ///< P09.000 RW 数据记录工作模式
        u16 u16LogTrigState;      ///< P09.001 RW 数据记录执行状态
        u16 u16LogTrigObj;        ///< P09.002 RW 数据记录触发对象
        s64 s64LogTrigThreshold;  ///< P09.003 RW 数据记录触发水平
        u16 u16LogTrigEdge;       ///< P09.007 RW 数据记录触发条件
        u16 u16LogTrigSet;        ///< P09.008 RW 数据记录触发设定
        u16 u16LogSampPrd;        ///< P09.009 RW 数据记录采样周期设定
        u16 u16LogSampPts;        ///< P09.010 RW 数据记录采样点数设定
        u16 u16LogChNbr;          ///< P09.011 RW 数据记录通道数量设定
        u16 u16LogChSrc[8];       ///< P09.012 RW 数据记录通道来源设定
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P09.019
} CurveTrace_u;

//-----------------------------------------------------------------------------
// 多段运行模式

EXPORT_PARA_GROUP typedef union {
    __packed struct {
        u16 u16MultStepMode;       ///< P10.000 RW 多段运行模式选择
        u16 u16StartStep;          ///< P10.001 RW 起始段选择
        u16 u16EndStep;            ///< P10.002 RW 终点段选择
        u16 u16Resv0;              ///< P10.003 NO 保留
        s32 s32StepRefs[16];       ///< P10.004 RW 位置/速度指令
        u16 u16StepAccTimes[16];   ///< P10.036 RW 加速时间
        u16 u16StepDecTimes[16];   ///< P10.052 RW 减速时间
        u16 u16StepSpdLims[16];    ///< P10.068 RW 运行速度上限
        u16 u16StepHoldTimes[16];  ///< P10.084 RW 运行/等待时间
    };
    u16 GROUP[GROUP_SIZE * 1];  ///< P10.099
} MulStp_u;

//-----------------------------------------------------------------------------
//

typedef struct {
    SysPara_u SysPara;   // 0
    MotDrv_u  MotDrv;    // 1
    MotSta_u  MotSta;    // 2
    TrqCtl_u  TrqCtl;    // 3
    SpdCtl_u  SpdCtl;    // 4
    PosCtl_u  PosCtl;    // 5
    DbgVal_u  DbgVal;    // 6
    WaveGen_u WaveGen1;  // 7
    WaveGen_u WaveGen2;  // 8
} ParaTable_t;

//-----------------------------------------------------------------------------
//

extern ParaTable_t gParatbl;

#define P(group)      gParatbl.group
#define P_ADDR(group) &P(group)

extern void SystemParaConfig(void);

#endif
