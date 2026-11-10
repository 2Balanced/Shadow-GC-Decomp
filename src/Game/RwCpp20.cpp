/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14D0;

extern "C" {
void fn_8045984C(s16 *object) {
    object[7] = object[7] + 1;
}

void fn_80458D10(void) {
    (&lbl_805F14D0)[1]--;
}
}
