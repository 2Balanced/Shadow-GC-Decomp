/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F16AC;
extern u32 lbl_805F16B0;
extern u32 lbl_805F16A8;

extern "C" {
u32 fn_804A2064(u8 *object) {
    return *(u32 *)(object + lbl_805F16AC);
}

void fn_804A1818(u8 *base) {
    *(u32 *)(base + lbl_805F16AC) = 0;
}

void fn_804A1828(u8 *base) {
    *(u32 *)(base + lbl_805F16AC) = 0;
}

void fn_804A1910(u8 *base) {
    *(u32 *)(base + lbl_805F16B0) = 0;
}

void fn_804A1920(u8 *base) {
    *(u32 *)(base + lbl_805F16B0) = 0;
}

void fn_804A1838(u8 *base, u8 *other) {
    if (*(s32 *)(other + lbl_805F16AC) != 0) {
        *(u32 *)(base + lbl_805F16AC) = 1;
    }
}

void fn_804A1930(u8 *base, u8 *other) {
    if (*(s32 *)(other + lbl_805F16B0) != 0) {
        *(u32 *)(base + lbl_805F16B0) = 1;
    }
}

u32 fn_804A18F0(u8 *object) {
    return *(u32 *)(object + lbl_805F16AC) != 0 ? 4 : 0;
}

u32 fn_804A19E8(u8 *object) {
    return *(u32 *)(object + lbl_805F16B0) != 0 ? 4 : 0;
}

u32 fn_804A22E4(u8 *object) {
    u32 *nested = *(u32 **)(object + lbl_805F16A8);
    if (nested == NULL) {
        return 0;
    }
    return nested[0x0C];
}
}
