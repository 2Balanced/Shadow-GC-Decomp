/* modf(x, &i): the fractional part of x, with the integral part stored through the pointer;
 * both have the sign of x. */

#include "fdlibm.h"

static const double one = 1.0;

double modf(double x, double *iptr) {
    int i0, i1, j0;
    unsigned int i;

    i0 = __HI(x);
    i1 = __LO(x);
    j0 = ((i0 >> 20) & 0x7FF) - 0x3FF; /* unbiased exponent */
    if (j0 < 20) { /* the fraction starts in the high word */
        if (j0 < 0) { /* |x| < 1: all fraction */
            __HI(*iptr) = i0 & 0x80000000;
            __LO(*iptr) = 0;
            return x;
        } else {
            i = (0x000FFFFF) >> j0;
            if (((i0 & i) | i1) == 0) { /* x is integral */
                *iptr = x;
                __HI(x) &= 0x80000000;
                __LO(x) = 0;
                return x;
            } else {
                __HI(*iptr) = i0 & (~i);
                __LO(*iptr) = 0;
                return x - *iptr;
            }
        }
    } else if (j0 > 51) { /* no fraction */
        *iptr = x * one;
        __HI(x) &= 0x80000000;
        __LO(x) = 0;
        return x;
    } else { /* the fraction is in the low word */
        i = ((unsigned int)(0xFFFFFFFF)) >> (j0 - 20);
        if ((i1 & i) == 0) { /* x is integral */
            *iptr = x;
            __HI(x) &= 0x80000000;
            __LO(x) = 0;
            return x;
        } else {
            __HI(*iptr) = i0;
            __LO(*iptr) = i1 & (~i);
            return x - *iptr;
        }
    }
}
