#ifndef MSL_FDLIBM_H
#define MSL_FDLIBM_H

/* Shared declarations of the double-precision math library (an fdlibm derivative). The functions
 * work on the two 32-bit halves of an IEEE double; on the big-endian PowerPC the high half (sign,
 * exponent, top of the mantissa) comes first. */

#define __HI(x) (*(int *)&(x))
#define __LO(x) (*(1 + (int *)&(x)))

/* float.c keeps the float.h limits as raw bit patterns. (Game sources still refer to the NaN by
 * its address-derived name.) */
#define __float_nan lbl_805EDCC8
extern unsigned long __float_nan[];
extern unsigned long __float_huge[];
#define NAN (*(float *)__float_nan)
#define INFINITY (*(float *)__float_huge)

extern int errno;
#define EDOM 0x21
#define ERANGE 0x22

/* Game sources still call these functions by address-derived names, so those are the names they
 * are built under. */
#define atan fn_803B11C0
#define floor fn_803B1618
#define tan fn_803B1B84
#define acos fn_803B1BFC
#define atan2 fn_803B1C3C
#define log10 fn_803B1C9C

/* Number classes, as <math.h> numbers them. */
#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline int __fpclassifyd(double x) {
    switch (__HI(x) & 0x7FF00000) {
    case 0x7FF00000:
        if ((__HI(x) & 0x000FFFFF) || (__LO(x) & 0xFFFFFFFF)) {
            return FP_NAN;
        } else {
            return FP_INFINITE;
        }
    case 0:
        if ((__HI(x) & 0x000FFFFF) || (__LO(x) & 0xFFFFFFFF)) {
            return FP_SUBNORMAL;
        } else {
            return FP_ZERO;
        }
    }
    return FP_NORMAL;
}

#define isfinite(x) (__fpclassifyd(x) > FP_INFINITE)

double __ieee754_acos(double x);
double __ieee754_asin(double x);
double __ieee754_atan2(double y, double x);
double __ieee754_fmod(double x, double y);
double __ieee754_log(double x);
double __ieee754_log10(double x);
double __ieee754_pow(double x, double y);
int __ieee754_rem_pio2(double x, double *y);
double __ieee754_sqrt(double x);

double __kernel_cos(double x, double y);
double __kernel_sin(double x, double y, int iy);
double __kernel_tan(double x, double y, int iy);
int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec, const int *ipio2);

double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double fmod(double x, double y);
double log(double x);
double log10(double x);
double pow(double x, double y);
double cos(double x);
double copysign(double x, double y);
double floor(double x);
double ldexp(double x, int n);
double sqrt(double x);

static inline double fabs(double x) { return __fabs(x); }

#endif
