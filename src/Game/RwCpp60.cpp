/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1668;
extern u32 lbl_805F1670;

extern "C" {
u32 fn_80499520(u32 unused, u32 value) {
    lbl_805F1668 = value;
    return 1;
}

void fn_80499B24(u8 *base) {
    *(u32 *)(base + lbl_805F1670 + 0x20) = 0x1000000;
}
}
