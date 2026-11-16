/* pow: the public entry point of the power function. */

#include "fdlibm.h"

double pow(double x, double y) {
    return __ieee754_pow(x, y);
}
