#include "spi_mcp2515.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mcp2515"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

/**
 * @brief bitrate configurations @8M crystal oscillator
 */
#define CLKIN_8MHz_1000kBPS_CFG1 0x00
#define CLKIN_8MHz_1000kBPS_CFG2 0x80
#define CLKIN_8MHz_1000kBPS_CFG3 0x80

#define CLKIN_8MHz_500kBPS_CFG1  0x00
#define CLKIN_8MHz_500kBPS_CFG2  0x90
#define CLKIN_8MHz_500kBPS_CFG3  0x82

#define CLKIN_8MHz_250kBPS_CFG1  0x00
#define CLKIN_8MHz_250kBPS_CFG2  0xB1
#define CLKIN_8MHz_250kBPS_CFG3  0x85

#define CLKIN_8MHz_200kBPS_CFG1  0x00
#define CLKIN_8MHz_200kBPS_CFG2  0xB4
#define CLKIN_8MHz_200kBPS_CFG3  0x86

#define CLKIN_8MHz_125kBPS_CFG1  0x01
#define CLKIN_8MHz_125kBPS_CFG2  0xB1
#define CLKIN_8MHz_125kBPS_CFG3  0x85

#define CLKIN_8MHz_100kBPS_CFG1  0x01
#define CLKIN_8MHz_100kBPS_CFG2  0xB4
#define CLKIN_8MHz_100kBPS_CFG3  0x86

#define CLKIN_8MHz_80kBPS_CFG1   0x01
#define CLKIN_8MHz_80kBPS_CFG2   0xBF
#define CLKIN_8MHz_80kBPS_CFG3   0x87

#define CLKIN_8MHz_50kBPS_CFG1   0x03
#define CLKIN_8MHz_50kBPS_CFG2   0xB4
#define CLKIN_8MHz_50kBPS_CFG3   0x86

#define CLKIN_8MHz_40kBPS_CFG1   0x03
#define CLKIN_8MHz_40kBPS_CFG2   0xBF
#define CLKIN_8MHz_40kBPS_CFG3   0x87

#define CLKIN_8MHz_33k3BPS_CFG1  0x47
#define CLKIN_8MHz_33k3BPS_CFG2  0xE2
#define CLKIN_8MHz_33k3BPS_CFG3  0x85

#define CLKIN_8MHz_31k25BPS_CFG1 0x07
#define CLKIN_8MHz_31k25BPS_CFG2 0xA4
#define CLKIN_8MHz_31k25BPS_CFG3 0x84

#define CLKIN_8MHz_20kBPS_CFG1   0x07
#define CLKIN_8MHz_20kBPS_CFG2   0xBF
#define CLKIN_8MHz_20kBPS_CFG3   0x87

#define CLKIN_8MHz_10kBPS_CFG1   0x0F
#define CLKIN_8MHz_10kBPS_CFG2   0xBF
#define CLKIN_8MHz_10kBPS_CFG3   0x87

#define CLKIN_8MHz_5kBPS_CFG1    0x1F
#define CLKIN_8MHz_5kBPS_CFG2    0xBF
#define CLKIN_8MHz_5kBPS_CFG3    0x87

/**
 * @brief bitrate configurations @16M crystal oscillator
 */
#define CLKIN_16MHz_1000kBPS_CFG1 0x00
#define CLKIN_16MHz_1000kBPS_CFG2 0xD0
#define CLKIN_16MHz_1000kBPS_CFG3 0x82

#define CLKIN_16MHz_500kBPS_CFG1  0x00
#define CLKIN_16MHz_500kBPS_CFG2  0xF0
#define CLKIN_16MHz_500kBPS_CFG3  0x86

#define CLKIN_16MHz_250kBPS_CFG1  0x41
#define CLKIN_16MHz_250kBPS_CFG2  0xF1
#define CLKIN_16MHz_250kBPS_CFG3  0x85

#define CLKIN_16MHz_200kBPS_CFG1  0x01
#define CLKIN_16MHz_200kBPS_CFG2  0xFA
#define CLKIN_16MHz_200kBPS_CFG3  0x87

#define CLKIN_16MHz_125kBPS_CFG1  0x03
#define CLKIN_16MHz_125kBPS_CFG2  0xF0
#define CLKIN_16MHz_125kBPS_CFG3  0x86

#define CLKIN_16MHz_100kBPS_CFG1  0x03
#define CLKIN_16MHz_100kBPS_CFG2  0xFA
#define CLKIN_16MHz_100kBPS_CFG3  0x87

#define CLKIN_16MHz_95kBPS_CFG1   0x03
#define CLKIN_16MHz_95kBPS_CFG2   0xAD
#define CLKIN_16MHz_95kBPS_CFG3   0x07

#define CLKIN_16MHz_83k3BPS_CFG1  0x03
#define CLKIN_16MHz_83k3BPS_CFG2  0xBE
#define CLKIN_16MHz_83k3BPS_CFG3  0x07

#define CLKIN_16MHz_80kBPS_CFG1   0x03
#define CLKIN_16MHz_80kBPS_CFG2   0xFF
#define CLKIN_16MHz_80kBPS_CFG3   0x87

#define CLKIN_16MHz_50kBPS_CFG1   0x07
#define CLKIN_16MHz_50kBPS_CFG2   0xFA
#define CLKIN_16MHz_50kBPS_CFG3   0x87

#define CLKIN_16MHz_40kBPS_CFG1   0x07
#define CLKIN_16MHz_40kBPS_CFG2   0xFF
#define CLKIN_16MHz_40kBPS_CFG3   0x87

#define CLKIN_16MHz_33k3BPS_CFG1  0x4E
#define CLKIN_16MHz_33k3BPS_CFG2  0xF1
#define CLKIN_16MHz_33k3BPS_CFG3  0x85

#define CLKIN_16MHz_20kBPS_CFG1   0x0F
#define CLKIN_16MHz_20kBPS_CFG2   0xFF
#define CLKIN_16MHz_20kBPS_CFG3   0x87

#define CLKIN_16MHz_10kBPS_CFG1   0x1F
#define CLKIN_16MHz_10kBPS_CFG2   0xFF
#define CLKIN_16MHz_10kBPS_CFG3   0x87

