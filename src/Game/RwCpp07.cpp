/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_805F1430;
extern u8 *lbl_805F15B4;

extern "C" {
void fn_80443A74(void);
u32 fn_80443A54(u8 *);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_80445CE0(void);
void fn_80444284(u32 *);

void fn_80443FE0(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F1430);
    target[0] = 0;
    target[1] = 0;
    target[2] = 0;
}

void *fn_80443FFC(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F1430);
    if (target[0] != 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)target[0]);
        target[0] = 0;
    }
    if (target[1] != 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)target[1]);
        target[1] = 0;
    }
    target[2] = 0;
    return base;
}

void *fn_80444088(u32 *object, u32 *other) {
    if (other[6] == 0) return object;
    u32 otherCount = fn_80443A54((u8 *)other[6]);
    if (otherCount == 0) return object;

    u32 *otherTarget = (u32 *)((u8 *)other + lbl_805F1430);
    u32 *target = (u32 *)((u8 *)object + lbl_805F1430);
    u32 bytes = fn_80443A54((u8 *)object[6]) * 4;
    target[0] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x30122);
    if (target[0] == 0) {
        u32 error[2];
        error[0] = 0x122;
        error[1] = fn_8047DC28(0x80000013, bytes);
        fn_8047DB84(error);
    } else {
        memset((void *)target[0], 0, bytes);
        if (otherTarget[0] != 0) {
            memcpy((void *)target[0], (void *)otherTarget[0], otherCount * 4);
        } else {
            memset((void *)target[0], 0, otherCount * 4);
        }
    }
    target[1] = 0;
    target[2] = otherTarget[2];
    return object;
}

u32 fn_804441A0(u32 *object) {
    if (fn_80443A54((u8 *)object[6]) != 0) {
        fn_80444284(object);
    }
    return 1;
}

void fn_804442A4(u32 *, u32);

u32 fn_804441E4(u32 *first) {
    fn_804442A4(first, 1);
    return 1;
}

typedef void (*RwCpp07Callback)(void);
s32 fn_80458B6C(u32, u32, RwCpp07Callback, RwCpp07Callback, RwCpp07Callback);
s32 fn_80458C28(u32, RwCpp07Callback);
s32 fn_80458C5C(u32, RwCpp07Callback);

u32 fn_8044420C(void) {
    s32 result = fn_80458B6C(0xC, 0x122, (RwCpp07Callback)fn_80443FE0,
                             (RwCpp07Callback)fn_80443FFC, (RwCpp07Callback)fn_80444088);
    lbl_805F1430 = result;
    if (result < 0) return 0;
    fn_80458C28(0x122, (RwCpp07Callback)fn_804441A0);
    fn_80458C5C(0x122, (RwCpp07Callback)fn_804441E4);
    return 1;
}

void fn_80444284(u32 *object) {
    u32 previous = object[0x12];
    *(u32 *)((u8 *)object + lbl_805F1430 + 8) = previous;
    object[0x12] = (u32)fn_80443A74;
}

void fn_804442A4(u32 *object, u32 value) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1430);
    if (target[2] != 0) {
        object[0x12] = target[2];
        target[2] = 0;
    }
    fn_80445CE0();
}

void *fn_804442E4(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1430);
    if (target[0] == 0) {
        u32 bytes = fn_80443A54((u8 *)object[6]) * 4;
        target[0] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x30122);
        if (target[0] == 0) {
            u32 error[2];
            error[0] = 0x122;
            error[1] = fn_8047DC28(0x80000013, bytes);
            fn_8047DB84(error);
        } else {
            memset((void *)target[0], 0, bytes);
        }
    }
    return (void *)target[0];
}
}
