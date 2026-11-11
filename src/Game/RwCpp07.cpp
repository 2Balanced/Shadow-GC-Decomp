/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_805F1430;
extern u8 *lbl_805F15B4;
extern float lbl_805FAFA8;
extern s32 lbl_805F1428;
extern u32 lbl_805F1578;

extern "C" {
void *fn_80443A74(u32 *);
u32 fn_80443A54(u8 *);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_80445CE0(void);
void fn_80444284(u32 *);
void fn_8045916C(u32 *, u32);
void fn_804591C4(u32 *);

void *fn_80443A74(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1430);
    u32 *owner = (u32 *)object[6];
    u32 *tree = *(u32 **)((u8 *)owner + lbl_805F1428);
    if (tree != NULL && target[0] != 0) {

    u32 *format = (u32 *)owner[0x17];
    float *vectorA = (float *)format[5];
    float *vectorB = (float *)format[6];
    u8 *colorTarget = (u8 *)owner[0xC];
    float *pairTarget = (float *)owner[0xD];
    for (u32 i = 0; i < tree[0]; i++) {
        float next = *(float *)(target[0] + i * 4);
        float current = *(float *)(tree[2] + i * 4);
        if (next != current) {
            float delta = next - current;
            u32 *record = (u32 *)(tree[1] + i * 0x40);
            float *sourceA = (float *)record[8];
            float *sourceB = (float *)record[9];
            u8 *sourceColor = (u8 *)record[0xA];
            float *sourcePair = (float *)record[0xB];
            fn_8045916C(owner, record[3]);

            if ((record[2] & 2) != 0) {
                u32 targetIndex = 0;
                for (u32 group = 0; group < record[5]; group++) {
                    u8 mask = *(u8 *)(record[4] + group);
                    if ((mask & 0x80) != 0) {
                        float *dst = vectorA + targetIndex * 3;
                        for (u32 j = 0; j < (mask & 0x7F); j++) {
                            dst[0] += delta * sourceA[0];
                            dst[1] += delta * sourceA[1];
                            dst[2] += delta * sourceA[2];
                            targetIndex++;
                            sourceA += 3;
                            dst += 3;
                        }
                    } else {
                        targetIndex += mask & 0x7F;
                    }
                }
            }
            if ((record[2] & 0x10) != 0) {
                u32 targetIndex = 0;
                for (u32 group = 0; group < record[5]; group++) {
                    u8 mask = *(u8 *)(record[4] + group);
                    if ((mask & 0x80) != 0) {
                        float *dst = vectorB + targetIndex * 3;
                        for (u32 j = 0; j < (mask & 0x7F); j++) {
                            dst[0] += delta * sourceB[0];
                            dst[1] += delta * sourceB[1];
                            dst[2] += delta * sourceB[2];
                            targetIndex++;
                            sourceB += 3;
                            dst += 3;
                        }
                    } else {
                        targetIndex += mask & 0x7F;
                    }
                }
            }
            if ((record[2] & 8) != 0) {
                u32 targetIndex = 0;
                for (u32 group = 0; group < record[5]; group++) {
                    u8 mask = *(u8 *)(record[4] + group);
                    if ((mask & 0x80) != 0) {
                        u8 *dst = colorTarget + targetIndex * 4;
                        for (u32 j = 0; j < (mask & 0x7F); j++) {
                            s32 change = (s32)(current * (float)sourceColor[3]);
                            dst[3] = (u8)(dst[3] - change);
                            change = (s32)(current * (float)sourceColor[2]);
                            dst[2] = (u8)(dst[2] - change);
                            change = (s32)(current * (float)sourceColor[1]);
                            dst[1] = (u8)(dst[1] - change);
                            change = (s32)(current * (float)sourceColor[0]);
                            dst[0] = (u8)(dst[0] - change);
                            change = (s32)(next * (float)sourceColor[3]);
                            dst[3] = (u8)(dst[3] + change);
                            change = (s32)(next * (float)sourceColor[2]);
                            dst[2] = (u8)(dst[2] + change);
                            change = (s32)(next * (float)sourceColor[1]);
                            dst[1] = (u8)(dst[1] + change);
                            change = (s32)(next * (float)sourceColor[0]);
                            dst[0] = (u8)(dst[0] + change);
                            targetIndex++;
                            sourceColor += 4;
                            dst += 4;
                        }
                    } else {
                        targetIndex += mask & 0x7F;
                    }
                }
            }
            if ((record[2] & 4) != 0) {
                u32 targetIndex = 0;
                for (u32 group = 0; group < record[5]; group++) {
                    u8 mask = *(u8 *)(record[4] + group);
                    if ((mask & 0x80) != 0) {
                        float *dst = pairTarget + targetIndex * 2;
                        for (u32 j = 0; j < (mask & 0x7F); j++) {
                            dst[0] += delta * sourcePair[0];
                            dst[1] += delta * sourcePair[1];
                            targetIndex++;
                            sourcePair += 2;
                            dst += 2;
                        }
                    } else {
                        targetIndex += mask & 0x7F;
                    }
                }
            }
            fn_804591C4(owner);
        }
        *(float *)(tree[2] + i * 4) = next;
    }
    }
    return ((void *(*)(u32 *))target[2])(object);
}

u8 *fn_80443FE0(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F1430);
    target[0] = 0;
    target[1] = 0;
    target[2] = 0;
    return base;
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

extern s32 lbl_805F1428;
extern u16 lbl_805EDF40;
void fn_8048071C(void *);
void fn_8039561C(u32);
s32 fn_8049A78C(u32);

