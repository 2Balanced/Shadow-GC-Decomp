/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_8056D708[];

extern "C" {
void fn_8044C38C(void) {
}

void fn_8044CB6C(void) {
}

u32 fn_8044CDBC(u32 *object) {
    return object[5];
}

void fn_8044CC98(u8 *base, u32 value) {
    *(u32 *)(base + lbl_8056D708[1]) = value;
}

u32 fn_8044CCAC(u8 *object) {
    return *(u32 *)(object + lbl_8056D708[2]);
}

void fn_8044C30C(u8 *base) {
    *(u32 *)(base + lbl_8056D708[2]) = 0;
}

void fn_8044C3FC(u8 *base, u8 *other) {
    u32 offset = lbl_8056D708[1];
    *(u32 *)(base + offset) = *(u32 *)(other + offset);
}

void fn_8044EE3C(void);

void fn_8044CB70(void) {
    fn_8044EE3C();
}

void fn_8044C3D8(u8 *base) {
    u32 offset = lbl_8056D708[1];
    if (*(u32 *)(base + offset) != 0) {
        *(u32 *)(base + offset) = 0;
    }
}
}