#define CLKIN_16MHz_5kBPS_CFG1    0x3F
#define CLKIN_16MHz_5kBPS_CFG2    0xFF
#define CLKIN_16MHz_5kBPS_CFG3    0x87

/**
 * @brief bitrate configurations @20M crystal oscillator
 */
#define CLKIN_20MHz_1000kBPS_CFG1 0x00
#define CLKIN_20MHz_1000kBPS_CFG2 0xD9
#define CLKIN_20MHz_1000kBPS_CFG3 0x82

#define CLKIN_20MHz_500kBPS_CFG1  0x00
#define CLKIN_20MHz_500kBPS_CFG2  0xFA
#define CLKIN_20MHz_500kBPS_CFG3  0x87

#define CLKIN_20MHz_250kBPS_CFG1  0x41
#define CLKIN_20MHz_250kBPS_CFG2  0xFB
#define CLKIN_20MHz_250kBPS_CFG3  0x86

#define CLKIN_20MHz_200kBPS_CFG1  0x01
#define CLKIN_20MHz_200kBPS_CFG2  0xFF
#define CLKIN_20MHz_200kBPS_CFG3  0x87

#define CLKIN_20MHz_125kBPS_CFG1  0x03
#define CLKIN_20MHz_125kBPS_CFG2  0xFA
#define CLKIN_20MHz_125kBPS_CFG3  0x87

#define CLKIN_20MHz_100kBPS_CFG1  0x04
#define CLKIN_20MHz_100kBPS_CFG2  0xFA
#define CLKIN_20MHz_100kBPS_CFG3  0x87

#define CLKIN_20MHz_83k3BPS_CFG1  0x04
#define CLKIN_20MHz_83k3BPS_CFG2  0xFE
#define CLKIN_20MHz_83k3BPS_CFG3  0x87

#define CLKIN_20MHz_80kBPS_CFG1   0x04
#define CLKIN_20MHz_80kBPS_CFG2   0xFF
#define CLKIN_20MHz_80kBPS_CFG3   0x87

#define CLKIN_20MHz_50kBPS_CFG1   0x09
#define CLKIN_20MHz_50kBPS_CFG2   0xFA
#define CLKIN_20MHz_50kBPS_CFG3   0x87

#define CLKIN_20MHz_40kBPS_CFG1   0x09
#define CLKIN_20MHz_40kBPS_CFG2   0xFF
#define CLKIN_20MHz_40kBPS_CFG3   0x87

#define CLKIN_20MHz_33k3BPS_CFG1  0x0B
#define CLKIN_20MHz_33k3BPS_CFG2  0xFF
#define CLKIN_20MHz_33k3BPS_CFG3  0x87

/**
 * @brief registers
 */
#define REG_RXF0SIDH           0x00
#define REG_RXF0SIDL           0x01
#define REG_RXF0EID8           0x02
#define REG_RXF0EID0           0x03
#define REG_RXF1SIDH           0x04
#define REG_RXF1SIDL           0x05
#define REG_RXF1EID8           0x06
#define REG_RXF1EID0           0x07
#define REG_RXF2SIDH           0x08
#define REG_RXF2SIDL           0x09
#define REG_RXF2EID8           0x0A
#define REG_RXF2EID0           0x0B
#define REG_CANSTAT            0x0E
#define REG_CANCTRL            0x0F
#define REG_RXF3SIDH           0x10
#define REG_RXF3SIDL           0x11
#define REG_RXF3EID8           0x12
#define REG_RXF3EID0           0x13
#define REG_RXF4SIDH           0x14
#define REG_RXF4SIDL           0x15
#define REG_RXF4EID8           0x16
#define REG_RXF4EID0           0x17
#define REG_RXF5SIDH           0x18
#define REG_RXF5SIDL           0x19
#define REG_RXF5EID8           0x1A
#define REG_RXF5EID0           0x1B
#define REG_TEC                0x1C
#define REG_REC                0x1D
#define REG_RXM0SIDH           0x20
#define REG_RXM0SIDL           0x21
#define REG_RXM0EID8           0x22
#define REG_RXM0EID0           0x23
#define REG_RXM1SIDH           0x24
#define REG_RXM1SIDL           0x25
#define REG_RXM1EID8           0x26
#define REG_RXM1EID0           0x27
#define REG_CNF3               0x28
#define REG_CNF2               0x29
#define REG_CNF1               0x2A
#define REG_CANINTE            0x2B
#define REG_CANINTF            0x2C
#define REG_MCP2515_ERROR_FLAG 0x2D
#define REG_TXB0CTRL           0x30
#define REG_TXB0SIDH           0x31
#define REG_TXB0SIDL           0x32
#define REG_TXB0EID8           0x33
#define REG_TXB0EID0           0x34
#define REG_TXB0DLC            0x35
#define REG_TXB0DATA           0x36
#define REG_TXB1CTRL           0x40
#define REG_TXB1SIDH           0x41
#define REG_TXB1SIDL           0x42
#define REG_TXB1EID8           0x43
#define REG_TXB1EID0           0x44
#define REG_TXB1DLC            0x45
#define REG_TXB1DATA           0x46
#define REG_TXB2CTRL           0x50
#define REG_TXB2SIDH           0x51
#define REG_TXB2SIDL           0x52
#define REG_TXB2EID8           0x53
#define REG_TXB2EID0           0x54
#define REG_TXB2DLC            0x55
#define REG_TXB2DATA           0x56
#define REG_RXB0CTRL           0x60
#define REG_RXB0SIDH           0x61
#define REG_RXB0SIDL           0x62
#define REG_RXB0EID8           0x63
#define REG_RXB0EID0           0x64
#define REG_RXB0DLC            0x65
#define REG_RXB0DATA           0x66
#define REG_RXB1CTRL           0x70
#define REG_RXB1SIDH           0x71
#define REG_RXB1SIDL           0x72
#define REG_RXB1EID8           0x73
#define REG_RXB1EID0           0x74
#define REG_RXB1DLC            0x75
#define REG_RXB1DATA           0x76

/**
 * @brief instructions
 */
#define INST_WRITE       0x02
#define INST_READ        0x03
#define INST_BITMOD      0x05
#define INST_LOAD_TX0    0x40
#define INST_LOAD_TX1    0x42
#define INST_LOAD_TX2    0x44
#define INST_RTS_TX0     0x81
#define INST_RTS_TX1     0x82
#define INST_RTS_TX2     0x84
#define INST_RTS_ALL     0x87
#define INST_READ_RX0    0x90
#define INST_READ_RX1    0x94
#define INST_READ_STATUS 0xA0
#define INST_RX_STATUS   0xB0
#define INST_RESET       0xC0

