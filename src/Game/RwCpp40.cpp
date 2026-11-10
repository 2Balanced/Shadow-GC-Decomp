/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_804790C4(u32 *object) {
    return object[5];
}

void fn_804790CC(u32 *object, u32 value) {
    object[5] = value;
}

void fn_804799B8(u32 *object, u32 value) {
    object[4] = value;
}

u32 fn_804799C0(u32 *object) {
    return object[4];
}
}
