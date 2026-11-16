/* Integer absolute values. */

#include "msl_internal.h"

int abs(int n) {
    if (n < 0) {
        return -n;
    }
    return n;
}

long labs(long n) {
    if (n < 0) {
        return -n;
    }
    return n;
}
