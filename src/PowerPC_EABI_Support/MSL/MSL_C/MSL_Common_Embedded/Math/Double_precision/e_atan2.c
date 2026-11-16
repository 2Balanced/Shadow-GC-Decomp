/* atan2(y, x): the angle of the point (x, y), in (-pi, pi]. The special cases (zeros, infinities,
 * NaNs) are answered directly; otherwise atan(|y/x|) is moved into the right quadrant. */

#include "fdlibm.h"

static const double tiny = 1.0e-300;
static const double zero = 0.0;
static const double pi_o_4 = 7.8539816339744827900e-01;
static const double pi_o_2 = 1.5707963267948965580e+00;
static const double pi = 3.1415926535897931160e+00;
static const double pi_lo = 1.2246467991473531772e-16;

double __ieee754_atan2(double y, double x) {
    double z;
    int k, m, hx, hy, ix, iy;
    unsigned int lx, ly;

    hx = __HI(x);
    ix = hx & 0x7FFFFFFF;
    lx = __LO(x);
    hy = __HI(y);
    iy = hy & 0x7FFFFFFF;
    ly = __LO(y);

    if (((ix | ((lx | -lx) >> 31)) > 0x7FF00000) || ((iy | ((ly | -ly) >> 31)) > 0x7FF00000)) {
        return x + y; /* a NaN operand */
    }
    if (((hx - 0x3FF00000) | lx) == 0) {
        return atan(y); /* x is 1 */
    }

    /* Bit 0: y is negative. Bit 1: x is negative. */
    m = ((hy >> 31) & 1) | ((hx >> 30) & 2);

    if ((iy | ly) == 0) { /* y is zero */
        switch (m) {
        case 0:
        case 1:
            return y;
        case 2:
            return pi + tiny;
        case 3:
            return -pi - tiny;
        }
    }

    if ((ix | lx) == 0) { /* x is zero */
        return (hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny;
    }

    if (ix == 0x7FF00000) { /* x is infinite */
        if (iy == 0x7FF00000) {
            switch (m) {
            case 0:
                return pi_o_4 + tiny;
            case 1:
                return -pi_o_4 - tiny;
            case 2:
                return 3.0 * pi_o_4 + tiny;
            case 3:
                return -3.0 * pi_o_4 - tiny;
            }
        } else {
            switch (m) {
            case 0:
                return zero;
            case 1:
                return -zero;
            case 2:
                return pi + tiny;
            case 3:
                return -pi - tiny;
            }
        }
    }

    if (iy == 0x7FF00000) { /* y is infinite */
        return (hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny;
    }

    k = (iy - ix) >> 20;
    if (k > 60) {
        z = pi_o_2 + 0.5 * pi_lo; /* |y/x| is huge */
    } else if (hx < 0 && k < -60) {
        z = 0.0; /* |y/x| is tiny and x is negative */
    } else {
        z = atan(fabs(y / x));
    }

    switch (m) {
    case 0:
        return z;
    case 1:
        __HI(z) ^= 0x80000000;
        return z;
    case 2:
        return pi - (z - pi_lo);
    default:
        return (z - pi_lo) - pi;
    }
}
