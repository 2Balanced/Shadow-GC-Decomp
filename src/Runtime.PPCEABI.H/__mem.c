/* memset and memcpy of the CodeWarrior runtime. They live in .init, next to the start-up code,
 * because __init_data uses them to copy .data and clear .bss before anything else is set up. */

__declspec(section ".init") void *memcpy(void *dst, const void *src, unsigned long n);
__declspec(section ".init") void __fill_mem(void *dst, int value, unsigned long n);
__declspec(section ".init") void *memset(void *dst, int value, unsigned long n);

void *memset(void *dst, int value, unsigned long n) {
    __fill_mem(dst, value, n);
    return dst;
}

/* Fills n bytes with (unsigned char)value: bytes up to a word boundary, then 32-byte blocks of
 * words, then the remaining words and bytes. The pointers run one element behind so that each
 * store can pre-increment (stbu/stwu). */
void __fill_mem(void *dst, int value, unsigned long n) {
    unsigned char *bytes;
    unsigned long *words;
    unsigned long count;
    unsigned long fill = (unsigned char)value;

    bytes = (unsigned char *)dst - 1;
    if (n >= 32) {
        count = ~(unsigned long)bytes & 3;
        if (count != 0) {
            n -= count;
            do {
                *++bytes = fill;
            } while (--count != 0);
        }
        if (fill != 0) {
            fill |= fill << 24 | fill << 16 | fill << 8;
        }

        words = (unsigned long *)(bytes + 1) - 1;
        count = n >> 5;
        if (count != 0) {
            do {
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
            } while (--count != 0);
        }
        count = (n & 31) >> 2;
        if (count != 0) {
            do {
                *++words = fill;
            } while (--count != 0);
        }
        bytes = (unsigned char *)(words + 1) - 1;
        n &= 3;
    }
    if (n != 0) {
        do {
            *++bytes = fill;
        } while (--n != 0);
    }
}

/* Copies forwards, or backwards when the destination is above the source, so that overlapping
 * ranges are handled like memmove. */
void *memcpy(void *dst, const void *src, unsigned long n) {
    const unsigned char *from;
    unsigned char *to;
    int count;

    if (src >= dst) {
        from = (const unsigned char *)src - 1;
        to = (unsigned char *)dst - 1;
        count = n + 1;
        while (--count != 0) {
            *++to = *++from;
        }
    } else {
        from = (const unsigned char *)src + n;
        to = (unsigned char *)dst + n;
        count = n + 1;
        while (--count != 0) {
            *--to = *--from;
        }
    }
    return dst;
}
