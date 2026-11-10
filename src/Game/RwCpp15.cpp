/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1490;

extern "C" {
void fn_80451F48(void) {
    (&lbl_805F1490)[1]++;
}

void fn_80451F5C(void) {
    (&lbl_805F1490)[1]--;
}
}
