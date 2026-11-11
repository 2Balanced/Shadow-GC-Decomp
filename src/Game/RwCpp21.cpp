/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * fn_8045B8EC returns the found index or -1 with retail's size; GC/2.7 folds its pointer decrement
 * into lwzu where retail uses a separate addi. fn_8045A7C4 and fn_8045AA44 also match size;
 * their remaining differences are copy-register and first-comparison scheduling choices. */
#include <dolphin/types.h>

extern "C" {
void fn_8045A7C4(u8 *object, f32 *values) {
    u32 first = ((u32 *)values)[0];
    u32 second = ((u32 *)values)[1];
    *(u32 *)(object + 24) = first;
    u32 third = ((u32 *)values)[2];
    *(u32 *)(object + 28) = second;
    u32 fourth = ((u32 *)values)[3];
    *(u32 *)(object + 32) = third;
    *(u32 *)(object + 36) = fourth;
    if (*(f32 *)(object + 24) == *(f32 *)(object + 28) &&
        *(f32 *)(object + 24) == *(f32 *)(object + 32)) {
        object[3] = 1;
    } else {
        object[3] = 0;
    }
}

extern f32 lbl_805FB120;
extern f32 lbl_805FB128;
double cos(double);

u8 *fn_8045AA44(u8 *object, f32 value) {
    if (value < lbl_805FB120 || value > lbl_805FB128) {
        return NULL;
    }
    *(f32 *)(object + 40) = -(f32)cos(value);
    return object;
}

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

s32 fn_8045B8EC(u32 *object, u32 value) {
    s32 count = object[1];
    u32 *entries = (u32 *)object[0] + count;
    while (count-- > 0) {
        entries--;
        if (entries[0] == value) {
            return count;
        }
    }
    return -1;
}

extern u8 lbl_8056D828[];
extern u32 lbl_805F14D8;
void fn_80482144(u8 *, u32 *);
void fn_8048D834(u32 *);

u32 fn_8045ACD4(u32 *object) {
    fn_80482144(lbl_8056D828, object);
    fn_8048D834(object);
    ((void (*)(u8 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
        *(u8 **)(lbl_805F15B4 + lbl_805F14D8), object);
    return 1;
}

extern u32 lbl_805F14E8;
void fn_8047FCDC(u32 *table);

u32 *fn_8045B0D8(u32 *object) {
    u32 *table = *(u32 **)(lbl_805F15B4 + lbl_805F14E8);
    if (table != NULL) {
        fn_8047FCDC(table);
        *(u32 **)(lbl_805F15B4 + lbl_805F14E8) = NULL;
    }
    (&lbl_805F14E8)[1]--;
    return object;
}

extern u8 lbl_8056D840[];
void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_8045AAE0(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D828, first, second, third, fourth);
}

void fn_8045B30C(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D840, first, second, third, fourth);
}

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_8045AAA0(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D828, first, second, third, fourth, fifth);
}

void fn_8045B2CC(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D840, first, second, third, fourth, fifth);
}
}
