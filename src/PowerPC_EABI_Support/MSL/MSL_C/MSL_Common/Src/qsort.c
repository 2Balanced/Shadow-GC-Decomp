/* qsort: an in-place heap sort, so it needs no stack or scratch memory and is O(n log n) in the
 * worst case. The table is treated as a 1-based heap: element i has children 2i and 2i+1. */

/* Sources elsewhere in the project still call this function by its address-derived name. */
#define qsort fn_803AB44C

typedef unsigned long size_t;
typedef int (*CompareFn)(const void *, const void *);

#define ELEMENT(i) ((char *)base + size * ((i) - 1))

/* Exchange two elements a byte at a time. */
#define EXCHANGE(a, b)                                                                             \
    do {                                                                                           \
        char *from = (char *)(b) - 1;                                                              \
        char *to = (char *)(a) - 1;                                                                \
        size_t left = size + 1;                                                                    \
        long saved;                                                                                \
        while (--left != 0) {                                                                      \
            saved = *++to;                                                                         \
            *to = *++from;                                                                         \
            *from = saved;                                                                         \
        }                                                                                          \
    } while (0)

void qsort(void *base, size_t count, size_t size, CompareFn compare) {
    size_t first;  /* the heap is built down from here; once it is 1 the heap is complete */
    size_t last;   /* end of the heap; everything above it is sorted */
    size_t child;
    char *firstPtr;
    char *lastPtr;
    char *parentPtr;
    char *childPtr;
    char *siblingPtr;

    if (count < 2) {
        return;
    }

    last = count;
    first = count / 2 + 1;
    firstPtr = ELEMENT(first);
    lastPtr = ELEMENT(last);

    for (;;) {
        if (first > 1) {
            /* Still building the heap: take in one more element. */
            first--;
            firstPtr -= size;
        } else {
            /* Move the largest element behind the heap and shrink the heap. */
            EXCHANGE(firstPtr, lastPtr);
            if (--last == 1) {
                return;
            }
            lastPtr -= size;
        }

        /* Sift the element at `first` down to its place. */
        child = first;
        childPtr = ELEMENT(child);
        while (child * 2 <= last) {
            child *= 2;
            parentPtr = childPtr;
            childPtr = ELEMENT(child);
            if (child < last) {
                siblingPtr = childPtr + size;
                if (compare(childPtr, siblingPtr) < 0) {
                    childPtr = siblingPtr;
                    child++;
                }
            }
            if (compare(parentPtr, childPtr) < 0) {
                EXCHANGE(parentPtr, childPtr);
            } else {
                break;
            }
        }
    }
}
