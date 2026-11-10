/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1430;

extern "C" {
void fn_80443FE0(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F1430);
    target[0] = 0;
    target[1] = 0;
    target[2] = 0;
}
}
