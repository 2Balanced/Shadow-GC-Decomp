/* cos(x): reduce the argument modulo pi/2 and evaluate the kernel the quadrant calls for. */

#include "fdlibm.h"

double cos(double x) {
    double y[2], z = 0.0;
    int n, ix;

    ix = __HI(x);
    ix &= 0x7FFFFFFF;
    if (ix <= 0x3FE921FB) { /* |x| <= pi/4 */
        return __kernel_cos(x, z);
    } else if (ix >= 0x7FF00000) {
        return x - x; /* infinity or NaN: NaN */
    } else {
        n = __ieee754_rem_pio2(x, y);
        switch (n & 3) {
        case 0:
            return __kernel_cos(y[0], y[1]);
        case 1:
            return -__kernel_sin(y[0], y[1], 1);
        case 2:
            return -__kernel_cos(y[0], y[1]);
        default:
            return __kernel_sin(y[0], y[1], 1);
        }
    }
}
