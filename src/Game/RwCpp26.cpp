/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805EDDD8;
extern u8 lbl_805E1688[];
extern u8 lbl_8056D8CC[];
extern u8 lbl_8056D934[];

extern "C" {
void fn_804631D4(void) {
}

void fn_80462C20(u32 value) {
    lbl_805EDDD8 = value;
}

u32 fn_80462C28(void) {
    return lbl_805EDDD8;
}

void fn_80462FA4(u8 *object, u8 value) {
    object[0x0A] = value;
}

u32 fn_804631CC(void) {
    return 1;
}

void fn_80462BF0(u32 *object, u32 value) {
    ((u32 *)object[5])[0] = value;
}

void fn_80462BFC(u32 *object, u32 value) {
    ((u32 *)object[5])[1] = value;
}

void fn_80462C08(u32 *object, u32 value) {
    ((u32 *)object[5])[2] = value;
}

void fn_80462C14(u32 *object, u32 value) {
    ((u32 *)object[5])[3] = value;
}

u8 *fn_80462E1C(void) {
    return lbl_805E1688;
}

void fn_80462E28(u8 *base, u32 offset) {
    *(u32 *)(base + offset) = 0;
}

void fn_80462F7C(u8 *object, u8 first, u8 second) {
    object[0] = first;
    object[0x0C] = second;
}

void fn_80462F88(u8 *object, u8 first, u8 second) {
    object[1] = first;
    object[0x0D] = second;
}

u8 *fn_80463D1C(void) {
    return lbl_8056D8CC;
}

u8 *fn_804641C0(void) {
    return lbl_8056D934;
}
}
