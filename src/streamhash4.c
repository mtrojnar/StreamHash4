/* streahmash4.c version 1.1
 *
 * ISC License
 *
 * Copyright (c) 2016, Michał Trojnara
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "streamhash4.h"
#include <string.h>

#ifndef __AES__
#error "-maes" compiler option is currently required
#endif

/* generated with http://hexpi.sourceforge.net/ */
static const unsigned char hexpi[64] = {
    0x24, 0x3F, 0x6A, 0x88, 0x85, 0xA3, 0x08, 0xD3,
    0x13, 0x19, 0x8A, 0x2E, 0x03, 0x70, 0x73, 0x44,
    0xA4, 0x09, 0x38, 0x22, 0x29, 0x9F, 0x31, 0xD0,
    0x08, 0x2E, 0xFA, 0x98, 0xEC, 0x4E, 0x6C, 0x89,
    0x45, 0x28, 0x21, 0xE6, 0x38, 0xD0, 0x13, 0x77,
    0xBE, 0x54, 0x66, 0xCF, 0x34, 0xE9, 0x0C, 0x6C,
    0xC0, 0xAC, 0x29, 0xB7, 0xC9, 0x7C, 0x50, 0xDD,
    0x3F, 0x84, 0xD5, 0xB5, 0xB5, 0x47, 0x09, 0x17};

/*************************************** AES NI implementation */

#ifdef __AES__

static const __m128i *consts = (__m128i *)hexpi;

static int STREAMHASH4_Init_NI(STREAMHASH4_CTX *ctx) {
    ctx->state[0] = consts[0];
    ctx->state[1] = consts[1];
    ctx->state[2] = consts[2];
    ctx->state[3] = consts[3];
    ctx->bit_len = _mm_setzero_si128();
    ctx->buf_len = 0;
    return 1;
}

#define block(src) \
    ctx->state[0] = _mm_aesenc_si128(ctx->state[0] ^ (src), consts[0]); \
    ctx->state[1] = _mm_aesenc_si128(ctx->state[1] ^ (src), consts[1]); \
    ctx->state[2] = _mm_aesenc_si128(ctx->state[2] ^ (src), consts[2]); \
    ctx->state[3] = _mm_aesenc_si128(ctx->state[3] ^ (src), consts[3])

static int STREAMHASH4_Update_NI(STREAMHASH4_CTX *ctx, const void *data, size_t len) {
    /* complete the buffer with new data */
    if (ctx->buf_len && ctx->buf_len + len >= 16) {
        size_t new_bytes = 16 - ctx->buf_len;
        ctx->bit_len += new_bytes << 3;
        memcpy((char *)&ctx->buf_data + ctx->buf_len, data, new_bytes);
        ctx->state[3] ^= ctx->bit_len;
        block(ctx->buf_data);
        data += new_bytes;
        len -= new_bytes;
        ctx->buf_len = 0;
    }

    /* process full 128-bit data blocks */
    while (len >= 16) {
        ctx->bit_len += 128;
        ctx->state[3] ^= ctx->bit_len;
        memcpy(&ctx->buf_data, data, 16); /* must be aligned */
        block(ctx->buf_data);
        data += 16;
        len -= 16;
    }

    /* save any remaining bytes */
    if (len) {
        ctx->bit_len += len << 3;
        memcpy((char *)&ctx->buf_data + ctx->buf_len, data, len);
        ctx->buf_len += len;
    }

    return 1;
}

static int STREAMHASH4_Final_NI(STREAMHASH4_CTX *ctx, void *md) {
    __m128i saved[4];

    /* process the last incomplete block */
    if (ctx->buf_len) {
        memset((char *)&ctx->buf_data + ctx->buf_len, 0, 16 - ctx->buf_len);
        ctx->state[3] ^= ctx->bit_len;
        block(ctx->buf_data);
    }

    /* save and reinitialize the state vector */
    saved[0] = ctx->state[0];
    saved[1] = ctx->state[1];
    saved[2] = ctx->state[2];
    saved[3] = ctx->state[3];
    ctx->state[0] = consts[0];
    ctx->state[1] = consts[1];
    ctx->state[2] = consts[2];
    ctx->state[3] = consts[3];

    /* update 3 times with a copy of the state vector (4 blocks each) */
    block(saved[0]); block(saved[1]); block(saved[2]); block(saved[3]);
    block(saved[0]); block(saved[1]); block(saved[2]); block(saved[3]);
    block(saved[0]); block(saved[1]); block(saved[2]); block(saved[3]);

    /* xor the state the saved first stream output */
    ctx->state[0] ^= saved[0];
    ctx->state[1] ^= saved[0];
    ctx->state[2] ^= saved[0];
    ctx->state[3] ^= saved[0];
    
    /* copy the md value */
    memcpy(md, ctx->state, 64);

    return 1;
}

#endif /* defined __AES__ */

/*************************************** portable implementation */

static int STREAMHASH4_Init_Portable(STREAMHASH4_CTX *ctx) {
    return 0; /* not implemented */
}

static int STREAMHASH4_Update_Portable(STREAMHASH4_CTX *ctx, const void *data, size_t len) {
    return 0; /* not implemented */
}

static int STREAMHASH4_Final_Portable(STREAMHASH4_CTX *ctx, void *md) {
    return 0; /* not implemented */
}

/*************************************** implementation chooser */

typedef enum {
    IMPLEMENTATION_NONE,
    IMPLEMENTATION_NI,
    /* TODO: put other implementations here */
    IMPLEMENTATION_PORTABLE
} IMPLEMENTATION;

static IMPLEMENTATION implementation = IMPLEMENTATION_NONE;

static IMPLEMENTATION get_implementation() {
#ifdef __AES__
    unsigned long a, b, c, d;

    __asm__ __volatile__ ("cpuid"
        : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
        : "a" (1));
    if (c & 0x2000000)
        return IMPLEMENTATION_NI;
#endif
    /* TODO: put other implementations here */
    return IMPLEMENTATION_PORTABLE;
}

int STREAMHASH4_Init(STREAMHASH4_CTX *ctx) {
    if (implementation == IMPLEMENTATION_NONE)
        implementation = get_implementation();
#ifdef __AES__
    if (implementation == IMPLEMENTATION_NI)
        return STREAMHASH4_Init_NI(ctx);
#endif
    /* TODO: put other implementations here */
    return STREAMHASH4_Init_Portable(ctx);
}

int STREAMHASH4_Update(STREAMHASH4_CTX *ctx, const void *data, size_t len) {
    if (implementation == IMPLEMENTATION_NONE)
        implementation = get_implementation();
#ifdef __AES__
    if (implementation == IMPLEMENTATION_NI)
        return STREAMHASH4_Update_NI(ctx, data, len);
#endif
    /* TODO: put other implementations here */
    return STREAMHASH4_Update_Portable(ctx, data, len);
}

int STREAMHASH4_Final(STREAMHASH4_CTX *ctx, void *md) {
    if (implementation == IMPLEMENTATION_NONE)
        implementation = get_implementation();
#ifdef __AES__
    if (implementation == IMPLEMENTATION_NI)
        return STREAMHASH4_Final_NI(ctx, md);
#endif
    /* TODO: put other implementations here */
    return STREAMHASH4_Final_Portable(ctx, md);
}

int STREAMHASH4(const void *data, size_t len, void *md) {
    STREAMHASH4_CTX ctx;

    return STREAMHASH4_Init(&ctx) &&
        STREAMHASH4_Update(&ctx, data, len) &&
        STREAMHASH4_Final(&ctx, md);
}

/* end of streamhash4.c */
