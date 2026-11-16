/* asin(x). On |x| < 0.5 it is x + x*x^2*R(x^2) with the same rational approximation as acos;
 * otherwise asin(x) = pi/2 - 2*asin(sqrt((1-|x|)/2)), evaluated with the square root split into
 * a high part and a correction when the result is not close to pi/2. */

#include "fdlibm.h"

static const double one = 1.00000000000000000000e+00;
static const double huge = 1.000e+300;
static const double pio2_hi = 1.57079632679489655800e+00;
static const double pio2_lo = 6.12323399573676603587e-17;
static const double pio4_hi = 7.85398163397448278999e-01;
static const double pS0 = 1.66666666666666657415e-01;
static const double pS1 = -3.25565818622400915405e-01;
static const double pS2 = 2.01212532134862925881e-01;
static const double pS3 = -4.00555345006794114027e-02;
static const double pS4 = 7.91534994289814532176e-04;
static const double pS5 = 3.47933107596021167570e-05;
static const double qS1 = -2.40339491173441421878e+00;
static const double qS2 = 2.02094576023350569471e+00;
static const double qS3 = -6.88283971605453293030e-01;
static const double qS4 = 7.70381505559019352791e-02;

double __ieee754_asin(double x) {
    double t, w, p, q, c, r, s;
    int hx, ix;

    hx = __HI(x);
    ix = hx & 0x7FFFFFFF;

    if (ix >= 0x3FF00000) { /* |x| >= 1 */
        if (((ix - 0x3FF00000) | __LO(x)) == 0) {
            return x * pio2_hi + x * pio2_lo; /* asin(+-1) = +-pi/2 */
        }
        return NAN; /* outside the domain */
    } else if (ix < 0x3FE00000) { /* |x| < 0.5 */
        if (ix < 0x3E400000) {
            if (huge + x > one) {
                return x; /* x is its own arc sine, to working precision */
            }
        } else {
            t = x * x;
        }
        /* (When the test above fails, t is used here without having been set: the library's
         * source binds only the squaring to the else.) */
        p = t * (pS0 + t * (pS1 + t * (pS2 + t * (pS3 + t * (pS4 + t * pS5)))));
        q = one + t * (qS1 + t * (qS2 + t * (qS3 + t * qS4)));
        w = p / q;
        return x + x * w;
    }

    /* 0.5 <= |x| < 1 */
    w = one - fabs(x);
    t = w * 0.5;
    p = t * (pS0 + t * (pS1 + t * (pS2 + t * (pS3 + t * (pS4 + t * pS5)))));
    q = one + t * (qS1 + t * (qS2 + t * (qS3 + t * qS4)));
    s = sqrt(t);
    if (ix >= 0x3FEF3333) { /* |x| > 0.975 */
        w = p / q;
        t = pio2_hi - (2.0 * (s + s * w) - pio2_lo);
    } else {
        w = s;
        __LO(w) = 0;
        c = (t - w * w) / (s + w);
        r = p / q;
        p = 2.0 * s * r - (pio2_lo - 2.0 * c);
        q = pio4_hi - 2.0 * w;
        t = pio4_hi - (p - q);
    }
    if (hx > 0) {
        return t;
    } else {
        return -t;
    }
}
