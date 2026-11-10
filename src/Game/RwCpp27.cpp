/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1530;

extern "C" {
void fn_804647C0(void) {
}

void fn_804647C4(void) {
}

void fn_804647B0(u8 *base) {
    *(u32 *)(base + lbl_805F1530) = 0;
}
}