/**
 * @brief error flags
 */
#define EFLG_RX1OVR        BV(7)
#define EFLG_RX0OVR        BV(6)
#define EFLG_TXBO          BV(5)
#define EFLG_TXEP          BV(4)
#define EFLG_RXEP          BV(3)
#define EFLG_TXWAR         BV(2)
#define EFLG_RXWAR         BV(1)
#define EFLG_EWARN         BV(0)

#define MCP2515_ERROR_MASK (EFLG_RX1OVR | EFLG_RX0OVR | EFLG_TXBO | EFLG_TXEP | EFLG_RXEP)

/**
 * @brief register mask or value
 */
#define CANCTRL_REQOP        0xE0
#define CANCTRL_ABAT         0x10
#define CANCTRL_OSM          0x08
#define CANCTRL_CLKEN        0x04
#define CANCTRL_CLKPRE       0x03

#define CANSTAT_OPMOD        0xE0
#define CANSTAT_ICOD         0x0E

#define CNF3_SOF             0x80

#define TXB_EXIDE_MASK       0x08
#define DLC_MASK             0x0F
#define RTR_MASK             0x40

#define RXBnCTRL_RXM_STD     0x20
#define RXBnCTRL_RXM_EXT     0x40
#define RXBnCTRL_RXM_STDEXT  0x00
#define RXBnCTRL_RXM_MASK    0x60
#define RXBnCTRL_RTR         0x08
#define RXB0CTRL_BUKT        0x04
#define RXB0CTRL_FILHIT_MASK 0x03
#define RXB1CTRL_FILHIT_MASK 0x07
#define RXB0CTRL_FILHIT      0x00
#define RXB1CTRL_FILHIT      0x01

#define CANINTF_RX0IF        0x01
#define CANINTF_RX1IF        0x02
#define CANINTF_TX0IF        0x04
#define CANINTF_TX1IF        0x08
#define CANINTF_TX2IF        0x10
#define CANINTF_ERRIF        0x20
#define CANINTF_WAKIF        0x40
#define CANINTF_MERRF        0x80

#define STAT_RX0IF           BV(0)
#define STAT_RX1IF           BV(1)
#define STAT_RXIF_MASK       (STAT_RX0IF | STAT_RX1IF)

#define TXB_ABTF             0x40
#define TXB_MLOA             0x20
#define TXB_TXERR            0x10
#define TXB_TXREQ            0x08
#define TXB_TXIE             0x04
#define TXB_TXP              0x03

/**
 * @brief mcp2515 frame byte offset
 */
#define FRAME_SIDH  0
#define FRAME_SIDL  1
#define FRAME_EID8  2
#define FRAME_EID0  3
#define FRAME_DLC   4
#define FRAME_DATA0 5
#define FRAME_DATA1 6
#define FRAME_DATA2 7
#define FRAME_DATA3 8
#define FRAME_DATA4 9
#define FRAME_DATA5 10
#define FRAME_DATA6 11
#define FRAME_DATA7 12

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

static uint8_t _MCP2515_ReadRegister(spi_mcp2515_t* pHandle, uint8_t u8Addr);
static void    _MCP2515_ReadRegisters(spi_mcp2515_t* pHandle, uint8_t u8Addr, uint8_t* pu8Data, uint8_t u8Count);
static void    _MCP2515_SetRegister(spi_mcp2515_t* pHandle, uint8_t u8Addr, uint8_t u8Data);
static void    _MCP2515_SetRegisters(spi_mcp2515_t* pHandle, uint8_t u8Addr, const uint8_t* cpu8Data, uint8_t u8Count);
static void    _MCP2515_ModifyRegister(spi_mcp2515_t* pHandle, uint8_t u8Addr, uint8_t u8Mask, uint8_t u8Data);
static void    _MCP2515_PrepareId(spi_mcp2515_t* pHandle, uint8_t* pBuffer, bool ext, uint32_t u32ID);

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void MCP2515_Init(spi_mcp2515_t* pHandle, can_bps_e eBitrate, mcp2515_clkin_e eClock)
{
    MCP2515_Reset(pHandle);
    MCP2515_SetBitrate(pHandle, eBitrate, eClock);
}

status_t MCP2515_Reset(spi_mcp2515_t* pHandle)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INST_RESET);
    SPI_Master_Deselect(pHandle->hSPI);

    DelayBlockMs(10);

    uint8_t au8NullByte[14] = {0};
    _MCP2515_SetRegisters(pHandle, REG_TXB0CTRL, au8NullByte, 14);
    _MCP2515_SetRegisters(pHandle, REG_TXB1CTRL, au8NullByte, 14);
    _MCP2515_SetRegisters(pHandle, REG_TXB2CTRL, au8NullByte, 14);

    _MCP2515_SetRegister(pHandle, REG_RXB0CTRL, 0);
    _MCP2515_SetRegister(pHandle, REG_RXB1CTRL, 0);

    _MCP2515_SetRegister(pHandle, REG_CANINTE, CANINTF_RX0IF | CANINTF_RX1IF | CANINTF_ERRIF | CANINTF_MERRF);

    // receives all valid messages using either Standard or Extended Identifiers that
    // meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
    _MCP2515_ModifyRegister(pHandle, REG_RXB0CTRL,
                            RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
                            RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
    _MCP2515_ModifyRegister(pHandle, REG_RXB1CTRL,
                            RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
                            RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

    // clear filters and masks
    // do not filter any standard frames for RXF0 used by RXB0
    // do not filter any extended frames for RXF1 used by RXB1
    mcp2515_filter_e aFilters[] = {
        MCP2515_FILTER_0,
        MCP2515_FILTER_1,
        MCP2515_FILTER_2,
        MCP2515_FILTER_3,
        MCP2515_FILTER_4,
        MCP2515_FILTER_5,
    };

    mcp2515_filter_mask_e aMask[] = {
        MCP2515_FILTER_MASK_0,
        MCP2515_FILTER_MASK_1,
    };

    for (uint8_t i = 0; i < ARRAY_SIZE(aFilters); i++)
    {
        ERRCHK_RETURN(MCP2515_SetFilter(pHandle, aFilters[i], i == 1, 0));
    }

    for (uint8_t i = 0; i < ARRAY_SIZE(aMask); i++)
    {
        ERRCHK_RETURN(MCP2515_SetFilterMask(pHandle, aMask[i], true, 0));
    }

    return ERR_NONE;
}

static uint8_t _MCP2515_ReadRegister(spi_mcp2515_t* pHandle, uint8_t u8Addr)
{
    uint8_t u8Data;

    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INST_READ);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Addr);
    SPI_Master_ReceiveByte(pHandle->hSPI, &u8Data);
    SPI_Master_Deselect(pHandle->hSPI);

    return u8Data;
}

