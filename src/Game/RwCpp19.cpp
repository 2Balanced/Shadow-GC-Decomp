/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

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
}
