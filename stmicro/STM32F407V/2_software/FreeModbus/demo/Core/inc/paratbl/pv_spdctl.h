#ifndef __PV_SPDCTL_H__
#define __PV_SPDCTL_H__

typedef enum {
    SPD_REF_SRC_DIGITAL,
    SPD_REF_SRC_MUTIL_DIGITAL,
    SPD_REF_SRC_ANALOG,
} SpdTgtSrc_e;

typedef enum {
    SPD_LIM_FWD,
    SPD_LIM_REV,
} SpdLimSrc_e;

#endif
