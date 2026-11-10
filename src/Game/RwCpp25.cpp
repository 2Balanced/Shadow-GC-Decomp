/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F150C;
extern u32 lbl_805F1510;
extern u32 lbl_805F1514;

extern "C" {
void fn_80461164(void) {
}

void fn_804612A4(void) {
}

u32 fn_80462884(void) {
    return 1;
}

u32 fn_804622F0(u8 *object) {
    return *(u32 *)(object + lbl_805F150C);
}

u32 fn_8046256C(u8 *object) {
    return *(u32 *)(object + lbl_805F1510);
}

u32 fn_804626EC(u8 *object) {
    return *(u32 *)(object + lbl_805F1514);
}
}
