#include "iir.cpp"
#include <stdio.h>
#include <math.h>
#include <stdint.h>

int main()
{
    uint32_t i = 0;

    float fs_hz = 3000;

    IIR notch_filter(notch, 400, 0.707, 0.0, fs_hz);
    IIR lowpass_filter(lowpass, 200, 0.707, 0.0, fs_hz);
    IIR highpass_filter(highpass, 200, BIQUAD_Q_ORDER_4_2, 0.0, fs_hz);

    FILE* file = fopen("./wave.csv", "w");

    if (file != NULL)
    {
        fprintf(file, "1,2,3,4,5,6,7\n", i);

        for (i = 0; i < 500; ++i)
        {
            int32_t signal1 = 0 * 32768;
            int32_t signal2 = 1000 * sin(50 * 2 * M_PI * i / fs_hz) * 32768;
            int32_t signal3 = 1000 * cos(400 * 2 * M_PI * i / fs_hz) * 32768;

            int32_t signal = signal1 + signal2 + signal3;

            int32_t sout1 = lowpass_filter.filter(signal);
            int32_t sout2 = highpass_filter.filter(signal);
            int32_t sout3 = notch_filter.filter(signal);

            fprintf(file, "%d,%d,%d,%d,%d,%d,%d\n", signal1, signal2, signal3, signal, sout1, sout2, sout3);
        }

        fclose(file);
    }

    return 0;
}