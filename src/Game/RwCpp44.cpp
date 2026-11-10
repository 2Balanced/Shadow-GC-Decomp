/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805EDEA0;
extern u8 *lbl_805F15B4;

extern "C" {
void fn_8047FA28(u32 value) {
    lbl_805EDEA0 = value;
}

u8 *fn_80480530(void) {
    return lbl_805F15B4 + 0x108;
}
}
