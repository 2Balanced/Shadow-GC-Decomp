/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1430;

extern "C" {
void fn_80443FE0(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F1430);
    target[0] = 0;
    target[1] = 0;
    target[2] = 0;
}

void fn_80443A74(void);

void fn_80444284(u32 *object) {
    u32 previous = object[0x12];
    *(u32 *)((u8 *)object + lbl_805F1430 + 8) = previous;
    object[0x12] = (u32)fn_80443A74;
}

void fn_804442A4(u32, u32);

u32 fn_804441E4(u32 first) {
    fn_804442A4(first, 1);
    return 1;
}
}
