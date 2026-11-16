/* tan(x): reduce the argument modulo pi/2 and evaluate the kernel the quadrant calls for. */

#include "fdlibm.h"

double tan(double x) {
    double y[2], z = 0.0;
    int n, ix;

    ix = __HI(x);
    ix &= 0x7FFFFFFF;
    if (ix <= 0x3FE921FB) { /* |x| <= pi/4 */
        return __kernel_tan(x, z, 1);
    } else if (ix >= 0x7FF00000) {
        return x - x; /* infinity or NaN: NaN */
    } else {
        n = __ieee754_rem_pio2(x, y);
        /* 1 for an even quadrant, -1 (the negative reciprocal) for an odd one */
        return __kernel_tan(y[0], y[1], 1 - ((n & 1) << 1));
    }
}
