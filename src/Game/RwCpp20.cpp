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

void fn_80458CF8(u32 unused, u32 value) {
    lbl_805F14D0 = value;
    (&lbl_805F14D0)[1]++;
}

void fn_8045ACD4(void);

void fn_8045A760(void) {
    fn_8045ACD4();
}

extern u8 lbl_8056D7E0[];
void fn_80481D98(u8 *, u32);

void fn_80458C90(u32 argument) {
    fn_80481D98(lbl_8056D7E0, argument);
}
}
