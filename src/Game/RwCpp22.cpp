/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern s16 *lbl_805F15B4;
extern u32 lbl_805F14F0;

extern "C" {
void fn_8045CA20(void) {
    s16 *target = (s16 *)((u8 *)lbl_805F15B4 + lbl_805F14F0);
    *target += 1;
}
}
