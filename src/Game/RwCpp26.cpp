/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805EDDD8;

extern "C" {
void fn_804631D4(void) {
}

void fn_80462C20(u32 value) {
    lbl_805EDDD8 = value;
}

u32 fn_80462C28(void) {
    return lbl_805EDDD8;
}

void fn_80462FA4(u8 *object, u8 value) {
    object[0x0A] = value;
}

u32 fn_804631CC(void) {
    return 1;
}
}
