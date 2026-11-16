/* log(x). x is reduced to 2^k * (1 + f) with sqrt(2)/2 < 1 + f < sqrt(2); with s = f / (2 + f),
 * log(1 + f) = 2s + s*R(s^2), R a degree-14 polynomial, and log(x) = k*ln2 + log(1 + f) with ln2
 * split in two so that k*ln2_hi is exact. */

#include "fdlibm.h"

static const double ln2_hi = 6.93147180369123816490e-01;
static const double ln2_lo = 1.90821492927058770002e-10;
static const double two54 = 1.80143985094819840000e+16;
static const double Lg1 = 6.666666666666735130e-01;
static const double Lg2 = 3.999999999940941908e-01;
static const double Lg3 = 2.857142874366239149e-01;
static const double Lg4 = 2.222219843214978396e-01;
static const double Lg5 = 1.818357216161805012e-01;
static const double Lg6 = 1.531383769920937332e-01;
static const double Lg7 = 1.479819860511658591e-01;

static double zero = 0.0;

double __ieee754_log(double x) {
    double hfsq, f, s, z, R, w, t1, t2, dk;
    int k, hx, i, j;
    unsigned int lx;

    hx = __HI(x);
    lx = __LO(x);

    k = 0;
    if (hx < 0x00100000) { /* x < 2^-1022 */
        if (((hx & 0x7FFFFFFF) | lx) == 0) {
            return -two54 / zero; /* log(+-0) = -infinity */
        }
        if (hx < 0) {
            errno = EDOM;
            return (x - x) / zero; /* log of a negative number is a NaN */
        }
        k -= 54;
        x *= two54; /* scale a subnormal up */
        hx = __HI(x);
    }
    if (hx >= 0x7FF00000) {
        return x + x;
    }

    k += (hx >> 20) - 1023;
    hx &= 0x000FFFFF;
    i = (hx + 0x95F64) & 0x100000;
    __HI(x) = hx | (i ^ 0x3FF00000); /* normalise x or x/2 */
    k += (i >> 20);
    f = x - 1.0;

    if ((0x000FFFFF & (2 + hx)) < 3) { /* |f| < 2^-20 */
        if (f == zero) {
            if (k == 0) {
                return zero;
            } else {
                dk = (double)k;
                return dk * ln2_hi + dk * ln2_lo;
            }
        }
        R = f * f * (0.5 - 0.33333333333333333 * f);
        if (k == 0) {
            return f - R;
        } else {
            dk = (double)k;
            return dk * ln2_hi - ((R - dk * ln2_lo) - f);
        }
    }

    s = f / (2.0 + f);
    dk = (double)k;
    z = s * s;
    i = hx - 0x6147A;
    w = z * z;
    j = 0x6B851 - hx;
    t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
    t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
    i |= j;
    R = t2 + t1;
    if (i > 0) {
        hfsq = 0.5 * f * f;
        if (k == 0) {
            return f - (hfsq - s * (hfsq + R));
        } else {
            return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
        }
    } else {
        if (k == 0) {
            return f - s * (f - R);
        } else {
            return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
        }
    }
}
