/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
u32 fn_8045B79C(u32 *object, u32 index) {
    return ((u32 *)object[0])[index];
}

void fn_8045B788(u32 *object) {
    object[2] = 0;
    object[0] = 0;
    object[1] = 0;
}

u32 fn_8045B020(u32 *object) {
    u32 *nested = (u32 *)object[2];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

extern u8 *lbl_805F15B4;

void fn_8045BF30(u32 first) {
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(first, 0x30502);
}

void fn_8045B8EC(u32 *object, u32 value) {
    u32 count = object[1];
    u32 *entries = (u32 *)object[0] + count;
    while (count != 0) {
        if (entries[0] == value) {
            return;
        }
        count--;
        entries--;
    }
}

extern u8 lbl_8056D828[];
extern u8 lbl_8056D840[];
void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_8045AAE0(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D828, first, second, third, fourth);
}

void fn_8045B30C(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D840, first, second, third, fourth);
}
}
