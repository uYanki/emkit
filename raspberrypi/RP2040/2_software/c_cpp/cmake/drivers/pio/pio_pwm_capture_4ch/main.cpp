#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "pwmin.h"

#define NUM_OF_PINS 4

int main()
{
    // needed for printf
    stdio_init_all();
    printf("pwmin on 4 pins\n");

    // set pwmin
    uint  pin_list[NUM_OF_PINS] = {14, 15, 18, 19};
    pwmin my_pwmin(pin_list, NUM_OF_PINS);

    while (true)
    {
        // adviced empty (for now) function of sdk
        tight_loop_contents();

        // translate pwm of input to output
        float PW_0 = my_pwmin.read_PW(0);
        float PW_1 = my_pwmin.read_PW(1);
        float PW_2 = my_pwmin.read_PW(2);
        float PW_3 = my_pwmin.read_PW(3);
        printf("PW_0=%f PW_1=%f PW_2=%f PW_3=%f\n", PW_0, PW_1, PW_2, PW_3);  // duty
        sleep_ms(100);
    }
}
