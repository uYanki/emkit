#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/** major version number (X.x.x) */
#define VERSION_MAJOR 5
/** minor version number (x.X.x) */
#define VERSION_MINOR 4
/** patch version number (x.x.X) */
#define VERSION_PATCH 0

/**
 * @brief macro to convert sdk version number into an integer
 * @note  to be used in comparisons, such as VERSION >= VERSION_VAL(1, 0, 0)
 */
#define VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))

/**
 * @brief current sdk version, as an integer
 * @note  to be used in comparisons, such as VERSION >= VERSION_VAL(1, 0, 0)
 */
#define VERSION VERSION_VAL(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif
