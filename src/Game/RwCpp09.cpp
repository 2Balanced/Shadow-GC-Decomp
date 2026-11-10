/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1440;
extern u32 lbl_805F1448;

extern "C" {
void fn_80445CF4(float *, const float *);
void fn_8044621C(float *, float *, float *, float);
void fn_80445E08(float *, float *, float *, float);
void fn_804468D0(float *, const float *, const float *);
void fn_80446790(float *, const float *);
void *fn_80446620(void *, u32 *);
u32 fn_804466DC(void *, u32 *);
u32 fn_80446780(u32 *);
void fn_8048722C(u32 *);
void fn_8048752C(u32 *, void *, u32 *);
u32 fn_8044758C(u8 *);

void *fn_80446A04(u32 *object, u32 *other) {
    u32 *entry = (u32 *)other[4];
    for (u32 i = 0; i < other[1]; i++) {
        if (fn_8044758C((u8 *)object) == entry[0]) {
            entry[3] = (u32)object;
        }
        entry += 4;
    }
    fn_8048722C(object);
    fn_8048752C(object, (void *)fn_80446A04, other);
    return object;
}

extern u32 lbl_805EDD70;
extern u32 lbl_805EDD74;
extern u32 lbl_805E1510;
void *fn_8047FC04(u32, void *, void *, u32, void *, u32);
void fn_80477344(void *);

void *fn_80446AA8(u32 *object) {
    void *result = fn_8047FC04(0x24, &lbl_805EDD70, &lbl_805EDD74, 4, &lbl_805E1510, 0x40011E);
    ((u32 *)&lbl_805F1440)[1] = (u32)result;
    if (result == NULL) object = NULL;
    u32 callbacks[12];
    callbacks[0] = 1;
    callbacks[1] = 0x24;
    callbacks[2] = 0x24;
    callbacks[3] = (u32)fn_80445CF4;
    callbacks[4] = (u32)fn_8044621C;
    callbacks[5] = (u32)fn_80445E08;
    callbacks[6] = (u32)fn_804468D0;
    callbacks[7] = (u32)fn_80446790;
    callbacks[8] = (u32)fn_80446620;
    callbacks[9] = (u32)fn_804466DC;
    callbacks[10] = (u32)fn_80446780;
    callbacks[11] = 0;
    fn_80477344(callbacks);
    return object;
}

void fn_8047FCDC(void *);

void *fn_80446B90(u32 *object) {
    if (((u32 *)&lbl_805F1440)[1] != 0) {
        fn_8047FCDC((void *)((u32 *)&lbl_805F1440)[1]);
        ((u32 *)&lbl_805F1440)[1] = 0;
    }
    return object;
}

void fn_80447418(u32 *);
void fn_804472AC(u32, u32, u32, u32);
u32 fn_8047D458(void *, void *, u32);

void *fn_80446BFC(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    if (target[1] != 0) {
        u32 *list = (u32 *)target[1];
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < list[1]; i++) {
            entry[3] = 0;
            entry += 4;
        }
        if (list[5] == (u32)object) fn_80447418(list);
        target[1] = 0;
    }
    target[0] = 0xFFFFFFFF;
    return object;
}

void *fn_80446C94(u32 *object, u32 *other) {
    u32 *otherTarget = (u32 *)((u8 *)other + lbl_805F1440);
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    u32 *list = (u32 *)otherTarget[1];
    target[0] = otherTarget[0];
    if (list != NULL && (list[0] & 1) == 0) {
        u32 *source = (u32 *)list[4];
        fn_804472AC(list[1], 0, source[8], 0);
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < list[1]; i++) {
            entry[3] = 0;
            entry[2] = source[2];
            entry[1] = source[1];
            entry[0] = source[0];
            source += 4;
            entry += 4;
        }
        target[1] = (u32)list;
        list[5] = (u32)object;
    }
    return object;
}

void *fn_80446D84(void *stream, u32 *object, u32 *plugin) {
    u32 value = 0x100;
    if (fn_8047D458(stream, &value, 4) == 0) return NULL;
    u32 *target = (u32 *)((u8 *)plugin + lbl_805F1440);
    if (fn_8047D458(stream, target, 4) == 0) return NULL;
    u32 *list = (u32 *)target[1];
    if (list != NULL && (list[0] & 1) == 0) {
        if (fn_8047D458(stream, list + 1, 4) == 0) return NULL;
        if (fn_8047D458(stream, list, 4) == 0) return NULL;
        if (fn_8047D458(stream, (u32 *)(list[8] + 0x20), 4) == 0) return NULL;
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < list[1]; i++) {
            if (fn_8047D458(stream, entry, 4) == 0) return NULL;
            if (fn_8047D458(stream, entry + 1, 4) == 0) return NULL;
            if (fn_8047D458(stream, entry + 2, 4) == 0) return NULL;
            entry += 4;
        }
    } else {
        value = 0;
        if (fn_8047D458(stream, &value, 4) == 0) return NULL;
    }
    return stream;
}

u32 fn_8044758C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440);
}

u32 fn_8044757C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440 + 4);
}

void fn_80447598(u8 *base) {
    *(u32 *)(base + lbl_805F1448) = 0;
}

void fn_80446BE0(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    target[1] = 0;
    target[0] = 0xFFFFFFFF;
}

void fn_80456FD8(void);

void fn_804483E8(void) {
    fn_80456FD8();
}

void fn_80448598(u8 *object) {
    u32 *nested = *(u32 **)(object + lbl_805F1448);
    nested[0x10] |= nested[0x0F];
    nested[0x0F] = 0;
}
}
