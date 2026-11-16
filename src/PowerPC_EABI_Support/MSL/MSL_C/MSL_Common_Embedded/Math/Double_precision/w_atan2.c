/* atan2: the public entry point of the two-argument arc tangent. */

#include "fdlibm.h"

double atan2(double y, double x) {
    return __ieee754_atan2(y, x);
}
