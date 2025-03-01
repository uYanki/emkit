#ifndef __WAVE_H__
#define __WAVE_H__

#include <stdbool.h>
#include "main.h"

typedef enum {
    WaveType_None,
    WaveType_Noise,
    WaveType_Triangle,
    WaveType_Custom,
} WaveType_e;

void WaveConfig(uint32_t channel, WaveType_e type, uint16_t* user_src, uint16_t user_size);
void WaveEnableOutput(uint32_t channel);
void WaveDisableOutput(uint32_t channel);
void WaveStart(uint32_t freq);

#endif
