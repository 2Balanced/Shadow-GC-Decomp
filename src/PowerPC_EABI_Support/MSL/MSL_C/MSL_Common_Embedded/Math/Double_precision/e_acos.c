/* acos(x), by a rational approximation R(x^2) of (asin(x) - x) / x^3 on |x| < 0.5 and the
 * half-angle identities outside it:
 *     |x| < 0.5:  acos(x) = pi/2 - (x + x*x^2*R(x^2))
 *     x < -0.5:   acos(x) = pi - 2*asin(sqrt((1+x)/2))
 *     x > 0.5:    acos(x) = 2*asin(sqrt((1-x)/2)), with the square root split into a high part
 *                 and a correction so no accuracy is lost. */

#include "fdlibm.h"

static const double one = 1.00000000000000000000e+00;
static const double pi = 3.14159265358979311600e+00;
static const double pio2_hi = 1.57079632679489655800e+00;
static const double pio2_lo = 6.12323399573676603587e-17;
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

double __ieee754_acos(double x) {
    double z, p, q, r, w, s, c, df;
    int hx, ix;

    hx = __HI(x);
    ix = hx & 0x7FFFFFFF;

    if (ix >= 0x3FF00000) { /* |x| >= 1 */
        if (((ix - 0x3FF00000) | __LO(x)) == 0) {
            if (hx > 0) {
                return 0.0; /* acos(1) */
            } else {
                return pi + 2.0 * pio2_lo; /* acos(-1) */
            }
        }
        return NAN; /* outside the domain */
    }

    if (ix < 0x3FE00000) { /* |x| < 0.5 */
        if (ix <= 0x3C600000) {
            return pio2_hi + pio2_lo; /* x is negligible against pi/2 */
        }
        z = x * x;
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        r = p / q;
        return pio2_hi - (x - (pio2_lo - x * r));
    } else if (hx < 0) { /* x < -0.5 */
        z = (one + x) * 0.5;
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        s = sqrt(z);
        r = p / q;
        w = r * s - pio2_lo;
        return pi - 2.0 * (s + w);
    } else { /* x > 0.5 */
        z = (one - x) * 0.5;
        s = sqrt(z);
        df = s;
        __LO(df) = 0;
        c = (z - df * df) / (s + df);
        p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
        q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
        r = p / q;
        w = r * s + c;
        return 2.0 * (df + w);
    }
}
