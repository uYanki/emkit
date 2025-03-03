#ifndef __ARCH_OPTS_H__
#define __ARCH_OPTS_H__

/**
 * @brief 芯片架构
 * @relates CONFIG_ARCH_TYPE
 * @{
 */

//
// cortex-m
//

#define ARCH_ARM_CM0   0
#define ARCH_ARM_CM0P  1
#define ARCH_ARM_CM1   2  // fpga
#define ARCH_ARM_CM23  3
#define ARCH_ARM_CM3   4
#define ARCH_ARM_CM4   5
#define ARCH_ARM_CM33  6
#define ARCH_ARM_CM35P 7
#define ARCH_ARM_CM52  8
#define ARCH_ARM_CM55  9
#define ARCH_ARM_CM7   10
#define ARCH_ARM_CM85  11

//
// cortex-r
//

#define ARCH_ARM_CR4  20
#define ARCH_ARM_CR5  21
#define ARCH_ARM_CR7  22
#define ARCH_ARM_CR8  23
#define ARCH_ARM_CR52 24

//
// cortex-a
//

#define ARCH_ARM_A5  30
#define ARCH_ARM_A7  31
#define ARCH_ARM_A53 32
#define ARCH_ARM_A12 33
#define ARCH_ARM_A17 34
#define ARCH_ARM_A32 35
#define ARCH_ARM_A35 36
#define ARCH_ARM_A8  37
#define ARCH_ARM_A9  38
#define ARCH_ARM_A15 39
#define ARCH_ARM_A57 40
#define ARCH_ARM_A72 41
#define ARCH_ARM_A73 42

//
// risc-v
//

#define ARCH_RISCV_RV64  50
#define ARCH_RISCV_THEAD 51

//
// dsp
//

#define ARCH_DSP_C2000 60

/**
 * @}
 */

/**
 * @brief 内存单元大小
 * @relates CONFIG_MEM_UNIT_SIZE
 */
#define MEM_UNIT_8BIT  0  // 8位内存单元(char分配8bit内存)
#define MEM_UNIT_16BIT 1  // 16位内存单元(char分配16bit内存)

#endif  // !__SDK_OPTS_H__
