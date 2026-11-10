/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_8047E03C(u32 *object) {
    return object[1];
}

u32 fn_8047E0E0(u32 *object) {
    return object[0];
}

void fn_8047DDB0(u32 *object) {
    object[1] = 0;
}
}
