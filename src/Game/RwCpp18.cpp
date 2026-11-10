/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14B0;

extern "C" {
void fn_8045620C(void) {
}

void fn_8045622C(void) {
}

void fn_80456230(void) {
}

void fn_80455CE4(void) {
    (&lbl_805F14B0)[1]--;
}

void fn_80455CF8(void) {
    (&lbl_805F14B0)[1]++;
}
}