static void _MCP2515_ReadRegisters(spi_mcp2515_t* pHandle, uint8_t u8Addr, uint8_t* pu8Data, uint8_t u8Count)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INST_READ);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Addr);
    SPI_Master_ReceiveBlock(pHandle->hSPI, pu8Data, u8Count);
    SPI_Master_Deselect(pHandle->hSPI);
}

static void _MCP2515_SetRegister(spi_mcp2515_t* pHandle, uint8_t u8Addr, uint8_t u8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INST_WRITE);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Addr);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Data);
    SPI_Master_Deselect(pHandle->hSPI);
}

static void _MCP2515_SetRegisters(spi_mcp2515_t* pHandle, uint8_t u8Addr, const uint8_t* cpu8Data, uint8_t u8Count)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INST_WRITE);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Addr);
    SPI_Master_TransmitBlock(pHandle->hSPI, cpu8Data, u8Count);
    SPI_Master_Deselect(pHandle->hSPI);
}

static void _MCP2515_ModifyRegister(spi_mcp2515_t* pHandle, uint8_t u8Addr, uint8_t u8Mask, uint8_t u8Data)
{
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INST_BITMOD);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Addr);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Mask);
    SPI_Master_TransmitByte(pHandle->hSPI, u8Data);
    SPI_Master_Deselect(pHandle->hSPI);
}

uint8_t MCP2515_GetStatus(spi_mcp2515_t* pHandle)
{
    uint8_t u8Status;
    SPI_Master_Select(pHandle->hSPI);
    SPI_Master_TransmitByte(pHandle->hSPI, INST_READ_STATUS);
    SPI_Master_ReceiveByte(pHandle->hSPI, &u8Status);
    SPI_Master_Deselect(pHandle->hSPI);
    return u8Status;
}

status_t MCP2515_SetMode(spi_mcp2515_t* pHandle, mcp2515_mode_e eMode)
{
    _MCP2515_ModifyRegister(pHandle, REG_CANCTRL, CANCTRL_REQOP, eMode);

    tick_t tEnd = GetTickMs() + 10;

    while (GetTickMs() < tEnd)
    {
        mcp2515_mode_e eNewMode = (mcp2515_mode_e)(_MCP2515_ReadRegister(pHandle, REG_CANSTAT) & CANSTAT_OPMOD);

        if (eNewMode == eMode)
        {
            return ERR_NONE;
        }
    }

    return ERR_FAIL;  // fail to change mode
}

