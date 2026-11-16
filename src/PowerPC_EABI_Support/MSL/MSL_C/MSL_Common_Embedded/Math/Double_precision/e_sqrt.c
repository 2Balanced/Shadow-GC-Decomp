/* sqrt(x), correctly rounded, by the bit-by-bit method on integers: the mantissa is held in two
 * words and the root is developed one bit per step, first the 21 bits of the high word, then the
 * 32 of the low word; the remainder decides the rounding. */

#include "fdlibm.h"

static const double one = 1.0;
static const double tiny = 1.0e-300;

double __ieee754_sqrt(double x) {
    double z;
    int sign = (int)0x80000000;
    unsigned int r, t1, s1, ix1, q1;
    int ix0, s0, q, m, t, i;

    ix0 = __HI(x);
    ix1 = __LO(x);

    if ((ix0 & 0x7FF00000) == 0x7FF00000) {
        errno = EDOM;
        return x * x + x; /* sqrt(NaN) = NaN, sqrt(+inf) = +inf, sqrt(-inf) = NaN */
    }
    if (ix0 <= 0) {
        if (((ix0 & (~sign)) | ix1) == 0) {
            return x; /* sqrt(+-0) = +-0 */
        } else if (ix0 < 0) {
            errno = EDOM;
            return NAN; /* sqrt of a negative number */
        }
    }

    /* normalise x */
    m = (ix0 >> 20);
    if (m == 0) { /* subnormal */
        while (ix0 == 0) {
            m -= 21;
            ix0 |= (ix1 >> 11);
            ix1 <<= 21;
        }
        for (i = 0; (ix0 & 0x00100000) == 0; i++) {
            ix0 <<= 1;
        }
        m -= i - 1;
        ix0 |= (ix1 >> (32 - i));
        ix1 <<= i;
    }
    m -= 1023; /* unbias the exponent */
    ix0 = (ix0 & 0x000FFFFF) | 0x00100000;
    if (m & 1) { /* odd exponent: double x to make it even */
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
    }
    m >>= 1; /* the root's exponent */

    /* develop the root bit by bit */
    ix0 += ix0 + ((ix1 & sign) >> 31);
    ix1 += ix1;
    q = q1 = s0 = s1 = 0; /* [q, q1] = sqrt(x) */
    r = 0x00200000;       /* the bit being decided */

    while (r != 0) {
        t = s0 + r;
        if (t <= ix0) {
            s0 = t + r;
            ix0 -= t;
            q += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    r = sign;
    while (r != 0) {
        t1 = s1 + r;
        t = s0;
        if ((t < ix0) || ((t == ix0) && (t1 <= ix1))) {
            s1 = t1 + r;
            if (((t1 & sign) == sign) && (s1 & sign) == 0) {
                s0 += 1;
            }
            ix0 -= t;
            if (ix1 < t1) {
                ix0 -= 1;
            }
            ix1 -= t1;
            q1 += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    /* a non-zero remainder: round according to the current rounding mode */
    if ((ix0 | ix1) != 0) {
        z = one - tiny; /* raises inexact */
        if (z >= one) {
            z = one + tiny;
            if (q1 == (unsigned int)0xFFFFFFFF) {
                q1 = 0;
                q += 1;
            } else if (z > one) {
                if (q1 == (unsigned int)0xFFFFFFFE) {
                    q += 1;
                }
                q1 += 2;
            } else {
                q1 += (q1 & 1);
            }
        }
    }
    ix0 = (q >> 1) + 0x3FE00000;
    ix1 = q1 >> 1;
    if ((q & 1) == 1) {
        ix1 |= sign;
    }
    ix0 += (m << 20);
    __HI(z) = ix0;
    __LO(z) = ix1;
    return z;
}
