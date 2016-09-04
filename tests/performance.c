/* performance.c - compute median cps performance for selected hashes
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
#include <alloca.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include "streamhash4.h"

typedef struct { unsigned long t[2]; } timing;
#define timing_now(x) asm volatile(".byte 15;.byte 49" : "=a"((x)->t[0]),"=d"((x)->t[1]))
#define timing_diff(x,y) (((x)->t[0] - (double) (y)->t[0]) + 4294967296.0 * ((x)->t[1] - (double) (y)->t[1]))

typedef void (*HASH)(void *, int);
static double median_cpb(HASH, int);
static int compare_doubles(const void *, const void *);
static void hash_streamhash4(void *, int);
static void hash_sha_256(void *, int);
static void hash_sha_512(void *, int);

int main() {
    int i;

    for(i=1; i<=256*1024; i*=2)
        printf("%d %f %f %f\n", i,
            median_cpb(hash_streamhash4, i),
            median_cpb(hash_sha_256, i),
            median_cpb(hash_sha_512, i));
    return 0;
}

#define SAMPLES 10000

static double median_cpb(HASH f, int data_len) {
    double cpb[SAMPLES];
    timing t_start, t_stop;
    void *data;
    int i;

    data=alloca(data_len);
    memset(data, 0, data_len);
    for(i=0; i<SAMPLES; ++i) {
        timing_now(&t_start);
        f(data, data_len);
        timing_now(&t_stop);
        cpb[i]=1.0*timing_diff(&t_stop, &t_start)/data_len;
    }
    qsort(cpb, SAMPLES, sizeof(double), compare_doubles);
    return cpb[SAMPLES/2];
}

static int compare_doubles(const void *x, const void *y) {
    double dx=*(double *)x, dy=*(double *)y;
    if(dx<dy)
        return -1;
    if(dx>dy)
        return +1;
    return 0;
}

static void hash_streamhash4(void *data, int data_len) {
    unsigned char hash_value[512/8];
    STREAMHASH4(data, data_len, hash_value);
}

static void hash_sha_256(void *data, int data_len) {
    unsigned char hash_value[256/8];
    SHA256(data, data_len, hash_value);
}

static void hash_sha_512(void *data, int data_len) {
    unsigned char hash_value[512/8];
    SHA512(data, data_len, hash_value);
}

/* end of performance.c */
