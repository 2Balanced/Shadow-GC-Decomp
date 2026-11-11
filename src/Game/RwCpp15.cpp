/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1490;
extern u32 lbl_8056D708[];
extern u32 lbl_805F1520;
extern u16 lbl_805EDF40;
extern u16 lbl_805F1678;
extern u8 *lbl_805F15B4;
extern u32 lbl_805F16B8;

struct RwCpp15Pair {
    float x;
    float y;
};

#define RWCPP15_MUL0(out, a, b)                                                                            \
    do {                                                                                                   \
        (out).x = (a).x * (b).x;                                                                            \
        (out).y = (a).y * (b).x;                                                                            \
    } while (0)
#define RWCPP15_MADD0(out, a, b, c)                                                                        \
    do {                                                                                                   \
        (out).x = (a).x * (b).x + (c).x;                                                                    \
        (out).y = (a).y * (b).x + (c).y;                                                                    \
    } while (0)
#define RWCPP15_MADD1(out, a, b, c)                                                                        \
    do {                                                                                                   \
        (out).x = (a).x * (b).y + (c).x;                                                                    \
        (out).y = (a).y * (b).y + (c).y;                                                                    \
    } while (0)

extern "C" {
u32 fn_8046335C(u32, u32, void *, void *, void *, void *);
void fn_8049B800(void *);
s32 fn_80458C90(u32);
s32 fn_804A2064(void *);
u32 fn_8044CCAC(u8 *);
void *fn_8044D924(u32 *, u32 *);
void *fn_8044DA3C(u32 *, u32 *);
void *fn_8044DE74(u32 *, u32 *);
void *fn_80451A6C(u32 *, u32 *);
void *fn_80451B20(u32 *, u32 *);
void *fn_80451BCC(u32 *, u32 *);
void fn_80463428(void);
s32 fn_804A8478(void *);
void fn_80463104(void *, u32);
s32 fn_80462C28(void);
void fn_80462C20(u32);
void fn_804A8120(void *, void *, u32);
void *RwFrameGetLTM(void *);
void fn_80462C30(void *, void *);
void fn_804950AC(void *, u32);
void fn_80394500(u32, u32);
u32 fn_804A2F08(u32);
u32 fn_8044DC4C(const float *, void *, int);
void fn_8044DD1C(u32 *, u32, int);
void fn_804A457C(u32 *, u32 *, u32 *, void *, u32 *);
u32 fn_8047D868(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D350(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);

void fn_80451F48(void) {
    (&lbl_805F1490)[1]++;
}

void fn_80451F5C(void) {
    (&lbl_805F1490)[1]--;
}

void fn_80450D6C(float *out, const float *matB, const float *matA, const float *initial,
                  const u8 *indices, u32 count) {
    const RwCpp15Pair *v = (const RwCpp15Pair *)initial;
    for (s32 n = (s32)count - 1; n >= 0; n--) {
        u32 index = indices[n];
        const RwCpp15Pair *a = (const RwCpp15Pair *)(matA + index * 16);
        const RwCpp15Pair *b = (const RwCpp15Pair *)(matB + index * 16);
        RwCpp15Pair *dest = (RwCpp15Pair *)(out + index * 16);
        RwCpp15Pair f0 = v[0], f1 = v[1], f2 = v[2], f3 = v[3];
        RwCpp15Pair f4 = v[4], f5 = v[5], f6 = v[6], f7 = v[7];
        RwCpp15Pair f8 = a[0], f9 = a[1], f10 = a[2], f11 = a[3];
        RwCpp15Pair f12 = a[4], f13 = a[5], f14 = a[6], f15 = a[7];
        RwCpp15Pair f16 = b[0], f17 = b[1], f18 = b[2], f19 = b[3];
        RwCpp15Pair f20 = b[4], f21 = b[5], f22 = b[6], f23 = b[7];
        RwCpp15Pair f24, f25, f26, f27, f28, f29, f30, f31;
        RWCPP15_MUL0(f24, f0, f8);
        RWCPP15_MUL0(f25, f1, f8);
        RWCPP15_MUL0(f26, f0, f10);
        RWCPP15_MUL0(f27, f1, f10);
        RWCPP15_MADD1(f24, f2, f8, f24);
        RWCPP15_MADD1(f25, f3, f8, f25);
        RWCPP15_MADD1(f26, f2, f10, f26);
        RWCPP15_MADD1(f27, f3, f10, f27);
        RWCPP15_MADD0(f24, f4, f9, f24);
        RWCPP15_MADD0(f25, f5, f9, f25);
        RWCPP15_MADD0(f26, f4, f11, f26);
        RWCPP15_MADD0(f27, f5, f11, f27);
        RWCPP15_MUL0(f28, f0, f12);
        RWCPP15_MUL0(f29, f1, f12);
        RWCPP15_MADD0(f30, f0, f14, f6);
        RWCPP15_MADD0(f31, f1, f14, f7);
        RWCPP15_MADD1(f28, f2, f12, f28);
        RWCPP15_MADD1(f29, f3, f12, f29);
        RWCPP15_MADD1(f30, f2, f14, f30);
        RWCPP15_MADD1(f31, f3, f14, f31);
        RWCPP15_MADD0(f28, f4, f13, f28);
        RWCPP15_MADD0(f29, f5, f13, f29);
        RWCPP15_MADD0(f30, f4, f15, f30);
        RWCPP15_MADD0(f31, f5, f15, f31);
        RwCpp15Pair o0, o1, o2, o3, o4, o5, o6, o7;
        RWCPP15_MUL0(o0, f24, f16);
        RWCPP15_MUL0(o1, f25, f16);
        RWCPP15_MUL0(o2, f24, f18);
        RWCPP15_MUL0(o3, f25, f18);
        RWCPP15_MADD1(o0, f26, f16, o0);
        RWCPP15_MADD1(o1, f27, f16, o1);
        RWCPP15_MADD1(o2, f26, f18, o2);
        RWCPP15_MADD1(o3, f27, f18, o3);
        RWCPP15_MADD0(o0, f28, f17, o0);
        RWCPP15_MADD0(o1, f29, f17, o1);
        RWCPP15_MADD0(o2, f28, f19, o2);
        RWCPP15_MADD0(o3, f29, f19, o3);
        RWCPP15_MUL0(o4, f24, f20);
        RWCPP15_MUL0(o5, f25, f20);
        RWCPP15_MADD0(o6, f24, f22, f30);
        RWCPP15_MADD0(o7, f25, f22, f31);
        RWCPP15_MADD1(o4, f26, f20, o4);
        RWCPP15_MADD1(o5, f27, f20, o5);
        RWCPP15_MADD1(o6, f26, f22, o6);
        RWCPP15_MADD1(o7, f27, f22, o7);
        RWCPP15_MADD0(o4, f28, f21, o4);
        RWCPP15_MADD0(o5, f29, f21, o5);
        RWCPP15_MADD0(o6, f28, f23, o6);
        RWCPP15_MADD0(o7, f29, f23, o7);
        dest[0] = o0;
        dest[1] = o1;
        dest[2] = o2;
        dest[3] = o3;
        dest[4] = o4;
        dest[5] = o5;
        dest[6] = o6;
        dest[7] = o7;
    }
}

void *fn_80451A6C(u32 *context, u32 *node) {
    if (fn_804A8478(node) != 0) {
        u32 *owner = (u32 *)context[6];
        if (*(u32 *)((u8 *)owner + lbl_805F1520) == 0) {
            fn_80463104(owner, lbl_805F16B8);
        }
        s32 state = fn_80462C28();
        if (state == 1) fn_80462C20(0);
        void *result = fn_8044D924(context, node);
        if (state == 1) fn_80462C20(1);
        return result;
    }
    return fn_8044D924(context, node);
}

void *fn_80451B20(u32 *context, u32 *node) {
    if (fn_8044DA3C(context, node) == NULL) return NULL;
    u32 *owner = (u32 *)context[6];
    u8 *format = *(u8 **)((u8 *)owner + lbl_805F1520);
    u32 *record = (u32 *)fn_8044CCAC((u8 *)owner);
    if (record[4] > 1 && format != NULL && format[0xD] != 0 && (owner[2] & 0x10) != 0) {
        fn_804A8120(node, format, owner[5]);
    }
    return context;
}

void *fn_80451BCC(u32 *context, u32 *node) {
    u32 *nodeData = (u32 *)node[0];
    u32 *callbackPair = (u32 *)((u8 *)nodeData + 0x18 + (nodeData[8] - 1) * 8 + 0x14);
    *(u16 *)((u8 *)nodeData + 0x18) = lbl_805EDF40;
    u32 *owner = (u32 *)context[6];
    u32 *descriptor = (u32 *)owner[0x54];
    void *frameMatrix = RwFrameGetLTM((void *)context[1]);
    u32 *format = *(u32 **)((u8 *)owner + lbl_805F1520);
    fn_80462C30(format, node);
    u32 *record = (u32 *)fn_8044CCAC((u8 *)owner);
    if (record[4] > 1) {
        fn_804950AC(frameMatrix, (node[2] >> 4) & 1);
    } else {
        fn_80394500(0, 1);
    }

    u32 last = (u32)record;
    if (record[4] == 1 && record[13] == 0) {
        u32 outputIndex = 0;
        u8 *indices = (u8 *)record[2];
        for (u32 i = 0; i < record[1]; i++) {
            const float *input = (const float *)(lbl_8056D708[3] + indices[i] * 0x40);
            last = fn_8044DC4C(input, (void *)outputIndex, (node[2] >> 4) & 1);
            outputIndex += 3;
        }
    }
    fn_804A2F08(last);

    u32 remaining = *(u16 *)((u8 *)descriptor + 4);
    u32 *entry = (u32 *)((u8 *)descriptor + 0x10);
    while (remaining != 0) {
        if (record[4] == 1 && record[13] != 0) {
            u32 total = *(u16 *)((u8 *)node[1] + 4);
            fn_8044DD1C(record, total - (remaining + 1), (node[2] >> 4) & 1);
        }
        fn_804A457C(entry, callbackPair, context, frameMatrix, node);
        callbackPair += 2;
        entry += 3;
        remaining--;
    }
    return context;
}

u32 fn_80451D60(u32 mode) {
    if ((mode & 1) != 0) {
        lbl_8056D708[10] = fn_8046335C(0x116, 1, (void *)fn_8044D924, (void *)fn_8044DA3C,
                                       (void *)fn_80463428, (void *)fn_8044DE74);
    }
    if ((mode & 2) != 0) {
        lbl_8056D708[11] = fn_8046335C(0x116, 2, (void *)fn_80451A6C, (void *)fn_80451B20,
                                       (void *)fn_80463428, (void *)fn_80451BCC);
    }
    return 1;
}

u32 fn_80451E0C(void) {
    if (lbl_8056D708[10] != 0) {
        fn_8049B800((void *)lbl_8056D708[10]);
        lbl_8056D708[10] = 0;
    }
    if (lbl_8056D708[11] != 0) {
        fn_8049B800((void *)lbl_8056D708[11]);
        lbl_8056D708[11] = 0;
    }
    return 1;
}

void fn_80451E6C(u32 *object, u32 mode) {
    u32 *record = (u32 *)fn_8044CCAC((u8 *)object[6]);
    u32 value;
    if (mode == 1) {
        record[11] = 1;
        value = 1;
    } else if (mode == 2) {
        record[11] = 2;
        value = 2;
    } else {
        if (fn_80458C90(0x120) > 0 && fn_804A2064(object) != 0) {
            record[11] = 2;
            value = 2;
        } else {
            record[11] = 1;
            value = 1;
        }
    }
    object[0x1B] = lbl_8056D708[9 + value];
}

void *fn_80451F70(void *stream, u32 *out) {
    u32 count;
    if (fn_8047D868(stream, &count, 4) == 0) return NULL;
    if (count > 0) {
        out[0] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count, 0x3011F);
        if (out[0] == 0 || fn_80480D4C(stream, (void *)out[0], count) == 0) return NULL;
    }
    if (fn_8047D868(stream, out + 1, 4) == 0 || fn_8047D868(stream, out + 2, 4) == 0) return NULL;

    switch (out[1]) {
    case 1:
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0 || fn_8047D868(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 2:
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0 || fn_8047D784(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 3: {
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0) return NULL;
        u32 *entries = (u32 *)out[3];
        for (u32 i = 0; i < out[2]; i++) {
            if (fn_8047D868(stream, &count, 4) == 0) return NULL;
            if (count > 0) {
                entries[i] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count, 0x3011F);
                if (entries[i] == 0 || fn_80480D4C(stream, (void *)entries[i], count) == 0) return NULL;
            } else {
                entries[i] = 0;
            }
        }
        break;
    }
    default:
        return NULL;
    }
    return stream;
}

void *fn_80452228(void *stream, u32 *out) {
    u32 size = out[0] != 0 ? ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)out[0]) + 1 : 0;
    if (fn_8047D458(stream, &size, 4) == 0) return NULL;
    if (size > 0 && fn_80480ED0(stream, (void *)out[0], size) == 0) return NULL;
    if (fn_8047D458(stream, out + 1, 4) == 0 || fn_8047D458(stream, out + 2, 4) == 0) return NULL;

    switch (out[1]) {
    case 1:
        if (fn_8047D458(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 2:
        if (fn_8047D350(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 3: {
        u32 *entries = (u32 *)out[3];
        for (u32 i = 0; i < out[2]; i++) {
            size = entries[i] != 0 ? ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)entries[i]) + 1 : 0;
            if (fn_8047D458(stream, &size, 4) == 0) return NULL;
            if (size > 0 && fn_80480ED0(stream, (void *)entries[i], size) == 0) return NULL;
        }
        break;
    }
    default:
        return NULL;
    }
    return stream;
}
}
