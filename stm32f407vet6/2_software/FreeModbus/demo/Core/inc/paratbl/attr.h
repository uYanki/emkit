#ifndef __PARA_ATTR_H__
#define __PARA_ATTR_H__

#include "stm32f4xx.h"             

//-----------------------------------------------------------------------------
// data decomposition

// 16 bits
#define W(DATA)       (u16)(DATA)

// 32 bits
#define WL(DATA)      (u16) GETBIT32((u32)DATA, 0, 16)
#define WH(DATA)      (u16) GETBIT32((u32)DATA, 16, 16)

// 64 bits
#define W0(DATA)      (u16) GETBIT64((u64)DATA, 0, 16)
#define W1(DATA)      (u16) GETBIT64((u64)DATA, 16, 16)
#define W2(DATA)      (u16) GETBIT64((u64)DATA, 32, 16)
#define W3(DATA)      (u16) GETBIT64((u64)DATA, 48, 16)

//-----------------------------------------------------------------------------
// access permissions

#define ATTR_STB_ACE  0ul
#define ATTR_LEN_ACE  2ul

#define V_ACE0        0ul  // user
#define V_ACE1        1ul  // admin
#define V_ACE2        2ul  // super
#define B_ACE0        (V_ACE0 << ATTR_STB_ACE)
#define B_ACE1        (V_ACE1 << ATTR_STB_ACE)
#define B_ACE2        (V_ACE2 << ATTR_STB_ACE)

//-----------------------------------------------------------------------------
// read/write

#define ATTR_STB_RW   (ATTR_STB_ACE + ATTR_LEN_ACE)
#define ATTR_LEN_RW   2ul

#define V_RO          0ul
#define V_WO          1ul
#define V_RW          2ul
#define B_RO          (V_RO << ATTR_STB_RW)
#define B_WO          (V_WO << ATTR_STB_RW)
#define B_RW          (V_RW << ATTR_STB_RW)

//-----------------------------------------------------------------------------
// signed/unsigned

#define ATTR_STB_SIG  (ATTR_STB_RW + ATTR_LEN_RW)
#define ATTR_LEN_SIG  1ul

#define V_UNSI        0ul  // unsigned
#define V_SIGN        1ul  // signed
#define B_UNSI        (V_UNSI << ATTR_STB_SIG)
#define B_SIGN        (V_SIGN << ATTR_STB_SIG)

//-----------------------------------------------------------------------------
// word length

#define ATTR_STB_LEN  (ATTR_STB_SIG + ATTR_LEN_SIG)
#define ATTR_LEN_LEN  3ul

#define V_SIG         0ul  // single word (16bits)
#define V_DBL0        1ul  // double word (32bits)
#define V_DBL1        2ul
#define V_QUA0        3ul  // quadruple word (64bits)
#define V_QUA1        4ul
#define V_QUA2        5ul
#define V_QUA3        6ul

#define B_SIG         (V_SIG << ATTR_STB_LEN)
#define B_DBL0        (V_DBL0 << ATTR_STB_LEN)
#define B_DBL1        (V_DBL1 << ATTR_STB_LEN)
#define B_QUA0        (V_QUA0 << ATTR_STB_LEN)
#define B_QUA1        (V_QUA1 << ATTR_STB_LEN)
#define B_QUA2        (V_QUA2 << ATTR_STB_LEN)
#define B_QUA3        (V_QUA3 << ATTR_STB_LEN)

//-----------------------------------------------------------------------------
// display format

#define ATTR_STB_FMT  (ATTR_STB_LEN + ATTR_LEN_LEN)
#define ATTR_LEN_FMT  2ul

#define V_BIN         0ul  // binary
#define V_OCT         1ul  // octal
#define V_DEC         2ul  // decimal
#define V_HEX         3ul  // hexadecimal

#define B_BIN         (V_BIN << ATTR_STB_FMT)
#define B_OCT         (V_OCT << ATTR_STB_FMT)
#define B_DEC         (V_DEC << ATTR_STB_FMT)
#define B_HEX         (V_HEX << ATTR_STB_FMT)

//-----------------------------------------------------------------------------
// synchronize to/from storage medium (eeprom/flash)

#define ATTR_STB_SYNC (ATTR_STB_FMT + ATTR_LEN_FMT)
#define ATTR_LEN_SYNC 1ul

#define V_SYNC        0ul  // sync
#define V_NSYNC       1ul  // not sync
#define B_SYNC        (V_SYNC << ATTR_STB_SYN)
#define B_NSYNC       (V_NSYNC << ATTR_STB_SYN)

//-----------------------------------------------------------------------------
// mode

#define ATTR_STB_MODE (ATTR_STB_SYNC + ATTR_LEN_SYNC)
#define ATTR_LEN_MODE 2ul

//-----------------------------------------------------------------------------
// reserve

#define ATTR_STB_RESV (ATTR_STB_MODE + ATTR_LEN_MODE)
#define ATTR_LEN_RESV (32ul - ATTR_STB_RESV)

//-----------------------------------------------------------------------------
// parameter attribution

typedef struct {
    union {
        u32 dword;
        struct {
            u32 ace : ATTR_LEN_ACE;    // access
            u32 rw : ATTR_LEN_RW;      // read & write
            u32 sig : ATTR_LEN_SIG;    // signed & unsigned
            u32 len : ATTR_LEN_LEN;    // length
            u32 fmt : ATTR_LEN_FMT;    // display format
            u32 sync : ATTR_LEN_SYNC;  // synchronize
            u32 mode : ATTR_LEN_MODE;  // mode
            u32 resv : ATTR_LEN_RESV;  // reserve
        } bits;
    } attr;
    struct {
        u16 init;  // initial
        u16 min;   // minimum
        u16 max;   // maximum
    } value;
} ParaAttr_t;

#endif
