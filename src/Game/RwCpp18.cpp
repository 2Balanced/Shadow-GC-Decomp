/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14B0;
extern u32 lbl_805F14B8;
extern u32 lbl_805F14BC;

extern "C" {
void fn_8045620C(void) {
}

void fn_8045622C(void) {
}

void fn_80456230(void) {
}

void fn_80455CE4(void) {
    (&lbl_805F14B0)[1]--;
}

void fn_80455CF8(void) {
    (&lbl_805F14B0)[1]++;
}

u8 *fn_804561F0(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F14B8);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return base;
}

u8 *fn_80456210(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F14BC);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return base;
}

void fn_80457468(void);
void fn_80456FD8(void);

void fn_80455D0C(void) {
    fn_80457468();
}

void fn_80455D2C(void) {
    fn_80456FD8();
}

u32 fn_80455E44(u32 *object) {
    u32 *nested = (u32 *)object[0x1B];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
extern u32 lbl_805F14C4;
extern u32 lbl_805F14C0;

#pragma use_lmw_stmw off
void *fn_80455D4C(void *stream, s32 type) {
    if (fn_8047D868(stream, &lbl_805F14C4, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &lbl_805F14C0, 4) == 0) {
        return NULL;
    }
    return stream;
}

void *fn_80455DC8(void *stream, void *entry, const u32 *object) {
    if (fn_8047D458(stream, (u8 *)object[0x1B] + 44, 4) == 0) {
        return NULL;
    }
    if (fn_8047D458(stream, (u8 *)object[0x1B] + 48, 4) != 0) {
        return stream;
    }
    return NULL;
}

void fn_80456560(void *);
u32 fn_8049B674(void *, void *, u32);
extern u32 lbl_805F1690;
extern u8 *lbl_805F15B4;

void *fn_80455E6C(void *object) {
    if (((u32 *)object)[0x13] & 2) {
        fn_80456560(object);
    }
    ((u8 *)object)[3] |= 1;
    return object;
}

void *fn_80455EB4(void *object) {
    void *resource = ((void **)object)[0x1B];
    if (resource == NULL) {
        resource = *(void **)(lbl_805F15B4 + lbl_805F1690 + 0x3C);
    }
    if (fn_8049B674(resource, object, 1) != 0) {
        return object;
    }
    return NULL;
}
#pragma use_lmw_stmw reset
}
