/* Registers the stream cleanup that exit() runs. */

#include "msl_internal.h"

extern void (*__stdio_exit)(void);

void __stdio_atexit(void) {
    __stdio_exit = __close_all;
}