status_t MCP2515_SetBitrate(spi_mcp2515_t* pHandle, can_bps_e eBitrate, mcp2515_clkin_e eClock)
{
    ERRCHK_RETURN(MCP2515_SetMode(pHandle, MCP2515_MODE_CONFIG));

    bool    bMatched = true;
    uint8_t aCFG[3]  = {0};

    // clang-format off
    switch (eClock)
    {
        case MCP2515_CLKIN_8MHZ:
        {
            switch (eBitrate)
            {
                case CAN_5KBPS:    aCFG[0] = CLKIN_8MHz_5kBPS_CFG1,    aCFG[1] = CLKIN_8MHz_5kBPS_CFG2,    aCFG[2] = CLKIN_8MHz_5kBPS_CFG3;    break;
                case CAN_10KBPS:   aCFG[0] = CLKIN_8MHz_10kBPS_CFG1,   aCFG[1] = CLKIN_8MHz_10kBPS_CFG2,   aCFG[2] = CLKIN_8MHz_10kBPS_CFG3;   break;
                case CAN_20KBPS:   aCFG[0] = CLKIN_8MHz_20kBPS_CFG1,   aCFG[1] = CLKIN_8MHz_20kBPS_CFG2,   aCFG[2] = CLKIN_8MHz_20kBPS_CFG3;   break;
                case CAN_31K25BPS: aCFG[0] = CLKIN_8MHz_31k25BPS_CFG1, aCFG[1] = CLKIN_8MHz_31k25BPS_CFG2, aCFG[2] = CLKIN_8MHz_31k25BPS_CFG3; break;
                case CAN_33KBPS:   aCFG[0] = CLKIN_8MHz_33k3BPS_CFG1,  aCFG[1] = CLKIN_8MHz_33k3BPS_CFG2,  aCFG[2] = CLKIN_8MHz_33k3BPS_CFG3;  break;
                case CAN_40KBPS:   aCFG[0] = CLKIN_8MHz_40kBPS_CFG1,   aCFG[1] = CLKIN_8MHz_40kBPS_CFG2,   aCFG[2] = CLKIN_8MHz_40kBPS_CFG3;   break;
                case CAN_50KBPS:   aCFG[0] = CLKIN_8MHz_50kBPS_CFG1,   aCFG[1] = CLKIN_8MHz_50kBPS_CFG2,   aCFG[2] = CLKIN_8MHz_50kBPS_CFG3;   break;
                case CAN_80KBPS:   aCFG[0] = CLKIN_8MHz_80kBPS_CFG1,   aCFG[1] = CLKIN_8MHz_80kBPS_CFG2,   aCFG[2] = CLKIN_8MHz_80kBPS_CFG3;   break;
                case CAN_100KBPS:  aCFG[0] = CLKIN_8MHz_100kBPS_CFG1,  aCFG[1] = CLKIN_8MHz_100kBPS_CFG2,  aCFG[2] = CLKIN_8MHz_100kBPS_CFG3;  break;
                case CAN_125KBPS:  aCFG[0] = CLKIN_8MHz_125kBPS_CFG1,  aCFG[1] = CLKIN_8MHz_125kBPS_CFG2,  aCFG[2] = CLKIN_8MHz_125kBPS_CFG3;  break;
                case CAN_200KBPS:  aCFG[0] = CLKIN_8MHz_200kBPS_CFG1,  aCFG[1] = CLKIN_8MHz_200kBPS_CFG2,  aCFG[2] = CLKIN_8MHz_200kBPS_CFG3;  break;
                case CAN_250KBPS:  aCFG[0] = CLKIN_8MHz_250kBPS_CFG1,  aCFG[1] = CLKIN_8MHz_250kBPS_CFG2,  aCFG[2] = CLKIN_8MHz_250kBPS_CFG3;  break;
                case CAN_500KBPS:  aCFG[0] = CLKIN_8MHz_500kBPS_CFG1,  aCFG[1] = CLKIN_8MHz_500kBPS_CFG2,  aCFG[2] = CLKIN_8MHz_500kBPS_CFG3;  break;
                case CAN_1000KBPS: aCFG[0] = CLKIN_8MHz_1000kBPS_CFG1, aCFG[1] = CLKIN_8MHz_1000kBPS_CFG2, aCFG[2] = CLKIN_8MHz_1000kBPS_CFG3; break;
                default: bMatched = false; break;
            }
            break;
        }

        case MCP2515_CLKIN_16MHZ:
        {
            switch (eBitrate)
            {
                case CAN_5KBPS:    aCFG[0] = CLKIN_16MHz_5kBPS_CFG1,    aCFG[1] = CLKIN_16MHz_5kBPS_CFG2,    aCFG[2] = CLKIN_16MHz_5kBPS_CFG3;    break;
                case CAN_10KBPS:   aCFG[0] = CLKIN_16MHz_10kBPS_CFG1,   aCFG[1] = CLKIN_16MHz_10kBPS_CFG2,   aCFG[2] = CLKIN_16MHz_10kBPS_CFG3;   break;
                case CAN_20KBPS:   aCFG[0] = CLKIN_16MHz_20kBPS_CFG1,   aCFG[1] = CLKIN_16MHz_20kBPS_CFG2,   aCFG[2] = CLKIN_16MHz_20kBPS_CFG3;   break;
                case CAN_33KBPS:   aCFG[0] = CLKIN_16MHz_33k3BPS_CFG1,  aCFG[1] = CLKIN_16MHz_33k3BPS_CFG2,  aCFG[2] = CLKIN_16MHz_33k3BPS_CFG3;  break;
                case CAN_40KBPS:   aCFG[0] = CLKIN_16MHz_40kBPS_CFG1,   aCFG[1] = CLKIN_16MHz_40kBPS_CFG2,   aCFG[2] = CLKIN_16MHz_40kBPS_CFG3;   break;
                case CAN_50KBPS:   aCFG[0] = CLKIN_16MHz_50kBPS_CFG1,   aCFG[1] = CLKIN_16MHz_50kBPS_CFG2,   aCFG[2] = CLKIN_16MHz_50kBPS_CFG3;   break;
                case CAN_80KBPS:   aCFG[0] = CLKIN_16MHz_80kBPS_CFG1,   aCFG[1] = CLKIN_16MHz_80kBPS_CFG2,   aCFG[2] = CLKIN_16MHz_80kBPS_CFG3;   break;
                case CAN_83K3BPS:  aCFG[0] = CLKIN_16MHz_83k3BPS_CFG1,  aCFG[1] = CLKIN_16MHz_83k3BPS_CFG2,  aCFG[2] = CLKIN_16MHz_83k3BPS_CFG3;  break;
                case CAN_95KBPS:   aCFG[0] = CLKIN_16MHz_95kBPS_CFG1,   aCFG[1] = CLKIN_16MHz_95kBPS_CFG2,   aCFG[2] = CLKIN_16MHz_95kBPS_CFG3;   break;
                case CAN_100KBPS:  aCFG[0] = CLKIN_16MHz_100kBPS_CFG1,  aCFG[1] = CLKIN_16MHz_100kBPS_CFG2,  aCFG[2] = CLKIN_16MHz_100kBPS_CFG3;  break;
                case CAN_125KBPS:  aCFG[0] = CLKIN_16MHz_125kBPS_CFG1,  aCFG[1] = CLKIN_16MHz_125kBPS_CFG2,  aCFG[2] = CLKIN_16MHz_125kBPS_CFG3;  break;
                case CAN_200KBPS:  aCFG[0] = CLKIN_16MHz_200kBPS_CFG1,  aCFG[1] = CLKIN_16MHz_200kBPS_CFG2,  aCFG[2] = CLKIN_16MHz_200kBPS_CFG3;  break;
                case CAN_250KBPS:  aCFG[0] = CLKIN_16MHz_250kBPS_CFG1,  aCFG[1] = CLKIN_16MHz_250kBPS_CFG2,  aCFG[2] = CLKIN_16MHz_250kBPS_CFG3;  break;
                case CAN_500KBPS:  aCFG[0] = CLKIN_16MHz_500kBPS_CFG1,  aCFG[1] = CLKIN_16MHz_500kBPS_CFG2,  aCFG[2] = CLKIN_16MHz_500kBPS_CFG3;  break;
                case CAN_1000KBPS: aCFG[0] = CLKIN_16MHz_1000kBPS_CFG1, aCFG[1] = CLKIN_16MHz_1000kBPS_CFG2, aCFG[2] = CLKIN_16MHz_1000kBPS_CFG3; break;
                default: bMatched = false; break;
            }
            break;
        }

        case MCP2515_CLKIN_20MHZ:
        {
            switch (eBitrate)
            {
                case CAN_33KBPS:   aCFG[0] = CLKIN_20MHz_33k3BPS_CFG1,   aCFG[1] = CLKIN_20MHz_33k3BPS_CFG2,  aCFG[2] = CLKIN_20MHz_33k3BPS_CFG3;  break;
                case CAN_40KBPS:   aCFG[0] = CLKIN_20MHz_40kBPS_CFG1,    aCFG[1] = CLKIN_20MHz_40kBPS_CFG2,   aCFG[2] = CLKIN_20MHz_40kBPS_CFG3;   break;
                case CAN_50KBPS:   aCFG[0] = CLKIN_20MHz_50kBPS_CFG1,    aCFG[1] = CLKIN_20MHz_50kBPS_CFG2,   aCFG[2] = CLKIN_20MHz_50kBPS_CFG3;   break;
                case CAN_80KBPS:   aCFG[0] = CLKIN_20MHz_80kBPS_CFG1,    aCFG[1] = CLKIN_20MHz_80kBPS_CFG2,   aCFG[2] = CLKIN_20MHz_80kBPS_CFG3;   break;
                case CAN_83K3BPS:  aCFG[0] = CLKIN_20MHz_83k3BPS_CFG1,   aCFG[1] = CLKIN_20MHz_83k3BPS_CFG2,  aCFG[2] = CLKIN_20MHz_83k3BPS_CFG3;  break;
                case CAN_100KBPS:  aCFG[0] = CLKIN_20MHz_100kBPS_CFG1,   aCFG[1] = CLKIN_20MHz_100kBPS_CFG2,  aCFG[2] = CLKIN_20MHz_100kBPS_CFG3;  break;
                case CAN_125KBPS:  aCFG[0] = CLKIN_20MHz_125kBPS_CFG1,   aCFG[1] = CLKIN_20MHz_125kBPS_CFG2,  aCFG[2] = CLKIN_20MHz_125kBPS_CFG3;  break;
                case CAN_200KBPS:  aCFG[0] = CLKIN_20MHz_200kBPS_CFG1,   aCFG[1] = CLKIN_20MHz_200kBPS_CFG2,  aCFG[2] = CLKIN_20MHz_200kBPS_CFG3;  break;
                case CAN_250KBPS:  aCFG[0] = CLKIN_20MHz_250kBPS_CFG1,   aCFG[1] = CLKIN_20MHz_250kBPS_CFG2,  aCFG[2] = CLKIN_20MHz_250kBPS_CFG3;  break;
                case CAN_500KBPS:  aCFG[0] = CLKIN_20MHz_500kBPS_CFG1,   aCFG[1] = CLKIN_20MHz_500kBPS_CFG2,  aCFG[2] = CLKIN_20MHz_500kBPS_CFG3;  break;
                case CAN_1000KBPS: aCFG[0] = CLKIN_20MHz_1000kBPS_CFG1,  aCFG[1] = CLKIN_20MHz_1000kBPS_CFG2, aCFG[2] = CLKIN_20MHz_1000kBPS_CFG3; break;
                default: bMatched = false; break;
            }
            break;
        }

        default: bMatched = false; break;
    }

    // clang-format on

    if (bMatched)
    {
        _MCP2515_SetRegister(pHandle, REG_CNF1, aCFG[0]);
        _MCP2515_SetRegister(pHandle, REG_CNF2, aCFG[1]);
        _MCP2515_SetRegister(pHandle, REG_CNF3, aCFG[2]);
        return ERR_NONE;
    }

    return ERR_FAIL;  // fail to set bitrate
}

