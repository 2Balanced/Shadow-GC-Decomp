/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern s16 *lbl_805F15B4;
extern u32 lbl_805F14F0;

extern "C" {
u32 fn_8045C9C8(u32 *object, u8 *entry) {
    if (entry[0] == 8 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    if (entry[0] == 7 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    goto variable_size;
fixed_size:
    return *(u16 *)((u8 *)object + 4) * 8 + 12;
variable_size:
    {
        u32 count = *(u16 *)((u8 *)object + 4);
        u32 result = count * 8;
        result += object[2] * 4;
        result += 12;
        return result;
    }
}

u32 *fn_8045C4E4(u32 *object, u32 (*callback)(u8 *, u32 *, u32), u32 argument) {
    s32 count = *(u16 *)((u8 *)object + 4);
    u8 *entry = (u8 *)object + object[3];
    entry += 16;
    while (count-- != 0) {
        if (callback(entry, object, argument) == 0) {
            return object;
        }
        entry += 12;
    }
    return object;
}

void fn_8045CA20(void) {
    s16 *target = (s16 *)((u8 *)lbl_805F15B4 + lbl_805F14F0);
    *target += 1;
}
}
