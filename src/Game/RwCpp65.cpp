/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F16AC;
extern u32 lbl_805F16B0;

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
}
