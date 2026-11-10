/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805E1950[];

extern "C" {
void fn_80473E8C(u32 value) {
    lbl_805E1950[0x58] = value;
}

void fn_80473D10(u32 *object) {
    ((void (*)(void))object[0x4E])();
}
}
