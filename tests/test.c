/* benchmark.c - basic StreamHash4 tests
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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "streamhash4.h"

/* benchmark complexity (input length and number of iterations) */
#define LEN (16*1024)
#define RTR (1024*1024)

void test_vector() {
    const char test_msg[] = "The quick brown fox jumps over the lazy dog";
    const unsigned char test_md[64] = {
        0x74, 0x4b, 0x9e, 0x66, 0xb0, 0xc3, 0x4b, 0x1f,
        0x46, 0x3f, 0xda, 0x51, 0xba, 0x79, 0x93, 0x90,
        0x52, 0x67, 0x0e, 0xb1, 0x01, 0x64, 0x76, 0x2a,
        0xb0, 0xa2, 0x0e, 0x05, 0xa5, 0xa0, 0xa0, 0x18,
        0x83, 0x96, 0x6a, 0x4f, 0x46, 0x9f, 0x6a, 0x47,
        0x6b, 0x15, 0x8e, 0x60, 0xca, 0x25, 0xb6, 0x78,
        0x62, 0xea, 0xda, 0x6b, 0x90, 0x6d, 0xdf, 0x77,
        0xe2, 0xa7, 0x36, 0x47, 0xdd, 0x8c, 0x3b, 0xf7};
    unsigned char md[64];

    assert(STREAMHASH4(test_msg, sizeof test_msg - 1, md) == 1);
    assert(memcmp(test_md, md, sizeof md) == 0);
}

void test_alignment(int rel) {
    FILE *f;
    unsigned char msg[128], md_ref[64], md[128];
    STREAMHASH4_CTX ctx;

    f = fopen("/dev/urandom", "r");
    assert(f != NULL);
    assert(fread(msg, sizeof msg, 1, f) == 1);
    fclose(f);

    /* make sure the input is *not* aligned */
    assert(STREAMHASH4(msg+rel, 33, md_ref) == 1);

    assert(STREAMHASH4_Init(&ctx) == 1);
    assert(STREAMHASH4_Update(&ctx, msg+rel, 8) == 1);
    assert(STREAMHASH4_Update(&ctx, msg+8+rel, 16) == 1);
    assert(STREAMHASH4_Update(&ctx, msg+24+rel, 1) == 1);
    assert(STREAMHASH4_Update(&ctx, msg+25+rel, 8) == 1);
    assert(STREAMHASH4_Final(&ctx, md+rel) == 1);
    assert(memcmp(md_ref, md+rel, 64) == 0);
}

void benchmark() {
    long i;
    unsigned char msg[LEN], md[64];
    time_t start;

    printf("Message size: %d bytes\n", LEN);
    memset(msg, 0, sizeof msg);
    assert(STREAMHASH4(msg, LEN, md) == 1); /* populate the caches */

    start=clock();
    for(i=0; i<RTR; ++i)
        STREAMHASH4(msg, LEN, md);
    printf("Performance: %.2f GB/s\n",
        0.000000001*LEN*RTR/(clock()-start)*CLOCKS_PER_SEC);
}

int main() {
    int i;

    test_vector();
    for(i=0; i<32; ++i)
        test_alignment(i);
    printf("Tests succeeded\n");
    benchmark();
    return 0;
}

/* end of benchmark.c */
