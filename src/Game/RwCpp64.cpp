/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u8 lbl_8056EC6C[];
extern u8 lbl_8056ECC0[];
extern u32 lbl_805F16A0;
extern u32 lbl_805F16A8;

extern "C" {
u8 *fn_8049FB0C(void) {
    return lbl_8056EC6C;
}

u8 *fn_804A0760(void) {
    return lbl_8056ECC0;
}

u32 fn_8049FAFC(u32 unused, u32 *object) {
    lbl_805F16A0 = object[0];
    return 1;
}

void fn_804A0884(u8 *base) {
    *(u32 *)(base + lbl_805F16A8) = 0;
}

u32 fn_80483B38(void *);

u32 fn_804A0DB8(void *argument) {
    u32 result = 4;
    if (argument != NULL) {
        result = fn_80483B38(argument) + 0x10;
    }
    return result;
}
}
