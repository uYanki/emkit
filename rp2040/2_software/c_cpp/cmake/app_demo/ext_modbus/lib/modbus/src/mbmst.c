

#include "mbmst.h"

#include <stdbool.h>
#include <string.h>

int mbmst_init(mbmst_context_t* ctx, mbmst_cb_t* cb)
{
    memset(ctx, 0, sizeof(mbmst_context_t));
    ctx->cb = *cb;

    if (ctx->cb.tx == NULL ||
        ctx->cb.get_tick_ms == NULL) {
        return -1;
    }

    return 0;
}

static inline void mb_reset(mbmst_context_t* ctx)
{
    ctx->response.pos    = 0;
    ctx->request_timeout = 0;
    if (ctx->current_request) {
        ctx->current_request->data[0] = 0;
        ctx->current_request->ready   = false;
        ctx->current_request          = NULL;
    }
}

void mbmst_rx(mbmst_context_t* ctx, uint8_t b)
{
    if (ctx->response.pos < (sizeof(ctx->response.data) - 1)) {
        ctx->response.data[ctx->response.pos++] = b;
    }
}

static enum mb_state mb_check_buf(mbmst_context_t* ctx)
{
    if (ctx->response.pos > 4) {
        if (ctx->response.frame.address != ctx->current_request->frame.address) {
            return MB_INVALID_SERVER_ADDRESS;
        }
        if (ctx->response.frame.function & 0x80) {
            if (ctx->response.pos == 5) {
                return MB_ERROR;
            }
        }

        switch (ctx->response.frame.function) {
            case MB_READ_COIL_STATUS:
            case MB_READ_INPUT_STATUS:
            case MB_READ_HOLDING_REGISTERS:
            case MB_READ_INPUT_REGISTERS:
                if (ctx->response.pos == ctx->response.frame.data[0] + 5) {
                    return MB_DATA_READY;
                }
                break;
            case MB_WRITE_SINGLE_COIL:
            case MB_WRITE_SINGLE_REGISTER:
            case MB_WRITE_MULTIPLE_COILS:
            case MB_WRITE_MULTIPLE_REGISTERS:
                if (ctx->response.pos == 8) {
                    return MB_DATA_READY;
                }
                break;
            default:
                return MB_INVALID_FUNCTION;
        }
    }
    return MB_DATA_INCOMPLETE;
}

static void mb_rx_rtu(mbmst_context_t* ctx)
{
    uint16_t registers[MB_MAX_REGISTERS];

    if (ctx->current_request == NULL) {
        return;
    }

    if (mb_crc16(ctx->response.data, ctx->response.pos)) {
        if (ctx->cb.status) {
            ctx->cb.status(ctx->current_request->frame.address, ctx->current_request->frame.function, MB_ERROR_INVALID_CRC);
        }
        return;
    }

    if (ctx->current_request->raw) {
        if (ctx->cb.raw_tx) {
            ctx->cb.raw_tx(ctx->response.data, ctx->response.pos);
        }
        return;
    }

    if (ctx->response.frame.function & 0x80) {
        if (ctx->cb.status) {
            ctx->cb.status(ctx->current_request->frame.address, ctx->response.frame.function & ~0x80, ctx->response.frame.data[0]);
        }
        return;
    }

    switch (ctx->response.frame.function) {
        case MB_READ_COIL_STATUS:
            if (ctx->cb.read_coil_status) {
                ctx->cb.read_coil_status(ctx->current_request->frame.address, ctx->current_request->start,
                                         ctx->current_request->count, &ctx->response.frame.data[1]);
            }
            break;
        case MB_READ_INPUT_STATUS:
            if (ctx->cb.read_coil_status) {
                ctx->cb.read_input_status(ctx->current_request->frame.address, ctx->current_request->start,
                                          ctx->current_request->count, &ctx->response.frame.data[1]);
            }
            break;
        case MB_READ_HOLDING_REGISTERS:
            if (ctx->cb.read_holding_registers) {
                // This will make sure the registers are aligned at 16 bits
                memcpy(registers, &ctx->response.frame.data[1], ctx->response.frame.data[0]);
                for (int i = 0; i < ctx->current_request->count; i++) {
                    registers[i] = __builtin_bswap16(registers[i]);
                }
                ctx->cb.read_holding_registers(ctx->current_request->frame.address, ctx->current_request->start,
                                               ctx->current_request->count, registers);
            }
            break;
        case MB_READ_INPUT_REGISTERS:
            if (ctx->cb.read_input_registers) {
                // This will make sure the registers are aligned at 16 bits
                memcpy(registers, &ctx->response.frame.data[1], ctx->response.frame.data[0]);
                for (int i = 0; i < ctx->current_request->count; i++) {
                    registers[i] = __builtin_bswap16(registers[i]);
                }
                ctx->cb.read_input_registers(ctx->current_request->frame.address, ctx->current_request->start,
                                             ctx->current_request->count, registers);
            }
            break;
        case MB_WRITE_SINGLE_COIL:
        case MB_WRITE_SINGLE_REGISTER:
        case MB_WRITE_MULTIPLE_COILS:
        case MB_WRITE_MULTIPLE_REGISTERS:
            if (ctx->cb.status) {
                uint16_t start = (uint16_t)__builtin_bswap16(*(uint16_t*)&ctx->response.frame.data[0]);
                uint16_t count = (uint16_t)__builtin_bswap16(*(uint16_t*)&ctx->response.frame.data[2]);
                uint8_t  error = 0;
                if (start != ctx->current_request->start || count != ctx->current_request->count) {
                    error = MB_ERROR_UNEXPECTED_RESPONSE;
                }
                ctx->cb.status(ctx->current_request->frame.address, ctx->response.frame.function, error);
            }
            break;
        default:
            break;
    }
}

