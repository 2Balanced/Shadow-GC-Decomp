/* copysign(x, y): x with the sign of y. */

#include "fdlibm.h"

double copysign(double x, double y) {
    __HI(x) = (__HI(x) & 0x7FFFFFFF) | (__HI(y) & 0x80000000);
    return x;
}
