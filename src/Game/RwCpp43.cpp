/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1550;
extern u32 lbl_805F1558;

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
}
