/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1458;
extern u32 lbl_805F1460;

extern "C" {
void fn_80453818(void) {
}

void fn_8045381C(void) {
}

u32 fn_80452C78(u32 *object) {
    return object[0];
}

u32 fn_80452C80(u32 *object) {
    return object[1];
}

u32 fn_80452C88(u32 *object, u32 index) {
    return ((u32 *)object[3])[index];
}

void fn_80452730(u8 *base, u32 offset) {
    u8 *target = base + offset;
    *(u32 *)target = 0;
    *(u32 *)(target + 4) = 0;
}

u32 fn_80452BD8(u8 *object, s32 index) {
    object += lbl_805F1458;
    s32 count = *(s32 *)object;
    if (index < count) {
        return *(u32 *)(object + 4) + index * 0x10;
    }
    return 0;
}

u32 fn_80452C4C(u8 *object, s32 index) {
    object += lbl_805F1460;
    s32 count = *(s32 *)object;
    if (index < count) {
        return *(u32 *)(object + 4) + index * 0x10;
    }
    return 0;
}

void fn_80453594(float *dest, const float *source) {
    dest[0] = source[0];
    dest[1] = source[1];
    dest[2] = source[4];
    dest[3] = source[5];
    dest[4] = source[0x0C];
    dest[5] = source[0x0D];
}
}
