/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14B8;
extern u32 lbl_805F14BC;

extern "C" {
void fn_80457608(u32 *object, u32 *other) {
    u32 next = object[2];
    u32 *node = object + 2;
    u32 newNext = (u32)other + 0x40;
    other[0x10] = next;
    other[0x11] = (u32)node;
    node = (u32 *)object[2];
    node[1] = newNext;
    object[2] = newNext;
    other[0x0F] = (u32)object;
}

void fn_80457630(u8 *other) {
    u32 *target = (u32 *)(other + lbl_805F14BC);
    *(u32 *)target[2] = target[1];
    *(u32 *)(target[1] + 4) = target[2];
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
}

void fn_80457664(u8 *other) {
    u32 *target = (u32 *)(other + lbl_805F14B8);
    *(u32 *)target[2] = target[1];
    *(u32 *)(target[1] + 4) = target[2];
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
}
}
