/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_80454494(void) {
    return 0x40;
}

u32 fn_804548E4(u32 *object) {
    return object[5];
}

u32 *fn_80454618(u32 *object) {
    ((u32 *)object[5])[0x10]++;
    return object;
}

u32 fn_80454B9C(u32 *object) {
    u32 *nested = (u32 *)object[0x1B];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

u32 fn_80454CBC(u32 *object) {
    u32 *nested = (u32 *)object[0x1F];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}
}
