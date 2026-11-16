/* The <float.h> limits that are not compile-time constants to the library: NaNs, infinities and
 * the extreme values, kept as raw IEEE bit patterns and read through casts (see fdlibm.h). The
 * linker keeps only the ones something refers to; these are the ones the game has, in order. */

/* Game sources still refer to these two by their address-derived names. */
#define __float_nan lbl_805EDCC8
#define __float_epsilon lbl_805EDCF0

unsigned long __float_nan[] = {0x7FFFFFFF};
unsigned long __float_huge[] = {0x7F800000};
unsigned long __double_max[] = {0x7FEFFFFF, 0xFFFFFFFF};
unsigned long __double_huge[] = {0x7FF00000, 0};
unsigned long __extended_min[] = {0x00100000, 0};
unsigned long __extended_max[] = {0x7FEFFFFF, 0xFFFFFFFF};
unsigned long __float_epsilon[] = {0x34000000};
