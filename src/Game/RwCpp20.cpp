/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14D0;

extern "C" {
void fn_8045984C(s16 *object) {
    object[7] = object[7] + 1;
}

void fn_80458D10(void) {
    (&lbl_805F14D0)[1]--;
}

void fn_80458CF8(u32 unused, u32 value) {
    lbl_805F14D0 = value;
    (&lbl_805F14D0)[1]++;
}

void fn_8045ACD4(void);

void fn_8045A760(void) {
    fn_8045ACD4();
}

extern u8 lbl_8056D7E0[];
void fn_80481D98(u8 *, u32);

void fn_80458C90(u32 argument) {
    fn_80481D98(lbl_8056D7E0, argument);
}

void fn_80481634(u8 *, u32, u32);
void fn_80481670(u8 *, u32, u32);

void fn_80458C28(u32 first, u32 second) {
    fn_80481634(lbl_8056D7E0, first, second);
}

void fn_80458C5C(u32 first, u32 second) {
    fn_80481670(lbl_8056D7E0, first, second);
}

void fn_8048D7C0(void);

u32 fn_80458CBC(u8 *object) {
    fn_8048D7C0();
    object[3] |= 1;
    return (u32)object;
}

extern u8 lbl_8056D810[];
void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_8045998C(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D810, first, second, third, fourth);
}

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_8045994C(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D810, first, second, third, fourth, fifth);
}

void fn_8048722C(u32 count);

u32 fn_8045A784(volatile u32 *object, float value) {
    u32 count = object[1];
    ((float *)object)[5] = value;
    if (count != 0) {
        fn_8048722C(count);
    }
    return (u32)object;
}

void *fn_80459464(void *, void *, void *);
u32 fn_804591C4(void *);
void fn_8045985C(void *);
extern f32 lbl_805FB118;

void *fn_80458D24(f32 value) {
    void *object = fn_80459464(NULL, NULL, NULL);
    if (object != NULL) {
        f32 *entry = ((f32 **)object)[0x17];
        f32 constant = lbl_805FB118;
        entry[1] = constant;
        entry[2] = constant;
        entry[3] = constant;
        entry[4] = value;
        if (fn_804591C4(object) == 0) {
            fn_8045985C(object);
            return NULL;
        }
    }
    return object;
}

s32 fn_8045B8EC(void *, u32);
s32 fn_8045B7AC(void *, u32);

void *fn_8045905C(void *object, void *output, u32 mode) {
    if (mode != 0) {
        if (fn_8045B8EC((u8 *)object + 0x20, mode) < 0) {
            return NULL;
        }
        if (fn_8045B7AC((u8 *)object + 0x20, mode) < 0) {
            return NULL;
        }
        *(s16 *)((u8 *)output + 6) = -1;
    } else {
        *(s16 *)((u8 *)output + 6) = 0;
    }
    return object;
}

void fn_8045C2D4(u32);

void *fn_8045916C(u32 *object, u32 value) {
    u16 *field = (u16 *)object + 6;
    *field |= value;
    if ((value & 1) != 0 && object[0x15] != 0) {
        fn_8045C2D4(object[0x15]);
        object[0x15] = 0;
    }
    return object;
}

#pragma use_lmw_stmw off
void *fn_804590E8(void *object, void *callback, void *data) {
    s32 count;
    s32 i = 0;
    u32 offset = 0;
    count = ((u32 *)object)[9];
    while (i < count) {
        if (((u32 (*)(u32, void *))callback)(((u32 **)object)[8][offset], data) != 0) {
            return object;
        }
        i++;
        offset++;
    }
    return object;
}
#pragma use_lmw_stmw reset
}
