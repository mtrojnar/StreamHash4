/* speed.c - display performance in the "openssl speed" format
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

#define step 10000

double benchmark(int len) {
    unsigned int i, n;
    unsigned char out[64], *msg;
    time_t start, end;
    double r;

    fprintf(stderr, "Doing streamhash4 for 3s on %d size blocks: ", len);
    fflush(stderr);
    msg = malloc(len);
    memset(msg, 0xc5, len);
    assert(STREAMHASH4(msg, len, out)==1); /* populate CPU caches */
    start = clock();
    end = start+3*CLOCKS_PER_SEC; /* expected value */
    for (n=0; clock() < end; n+=step)
        for(i=0; i<step; ++i)
            STREAMHASH4(msg, len, out);
    end = clock(); /* actual value */
    r = 0.001*n*len/(end-start)*CLOCKS_PER_SEC;
    free(msg);
    fprintf(stderr, "%d streamhash4's in %1.2fs\n",
        n, 1.0*(end-start)/CLOCKS_PER_SEC);
    return r;
}

int main() {
    double p16, p64, p256, p1024, p8192, p16384;

    p16 = benchmark(16);
    p64 = benchmark(64);
    p256 = benchmark(256);
    p1024 = benchmark(1024);
    p8192 = benchmark(8192);
    p16384 = benchmark(16384);
    printf("type             16 bytes     64 bytes    256 bytes   1024 bytes   8192 bytes  16384 bytes\n");
    printf("%-13s%12.2fk%12.2fk%12.2fk%12.2fk%12.2fk%12.2fk\n",
        "streamhash4", p16, p64, p256, p1024, p8192, p16384);
    return 0;
}

/* end of speed.c */
