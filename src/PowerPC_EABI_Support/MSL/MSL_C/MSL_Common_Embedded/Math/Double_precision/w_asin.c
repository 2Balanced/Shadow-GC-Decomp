/* asin: the public entry point of the arc sine. */

#include "fdlibm.h"

double asin(double x) {
    return __ieee754_asin(x);
}
