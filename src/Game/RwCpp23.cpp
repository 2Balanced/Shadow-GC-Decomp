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

u32 fn_8045F374(u32 value, u32 *object) {
    if (object[0] == value) {
        object[1] = 1;
    }
    return 0;
}

void fn_8045E904(u32, u32, u32);

void fn_8045EF3C(u32 first) {
    fn_8045E904(first, 0, 1);
}
}
