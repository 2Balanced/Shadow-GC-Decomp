/* PowerPC-specific C++ support: __copy, the byte copy the compiler calls to copy an object of a
 * class with a trivial copy constructor whose size isn't known at compile time. */
extern "C" void *__copy(char *dst, char *src, unsigned long size) {
    char *p;

    if (dst && size) {
        p = dst;
        do {
            *p = *src;
            ++p;
            ++src;
            --size;
        } while (size);
    }
    return dst;
}
