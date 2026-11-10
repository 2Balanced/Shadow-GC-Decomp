/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805EDDD8;
extern u8 lbl_805E1688[];
extern u8 lbl_8056D8CC[];
extern u8 lbl_8056D934[];
extern u32 lbl_805F1528;

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

void fn_80462F94(u8 *base, u32 offset, u8 first, u8 second) {
    base += offset;
    base[1] = first;
    base[0x0D] = second;
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

void fn_80462EC0(void) {
    (&lbl_805F1528)[1]--;
}

void fn_80463F10(void);
void fn_80463DF4(void);
void fn_80462914(void);

u32 fn_80464188(u32 *object) {
    u32 *table = (u32 *)object[5];
    table[0] = (u32)fn_80463F10;
    table[1] = 0;
    table[2] = (u32)fn_80463DF4;
    table[3] = (u32)fn_80462914;
    return 1;
}

void fn_804630C0(void);

void *fn_80462E34(u8 *object, u32 offset) {
    if (*(u32 *)(object + offset) != 0) {
        fn_804630C0();
    }
    return object;
}

void fn_804639AC(void);
void fn_80463840(void);
void fn_80463428(void);

u32 fn_80463CE0(u32 *object) {
    u32 *table = (u32 *)object[5];
    table[0] = (u32)fn_804639AC;
    table[1] = (u32)fn_80463840;
    table[2] = (u32)fn_80463428;
    table[3] = (u32)fn_80462914;
    return 1;
}

s32 fn_80462ED4(void);
s32 fn_804647C8(void);

u32 fn_8046318C(void) {
    if (fn_80462ED4() == 0) {
        return 0;
    }
    u32 result = fn_804647C8();
    return (-result | result) >> 31;
}
}
