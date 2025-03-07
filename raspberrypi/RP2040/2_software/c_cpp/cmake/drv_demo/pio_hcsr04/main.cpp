#include <stdio.h>

#include "pico/stdlib.h"

#include "HCSR04.hpp"

int main()
{
    // needed for printf
    stdio_init_all();
    // the instance of the HCSR04 (Echo pin = 14, Trig pin = 15)
    HCSR04 my_HCSR04(14, 15);
    // infinite loop to print distance measurements
    while (true) {
        // read the distance sensor and print the result
        float cm = my_HCSR04.read();
        printf("cm = %f\n", cm);
        sleep_ms(100);
    }
}
