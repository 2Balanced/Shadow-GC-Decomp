/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14F8;

extern "C" {
void fn_8045F2A0(void) {
    (&lbl_805F14F8)[1]++;
}

void fn_8045F2B4(void) {
    (&lbl_805F14F8)[1]--;
}
}
