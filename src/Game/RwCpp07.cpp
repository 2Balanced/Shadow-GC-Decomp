/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_805F1430;
extern u8 *lbl_805F15B4;
extern float lbl_805FAFA8;

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

void *fn_804443A0(u32 *object, u32 *channels) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1430);
    if (channels == NULL) {
        if (target[1] == 0) return object;
        u32 **block = (u32 **)target[1];
        u32 count = fn_80443A54((u8 *)object[6]);
        for (u32 i = 0; i < count; i++) {
            block[2][i] = 0;
        }
        block[0] = 0;
        return object;
    }

    if (target[1] == 0) {
        u32 count = fn_80443A54((u8 *)object[6]);
        u32 bytes = count * 8 + 0x18;
        u32 *block = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x30122);
        if (block == NULL) {
            u32 error[2];
            error[0] = 0x122;
            error[1] = fn_8047DC28(0x80000013, bytes);
            fn_8047DB84(error);
            return NULL;
        }
        memset(block, 0, bytes);
        block[3] = (u32)(block + 6);
        block[2] = (u32)(block + 6) + count * 4;
        target[1] = (u32)block;
    }
    if (target[0] == 0) {
        u32 bytes = fn_80443A54((u8 *)object[6]) * 4;
        target[0] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x30122);
        if (target[0] == 0) {
            u32 error[2];
            error[0] = 0x122;
            error[1] = fn_8047DC28(0x80000013, bytes);
            fn_8047DB84(error);
            return NULL;
        }
        memset((void *)target[0], 0, bytes);
    }

    u32 *block = (u32 *)target[1];
    block[0] = (u32)channels;
    *(float *)(block + 1) = lbl_805FAFA8;
    u32 count = fn_80443A54((u8 *)object[6]);
    for (u32 i = 0; i < count; i++) {
        u32 *channel = NULL;
        if (i < channels[0]) channel = *(u32 **)((u8 *)channels[1] + i * 8 + 4);
        if (channel != NULL) {
            *(u32 **)(block[2] + i * 4) = channel;
            *(float *)(block[3] + i * 4) = lbl_805FAFA8;
            *(float *)(target[0] + i * 4) = *(float *)channel;
        } else {
            *(u32 **)(block[2] + i * 4) = NULL;
            *(float *)(block[3] + i * 4) = lbl_805FAFA8;
            *(float *)(target[0] + i * 4) = lbl_805FAFA8;
        }
    }
    return object;
}

void *fn_80444630(u32 *object, float delta) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1430);
    u32 *block = (u32 *)target[1];
    *(float *)(block + 1) += delta;
    u32 **channelPointers = (u32 **)block[2];
    float *times = (float *)block[3];
    u32 count = fn_80443A54((u8 *)object[6]);
    u32 completed = 0;
    for (u32 i = 0; i < count; i++) {
        u32 *channel = channelPointers[i];
        if (channel != NULL) {
            times[i] += delta;
            while (times[i] > *(float *)(channel + 2)) {
                times[i] -= *(float *)(channel + 2);
                u32 next = channel[4];
                if (next == 0xFFFFFFFF) {
                    channelPointers[i] = 0;
                    break;
                }
                u32 *channelArray = *(u32 **)((u8 *)block[0] + 4);
                u32 *base = *(u32 **)((u8 *)channelArray + i * 8 + 4);
                u32 *nextChannel = (u32 *)((u8 *)base + next * 0x14);
                completed |= channel == nextChannel;
                channelPointers[i] = nextChannel;
                channel = nextChannel;
            }
            channel = channelPointers[i];
            if (channel != NULL) {
                float start = *(float *)channel;
                float end = *(float *)(channel + 1);
                float time = times[i];
                float scale = *(float *)(channel + 3);
                *(float *)(target[0] + i * 4) = start + time * scale * (end - start);
            }
        }
    }
    if (completed != 0 && block[4] != 0) {
        ((void (*)(u32 *, u32))block[4])(object, block[5]);
    }
    return object;
}

extern s32 lbl_805EDD68[];
void fn_80445C38(u32);
void fn_80445C9C(void);
s32 fn_80485D78(u32, u32, RwCpp07Callback, RwCpp07Callback);
s32 fn_804437B0(void);

void *fn_804447B0(u32 *object) {
    if (lbl_805EDD68[1] == 0) {
        fn_80445C38(1);
    }
    lbl_805EDD68[1]++;
    return object;
}

void *fn_80444808(u32 *object) {
    if (--lbl_805EDD68[1] == 0) {
        fn_80445C9C();
    }
    return object;
}

u32 fn_8044484C(void) {
    s32 result = fn_80485D78(0, 0x122, (RwCpp07Callback)fn_804447B0,
                             (RwCpp07Callback)fn_80444808);
    if (result < 0) return 0;
    if (fn_804437B0() == 0) return 0;
    u32 initialized = fn_8044420C();
    return ((0 - initialized) | initialized) >> 31;
}
}
