/* log: the public entry point of the natural logarithm. */

#include "fdlibm.h"

double log(double x) {
    return __ieee754_log(x);
}
