/* RenderWare callbacks with no observable work. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_80481D8C(void) {
}

void fn_80481D90(void) {
}

void fn_80481D94(void) {
}

u32 fn_80481D98(u32 *object, u32 value) {
    object = (u32 *)object[4];
    while (object != NULL) {
        if (object[2] == value) {
            return object[0];
        }
        object = (u32 *)object[0x0C];
    }
    return 0xFFFFFFFF;
}

float fn_80482340(float);

float fn_80482300(const float *object) {
    float x = object[0];
    float y = object[1];
    float z = object[2];
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    return fn_80482340(xx + yy + zz);
}
}
