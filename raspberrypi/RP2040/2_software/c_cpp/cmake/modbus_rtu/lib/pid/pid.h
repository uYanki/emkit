#pragma once

#include "usdk.types.h"

struct PID;
typedef f32 (*PID_handler_t)(struct PID* PID);

typedef struct PID {
    RO f32* Kp;     //!< proportional gain
    RO f32* Ki;     //!< integral gain
    RO f32* Kd;     //!< derivative gain

    RO f32* ref;    //!< reference input value
    RO f32* fbk;    //!< feedback input value

    f32 err[2];     //!< error
    f32 prop;       //!< proportional value
    f32 inte;       //!< integral value
    f32 deri;       //!< derivative value

    RO f32* ramp;   //!< ramp limit
    RO f32* lower;  //!< lower saturation limit
    RO f32* upper;  //!< upper saturation limit

    f32 out[2];     //!< output value

    f32 Ts;         //!< samples period

    PID_handler_t cb;
} PID_t;

f32 PID_Handler_Basic(PID_t* PID);
