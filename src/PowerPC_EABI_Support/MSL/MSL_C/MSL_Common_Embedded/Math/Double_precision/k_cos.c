/* cos(x + y) for |x| <= pi/4, y being the tail of x: 1 - x^2/2 + x^4*C(x^2), with the subtraction
 * arranged (through an exactly representable qx near x^2/2) so that no accuracy is lost when x is
 * not small. */

#include "fdlibm.h"

static const double one = 1.00000000000000000000e+00;
static const double C1 = 4.16666666666666019037e-02;
static const double C2 = -1.38888888888741095749e-03;
static const double C3 = 2.48015872894767294178e-05;
static const double C4 = -2.75573143513906633035e-07;
static const double C5 = 2.08757232129817482790e-09;
static const double C6 = -1.13596475577881948265e-11;

double __kernel_cos(double x, double y) {
    double a, hz, z, r, qx;
    int ix;

    ix = __HI(x) & 0x7FFFFFFF;
    if (ix < 0x3E400000) { /* |x| < 2^-27 */
        if (((int)x) == 0) {
            return one;
        }
    }
    z = x * x;
    r = z * (C1 + z * (C2 + z * (C3 + z * (C4 + z * (C5 + z * C6)))));
    if (ix < 0x3FD33333) { /* |x| < 0.3 */
        return one - (0.5 * z - (z * r - x * y));
    } else {
        if (ix > 0x3FE90000) { /* |x| > 0.78125 */
            qx = 0.28125;
        } else {
            __HI(qx) = ix - 0x00200000; /* x / 4 */
            __LO(qx) = 0;
        }
        hz = 0.5 * z - qx;
        a = one - qx;
        return a - (hz - (z * r - x * y));
    }
}
