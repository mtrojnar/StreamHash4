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
        0xab, 0xe2, 0xb2, 0xb6, 0xbf, 0x3f, 0xf0, 0x3d,
        0xb0, 0xac, 0xd7, 0x3f, 0xef, 0xcf, 0x3f, 0xa4,
        0x78, 0x2b, 0x72, 0x50, 0x8e, 0x89, 0x5f, 0xd8,
        0xf2, 0x81, 0xdc, 0x32, 0xf4, 0xec, 0x60, 0xce,
        0xd6, 0x13, 0x9f, 0x4b, 0xd1, 0x75, 0x8b, 0x7d,
        0x74, 0xcf, 0x8a, 0xf6, 0xa1, 0x93, 0x2c, 0xab,
        0x10, 0x42, 0x42, 0x0c, 0x33, 0x0d, 0x15, 0x85,
        0xc6, 0x15, 0x71, 0x01, 0x60, 0xa4, 0x63, 0xba};
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
