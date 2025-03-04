#include "pid.h"

#define rshift(arr)                            \
    do {                                       \
        uint16_t cnt = ARRAY_SIZE(arr);        \
        while (--cnt) arr[cnt] = arr[cnt - 1]; \
    } while (0)

f32 PID_PreHandler(PID_t* PID)
{
    PID->err[0] = *(PID->ref) - *(PID->fbk);
}

f32 PID_PostHandler(PID_t* PID)
{
    // 斜率限制
    if (*(PID->ramp) > 0.0f) {
        f32 rate = (PID->out[0] - PID->out[1]) / PID->Ts;
        if (rate > *(PID->ramp)) {
            PID->out[0] = PID->out[1] + *(PID->ramp) * PID->Ts;
        } else if (rate < -*(PID->ramp)) {
            PID->out[0] = PID->out[1] - *(PID->ramp) * PID->Ts;
        }
    }

    // 幅度限制
    if (PID->out[0] > *(PID->upper)) {
        PID->out[0] = *(PID->upper);
    } else if (PID->out[0] < *(PID->lower)) {
        PID->out[0] = *(PID->lower);
    }

    rshift(PID->err);
    rshift(PID->out);

    return PID->out[0];
}

f32 PID_Handler_Basic(PID_t* PID)
{
    PID_PreHandler(PID);

    PID->prop = PID->err[0];
    PID->inte += PID->err[0] * PID->Ts;
    PID->deri = (PID->err[0] - PID->err[1]) / PID->Ts;

    PID->out[0] = *(PID->Kp) * PID->prop +
                  *(PID->Ki) * PID->inte +
                  *(PID->Kd) * PID->deri;

    return PID_PostHandler(PID);
}
