

#pragma once

#include "mbcom.h"

#define MBSLV_RECEIVE_TIMEOUT 500

typedef struct {
    mb_result_e (*read_coil_status)(uint16_t start, uint16_t count);
    mb_result_e (*read_input_status)(uint16_t start, uint16_t count);
    mb_result_e (*read_holding_registers)(uint16_t start, uint16_t count);
    mb_result_e (*read_input_registers)(uint16_t start, uint16_t count);
    mb_result_e (*write_single_coil)(uint16_t start, uint16_t value);
    mb_result_e (*write_single_register)(uint16_t start, uint16_t value);
    mb_result_e (*write_multiple_coils)(uint16_t start, uint8_t* data, uint16_t count);
    mb_result_e (*write_multiple_registers)(uint16_t start, uint16_t* data, uint16_t count);

    void (*raw_tx)(uint8_t* data, size_t len);  // this function will be called, and when the current frame is not mine, it may be another frame
    void (*tx)(uint8_t* data, size_t len);
    uint32_t (*get_tick_ms)(void);
} mbslv_cb_t;

typedef struct {
    union {
        uint8_t       data[MBRTU_FRAME_MAXSIZEE];
        mbrtu_frame_t frame;
    };
    size_t pos;
} mbslv_buffer_t;

typedef struct {
    uint8_t        address;
    mbslv_cb_t     cb;
    mbslv_buffer_t request;
    mbslv_buffer_t response;
    uint32_t       timeout;
} mbslv_context_t;

int  mbslv_init(mbslv_context_t* ctx, uint8_t address, mbslv_cb_t* cb);
void mbslv_rx(mbslv_context_t* ctx, uint8_t b);
void mbslv_add_response(mbslv_context_t* ctx, uint16_t value);
void mbslv_task(mbslv_context_t* ctx);
