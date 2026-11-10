/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1668;
extern u32 lbl_805F1670;

extern "C" {
u32 fn_80499520(u32 unused, u32 value) {
    lbl_805F1668 = value;
    return 1;
}

void fn_80499B24(u8 *base) {
    *(u32 *)(base + lbl_805F1670 + 0x20) = 0x1000000;
}

void fn_804990D4(u32);

void fn_804994B8(void) {
    fn_804990D4(lbl_805F1668);
}

extern u32 lbl_8056EC1C[];

void fn_80499BC8(void) {
    u32 *target = lbl_8056EC1C + 8;
    u32 count = 8;
    while (count-- > 1) {
        *--target = 0;
    }
}

void fn_80499B24(u8 *);
void fn_80499B38(void);
u32 fn_8048D280(u32, u32, void *, void *, u32);

void fn_80499BF8(void) {
    lbl_805F1670 = fn_8048D280(0x24, 0x40C, fn_80499B24, fn_80499B38, 0);
}
}
