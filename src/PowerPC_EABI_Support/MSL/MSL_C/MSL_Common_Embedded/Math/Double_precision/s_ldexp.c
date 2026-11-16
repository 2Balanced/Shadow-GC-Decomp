/* ldexp(x, n) = x * 2^n, by adjusting the exponent field, with gradual underflow. */

#include "fdlibm.h"

static const double two54 = 1.80143985094819840000e+16;
static const double twom54 = 5.55111512312578270212e-17;
static const double huge = 1.0e+300;
static const double tiny = 1.0e-300;

double ldexp(double x, int n) {
    int k, hx, lx;

    if (!isfinite(x) || x == 0.0) {
        return x;
    }

    hx = __HI(x);
    lx = __LO(x);
    k = (hx & 0x7FF00000) >> 20; /* exponent field */
    if (k == 0) { /* zero or subnormal */
        if ((lx | (hx & 0x7FFFFFFF)) == 0) {
            return x;
        }
        x *= two54;
        hx = __HI(x);
        k = ((hx & 0x7FF00000) >> 20) - 54;
        if (n < -50000) {
            return tiny * x; /* underflow */
        }
    }
    if (k == 0x7FF) {
        return x + x; /* infinity or NaN */
    }
    k = k + n;
    if (k > 0x7FE) {
        return huge * copysign(huge, x); /* overflow */
    }
    if (k > 0) { /* normal result */
        __HI(x) = (hx & 0x800FFFFF) | (k << 20);
        return x;
    }
    if (k <= -54) {
        if (n > 50000) { /* the sum wrapped around */
            return huge * copysign(huge, x);
        } else {
            return tiny * copysign(tiny, x); /* underflow */
        }
    }
    k += 54; /* subnormal result */
    __HI(x) = (hx & 0x800FFFFF) | (k << 20);
    return x * twom54;
}
