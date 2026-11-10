/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1440;
extern u32 lbl_805F1448;

extern "C" {
u32 fn_8044758C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440);
}

u32 fn_8044757C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440 + 4);
}

void fn_80447598(u8 *base) {
    *(u32 *)(base + lbl_805F1448) = 0;
}
}