void MCP2515_SetClkOut(spi_mcp2515_t* pHandle, mcp2515_clkdiv_e eDivisor)
{
    if (eDivisor == MCP2515_CLKOUT_DISABLE)
    {
        /* Turn off CLKEN */
        _MCP2515_ModifyRegister(pHandle, REG_CANCTRL, CANCTRL_CLKEN, 0x00);

        /* Turn on CLKOUT for SOF */
        _MCP2515_ModifyRegister(pHandle, REG_CNF3, CNF3_SOF, CNF3_SOF);
    }
    else
    {
        /* Set the MCP2515_Prescaler (pHandle,CLKPRE) */
        _MCP2515_ModifyRegister(pHandle, REG_CANCTRL, CANCTRL_CLKPRE, eDivisor);

        /* Turn on CLKEN */
        _MCP2515_ModifyRegister(pHandle, REG_CANCTRL, CANCTRL_CLKEN, CANCTRL_CLKEN);

        /* Turn off CLKOUT for SOF */
        _MCP2515_ModifyRegister(pHandle, REG_CNF3, CNF3_SOF, 0x00);
    }
}

static void _MCP2515_PrepareId(spi_mcp2515_t* pHandle, __OUT uint8_t* pu8Buffer, bool bExtFrame, uint32_t u32FrameID)
{
    if (bExtFrame)
    {
        pu8Buffer[FRAME_EID0] = (uint8_t)(u32FrameID & 0xFF);
        pu8Buffer[FRAME_EID8] = (uint8_t)(u32FrameID >> 8);

        u32FrameID >>= 16;
        pu8Buffer[FRAME_SIDL] = (uint8_t)(u32FrameID & 0x03);
        pu8Buffer[FRAME_SIDL] += (uint8_t)((u32FrameID & 0x1C) << 3);
        pu8Buffer[FRAME_SIDL] |= TXB_EXIDE_MASK;
        pu8Buffer[FRAME_SIDH] = (uint8_t)(u32FrameID >> 5);
    }
    else
    {
        pu8Buffer[FRAME_SIDH] = (uint8_t)(u32FrameID >> 3);
        pu8Buffer[FRAME_SIDL] = (uint8_t)((u32FrameID & 0x07) << 5);
        pu8Buffer[FRAME_EID0] = 0;
        pu8Buffer[FRAME_EID8] = 0;
    }
}

status_t MCP2515_SetFilterMask(spi_mcp2515_t* pHandle, mcp2515_filter_mask_e eMask, bool bExtFrame, uint32_t u32Data)
{
    ERRCHK_RETURN(MCP2515_SetMode(pHandle, MCP2515_MODE_CONFIG));

    uint8_t u8Addr;
    uint8_t au8Data[4];

    switch (eMask)
    {
        case MCP2515_FILTER_MASK_0: u8Addr = REG_RXM0SIDH; break;
        case MCP2515_FILTER_MASK_1: u8Addr = REG_RXM1SIDH; break;
        default: return ERR_INVALID_VALUE;  // invaild value
    }

    _MCP2515_PrepareId(pHandle, au8Data, bExtFrame, u32Data);
    _MCP2515_SetRegisters(pHandle, u8Addr, au8Data, 4);

    return ERR_NONE;
}

