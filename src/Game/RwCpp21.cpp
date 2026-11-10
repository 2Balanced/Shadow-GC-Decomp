/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_8045B79C(u32 *object, u32 index) {
    return ((u32 *)object[0])[index];
}

void fn_8045B788(u32 *object) {
    object[2] = 0;
    object[0] = 0;
    object[1] = 0;
}

u32 fn_8045B020(u32 *object) {
    u32 *nested = (u32 *)object[2];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

extern u8 *lbl_805F15B4;

void fn_8045BF30(u32 first) {
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(first, 0x30502);
}
}
