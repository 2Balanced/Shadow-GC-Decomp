/* Dolphin SDK DSP: the debug print hook. Release builds compile it to nothing. */
#include <dolphin/dsp.h>

void __DSP_debug_printf(const char *fmt, ...) {}