void fn_804448B8(u32 *object) {
    if (object[9] != 0) {
        fn_8048071C(*(void **)(object[9] - 4));
    } else {
        if (*(u16 *)((u8 *)object + 0x18) == lbl_805EDF40) {
            fn_8039561C(lbl_805EDF40);
            s32 next = (s32)lbl_805EDF40 + 1;
            lbl_805EDF40 = (u16)(next % 0xE000);
        }
        while (fn_8049A78C(*(u16 *)((u8 *)object + 0x18)) == 0) {
        }
    }

    u32 *owner = (u32 *)object[3];
    if (*(u8 *)owner == 1) owner = *(u32 **)((u8 *)owner + 0x18);
    u32 *tree = *(u32 **)((u8 *)owner + lbl_805F1428);
    object[9] = tree[3];
    if ((owner[2] & 0x10) != 0) {
        object[0xB] = tree[4];
    }
    tree[3] = 0;
}

u32 fn_804449A4(u8 *object) {
    if (*(u16 *)(object + 0x18) == lbl_805EDF40) {
        fn_8039561C(lbl_805EDF40);
        s32 next = (s32)lbl_805EDF40 + 1;
        lbl_805EDF40 = (u16)(next % 0xE000);
    }
    s32 result;
    do {
        result = fn_8049A78C(*(u16 *)(object + 0x18));
    } while (result == 0);
    return result;
}

void *fn_80480878(void *, void *, u32, void *);

static inline void RwCpp07MoveToFront(u32 *node) {
    if (node[0] == 0) return;
    u32 *next = (u32 *)node[0];
    u32 *previous = (u32 *)node[1];
    previous[0] = (u32)next;
    next[1] = (u32)previous;
    u32 *sentinel = *(u32 **)(lbl_805F15B4 + lbl_805F1578 + 0x24);
    node[0] = sentinel[0];
    node[1] = (u32)sentinel;
    *(u32 *)(sentinel[0] + 4) = (u32)node;
    sentinel[0] = (u32)node;
}

u32 fn_80444A2C(u32 *object, u32 *params, u32 *nodeOut, u32 *links, u32 flagsA, u32 flagsB) {
    u32 *owner = (u32 *)object[6];
    u32 stride = owner[5];
    u32 *node = (u32 *)*nodeOut;
    RwCpp07MoveToFront(node);
    node[5] = (u32)fn_804448B8;

    if (links[0] == 0) {
        links[0] = params[3];
        params[3] = 0;
        u32 index = 1;
        if ((flagsA & 0x10) != 0) {
            if ((flagsB & 0x10) != 0) links[index] = params[index * 2 + 3];
            index++;
        }
        if ((flagsA & 0x20) != 0) {
            if ((flagsB & 0x20) != 0) links[index] = params[index * 2 + 3];
            index++;
        }
        if ((flagsA & 0x84) != 0 && (flagsB & 0x84) != 0) {
            links[index] = params[index * 2 + 3];
        }
    }

    if (params[3] != 0) {
        u32 *existing = (u32 *)(params[3] - 4);
        if (fn_8049A78C(*(u16 *)((u8 *)existing + 0x18)) == 0) {
            existing[4] = 0;
            params[3] = 0;
        }
    }

    if (params[3] == 0) {
        u32 sizes[4] = {0, 0, 0, 0};
        sizes[0] = (((u8 *)params)[0x11] * stride + 0x1F) & ~0x1F;
        u32 total = sizes[0] + 6;
        u32 index = 1;
        if ((flagsA & 0x10) != 0) {
            if ((flagsB & 0x10) != 0) {
                sizes[index] = (((u8 *)params)[index * 8 + 0x11] * stride + 0x1F) & ~0x1F;
                total += sizes[index];
            }
            index++;
        }
        if ((flagsA & 0x20) != 0) {
            if ((flagsB & 0x20) != 0) {
                sizes[index] = (((u8 *)params)[index * 8 + 0x11] * stride + 0x1F) & ~0x1F;
                total += sizes[index];
            }
            index++;
        }
        if ((flagsA & 0x84) != 0 && (flagsB & 0x84) != 0) {
            sizes[index] = (((u8 *)params)[index * 8 + 0x11] * stride + 0x1F) & ~0x1F;
            total += sizes[index];
        }

        void *memory = fn_80480878(object, params + 3, total, fn_804449A4);
        u32 ownerValue = owner[6] == 1 ? owner[0x16] : object[5];
        if (memory == NULL || ownerValue == 0) {
            if (memory != NULL) fn_8048071C(memory);
            return 0;
        }
        params[3] = (params[3] + 0x3D) & ~0x1F;

        index = 1;
        if ((flagsA & 0x10) != 0) {
            if ((flagsB & 0x10) != 0) params[index * 2 + 3] = params[index * 2 + 1] + sizes[index];
            index++;
        }
        if ((flagsA & 0x20) != 0) {
            if ((flagsB & 0x20) != 0) params[index * 2 + 3] = params[index * 2 + 1] + sizes[index];
            index++;
        }
        if ((flagsA & 0x84) != 0 && (flagsB & 0x84) != 0) {
            params[index * 2 + 3] = params[index * 2 + 1] + sizes[index];
        }
        *(u32 *)(params[3] - 4) = (u32)memory;
    } else {
        RwCpp07MoveToFront((u32 *)(params[3] - 4));
    }

    u32 *result = (u32 *)(params[3] - 4);
    *(u16 *)((u8 *)result + 0x18) = lbl_805EDF40;
    return 1;
}
}
