/* sin(x + y) for |x| <= pi/4, y being the tail of x (iy is 0 when y is exactly zero):
 * x + x^3*(S1 + x^2*S(x^2)), with y folded in to first order. */

#include "fdlibm.h"

static const double half = 5.00000000000000000000e-01;
static const double S1 = -1.66666666666666324348e-01;
static const double S2 = 8.33333333332248946124e-03;
static const double S3 = -1.98412698298579493134e-04;
static const double S4 = 2.75573137070700676789e-06;
static const double S5 = -2.50507602534068634195e-08;
static const double S6 = 1.58969099521155010221e-10;

double __kernel_sin(double x, double y, int iy) {
    double z, r, v;
    int ix;

    ix = __HI(x) & 0x7FFFFFFF;
    if (ix < 0x3E400000) { /* |x| < 2^-27 */
        if ((int)x == 0) {
            return x;
        }
    }
    z = x * x;
    v = z * x;
    r = S2 + z * (S3 + z * (S4 + z * (S5 + z * S6)));
    if (iy == 0) {
        return x + v * (S1 + z * r);
    } else {
        return x - ((z * (half * y - v * r) - y) - v * S1);
    }
}
