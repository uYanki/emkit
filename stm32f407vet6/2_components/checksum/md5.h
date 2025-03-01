#ifndef __MD5_H__
#define __MD5_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <compiler.h>

#define MD5_LEN 16
typedef unsigned int __u32;

struct MD5Context {
    __u32 buf[4];
    __u32 bits[2];
    union {
        unsigned char in[64];
        __u32         in32[16];
    };
};

/**
 * @brief Calculate and store in 'output' the MD5 digest of 'len' bytes at
 *        'input'. 'output' must have enough space to hold 16 bytes.
 */
void md5(const unsigned char* input, const int len, unsigned char output[16]);

/**
 * @brief Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 *        initialization constants.
 */
void MD5Init(struct MD5Context* ctx);

/**
 * @brief Update context to reflect the concatenation of another buffer full of bytes.
 */
void MD5Update(struct MD5Context* ctx, unsigned char const* buf, unsigned len);

/**
 * @brief Final wrapup - pad to 64-byte boundary with the bit pattern
 *        1 0* (64-bit count of bits processed, MSB-first)
 */
void MD5Final(unsigned char digest[16], struct MD5Context* ctx);

#if defined(__cplusplus)
}
#endif

#endif /* __MD5_H__ */
