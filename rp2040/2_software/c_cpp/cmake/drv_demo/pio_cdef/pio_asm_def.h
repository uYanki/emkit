
/**
 * @brief RP2040 PIO v1.1
 * @date  2022-11-27
 *
 *    https://www.armbbs.cn/forum.php?mod=viewthread&tid=111476&fromuid=58
 *
 */

#ifndef _PIO_ASM_DEF_H
#define _PIO_ASM_DEF_H

#define C_JMP (0 << 13)
enum IFEnum {
    IF_X_EQU_0         = (1 << 5),
    IF_X_DEC_NONE_ZERO = (2 << 5),
    IF_Y_EQU_0         = (3 << 5),
    IF_Y_DEC_NONE_ZERO = (4 << 5),
    IF_X_NOT_EQU_Y     = (5 << 5),
    IF_PIN             = (6 << 5),
    IF_OSR_NOT_EMPTY   = (7 << 5)
};
enum TAREnum {
    TAR_0,
    TAR_1,
    TAR_2,
    TAR_3,
    TAR_4,
    TAR_5,
    TAR_6,
    TAR_7,
    TAR_8,
    TAR_9,
    TAR_10,
    TAR_11,
    TAR_12,
    TAR_13,
    TAR_14,
    TAR_15,
    TAR_16,
    TAR_17,
    TAR_18,
    TAR_19,
    TAR_20,
    TAR_21,
    TAR_22,
    TAR_23,
    TAR_24,
    TAR_25,
    TAR_26,
    TAR_27,
    TAR_28,
    TAR_29,
    TAR_30,
    TAR_31
};

#define C_WAIT (1 << 13)
enum WaitForEnum {
    WAIT_FOR_0 = (0 << 7),
    WAIT_FOR_1 = (1 << 7)
};
enum WaitSrcEnum {
    WAIT_SRC_GPIO = (0 << 5),
    WAIT_SRC_PIN  = (1 << 5),
    WAIT_SRC_IRQ  = (2 << 5),
};

#define C_IN (2 << 13)
enum InSrcEnum {
    IN_SRC_PINS = (0 << 5),
    IN_SRC_X    = (1 << 5),
    IN_SRC_Y    = (2 << 5),
    IN_SRC_NULL = (3 << 5),
    IN_SRC_ISR  = (6 << 5),
    IN_SRC_OSR  = (7 << 5),
};

#define C_OUT (3 << 13)
enum OutDestEnum {
    OUT_DEST_PINS    = (0 << 5),
    OUT_DEST_X       = (1 << 5),
    OUT_DEST_Y       = (2 << 5),
    OUT_DEST_NULL    = (3 << 5),
    OUT_DEST_PINDIRS = (4 << 5),
    OUT_DEST_PC      = (5 << 5),
    OUT_DEST_ISR     = (6 << 5),
    OUT_DEST_EXEC    = (7 << 5),
};

enum {
    BIT_COUNT_32 = 0,
    BIT_COUNT_1,
    BIT_COUNT_2,
    BIT_COUNT_3,
    BIT_COUNT_4,
    BIT_COUNT_5,
    BIT_COUNT_6,
    BIT_COUNT_7,
    BIT_COUNT_8,
    BIT_COUNT_9,
    BIT_COUNT_10,
    BIT_COUNT_11,
    BIT_COUNT_12,
    BIT_COUNT_13,
    BIT_COUNT_14,
    BIT_COUNT_15,
    BIT_COUNT_16,
    BIT_COUNT_17,
    BIT_COUNT_18,
    BIT_COUNT_19,
    BIT_COUNT_20,
    BIT_COUNT_21,
    BIT_COUNT_22,
    BIT_COUNT_23,
    BIT_COUNT_24,
    BIT_COUNT_25,
    BIT_COUNT_26,
    BIT_COUNT_27,
    BIT_COUNT_28,
    BIT_COUNT_29,
    BIT_COUNT_30,
    BIT_COUNT_31,
};

#define C_PUSH (4 << 13)
enum {
    IF_FULL    = (1 << 6),
    PUSH_BLOCK = (1 << 5),
};
#define C_PULL ((4 << 13) | (1 << 7))
enum {
    IF_EMPTY   = (1 << 6),
    PULL_BLOCK = (1 << 5),
};
#define C_MOV (5 << 13)
enum {
    MOV_DEST_PINS = (0 << 5),
    MOV_DEST_X    = (1 << 5),
    MOV_DEST_Y    = (2 << 5),
    MOV_DEST_EXEC = (4 << 5),
    MOV_DEST_PC   = (5 << 5),
    MOV_DEST_ISR  = (6 << 5),
    MOV_DEST_OSR  = (7 << 5),
};
enum {
    MOV_OP_NONE   = (0 << 3),
    MOV_OP_INVERT = (1 << 3),
};
enum {
    MOV_SRC_PINS   = (0 << 0),
    MOV_SRC_X      = (1 << 0),
    MOV_SRC_Y      = (2 << 0),
    MOV_SRC_NULL   = (3 << 0),
    MOV_SRC_STATUS = (5 << 0),
    MOV_SRC_ISR    = (6 << 0),
    MOV_SRC_OSR    = (7 << 0),
};

#define C_IRQ (6 << 13)
enum {
    IRQ_SET   = (0 << 6),
    IRQ_CLEAR = (1 << 6),

    IRQ_WAIT = (1 << 5),
};

#define C_SET (7 << 13)
enum {
    SET_DEST_PINS    = (0 << 5),
    SET_DEST_X       = (1 << 5),
    SET_DEST_Y       = (2 << 5),
    SET_DEST_PINDIRS = (4 << 5),
};
#endif

/* 侧集指令 Delay/side-set  5 bits */
#define SIDE_SET(x) (x << 8)
