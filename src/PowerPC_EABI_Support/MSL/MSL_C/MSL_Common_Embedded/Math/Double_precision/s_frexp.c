/* frexp(x, &e): split x into a fraction in [0.5, 1) and a power of two. */

#include "fdlibm.h"

static const double two54 = 1.80143985094819840000e+16;

double frexp(double x, int *eptr) {
    int hx, ix, lx;

    hx = __HI(x);
    ix = 0x7FFFFFFF & hx;
    lx = __LO(x);
    *eptr = 0;
    if (ix >= 0x7FF00000 || ((ix | lx) == 0)) {
        return x; /* zero, infinity or NaN */
    }
    if (ix < 0x00100000) { /* subnormal: scale up first */
        x *= two54;
        hx = __HI(x);
        ix = hx & 0x7FFFFFFF;
        *eptr = -54;
    }
    *eptr += (ix >> 20) - 1022;
    hx = (hx & 0x800FFFFF) | 0x3FE00000;
    __HI(x) = hx;
    return x;
}
