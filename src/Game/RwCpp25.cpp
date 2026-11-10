/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F150C;
extern u32 lbl_805F1510;
extern u32 lbl_805F1514;
extern u32 lbl_805F1508;
extern u32 lbl_805F1518;
extern u8 *lbl_805F15B4;

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

void fn_804612A8(void);
u32 fn_8045C9C8(u32, u32);
void fn_80463244(void);
void fn_80463308(void);
void fn_804631D4(void);

void fn_80461238(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1510);
    target[0] = 0;
    target[1] = *(u32 *)(lbl_805F15B4 + lbl_805F1518 + 8);
}

void fn_80461548(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1514);
    target[0] = 0;
    target[1] = ((u32 *)object)[4];
    ((u32 *)object)[4] = (u32)fn_804612A8;
}

void fn_80461BF0(const u32 *object) {
    fn_8045C9C8(object[0x15], (u32)object);
}

void fn_80462770(void) {
    fn_80463244();
    fn_80463308();
    fn_804631D4();
}

u32 fn_80462184(u32 unused, u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1508);
    if (target[3] != 0) {
        target[3] = 0;
        target[2] = 0;
    }
    return 0;
}

void fn_8045C56C(u32, u32, u32);

void fn_80461B60(u32 first, u32 second, u32 *third) {
    fn_8045C56C(third[0x15], first, second + 0x20);
}

void fn_80461030(void);
void fn_804621C0(u32, u32);
void fn_80462578(u32, u32);
void fn_80462130(u32, u32);
u32 fn_8045FEA0(void);

void fn_8046112C(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F150C);
    target[0] = 0;
    ((u16 *)object)[0x30] = ((u16 *)lbl_805F15B4)[4] - 1;
    target[1] = ((u32 *)object)[4];
    ((u32 *)object)[4] = (u32)fn_80461030;
}

u32 fn_804619C8(u32 first, u32 second) {
    fn_804621C0(second, first);
    return first;
}

u32 fn_80461A50(u32 first, u32 second) {
    fn_80462578(second, first);
    return first;
}

u32 fn_80461AD8(u32 first, u32 second) {
    fn_80462130(second, first);
    return first;
}

u32 fn_80461DE8(u32 *object) {
    return fn_8045C9C8(object[0x1E], fn_8045FEA0());
}
}
