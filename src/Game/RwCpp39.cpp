/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_80476CF8(void);

void fn_804771C0(void) {
    fn_80476CF8();
}

extern u8 *lbl_805F15B4;

void fn_80477A8C(void) {
    ((void (*)(void))*(u32 *)(lbl_805F15B4 + 0x10C))();
}
}
