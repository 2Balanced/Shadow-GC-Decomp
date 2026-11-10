/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1530;

extern "C" {
void fn_804647C0(void) {
}

void fn_804647C4(void) {
}

void fn_804647B0(u8 *base) {
    *(u32 *)(base + lbl_805F1530) = 0;
}

u32 fn_8046510C(volatile u32 *object) {
    if ((object[2] & 0x01000000) != 0) {
        if (object[0x16] != 0) {
            return object[2] - 7;
        }
    }
    return 0;
}

void fn_80464A94(u32, u8 *, u8 *, u32);

void fn_80465180(u32 first, u8 *object) {
    u16 *header = *(u16 **)(object + 0x54);
    fn_80464A94(first, object, object + 0x58, header[2]);
}

void fn_80465260(u32 first, u8 *object) {
    u16 *header = *(u16 **)(object + 0x78);
    fn_80464A94(first, object, object + 0x34, header[2]);
}

void fn_80396B54(u32, u32 *);

void fn_8046551C(u32 unused, u32 *other) {
    u32 value = other[0];
    fn_80396B54(4, &value);
}
}
