/* tan(x + y) for |x| <= pi/4, y being the tail of x; with iy = -1 the result is -1/tan(x + y).
 * Above 0.67434 the argument is reflected through pi/4 so the odd polynomial stays accurate. */

#include "fdlibm.h"

static const double one = 1.00000000000000000000e+00;
static const double pio4 = 7.85398163397448278999e-01;
static const double pio4lo = 3.06161699786838301793e-17;
static const double T[] = {
    3.33333333333334091986e-01,
    1.33333333333201242699e-01,
    5.39682539762260521377e-02,
    2.18694882948595424599e-02,
    8.86323982359930005737e-03,
    3.59207910759131235356e-03,
    1.45620945432529025516e-03,
    5.88041240820264096874e-04,
    2.46463134818469906812e-04,
    7.81794442939557092300e-05,
    7.14072491382608190305e-05,
    -1.85586374855275456654e-05,
    2.59073051863633712884e-05,
};

double __kernel_tan(double x, double y, int iy) {
    double z, r, v, w, s;
    int ix, hx;

    hx = __HI(x);
    ix = hx & 0x7FFFFFFF;
    if (ix < 0x3E300000) { /* |x| < 2^-28 */
        if ((int)x == 0) {
            if (((ix | __LO(x)) | (iy + 1)) == 0) {
                return one / fabs(x);
            } else {
                return (iy == 1) ? x : -one / x;
            }
        }
    }
    if (ix >= 0x3FE59428) { /* |x| >= 0.6744 */
        if (hx < 0) {
            x = -x;
            y = -y;
        }
        z = pio4 - x;
        w = pio4lo - y;
        x = z + w;
        y = 0.0;
    }
    z = x * x;
    w = z * z;
    /* odd and even terms separately, for speed */
    r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
    v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
    s = z * x;
    r = y + z * (s * (r + v) + y);
    r += T[0] * s;
    w = x + r;
    if (ix >= 0x3FE59428) {
        v = (double)iy;
        return (double)(1 - ((hx >> 30) & 2)) * (v - 2.0 * (x - (w * w / (w + v) - r)));
    }
    if (iy == 1) {
        return w;
    } else {
        /* -1 / (x + r), computed accurately */
        double a, t;

        z = w;
        __LO(z) = 0;
        v = r - (z - x);  /* z + v = r + x */
        t = a = -1.0 / w; /* a = -1 / w */
        __LO(t) = 0;
        s = 1.0 + t * z;
        return t + a * (s + t * v);
    }
}
