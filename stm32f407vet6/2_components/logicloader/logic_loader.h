/**
 * @brief logic firmware burning
 */

#ifndef __LOGIC_LOADER_H__
#define __LOGIC_LOADER_H__

#include "gsdk.h"
#include "pinctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

typedef struct {
    __IN const pin_t DCLK;    // dclk_o
    __IN const pin_t DATA;    // data_o
    __IN const pin_t RST;     // rst_o
    __IN const pin_t CONFIG;  // n_config_o
    __IN const pin_t STATUS;  // n_status_i
    __IN const pin_t DONE;    // conf_done_i
} fpag_loader_t;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

err_t LogicLoader_Init(fpag_loader_t* pHandle);

/**
 * @param[in] pu8Buffer fireware buffer
 * @param[in] u32Size   fireware size in bytes
 * @param[in] eBitOrder MSBFirst: lattice
 *                      LSBFirst: cyclone10
 */
err_t LogicLoader_Program(fpag_loader_t* pHandle, const uint8_t* cpu8Buffer, uint32_t u32Size, bit_order_e eBitOrder);

#ifdef __cplusplus
}
#endif

#endif
