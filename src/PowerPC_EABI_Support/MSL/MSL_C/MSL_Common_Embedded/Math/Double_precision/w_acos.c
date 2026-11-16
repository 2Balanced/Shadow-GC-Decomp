/* acos: the public entry point of the arc cosine. */

#include "fdlibm.h"

double acos(double x) {
    return __ieee754_acos(x);
}