status_t MCP2515_SetFilter(spi_mcp2515_t* pHandle, mcp2515_filter_e eFliter, bool bExtFrame, uint32_t u32Data)
{
    ERRCHK_RETURN(MCP2515_SetMode(pHandle, MCP2515_MODE_CONFIG));

    uint8_t u8Addr;
    uint8_t au8Data[4];

    switch (eFliter)
    {
        case MCP2515_FILTER_0: u8Addr = REG_RXF0SIDH; break;
        case MCP2515_FILTER_1: u8Addr = REG_RXF1SIDH; break;
        case MCP2515_FILTER_2: u8Addr = REG_RXF2SIDH; break;
        case MCP2515_FILTER_3: u8Addr = REG_RXF3SIDH; break;
        case MCP2515_FILTER_4: u8Addr = REG_RXF4SIDH; break;
        case MCP2515_FILTER_5: u8Addr = REG_RXF5SIDH; break;
        default: return ERR_INVALID_VALUE;
    }

    _MCP2515_PrepareId(pHandle, au8Data, bExtFrame, u32Data);
    _MCP2515_SetRegisters(pHandle, u8Addr, au8Data, 4);

    return ERR_NONE;
}

status_t MCP2515_SendMessage(spi_mcp2515_t* pHandle, can_frame_t* pFrame)
{
    // tx buffer register
    static struct {
        uint8_t CTRL;
        uint8_t SIDH;
        uint8_t DATA;
    } aTxbufReg[] = {
        {REG_TXB0CTRL, REG_TXB0SIDH, REG_TXB0DATA},
        {REG_TXB1CTRL, REG_TXB1SIDH, REG_TXB1DATA},
        {REG_TXB2CTRL, REG_TXB2SIDH, REG_TXB2DATA}
    };

    if (pFrame->u8DLC > CAN_MAX_DLEN)
    {
        return ERR_OVERFLOW;  // frame length is too long
    }

    for (int i = 0; i < ARRAY_SIZE(aTxbufReg); i++)
    {
        uint8_t ctrlval = _MCP2515_ReadRegister(pHandle, aTxbufReg[i].CTRL);

        if ((ctrlval & TXB_TXREQ) == 0)
        {
            uint8_t au8Data[FRAME_DATA0];

            bool     bExtFrame  = (pFrame->u32ID & CAN_EFF_FLAG);
            bool     bRtrFrame  = (pFrame->u32ID & CAN_RTR_FLAG);
            uint32_t u32FrameID = (pFrame->u32ID & (bExtFrame ? CAN_EFF_MASK : CAN_SFF_MASK));

            _MCP2515_PrepareId(pHandle, au8Data, bExtFrame, u32FrameID);
            au8Data[FRAME_DLC] = bRtrFrame ? (pFrame->u8DLC | RTR_MASK) : pFrame->u8DLC;

            _MCP2515_SetRegisters(pHandle, aTxbufReg[i].SIDH + FRAME_SIDH, au8Data, FRAME_DATA0 + pFrame->u8DLC);
            _MCP2515_SetRegisters(pHandle, aTxbufReg[i].SIDH + FRAME_DATA0, pFrame->au8Data, pFrame->u8DLC);
            _MCP2515_ModifyRegister(pHandle, aTxbufReg[i].CTRL, TXB_TXREQ, TXB_TXREQ);

            if ((_MCP2515_ReadRegister(pHandle, aTxbufReg[i].CTRL) & (TXB_ABTF | TXB_MLOA | TXB_TXERR)) != 0)
            {
                return ERR_FAIL;  // fail to tx
            }

            return ERR_NONE;
        }
    }

    return ERR_BUSY;  // tx buffer is full
}

status_t MCP2515_ReadMessage(spi_mcp2515_t* pHandle, can_frame_t* pFrame)
{
    // rx buffer register
    static struct {
        uint8_t CTRL;
        uint8_t SIDH;
        uint8_t DATA;
        uint8_t CANINTF_RXnIF;
    } aRxbufReg[] = {
        {REG_RXB0CTRL, REG_RXB0SIDH, REG_RXB0DATA, CANINTF_RX0IF},
        {REG_RXB1CTRL, REG_RXB1SIDH, REG_RXB1DATA, CANINTF_RX1IF},
    };

    uint8_t u8Stat  = MCP2515_GetStatus(pHandle);
    uint8_t u8Index = 0;

    if (u8Stat & STAT_RX0IF)
    {
        u8Index = 0;
    }
    else if (u8Stat & STAT_RX1IF)
    {
        u8Index = 1;
    }
    else
    {
        return ERR_NO_MESSAGE;  // no message;
    }

    uint8_t au8Data[5];

    _MCP2515_ReadRegisters(pHandle, aRxbufReg[u8Index].SIDH, au8Data, 5);

    uint32_t u32ID = (au8Data[FRAME_SIDH] << 3) + (au8Data[FRAME_SIDL] >> 5);

    if ((au8Data[FRAME_SIDL] & TXB_EXIDE_MASK) == TXB_EXIDE_MASK)
    {
        u32ID = (u32ID << 2) + (au8Data[FRAME_SIDL] & 0x03);
        u32ID = (u32ID << 8) + au8Data[FRAME_EID8];
        u32ID = (u32ID << 8) + au8Data[FRAME_EID0];
        u32ID |= CAN_EFF_FLAG;
    }

    uint8_t u8DLC = (au8Data[FRAME_DLC] & DLC_MASK);

    if (u8DLC > CAN_MAX_DLEN)
    {
        return ERR_OVERFLOW;  // frame length is too long
    }

    if (_MCP2515_ReadRegister(pHandle, aRxbufReg[u8Index].CTRL) & RXBnCTRL_RTR)
    {
        u32ID |= CAN_RTR_FLAG;
    }

    pFrame->u32ID = u32ID;
    pFrame->u8DLC = u8DLC;

    _MCP2515_ReadRegisters(pHandle, aRxbufReg[u8Index].DATA, pFrame->au8Data, u8DLC);
    _MCP2515_ModifyRegister(pHandle, REG_CANINTF, aRxbufReg[u8Index].CANINTF_RXnIF, 0);

    return ERR_NONE;
}

bool MCP2515_CheckReceive(spi_mcp2515_t* pHandle)
{
    return (MCP2515_GetStatus(pHandle) & STAT_RXIF_MASK) ? true : false;
}

bool MCP2515_CheckError(spi_mcp2515_t* pHandle)
{
    return (MCP2515_GetErrorFlags(pHandle) & MCP2515_ERROR_MASK) ? true : false;
}

