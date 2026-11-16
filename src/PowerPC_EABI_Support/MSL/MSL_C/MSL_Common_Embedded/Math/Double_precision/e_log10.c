/* log10(x) = n*log10(2) + log10(x / 2^n), the second term through the natural logarithm. */

#include "fdlibm.h"

static const double two54 = 1.80143985094819840000e+16;
static const double ivln10 = 4.34294481903251816668e-01;
static const double log10_2hi = 3.01029995663611771306e-01;
static const double log10_2lo = 3.69423907715893078616e-13;

static double zero = 0.0;

double __ieee754_log10(double x) {
    double y, z;
    int i, k, hx;
    unsigned int lx;

    hx = __HI(x);
    lx = __LO(x);

    k = 0;
    if (hx < 0x00100000) { /* x < 2^-1022 */
        if (((hx & 0x7FFFFFFF) | lx) == 0) {
            errno = EDOM;
            return -two54 / zero; /* log10(+-0) = -infinity */
        }
        if (hx < 0) {
            errno = EDOM;
            return (x - x) / zero; /* log10 of a negative number is a NaN */
        }
        k -= 54;
        x *= two54; /* scale a subnormal up */
        hx = __HI(x);
    }
    if (hx >= 0x7FF00000) {
        return x + x;
    }

    k += (hx >> 20) - 1023;
    i = ((unsigned int)k & 0x80000000) >> 31;
    hx = (hx & 0x000FFFFF) | ((0x3FF - i) << 20);
    y = (double)(k + i);
    __HI(x) = hx;
    z = y * log10_2lo + ivln10 * __ieee754_log(x);
    return z + y * log10_2hi;
}