void mbmst_task(mbmst_context_t* ctx)
{
    // Check the receiving buffer
    switch (mb_check_buf(ctx)) {
        case MB_INVALID_FUNCTION:
            mb_reset(ctx);
            break;
        case MB_ERROR:
        case MB_DATA_READY:
            mb_rx_rtu(ctx);
            mb_reset(ctx);
        default:
        case MB_INVALID_SERVER_ADDRESS:
        case MB_DATA_INCOMPLETE:
            break;
    }

    // Check if we have a timeout
    if (ctx->request_timeout > 0 && ctx->cb.get_tick_ms() - ctx->request_timeout > MBMST_REQUEST_TIMEOUT) {
        if (ctx->cb.status) {
            ctx->cb.status(ctx->current_request->frame.address, ctx->current_request->frame.function, MB_ERROR_TIMEOUT);
        }
        mb_reset(ctx);
    }

    if (ctx->current_request == NULL) {
        // Check if there is a new request available
        for (int i = 0; i < MBMST_QUEUE_SIZE; i++) {
            mbmst_buffer_t* request = &ctx->request_queue[i];
            if (request->data[0] && request->ready) {
                request->ready       = false;
                ctx->current_request = request;
                ctx->cb.tx(request->data, request->pos);
                ctx->response.pos    = 0;
                ctx->request_timeout = ctx->cb.get_tick_ms();
                return;
            }
        }
    }
}

static void mb_request_add(mbmst_buffer_t* request, uint16_t value)
{
    request->data[request->pos++] = (value >> 8) & 0xFF;
    request->data[request->pos++] = value & 0xFF;
}

static mbmst_buffer_t* get_request_buffer(mbmst_context_t* ctx)
{
    for (int i = 0; i < MBMST_QUEUE_SIZE; i++) {
        if (!ctx->request_queue[i].data[0]) {
            return &ctx->request_queue[i];
        }
    }
    return NULL;
}

int mbmst_read_write(mbmst_context_t* ctx, uint8_t address, uint8_t fn, uint16_t start, uint16_t count)
{
    if (ctx == NULL || address == 0) {
        return -1;
    }

    mbmst_buffer_t* request = get_request_buffer(ctx);
    if (request == NULL) {
        return -1;
    }

    request->frame.address  = address;
    request->frame.function = fn;
    request->start          = start;
    request->count          = count;
    request->pos            = offsetof(mbmst_buffer_t, frame.data);
    mb_request_add(request, start);
    mb_request_add(request, count);
    mb_request_add(request, mb_crc16(request->data, request->pos));
    request->raw   = false;
    request->ready = true;
    return 0;
}

int mbmst_send_raw(mbmst_context_t* ctx, uint8_t* data, size_t len)
{
    if (ctx == NULL || data == NULL || len == 0) {
        return -1;
    }

    mbmst_buffer_t* request = get_request_buffer(ctx);
    if (request == NULL) {
        return -1;
    }

    memcpy(request->data, data, len);
    request->raw   = true;
    request->pos   = len;
    request->ready = true;
    return 0;
}

int mbmst_read_coil_status(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count)
{
    return mbmst_read_write(ctx, address, MB_READ_COIL_STATUS, start, count);
}

int mbmst_read_input_status(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count)
{
    return mbmst_read_write(ctx, address, MB_READ_INPUT_STATUS, start, count);
}

int mbmst_read_holding_registers(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count)
{
    return mbmst_read_write(ctx, address, MB_READ_HOLDING_REGISTERS, start, count);
}

int mbmst_read_input_registers(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t count)
{
    return mbmst_read_write(ctx, address, MB_READ_INPUT_REGISTERS, start, count);
}

int mbmst_write_single_coil(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t value)
{
    return mbmst_read_write(ctx, address, MB_WRITE_SINGLE_COIL, start, value);
}

int mbmst_write_single_register(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t value)
{
    return mbmst_read_write(ctx, address, MB_WRITE_SINGLE_REGISTER, start, value);
}

int mbmst_write_multiple_coils(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint8_t* data, uint16_t count)
{
    // TODO Not implemented
    return -1;
}

int mbmst_write_multiple_registers(mbmst_context_t* ctx, uint8_t address, uint16_t start, uint16_t* data, uint16_t count)
{
    if (ctx == NULL || address == 0) {
        return -1;
    }

    mbmst_buffer_t* request = get_request_buffer(ctx);
    if (request == NULL) {
        return -1;
    }

    request->frame.address  = address;
    request->frame.function = MB_WRITE_MULTIPLE_REGISTERS;
    request->start          = start;
    request->count          = count;
    request->pos            = offsetof(mbmst_buffer_t, frame.data);
    mb_request_add(request, start);
    mb_request_add(request, count);

    request->data[request->pos++] = count * sizeof(uint16_t);
    for (int i = 0; i < count; i++) {
        mb_request_add(request, data[i]);
    }

    mb_request_add(request, mb_crc16(request->data, request->pos));
    request->raw   = false;
    request->ready = true;
    return 0;
}
