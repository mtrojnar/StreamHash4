/* streamhash4sum.c - re-implemented basic features of the coreutils tool
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "streamhash4.h"

int hash(const char *name) {
    FILE *f;
    int inbytes, i;
    unsigned char buff[16*1024], md[64];
    STREAMHASH4_CTX ctx;

    f = strcmp(name, "-") ? fopen(name, "r") : stdin;
    if (!f) {
        fprintf(stderr, "File open failed: %s\n", name);
        return 1;
    }
    assert(STREAMHASH4_Init(&ctx) == 1);
    while (!feof(f)) {
        inbytes = fread(buff, 1, sizeof buff, f);
        assert(STREAMHASH4_Update(&ctx, buff, inbytes) == 1);
        if (!inbytes && ferror(f)) {
            fprintf(stderr, "File read failed: %s\n", name);
            return 1;
        }
    }
    assert(STREAMHASH4_Final(&ctx, md) == 1);
    fclose(f);

    for (i=0; i<64; ++i)
        printf("%02x", md[i]);
    printf("  %s\n", name);
    return 0;
}

int main(int argc, char *argv[]) {
    int i;

    if (argc<2) {
        if (hash("-"))
            return 1;
    } else {
        for (i=1; i<argc; ++i)
            if (hash(argv[i]))
                return 1;
    }
    return 0;
}

/* end of streamhash4sum.c */
