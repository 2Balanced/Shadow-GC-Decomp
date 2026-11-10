/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14B0;
extern u32 lbl_805F14B8;
extern u32 lbl_805F14BC;

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

void fn_804561F0(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F14B8);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
}

void fn_80456210(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F14BC);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
}

void fn_80457468(void);
void fn_80456FD8(void);

void fn_80455D0C(void) {
    fn_80457468();
}

void fn_80455D2C(void) {
    fn_80456FD8();
}
}
