/* RenderWare immediate-mode resource and render-state helpers, 0x804A29A8..0x804A3DAF.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F16A8;
extern s32 lbl_805F1658;
extern struct RwRenderState {
    s32 m0;
    s32 m1;
    s32 m2;
} lbl_805E32C8;
#define lbl_805E32CC lbl_805E32C8.m1
#define lbl_805E32D0 lbl_805E32C8.m2
extern u32 lbl_805FB450;
extern u32 lbl_805FB454;
extern u32 lbl_805FB470;
extern u32 lbl_805FB474;
extern f32 lbl_805FB478;
extern f32 lbl_805FB47C;
extern f32 lbl_805FB468;
extern f32 lbl_805FB46C;
extern f32 lbl_805E31CC[16];

void fn_8048CD8C(void *);
void fn_804A55A4(u32 *, u32);
void fn_80499C38(void *, u32);
void fn_80497778(u32);
void fn_80396B54(u32, void *);
void fn_80396A6C(u32, void *);
void fn_80396C3C(u32);
void fn_80396C78(u32, u32, u32, u32, u32, u32, u32);
void fn_803989A0(u32);
void fn_803951BC(u32);
void fn_80398804(u32, u32, u32, u32);
void fn_803982A8(u32, u32);
void fn_80394F3C(u32, u32, u32, u32, u32, u32);
void fn_80399000(u32, u32);
void fn_803992BC(void *, u32, u32);
void *RwFrameGetLTM(void *);
void fn_8047F0B0(void *, const void *);
void fn_8047ECB4(void *, const void *);
void fn_8047EBB8(void *, const void *, const void *);

static inline u32 *rwSlot(u8 *object, u8 index, int state) {
    u32 *entries = *(u32 **)(object + lbl_805F16A8);
    u32 *entry;
    u8 i;
    for (i = 0; i < 2; i++) {
        entry = entries + i * 6;
        if ((s32)entry[5] == state) goto found;
    }
    entry = NULL;
found:
    return entry;
}

f32 fn_804A29A8(u8 *object) {
    u32 *entry = rwSlot(object, 0, 2);
    return *(f32 *)(entry + 2);
}

void *fn_804A29F0(u8 *object, u8 *resource) {
    u32 *entry = rwSlot(object, 0, 4);
    *(u32 *)(resource + 0x54) += 1;
    if (entry[0] != 0) {
        fn_8048CD8C((void *)entry[0]);
        entry[0] = 0;
    }
    entry[0] = (u32)resource;
    fn_804A55A4(entry, 2);
    fn_804A55A4(entry, 1);
    return object;
}

void *fn_804A2AB0(u8 *object, u32 a, u32 b) {
    u32 *entry = rwSlot(object, 0, 4);
    entry[1] = a;
    entry[2] = b;
    fn_804A55A4(entry, 10);
    fn_804A55A4(entry, 11);
    return object;
}

u32 fn_804A2B44(u8 *object) {
    return rwSlot(object, 0, 4)[0];
}

void fn_804A2B8C(u8 *object, u32 *a, u32 *b) {
    u32 *entry = rwSlot(object, 0, 4);
    *a = entry[1];
    *b = entry[2];
}

void fn_804A2BE0(u8 *object, u32 a, u32 b) {
    u32 *entry = rwSlot(object, 0, 5);
    entry[0] = a;
    entry[1] = b;
}

void fn_804A2C2C(u8 *object, u32 *a, u32 *b) {
    u32 *entry = rwSlot(object, 0, 5);
    if (a != NULL) *a = entry[0];
    if (b != NULL) *b = entry[1];
}

void fn_804A2C90(u8 *object, u8 *resource) {
    u32 color;
    u32 flags = *(u32 *)(object + 8);
    if (*(s32 *)(object + 0x24) < 1) {
        if ((flags & 8) == 0) {
            if (*(s32 *)(object + 0x1C) == 0) {
                if ((flags & 0x40) != 0) {
                    color = resource[7];
                    fn_80396B54(4, &color);
                }
            } else {
                f32 intensity = *(f32 *)(resource + 0xC);
                if ((flags & 0x40) == 0) {
                    intensity *= 255.0f;
                    ((u8 *)&color)[0] = (u8)(intensity * *(f32 *)(object + 0xC));
                    ((u8 *)&color)[1] = (u8)(intensity * *(f32 *)(object + 0x10));
                    ((u8 *)&color)[2] = (u8)(intensity * *(f32 *)(object + 0x14));
                    ((u8 *)&color)[3] = 0xFF;
                } else {
                    ((u8 *)&color)[0] = (u8)(resource[4] * intensity * *(f32 *)(object + 0xC));
                    ((u8 *)&color)[1] = (u8)(resource[5] * intensity * *(f32 *)(object + 0x10));
                    ((u8 *)&color)[2] = (u8)(resource[6] * intensity * *(f32 *)(object + 0x14));
                    ((u8 *)&color)[3] = resource[7];
                }
                fn_80396B54(4, &color);
            }
        } else if ((flags & 0x40) != 0) {
            color = *(u32 *)(resource + 4);
            fn_80396B54(4, &color);
        }
    } else {
        if (*(s32 *)(object + 0x1C) != 0 && (flags & 8) == 0) {
            f32 intensity = 255.0f * *(f32 *)(resource + 0xC);
            ((u8 *)&color)[0] = (u8)(intensity * *(f32 *)(object + 0xC));
            ((u8 *)&color)[1] = (u8)(intensity * *(f32 *)(object + 0x10));
            ((u8 *)&color)[2] = (u8)(intensity * *(f32 *)(object + 0x14));
            ((u8 *)&color)[3] = 0xFF;
            fn_80396A6C(4, &color);
        }
        if ((flags & 0x40) != 0) {
            color = *(u32 *)(resource + 4);
            fn_80396B54(4, &color);
        }
    }
}

void fn_804A2F08(void) {
    lbl_805E32C8.m2 = 0;
    lbl_805E32C8.m0 = 0;
    lbl_805E32C8.m1 = 0;
}

#define RW_RENDER_SETUP(object, rootObject)                                                       \
    do {                                                                                          \
        u32 color;                                                                                \
        if (lbl_805E32C8.m0 == 0) {                                                                  \
            u32 flags = *(u32 *)((object) + 8);                                                    \
            if (*(s32 *)((object) + 0x24) < 1) {                                                   \
                if ((flags & 8) == 0) {                                                            \
                    if (*(s32 *)((object) + 0x1C) == 0 && (flags & 0x40) == 0) {                  \
                        color = lbl_805FB454;                                                       \
                        fn_80396B54(4, &color);                                                     \
                    }                                                                             \
                    fn_80396C3C(1);                                                                \
                    fn_80396C78(4, 0, 0, 0, 0, 0, 2);                                             \
                } else if ((flags & 0x40) == 0) {                                                  \
                    fn_80396C3C(1);                                                                \
                    fn_80396C78(4, 0, 0, 1, 0, 0, 2);                                             \
                } else {                                                                          \
                    fn_80396C3C(1);                                                                \
                    fn_80396C78(4, 1, 1, 0, 0, 0, 2);                                             \
                }                                                                                 \
            } else {                                                                              \
                u32 mode;                                                                         \
                if ((flags & 8) == 0) {                                                            \
                    mode = 0;                                                                     \
                    if (*(s32 *)((object) + 0x1C) == 0) {                                          \
                        color = lbl_805FB454;                                                       \
                        fn_80396A6C(4, &color);                                                     \
                    }                                                                             \
                } else {                                                                          \
                    mode = 1;                                                                     \
                }                                                                                 \
                fn_80396C3C(1);                                                                    \
                fn_80396C78(4, 1, mode, 0, *(u32 *)((object) + 0x20), 2, 1);                      \
                if ((flags & 0x40) == 0) {                                                         \
                    color = lbl_805FB450;                                                           \
                    fn_80396B54(4, &color);                                                         \
                }                                                                                 \
            }                                                                                     \
            lbl_805E32C8.m0 = 1;                                                                     \
        }                                                                                         \
        if (lbl_805E32CC != *(s32 *)((rootObject) + 8)) {                                         \
            fn_804A2C90((object), (rootObject));                                                   \
            lbl_805E32CC = *(s32 *)((rootObject) + 8);                                            \
        }                                                                                         \
        if (lbl_805E32D0 == 0) {                                                                  \
            fn_803989A0(1);                                                                       \
            if ((*(u32 *)((object) + 8) & 0x84) == 0) {                                            \
                fn_803951BC(0);                                                                   \
                fn_80398804(0, 0xFF, 0xFF, 4);                                                    \
                fn_803982A8(0, 4);                                                                \
            } else {                                                                              \
                fn_803951BC(1);                                                                   \
                fn_80394F3C(0, 1, 4, 0x3C, 0, 0x7D);                                              \
                fn_80398804(0, 0, 0, 4);                                                          \
                fn_803982A8(0, 0);                                                                \
            }                                                                                     \
            lbl_805E32D0 = 1;                                                                     \
        }                                                                                         \
    } while (0)

static inline void rwDrawVertices(u32 *vertices) {
    fn_80399000(vertices[0], vertices[1]);
}

static inline void rwRestoreTexture(u32 *root) {
    fn_80499C38(root, 0);
    if (root != NULL && root[0] != 0) {
        u32 *child = (u32 *)root[0];
        fn_80497778((*(u32 *)(*(u32 *)child + lbl_805F1658 + 0x14) & 1) ^ 1);
    } else {
        fn_80497778(1);
    }
}

void fn_804A2F24(u8 *param1, u32 *vertices, u8 *object) {
    u8 *rootObject = *(u8 **)(param1 + 8);
    u32 *slots = *(u32 **)(rootObject + lbl_805F16A8);
    RW_RENDER_SETUP(object, rootObject);
    rwRestoreTexture(**(u32 ***)(param1 + 8));
    rwDrawVertices(vertices);
    u32 saved10;
    u32 saved11;
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(10, &saved10);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(11, &saved11);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(10, slots[1]);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(11, slots[2]);
    if ((*(u32 *)(object + 8) & 0x80) != 0) {
        fn_80394F3C(0, 1, 5, 0x3C, 0, 0x7D);
        lbl_805E32D0 = 0;
    }
    rwRestoreTexture((u32 *)slots[0]);
    rwDrawVertices(vertices);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(10, saved10);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(11, saved11);
}

#define RW_TEXTURE_PACKET(packet, texture, mode)                                                  \
    do {                                                                                          \
        (packet)[0] = (texture)[0];                                                                \
        (packet)[1] = (texture)[4];                                                                \
        (packet)[2] = lbl_805FB470;                                                                \
        (packet)[3] = (texture)[0xC];                                                              \
        (packet)[4] = (texture)[1];                                                                \
        (packet)[5] = (texture)[5];                                                                \
        (packet)[6] = lbl_805FB470;                                                                \
        (packet)[7] = (texture)[0xD];                                                              \
        fn_803992BC((packet), 0x1E, 1);                                                            \
        fn_80394F3C(0, 1, (mode), 0x1E, 0, 0x7D);                                                 \
    } while (0)

void fn_804A3308(u8 *param1, u32 *vertices, u8 *object) {
    u8 *rootObject = *(u8 **)(param1 + 8);
    u32 *slots = *(u32 **)(rootObject + lbl_805F16A8);
    RW_RENDER_SETUP(object, rootObject);
    u32 packet[8];
    u32 *texture = (u32 *)slots[0];
    if (texture != NULL) RW_TEXTURE_PACKET(packet, texture, 4);
    rwRestoreTexture(**(u32 ***)(param1 + 8));
    rwDrawVertices(vertices);
    texture = (u32 *)slots[1];
    if ((*(u32 *)(object + 8) & 0x80) == 0) {
        if (texture == NULL) {
            if (slots[0] != 0) fn_80394F3C(0, 1, 4, 0x3C, 0, 0x7D);
        } else {
            RW_TEXTURE_PACKET(packet, texture, 4);
            lbl_805E32D0 = 0;
        }
    } else {
        if (texture == NULL) {
            fn_80394F3C(0, 1, 5, 0x3C, 0, 0x7D);
            lbl_805E32D0 = 0;
        } else {
            RW_TEXTURE_PACKET(packet, texture, 5);
            lbl_805E32D0 = 0;
        }
    }
    u32 saved10;
    u32 saved11;
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(10, &saved10);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(11, &saved11);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(10, slots[7]);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(11, slots[8]);
    rwRestoreTexture((u32 *)slots[6]);
    rwDrawVertices(vertices);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(10, saved10);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(11, saved11);
}

void fn_804A388C(u8 *param1, u32 *vertices, u8 *object, void *frame) {
    u8 *rootObject = *(u8 **)(param1 + 8);
    u32 *slots = *(u32 **)(rootObject + lbl_805F16A8);
    u32 color = lbl_805FB474;
    RW_RENDER_SETUP(object, rootObject);
    fn_80499C38(**(u32 ***)(param1 + 8), 0);
    fn_80497778(1);
    rwDrawVertices(vertices);
    u32 saved10;
    u32 saved11;
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(10, &saved10);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(11, &saved11);
    u32 mode = slots[3] != 0 ? 7 : 2;
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(10, mode);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(11, 2);
    fn_80396C78(4, 0, 0, 0, 0, 2, 2);
    fn_80396C78(5, 0, 0, 0, 0, 2, 2);
    f32 fade = *(f32 *)(slots + 2);
    if (lbl_805FB478 <= fade) {
        color = 0xFFFFFFFF;
    } else {
        u8 intensity = (u8)(lbl_805FB47C * fade);
        ((u8 *)&color)[0] = intensity;
        ((u8 *)&color)[1] = intensity;
        ((u8 *)&color)[2] = intensity;
        ((u8 *)&color)[3] = 0xFF;
    }
    fn_80396B54(4, &color);
    fn_803951BC(1);
    fn_80394F3C(0, 1, 1, 0x1E, 0, 0x7D);
    fn_803989A0(1);
    fn_803982A8(0, 0);
    fn_80398804(0, 0, 0, 4);
    lbl_805E32CC = 0;
    lbl_805E32C8.m0 = 0;
    lbl_805E32D0 = 0;
    fn_80499C38((void *)slots[1], 0);
    f32 matrixA[16];
    f32 matrixB[16];
    f32 *matrix;
    if (slots[0] == 0) {
        matrix = lbl_805E31CC;
    } else {
        fn_8047F0B0(matrixA, RwFrameGetLTM((void *)slots[0]));
        fn_8047ECB4(matrixA, matrixA);
        matrix = matrixA;
    }
    if (frame != NULL) {
        fn_8047EBB8(matrixB, frame, matrix);
        fn_8047ECB4(matrixB, matrixB);
        matrix = matrixB;
    }
    f32 packet[8];
    packet[0] = lbl_805FB468 * matrix[0];
    packet[1] = lbl_805FB468 * matrix[4];
    packet[2] = lbl_805FB468 * matrix[8];
    packet[3] = lbl_805FB46C;
    packet[4] = lbl_805FB468 * matrix[1];
    packet[5] = lbl_805FB468 * matrix[5];
    packet[6] = lbl_805FB468 * matrix[9];
    packet[7] = lbl_805FB46C;
    fn_803992BC(packet, 0x1E, 1);
    rwDrawVertices(vertices);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(10, saved10);
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x20))(11, saved11);
}
}
