/* prng.c - prng generator designed to expose any weak StreamHash4 properties
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
#include <strings.h>
#include "streamhash4.h"

/* 64-bit input */
#define INLEN 8

/* 512-bit output */
#define OUTLEN 64

int main(int argc, char *argv[]) {
    unsigned long long i; /* 64-bit integer to prevent overflow */
    int big_endian;

    if(argc!=2) {
        fprintf(stderr, "Usage:\n\t%s [big|little]\n", argv[0]);
        return 1;
    }
    if(!strcasecmp(argv[1], "big")) {
        big_endian=1;
    } else if(!strcasecmp(argv[1], "little")) {
        big_endian=0;
    } else {
        fprintf(stderr, "Usage:\n\t%s [big|little]\n", argv[0]);
        return 1;
    }
    for(i=0; ;++i) {
        unsigned long long j;
        unsigned char in[INLEN], out[OUTLEN];

        for(j=0; j<INLEN; ++j)
            if(big_endian) /* convert to big-endian */
                in[j]=i>>(8*(INLEN-j-1));
            else /* convert to little-endian */
                in[j]=i>>(8*j);
        if(!STREAMHASH4(in, INLEN, out)) {
            fprintf(stderr, "STREAMHASH4() failed\n");
            return 1;
        }
        /* fwrite(in, INLEN, 1, stdout); */
        fwrite(out, OUTLEN, 1, stdout);
    }
    return 0; /* it should never get here */
}

/* end of prng.c */
