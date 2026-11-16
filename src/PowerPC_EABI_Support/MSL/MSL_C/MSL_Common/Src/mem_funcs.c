/* Word-at-a-time block copies for memmove: forwards and backwards, for blocks whose source and
 * destination share the same alignment within a word and for those that don't. All four first
 * copy single bytes until the destination is word aligned.
 *
 * `src` and `dst` are walked both as byte and as word pointers, through the compiler's
 * cast-as-lvalue extension, so each stays in one register. */

#include "msl_internal.h"

#define SRC_B ((unsigned char *)src)
#define DST_B ((unsigned char *)dst)
#define SRC_W ((unsigned long *)src)
#define DST_W ((unsigned long *)dst)

void __copy_longs_aligned(void *dst, const void *src, size_t n) {
    unsigned long i;

    i = (-(unsigned long)dst) & 3;

    SRC_B = ((unsigned char *)src) - 1;
    DST_B = ((unsigned char *)dst) - 1;

    if (i) {
        n -= i;

        do {
            *++DST_B = *++SRC_B;
        } while (--i);
    }

    SRC_W = ((unsigned long *)(SRC_B + 1)) - 1;
    DST_W = ((unsigned long *)(DST_B + 1)) - 1;

    i = n >> 5;

    if (i) {
        do {
            *++DST_W = *++SRC_W;
            *++DST_W = *++SRC_W;
            *++DST_W = *++SRC_W;
            *++DST_W = *++SRC_W;
            *++DST_W = *++SRC_W;
            *++DST_W = *++SRC_W;
            *++DST_W = *++SRC_W;
            *++DST_W = *++SRC_W;
        } while (--i);
    }

    i = (n & 31) >> 2;

    if (i) {
        do {
            *++DST_W = *++SRC_W;
        } while (--i);
    }

    SRC_B = ((unsigned char *)(SRC_W + 1)) - 1;
    DST_B = ((unsigned char *)(DST_W + 1)) - 1;

    n &= 3;

    if (n) {
        do {
            *++DST_B = *++SRC_B;
        } while (--n);
    }

    return;
}

void __copy_longs_rev_aligned(void *dst, const void *src, size_t n) {
    unsigned long i;

    SRC_B = ((unsigned char *)src) + n;
    DST_B = ((unsigned char *)dst) + n;

    i = ((unsigned long)DST_B) & 3;

    if (i) {
        n -= i;

        do {
            *--DST_B = *--SRC_B;
        } while (--i);
    }

    i = n >> 5;

    if (i) {
        do {
            *--DST_W = *--SRC_W;
            *--DST_W = *--SRC_W;
            *--DST_W = *--SRC_W;
            *--DST_W = *--SRC_W;
            *--DST_W = *--SRC_W;
            *--DST_W = *--SRC_W;
            *--DST_W = *--SRC_W;
            *--DST_W = *--SRC_W;
        } while (--i);
    }

    i = (n & 31) >> 2;

    if (i) {
        do {
            *--DST_W = *--SRC_W;
        } while (--i);
    }

    n &= 3;

    if (n) {
        do {
            *--DST_B = *--SRC_B;
        } while (--n);
    }

    return;
}

void __copy_longs_unaligned(void *dst, const void *src, size_t n) {
    unsigned long i;
    unsigned long v1;
    unsigned long v2;
    unsigned int src_offset;
    unsigned int left_shift;
    unsigned int right_shift;

    i = (-(unsigned long)dst) & 3;

    SRC_B = ((unsigned char *)src) - 1;
    DST_B = ((unsigned char *)dst) - 1;

    if (i) {
        n -= i;

        do {
            *++DST_B = *++SRC_B;
        } while (--i);
    }

    src_offset = ((unsigned int)(SRC_B + 1)) & 3;

    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;

    SRC_B -= src_offset;

    SRC_W = ((unsigned long *)(SRC_B + 1)) - 1;
    DST_W = ((unsigned long *)(DST_B + 1)) - 1;

    i = n >> 3;

    v1 = *++SRC_W;

    do {
        v2 = *++SRC_W;
        *++DST_W = (v1 << left_shift) | (v2 >> right_shift);
        v1 = *++SRC_W;
        *++DST_W = (v2 << left_shift) | (v1 >> right_shift);
    } while (--i);

    if (n & 4) {
        v2 = *++SRC_W;
        *++DST_W = (v1 << left_shift) | (v2 >> right_shift);
    }

    SRC_B = ((unsigned char *)(SRC_W + 1)) - 1;
    DST_B = ((unsigned char *)(DST_W + 1)) - 1;

    n &= 3;

    if (n) {
        SRC_B -= 4 - src_offset;
        do {
            *++DST_B = *++SRC_B;
        } while (--n);
    }

    return;
}

void __copy_longs_rev_unaligned(void *dst, const void *src, size_t n) {
    unsigned long i;
    unsigned long v1;
    unsigned long v2;
    unsigned int src_offset;
    unsigned int left_shift;
    unsigned int right_shift;

    SRC_B = ((unsigned char *)src) + n;
    DST_B = ((unsigned char *)dst) + n;

    i = ((unsigned long)DST_B) & 3;

    if (i) {
        n -= i;

        do {
            *--DST_B = *--SRC_B;
        } while (--i);
    }

    src_offset = ((unsigned int)SRC_B) & 3;

    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;

    SRC_B += 4 - src_offset;

    i = n >> 3;

    v1 = *--SRC_W;

    do {
        v2 = *--SRC_W;
        *--DST_W = (v2 << left_shift) | (v1 >> right_shift);
        v1 = *--SRC_W;
        *--DST_W = (v1 << left_shift) | (v2 >> right_shift);
    } while (--i);

    if (n & 4) {
        v2 = *--SRC_W;
        *--DST_W = (v2 << left_shift) | (v1 >> right_shift);
    }

    n &= 3;

    if (n) {
        SRC_B += src_offset;
        do {
            *--DST_B = *--SRC_B;
        } while (--n);
    }

    return;
}
