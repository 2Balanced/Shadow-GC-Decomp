/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F16BC;
extern u32 lbl_805F16C0;

extern "C" {
u32 fn_804A55A4(void) {
    return 1;
}

void fn_804A558C(u32 *object) {
    object[0x1B] = lbl_805F16BC;
}

void fn_804A5598(u32 *object) {
    object[0x1F] = lbl_805F16C0;
}

void fn_804A1A08(void);

void fn_804A55AC(void) {
    fn_804A1A08();
}

u32 fn_804A55CC(u8 *object) {
    u32 result = object[0x3B] * 0x34 + 0x38;
    result += object[0x39] * 0x0C;
    result += object[0x3D] * 0x1C;
    result += object[0x3C] << 3;
    result += object[0x3A] * 0x1C;
    return result;
}
}
