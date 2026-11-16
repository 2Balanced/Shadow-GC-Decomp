/* fmod: the public entry point of the floating-point remainder. */

#include "fdlibm.h"

double fmod(double x, double y) {
    return __ieee754_fmod(x, y);
}
