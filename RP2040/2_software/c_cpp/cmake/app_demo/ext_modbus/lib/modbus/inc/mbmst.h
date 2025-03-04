

#pragma once

#include "mbcom.h"

#define MBMST_REQUEST_TIMEOUT 1000
#define MBMST_QUEUE_SIZE      10

typedef struct {
    void (*read_coil_status)(uint8_t address, uint16_t start, uint16_t count, uint8_t* data);
    void (*read_input_status)(uint8_t address, uint16_t start, uint16_t count, uint8_t* data);
    void (*read_holding_registers)(uint8_t address, uint16_t start, uint16_t count, uint16_t* data);
    void (*read_input_registers)(uint8_t address, uint16_t start, uint16_t count, uint16_t* data);
    void (*status)(uint8_t address, uint8_t function, uint8_t error_code);

    void (*raw_tx)(uint8_t* data, size_t len);
    void (*tx)(uint8_t* data, size_t len);

    uint32_t (*get_tick_ms)(void);
} mbmst_cb_t;

typedef struct {
    union {
        uint8_t       data[MBRTU_FRAME_MAXSIZEE];
        mbrtu_frame_t frame;
    };
    size_t   pos;
    uint16_t start;
    uint16_t count;
    bool     raw;
    bool     ready;
} mbmst_buffer_t;

typedef struct {
    mbmst_cb_t      cb;
    mbmst_buffer_t* current_request;
    mbmst_buffer_t  request_queue[MBMST_QUEUE_SIZE];
    mbmst_buffer_t  response;
    uint32_t        request_timeout;
} mbmst_context_t;

int mbmst_init(mbmst_context_t* ctx, mbmst_cb_t* cb);
int mbmst_read_coil_status(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count);
int mbmst_read_input_status(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count);
int mbmst_read_holding_registers(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count);
int mbmst_read_input_registers(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count);
int mbmst_write_single_coil(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t value);
int mbmst_write_single_register(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t value);
int mbmst_write_multiple_coils(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint8_t* data, uint16_t count);
int mbmst_write_multiple_registers(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t* data, uint16_t count);

int mbmst_send_raw(mbmst_context_t* ctx, uint8_t* data, size_t len);

void mbmst_rx(mbmst_context_t* ctx, uint8_t b);
void mbmst_task(mbmst_context_t* ctx);
