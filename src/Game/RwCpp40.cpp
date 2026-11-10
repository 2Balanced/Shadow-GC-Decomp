/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_804790C4(u32 *object) {
    return object[5];
}

void fn_804790CC(u32 *object, u32 value) {
    object[5] = value;
}

void fn_804799B8(u32 *object, u32 value) {
    object[4] = value;
}

u32 fn_804799C0(u32 *object) {
    return object[4];
}

void fn_80478700(void);

void fn_80478AC8(void) {
    fn_80478700();
}

void fn_8047DFC8(u32);

u32 fn_8047909C(u32 *object) {
    fn_8047DFC8(object[4]);
    return 1;
}

extern u32 lbl_805E2A40[];
extern u32 lbl_805F1548;

u32 fn_80479A50(u32 value) {
    lbl_805E2A40[1] = value;
    lbl_805E2A40[2] = 0;
    lbl_805E2A40[5] = 0;
    lbl_805F1548 = 0;
    lbl_805E2A40[0] = 0;
    lbl_805E2A40[3] = 1;
    return 1;
}
}
