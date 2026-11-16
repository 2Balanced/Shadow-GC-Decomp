#include <TRK_MINNOW_DOLPHIN/trk.h>

/* The nub's own memcpy and memset. Like the runtime's they sit in .init, away from code a
 * debugger might be patching. */

void *TRK_memcpy(void *dest, const void *src, TRKSize length) {
    const u8 *from = (const u8 *)src - 1;
    u8 *to = (u8 *)dest - 1;

    length++;
    while (--length != 0) {
        *++to = *++from;
    }
    return dest;
}

void *TRK_memset(void *dest, int value, TRKSize length) {
    TRK_fill_mem(dest, value, length);
    return dest;
}

/* Fills length bytes with (u8)value: single bytes up to a word boundary, 32-byte blocks of words,
 * the remaining words, then the remaining bytes. Each pointer trails its next store by one
 * element so the stores can pre-increment. */
void TRK_fill_mem(void *dest, int value, TRKSize length) {
    u8 *bytes = (u8 *)dest - 1;
    u32 *words;
    u32 count;
    u32 fill = (u8)value;

    if (length >= 32) {
        count = ~(u32)bytes & 3;
        if (count != 0) {
            length -= count;
            do {
                *++bytes = fill;
            } while (--count != 0);
        }
        if (fill != 0) {
            fill |= fill << 24 | fill << 16 | fill << 8;
        }
        words = (u32 *)(bytes + 1) - 1;
        count = length >> 5;
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
        count = (length & 31) >> 2;
        if (count != 0) {
            do {
                *++words = fill;
            } while (--count != 0);
        }
        bytes = (u8 *)(words + 1) - 1;
        length &= 3;
    }
    if (length != 0) {
        do {
            *++bytes = fill;
        } while (--length != 0);
    }
}
