#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "pwmin.hpp"

int main()
{
    stdio_init_all();
    PwmIn inst(16);
    while (true) {
        inst.update();  // blocking read
        printf("pw=%.8f \tp=%.8f \tdc=%.8f \tfrq=%.8f\n",
               inst.pulsewidth(),
               inst.period(),
               inst.dutycycle(),
               inst.frequency());
    }
}
