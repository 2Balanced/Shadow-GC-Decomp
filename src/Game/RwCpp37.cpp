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

extern float lbl_805FB254;

void fn_80473CE0(void) {
    ((void (*)(void))lbl_805E1950[0x210])();
}

void fn_80473E9C(void) {
    ((void (*)(void))lbl_805E1950[0xE6])();
}

u32 fn_80474228(float value) {
    if (value > lbl_805FB254) {
        ((float *)lbl_805E1950)[0xC8] = value;
        lbl_805E1950[0xC6] = 0;
        return 1;
    }
    return 0;
}
}
