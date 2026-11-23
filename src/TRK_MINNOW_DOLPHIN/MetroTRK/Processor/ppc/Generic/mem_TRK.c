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
 * element so the stores can pre-increment.
 *
 * The algorithm is the runtime's __fill_mem, but the nub's copy was written separately: retail
 * keeps the word pointer in r3 and the two word counters in r4, where __fill_mem has them the other
 * way round. That takes the word pointer declared first, a counter of its own for each of the three
 * loops (the third declared after the fill value) and the two word counts assigned inside their
 * tests; with one shared counter, or the counts assigned in statements of their own, GC/2.7 gives
 * __fill_mem's registers or adds a register copy. The pointer is stepped back in a statement of
 * its own: one statement fewer and GC/2.7 inlines the function into TRK_memset, which retail's
 * calls it. */
void TRK_fill_mem(void *dest, int value, TRKSize length) {
    u32 *words;
    u8 *bytes;
    u32 head;
    u32 blocks;
    u32 fill;
    u32 rest;

    fill = (u8)value;
    bytes = (u8 *)dest;
    bytes--;
    if (length >= 32) {
        head = ~(u32)bytes & 3;
        if (head != 0) {
            length -= head;
            do {
                *++bytes = fill;
            } while (--head != 0);
        }
        if (fill != 0) {
            fill |= fill << 24 | fill << 16 | fill << 8;
        }
        words = (u32 *)(bytes + 1) - 1;
        if ((blocks = length >> 5) != 0) {
            do {
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
                *++words = fill;
            } while (--blocks != 0);
        }
        if ((rest = (length & 31) >> 2) != 0) {
            do {
                *++words = fill;
            } while (--rest != 0);
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
