#ifndef DOLPHIN_TYPES_H
#define DOLPHIN_TYPES_H

/* Sized scalar names in the style the Dolphin SDK uses. On the Gekko with MWCC, char is 8 bits,
 * short 16, int and long 32, long long 64. */

/* Unsigned and signed integers. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

/* Floating point. */
typedef float f32;
typedef double f64;

/* Volatile variants, for hardware registers and memory shared with interrupts. */
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile f32 vf32;
typedef volatile f64 vf64;

/* SDK-style truth values. */
typedef int BOOL;
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#define NULL 0
#endif

#endif