uint8_t MCP2515_GetErrorFlags(spi_mcp2515_t* pHandle)
{
    return _MCP2515_ReadRegister(pHandle, REG_MCP2515_ERROR_FLAG);
}

void MCP2515_ClearRXnOVRFlags(spi_mcp2515_t* pHandle)
{
    _MCP2515_ModifyRegister(pHandle, REG_MCP2515_ERROR_FLAG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
}

uint8_t MCP2515_GetInterrupts(spi_mcp2515_t* pHandle)
{
    return _MCP2515_ReadRegister(pHandle, REG_CANINTF);
}

void MCP2515_ClearInterrupts(spi_mcp2515_t* pHandle)
{
    _MCP2515_SetRegister(pHandle, REG_CANINTF, 0);
}

uint8_t MCP2515_GetInterruptMask(spi_mcp2515_t* pHandle)
{
    return _MCP2515_ReadRegister(pHandle, REG_CANINTE);
}

void MCP2515_ClearTXInterrupts(spi_mcp2515_t* pHandle)
{
    _MCP2515_ModifyRegister(pHandle, REG_CANINTF, (CANINTF_TX0IF | CANINTF_TX1IF | CANINTF_TX2IF), 0);
}

void MCP2515_ClearRXnOVR(spi_mcp2515_t* pHandle)
{
    uint8_t u8ErrFlgs = MCP2515_GetErrorFlags(pHandle);

    if (u8ErrFlgs != 0)
    {
        MCP2515_ClearRXnOVRFlags(pHandle);
        MCP2515_ClearInterrupts(pHandle);
        // _MCP2515_ModifyRegister(pHandle,REG_CANINTF, CANINTF_ERRIF, 0);
    }
}

void MCP2515_ClearMERR(spi_mcp2515_t* pHandle)
{
    // _MCP2515_ModifyRegister(pHandle,MCP2515_MCP2515_ERROR_FLAG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
    // MCP2515_ClearInterrupts(pHandle);
    _MCP2515_ModifyRegister(pHandle, REG_CANINTF, CANINTF_MERRF, 0);
}

void MCP2515_ClearERRIF(spi_mcp2515_t* pHandle)
{
    // _MCP2515_ModifyRegister(pHandle,MCP2515_MCP2515_ERROR_FLAG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
    // MCP2515_ClearInterrupts(pHandle);
    _MCP2515_ModifyRegister(pHandle, REG_CANINTF, CANINTF_ERRIF, 0);
}

uint8_t MCP2515_ErrorCountRX(spi_mcp2515_t* pHandle)
{
    return _MCP2515_ReadRegister(pHandle, REG_REC);
}

uint8_t MCP2515_ErrorCountTX(spi_mcp2515_t* pHandle)
{
    return _MCP2515_ReadRegister(pHandle, REG_TEC);
}

//---------------------------------------------------------------------------
// Example
//---------------------------------------------------------------------------

#if CONFIG_DEMOS_SW

void MCP2515_Test(void)
{
#if defined(BOARD_STM32F407VET6_XWS)

    spi_mst_t spi = {
        .MISO = {GPIOA, GPIO_PIN_4},
        .MOSI = {GPIOA, GPIO_PIN_5},
        .SCLK = {GPIOA, GPIO_PIN_6},
        .CS   = {GPIOA, GPIO_PIN_3},
    };

#elif defined(BOARD_AT32F415CB_DEV)

    spi_mst_t spi = {
        .MOSI = {GPIOB, GPIO_PINS_15}, /*MOSI*/
        .MISO = {GPIOB, GPIO_PINS_14}, /*MISO*/
        .SCLK = {GPIOB, GPIO_PINS_13}, /*SCLK*/
        .CS   = {GPIOB, GPIO_PINS_12}, /*CS*/
        // .SPIx = SPI2,
    };

#elif defined(BOARD_CS32F103C8T6_QG)

    spi_mst_t spi = {
        .MOSI = {GPIOA, GPIO_PIN_7 }, /*MOSI*/
        .MISO = {GPIOA, GPIO_PIN_6 }, /*MISO*/
        .SCLK = {GPIOA, GPIO_PIN_5 }, /*SCLK*/
        .CS   = {GPIOB, GPIO_PIN_13}, /*CS*/
    };

#endif

    spi_mcp2515_t mcp2515 = {
        .hSPI = &spi,
    };

    can_frame_t sCanMsgTx1 = {
        .u32ID   = 0x0F6,
        .u8DLC   = 8,
        .au8Data = {0x8E, 0x87, 0x32, 0xFA, 0x26, 0x8E, 0xBE, 0x86},
    };

    can_frame_t sCanMsgTx2 = {
        .u32ID   = 0x11136 | CAN_EFF_FLAG,
        .u8DLC   = 8,
        .au8Data = {0x0E, 0x00, 0x01, 0x08, 0x01, 0x00, 0x00, 0xA0},
    };

    can_frame_t sCanMsgRx1 = {0};

    SPI_Master_Init(&spi, 1000000, SPI_DUTYCYCLE_33_67, MCP2515_SPI_TIMING | SPI_FLAG_SOFT_CS);

    MCP2515_Init(&mcp2515, CAN_125KBPS, MCP2515_CLKIN_8MHZ);
    MCP2515_SetMode(&mcp2515, MCP2515_MODE_NORMAL);  // 模式配置成功，但发送失败，可拔掉/接上终端电阻

    while (1)  // 中断模式未进行测试
    {
#if 1

        PeriodicTask(1 * UNIT_S, {
            MCP2515_SendMessage(&mcp2515, &sCanMsgTx1);
            MCP2515_SendMessage(&mcp2515, &sCanMsgTx2);
        });

        if (MCP2515_ReadMessage(&mcp2515, &sCanMsgRx1) == ERR_NONE)
        {
            MCP2515_SendMessage(&mcp2515, &sCanMsgRx1);
        }
#else

        // speed test

        static uint16_t u16FrameCount = 0;

        if (MCP2515_ReadMessage(&mcp2515, &sCanMsgRx1) == ERR_NONE)
        {
            u16FrameCount++;
        }

        PeriodicTask(1 * UNIT_S, {
            PRINTLN("%d msg/sec", u16FrameCount);
            u16FrameCount = 0;
        });

#endif
    }
}

#endif
