/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1548;
extern u8 lbl_805EDE94;
extern u8 lbl_805E2A40[];

extern "C" {
void fn_8047A168(u32 value) {
    lbl_805F1548 = value;
}

u32 fn_8047A170(void) {
    return lbl_805F1548;
}

u32 fn_8047A7C0(u32 *object) {
    return object[3];
}

u8 *fn_8047A7EC(void) {
    return &lbl_805EDE94;
}

void fn_8047A970(u32 value, u32 *object) {
    object[0x0B] = value;
}

u8 *fn_80479FC4(void) {
    return lbl_805E2A40;
}

u32 fn_8047B050(u32 *object) {
    object[9] = 2;
    return 0;
}

void fn_804797F0(void);

void fn_8047B3EC(void) {
    fn_804797F0();
}

u32 fn_8047A7C8(u32 *object, u32 index) {
    if (index < object[1]) {
        return object[0x16] + index * 0x90;
    }
    return 0;
}

void fn_8037C6A0(u8 *, u32);

void fn_8047B384(u8 *object) {
    fn_8037C6A0(object + 0x40, 0);
}

void fn_8047A650(u32 *object) {
    ((void (*)(void))*(u32 *)(object[0x0E] + 0x44))();
}

extern u8 *lbl_805F15B4;

void fn_8047A7F4(u32 *object) {
    ((void (*)(u32))*(u32 *)(lbl_805F15B4 + 0x10C))(object[0x16]);
}
}
