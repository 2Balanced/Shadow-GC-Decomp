/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1438;

extern "C" {
u32 fn_80446780(u32 *object) {
    return object[1] * 0x24;
}

void fn_80445CE0(u32 *object, u32 index) {
    object[0x1B] = *(u32 *)((u8 *)&lbl_805F1438 + index * 4);
}
}
