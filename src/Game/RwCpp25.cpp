/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F150C;
extern u32 lbl_805F1510;
extern u32 lbl_805F1514;
extern u32 lbl_805F1508;

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

u32 fn_804621B0(u8 *object) {
    return *(u32 *)(object + lbl_805F1508 + 0x0C);
}

void fn_8046510C(void);
void fn_804651AC(void);

void fn_80461C80(void) {
    fn_8046510C();
}

void fn_80461D08(void) {
    fn_804651AC();
}

void fn_80465138(u32, u32);
void fn_80465200(u32, u32);

void fn_80461C18(u32 first, u32 second, u32 third) {
    fn_80465138(first, third);
}

void fn_80461CA0(u32 first, u32 second, u32 third) {
    fn_80465200(first, third);
}
}
