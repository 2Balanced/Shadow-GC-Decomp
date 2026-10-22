/* PowerPC C++ runtime support. The compiler emits calls to __copy when it must copy an object
 * whose class has a trivial copy constructor but whose size is only known at run time. */

/* Byte-by-byte copy of `count` bytes; a null destination or a zero count copies nothing.
 * Returns the destination. */
extern "C" void *__copy(char *to, char *from, unsigned long count) {
    char *out;

    if (to != 0 && count != 0) {
        out = to;
        do {
            *out++ = *from++;
        } while (--count != 0);
    }
    return to;
}
