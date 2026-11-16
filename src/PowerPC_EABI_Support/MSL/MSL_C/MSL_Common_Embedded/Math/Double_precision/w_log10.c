/* log10: the public entry point of the base-10 logarithm. */

#include "fdlibm.h"

double log10(double x) {
    return __ieee754_log10(x);
}
