/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
void fn_80476CF8(void);

void fn_804771C0(void) {
    fn_80476CF8();
}

extern u8 *lbl_805F15B4;

void fn_80477A8C(void) {
    ((void (*)(void))*(u32 *)(lbl_805F15B4 + 0x10C))();
}

u32 fn_8047759C(void) {
    ((void (*)(void))*(u32 *)(lbl_805F15B4 + 0x10C))();
    return 1;
}

u8 *fn_80477978(u32 *object) {
    u32 *table = (u32 *)object[0];
    void (*callback)(void) = (void (*)(void))table[0x0A];
    callback();
    return (u8 *)object + 0x14;
}

u32 fn_804779A8(u32 *object) {
    u32 count = 0;
    u32 *cursor = object + 4;
    u32 stride = ((u32 *)object[0])[2];
    while (cursor[0] != (u32)object) {
        cursor = (u32 *)((u8 *)cursor + stride);
        count++;
    }
    return count;
}
}
