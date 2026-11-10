/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1550;
extern volatile u32 lbl_805F1558;

extern "C" {
u32 fn_8047E03C(u32 *object) {
    return object[1];
}

u32 fn_8047E0E0(u32 *object) {
    return object[0];
}

void fn_8047DDB0(u32 *object) {
    object[1] = 0;
}

void fn_8047DF9C(u32 *object, u32 value) {
    object[1] -= value;
}

void fn_8047DB10(void) {
    (&lbl_805F1550)[1]++;
}

void fn_8047DB24(void) {
    (&lbl_805F1550)[1]--;
}

void fn_8047DB70(void) {
    (&lbl_805F1558)[1]--;
}

u32 fn_8047E0CC(u32 *object, u32 value) {
    return object[0] + object[3] * value;
}

u32 fn_8047E0E8(u32 *object) {
    u32 stride = object[3];
    u32 value = object[1];
    return object[0] + stride * value;
}

void fn_8047EBA0(u32 *object) {
    object[3] &= 0xFFFDFFFC;
}

u32 fn_8047DFAC(s32 *object) {
    if (object[1] != 0) {
        return object[0];
    }
    return 0;
}

void fn_8047E298(void);

void fn_8047ECB4(void) {
    fn_8047E298();
}

extern u32 lbl_805F1560;
extern u8 *lbl_805F15B4;

u32 fn_8047E960(u32 *object) {
    u32 *target = (u32 *)(lbl_805F15B4 + lbl_805F1560 + 0x0C);
    target[0] = object[0];
    target[1] = object[1];
    target[2] = object[2];
    return 1;
}

void fn_8047DB38(u32 value) {
    lbl_805F1558 = value;
    (&lbl_805F1558)[1]++;
    *(u32 *)(lbl_805F15B4 + value) = 0;
    value = lbl_805F1558;
    *(u32 *)(lbl_805F15B4 + value + 4) = 0x80000000;
}
}
