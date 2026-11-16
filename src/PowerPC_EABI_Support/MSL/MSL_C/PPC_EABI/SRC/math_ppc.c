/* Single-precision entry points that go through the double-precision functions. */

/* Sources elsewhere in the project still call these functions by their address-derived names. */
#define cosf fn_803B1F24
#define powf fn_803B1F00

double cos(double x);
double pow(double x, double y);

float cosf(float x) {
    return cos(x);
}

float powf(float x, float y) {
    return pow(x, y);
}
