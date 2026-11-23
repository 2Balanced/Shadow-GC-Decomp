/* RenderWare PowerPipe: a key sort, 0x8049F3F8..0x8049F8AC.  fn_8049F5B4 sorts an array of
 * records by an unsigned word at a given offset: a radix exchange sort (fn_8049F3F8) on the key's
 * bits down to runs of five, then an insertion sort behind a sentinel.  Its only caller is the
 * dependency chase (fn_8049E54C), which is why it sits among the PowerPipe files; it has no data,
 * and the module it belongs to has no public name we know of, hence the descriptive file name.
 * Status: fn_8049F3F8 matches; fn_8049F5B4's swap loop gets other scratch registers. */
#include <dolphin/types.h>

#define SORT_KEY(element, keyOffset) (*(u32 *)((u8 *)(element) + (keyOffset)))

/* Exchanges two elements a word at a time. */
#define SORT_SWAP(first, second, size) \
    do { \
        u32 *a = (u32 *)(first); \
        u32 *b = (u32 *)(second); \
        u32 left; \
        for (left = (size); left >= 4; left -= 4) { \
            u32 word = *b; \
            *b = *a; \
            *a = word; \
            a++; \
            b++; \
        } \
    } while (0)

typedef struct SortRange {
    u8 *low;
    u8 *high;
    u32 bit;
} SortRange;

/* Radix exchange sort on the key's bits from `bit` down; ranges shorter than five elements are
 * left for the insertion sort of the caller. */
void fn_8049F3F8(u8 *low, u8 *high, u32 size, u32 keyOffset, u32 bit) {
    SortRange stack[32];
    SortRange *top = stack;
    u8 *first;
    u8 *last;

    top->low = low;
    top->high = high;
    top->bit = bit;
    top++;
    do {
        top--;
        low = top->low;
        high = top->high;
        bit = top->bit;
        for (;;) {
            first = low;
            last = high;
            for (;;) {
                while ((SORT_KEY(low, keyOffset) & bit) == 0) {
                    low += size;
                    if (low > high) goto partitioned;
                }
                while ((SORT_KEY(high, keyOffset) & bit) != 0) {
                    high -= size;
                    if (low > high) goto partitioned;
                }
                SORT_SWAP(low, high, size);
                low += size;
                high -= size;
                if (low > high) break;
            }
        partitioned:
            bit >>= 1;
            if (bit == 0) break;
            if (last >= high + size + size * 5) {
                top->low = high + size;
                top->high = last;
                top->bit = bit;
                top++;
            }
            high = low - size;
            low = first;
            if (high < first + size * 5) break;
        }
    } while (top != stack);
}

/* Sorts `count` elements of `size` bytes by the unsigned word at `keyOffset`; the keys lie in
 * [minKey, maxKey]. */
void fn_8049F5B4(u8 *base, u32 count, u32 size, u32 keyOffset, u32 minKey, u32 maxKey) {
    u32 index;
    u32 bestKey;
    u32 best;
    u32 remaining;
    u8 *element;
    u32 key;
    u8 *slot;

    if (base == NULL) return;
    if (keyOffset + 4 > size) return;
    if (minKey >= maxKey) return;
    if (count > 5) {
        s32 top;
        if (maxKey != 0) {
            top = 0;
            while ((maxKey >>= 1) != 0) top++;
        } else {
            top = -1;
        }
        fn_8049F3F8(base, base + (count - 1) * size, size, keyOffset, 1 << top);
    }
    if (count <= 1) return;

    /* the smallest of the first five goes to the front as the sentinel */
    index = 4;
    if (count - 1 < 4) index = count - 1;
    best = index;
    bestKey = SORT_KEY(base + index * size, keyOffset);
    index--;
    do {
        key = SORT_KEY(base + index * size, keyOffset);
        if (key < bestKey) {
            bestKey = key;
            best = index;
        }
    } while (index-- != 0);
    if (best != 0) {
        SORT_SWAP(base, base + best * size, size);
    }

    remaining = count;
    element = base;
    while (--remaining != 0) {
        element += size;
        key = SORT_KEY(element, keyOffset);
        slot = element;
        for (slot -= size; SORT_KEY(slot, keyOffset) > key; slot -= size) {
            SORT_SWAP(slot, slot + size, size);
        }
    }
}
