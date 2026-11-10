/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_805F1440;
extern u32 lbl_805F1448;
extern u8 *lbl_805F15B4;

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

u32 fn_8047D868(void *, void *, u32);
u32 fn_804779DC(u32, u32);

void *fn_80446F40(void *stream, u32 *plugin) {
    u32 marker;
    if (fn_8047D868(stream, &marker, 4) == 0 || marker != 0x100) return NULL;
    u32 *target = (u32 *)((u8 *)plugin + lbl_805F1440);
    if (fn_8047D868(stream, target, 4) == 0) return NULL;
    u32 count;
    if (fn_8047D868(stream, &count, 4) == 0) return NULL;
    if ((s32)count <= 0) return stream;
    u32 first;
    u32 second;
    if (fn_8047D868(stream, &first, 4) == 0 || fn_8047D868(stream, &second, 4) == 0) return NULL;

    u32 *list = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(0x24, 0x3011E);
    if (list != NULL) {
        memset(list, 0, 0x24);
        list[8] = fn_804779DC(count, second);
        list[0] = first;
        list[5] = (u32)plugin;
        list[1] = count;
        list[6] = (u32)list;
        if ((list[0] & 2) == 0) {
            list[2] = 0;
            list[3] = 0;
        } else {
            u32 bytes = (count << 6) + 0xF;
            void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x3011E);
            list[2] = ((u32)memory + 0xF) & ~0xF;
            list[3] = (u32)memory;
        }
        list[4] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count << 4, 0x3011E);
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < count; i++) {
            if (fn_8047D868(stream, entry, 4) == 0) return NULL;
            if (fn_8047D868(stream, entry + 1, 4) == 0) return NULL;
            if (fn_8047D868(stream, entry + 2, 4) == 0) return NULL;
            entry[3] = 0;
            entry += 4;
        }
    }
    target[1] = (u32)list;
    return stream;
}

u32 fn_8044719C(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    if (target[0] == 0xFFFFFFFF && target[1] == 0) return 0;
    u32 *list = (u32 *)target[1];
    if (list == NULL || (list[0] & 1) != 0) return 0xC;
    return list[1] * 0xC + 0x14;
}

typedef void (*RwCpp09Callback)(void);
void fn_80446BE0(u32 *);
s32 fn_80485D78(u32, u32, RwCpp09Callback, RwCpp09Callback);
s32 fn_804878D4(u32, u32, RwCpp09Callback, RwCpp09Callback, RwCpp09Callback);
s32 fn_804836EC(u32, RwCpp09Callback, RwCpp09Callback, RwCpp09Callback);

u32 fn_80447200(void) {
    s32 result = fn_80485D78(0, 0x11E, (RwCpp09Callback)fn_80446AA8,
                             (RwCpp09Callback)fn_80446B90);
    if (result < 0) return 0;
    result = fn_804878D4(8, 0x11E, (RwCpp09Callback)fn_80446BE0,
                         (RwCpp09Callback)fn_80446BFC, (RwCpp09Callback)fn_80446C94);
    lbl_805F1440 = result;
    if (fn_804836EC(0x11E, (RwCpp09Callback)fn_80446F40,
                    (RwCpp09Callback)fn_80446D84, (RwCpp09Callback)fn_8044719C) < 0) {
        return 0;
    }
    return lbl_805F1440 >= 0;
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
