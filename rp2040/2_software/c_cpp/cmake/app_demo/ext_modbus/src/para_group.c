#include "para_group.h"

paragrp_t g_paragrp;

#if CONFIG_EXT_MODULE
uint16_t g_extgrp[EXT_MODULE_COUNT][EXT_MODULE_BUFSIZE] = {0};
#endif

// param table default value
void paragrp_reset(void)
{
    g_paragrp.salveID = 10;

#if CONFIG_LED_MODULE
    g_paragrp.blink.period = 100;
#endif

#if CONFIG_PID_MODULE
    g_paragrp.PID.Kp    = 0.2f;
    g_paragrp.PID.Ki    = 0.1f;
    g_paragrp.PID.Kd    = 0.0f;
    g_paragrp.PID.ref   = 0.0f;
    g_paragrp.PID.fbk   = 0.0f;
    g_paragrp.PID.ramp  = 0.0f;
    g_paragrp.PID.lower = FLT_MIN;
    g_paragrp.PID.upper = FLT_MAX;
    g_paragrp.PID.out   = 0.0f;
    g_paragrp.PID.Ts    = 0.1f;  // 100ms
#endif

#if CONFIG_EXT_MODULE
    uint8_t i;
    for (i = 0; i < ARRAY_SIZE(g_paragrp.EXT); ++i) {
        g_paragrp.EXT[i].salveID    = i + 1;
        g_paragrp.EXT[i].regStart   = 0;
        g_paragrp.EXT[i].regCount   = 0;
        g_paragrp.EXT[i].scanPeriod = 1000;
        g_paragrp.EXT[i].error      = 0;
    }
#endif

    g_paragrp.cmdword.value = 0;  // 命令字
}
