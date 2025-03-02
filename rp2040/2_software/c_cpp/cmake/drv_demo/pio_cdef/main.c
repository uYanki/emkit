
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "pio_asm_def.h"

#define blink_wrap_target 2
#define blink_wrap        7

static const uint16_t blink_program_instructions[] = {
    // 0x80a0, 0: pull   block
    C_PULL | PULL_BLOCK,

    //  0x6040, 1: out    y, 32
    C_OUT | OUT_DEST_Y | BIT_COUNT_32,

    //     .wrap_target

    // 0xa022,  2: mov    x, y
    C_MOV | MOV_DEST_X | MOV_SRC_Y,

    // 0xe001,  3: set    pins, 1
    C_SET | SET_DEST_PINS | 1,

    //  0x0044, 4: jmp    x--, 4
    C_JMP | IF_X_DEC_NONE_ZERO | 4,

    //  0xa022, 5: mov    x, y
    C_MOV | MOV_DEST_X | MOV_SRC_Y,

    // 0xe000,  6: set    pins, 0
    C_SET | SET_DEST_PINS | 0,

    //  0x0047, 7: jmp    x--, 7
    C_JMP | IF_X_DEC_NONE_ZERO | 7

    //     .wrap
};

static const struct pio_program blink_program = {
    .instructions = blink_program_instructions,
    .length       = 8,
    .origin       = -1,
};

static inline pio_sm_config blink_program_get_default_config(uint offset)
{
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + blink_wrap_target, offset + blink_wrap);
    return c;
}

// this is a raw helper function for use by the user which sets up the GPIO output,
// and configures the SM to output on a particular pin
void blink_program_init(PIO pio, uint sm, uint offset, uint pin)
{
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = blink_program_get_default_config(offset);
    sm_config_set_set_pins(&c, pin, 1);
    pio_sm_init(pio, sm, offset, &c);
}

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq)
{
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    pio->txf[sm] = clock_get_hz(clk_sys) / (2 * freq);
}

int main()
{
    setup_default_uart();

    // todo get free sm
    PIO  pio    = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);

    blink_pin_forever(pio, 0, offset, 23, 2);
    blink_pin_forever(pio, 1, offset, 24, 4);
    blink_pin_forever(pio, 2, offset, 25, 8);

    while (1)
        ;
}
