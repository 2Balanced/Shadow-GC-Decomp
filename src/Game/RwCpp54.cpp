/* RenderWare quantization driver, palette conversion, and render-state helpers, 0x8048F5D0..0x80490E2F.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern u8 *lbl_805F15B0;
extern s32 lbl_805EDF20;
extern u32 lbl_805E2D58[];
extern u32 lbl_805FB560;
extern u32 *lbl_805F1664;
extern u8 *lbl_805F164C;
extern s32 lbl_805F15F0;
extern s32 lbl_805EDF28;
extern s32 lbl_805F1648;
extern u32 lbl_805E3158[];
extern u32 lbl_8056E07C[];
extern u32 lbl_8056E098[];
extern f32 lbl_805FB368;
extern f32 lbl_805FB378;
extern f32 lbl_805FB388;
extern f32 lbl_805FB38C;
extern f32 lbl_805FB390;
extern f64 lbl_805FB398;
extern f64 lbl_805FB3A0;
extern f32 lbl_805FB450;
extern f32 lbl_805FB454;
extern f32 lbl_805FB470;
extern f32 lbl_805FB474;
extern f32 lbl_805FB478;
extern f32 lbl_805FB47C;
extern f32 lbl_805FB468;
extern f32 lbl_805FB46C;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void *fn_8047FBD8(u32, u32, u32, u32);
void fn_8047FCDC(void *);
void fn_8048EB24(f32 *, const u8 *, s32);
u32 fn_8048EEFC(u32 *, s32);
u32 fn_8048F1E4(f32 *, u8 *, u32, s32);
void fn_8048E300(u32 *, u32 *, u32, u32 *, u32);
void fn_8048E47C(f32 *, u32 *, u32, u32 *, f32 *);
f32 fn_8048E748(f32 *, u32 *, u32, u32 *, f32 *);
u32 fn_8048E988(f32 *, f32 *, u32 *, u32 *);
u32 fn_8048F4B4(u32 *, u32, s32);
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
void fn_8039494C(void);
void fn_80394500(u32, u32);
void fn_80394984(u32, u32, u32, u32, u32);
void fn_80497804(void);
void fn_80399458(void);
u32 fn_8037F1BC(void);
void fn_80399400(u32);
void fn_803991A0(void *);
void fn_80399114(void *);
void fn_803991E8(void *, u32);
void fn_80399288(u32);
void fn_803994A0(u32, u32, u32, ...);
void fn_80399518(u32, u32);
void fn_80395B28(u32, u32, u32);
void fn_80399288(u32);
void fn_80499C38(void *, u32);
void fn_80497778(u32);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))
#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)

u32 fn_8048F5D0(u8 *dest, u32 count, u8 *work) {
    fn_8048EB24(*(f32 **)(work + 0x4000), (const u8 *)&lbl_805FB560, lbl_805EDF20);
    memset(dest, 0, count);
    u32 leaves = fn_8048EEFC(*(u32 **)(work + 0x4000), lbl_805EDF20);
    if (leaves <= count) return fn_8048F1E4(*(f32 **)(work + 0x4000), dest, 0, lbl_805EDF20);

    u32 point[4] = {0, 0, 0, 0};
    u32 bounds[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for (u32 i = 0; i < 4; i++) bounds[i + 4] = 1 << lbl_805EDF20;
    f32 candidates[256];
    for (u32 i = 0; i < count; i++) candidates[i] = lbl_805FB368;
    u32 selected = 0;
    for (u32 i = 1; i <= count; i++) {
        s32 best = -1;
        f32 error = lbl_805FB368;
        for (u32 j = 0; j < i; j++) {
            if (candidates[j] > error) {
                error = candidates[j];
                best = (s32)j;
            }
        }
        if (best < 0) break;
        u32 bestBounds[8];
        if (fn_8048E988(*(f32 **)(work + 0x4000), candidates + best, bounds, bestBounds) != 0) {
            fn_8048E47C(*(f32 **)(work + 0x4000), point, lbl_805EDF20, bounds, candidates + best);
            selected++;
        } else {
            candidates[best] = lbl_805FB368;
        }
    }
    for (u32 i = 0; i < selected; i++) {
        fn_8048E300(*(u32 **)(work + 0x4000), point, lbl_805EDF20, bounds, i);
        fn_8048F1E4(*(f32 **)(work + 0x4000), dest, i, lbl_805EDF20);
    }
    return selected;
}

void fn_8048FAB0(u8 *dest, u32 destStride, u32 kind, u32 pack, u8 *work, u32 *source) {
    u32 srcStride = source[4];
    u8 *pixels = (u8 *)source[5];
    for (u32 row = 0; row < source[2]; row++) {
        u8 *input = pixels;
        u8 *output = dest;
        for (u32 x = 0; x < source[1]; x++) {
            u8 rgba[4];
            if (source[3] == 0x20) {
                rgba[0] = input[0];
                rgba[1] = input[1];
                rgba[2] = input[2];
                rgba[3] = input[3];
                input += 4;
            } else {
                u8 *color = (u8 *)source[6] + *input * 4;
                rgba[0] = color[0];
                rgba[1] = color[1];
                rgba[2] = color[2];
                rgba[3] = color[3];
                input++;
            }
            u32 path = ((u32)rgba[0] << 24) | ((u32)rgba[1] << 16) |
                       ((u32)rgba[2] << 8) | rgba[3];
            u8 index = (u8)fn_8048F4B4(*(u32 **)(work + 0x4000), path, lbl_805EDF20);
            if (kind == 4 && pack == 1) {
                if ((x & 1) == 0) {
                    *output = (*output & 0xF0) | (index & 0xF);
                } else {
                    *output = (*output & 0xF) | (index << 4);
                    output++;
                }
            } else {
                *output++ = index;
            }
        }
        pixels += srcStride;
        dest += destStride;
    }
}

u32 fn_8048FED8(u8 *work) {
    u32 depth = lbl_805EDF20;
    u32 *table = lbl_805E2D58;
    for (u32 i = 0; i < (1u << depth); i++) {
        u32 value = 0;
        for (u32 bit = 0; bit < depth; bit++) {
            if ((i & (1 << bit)) != 0) value |= 1 << ((depth - bit - 1) * 4);
        }
        table[i] = value;
    }
    *(void **)(work + 0x4004) = fn_8047FBD8(0x5C, 0x400, 4, 0x30411);
    *(u32 **)(work + 0x4000) = (u32 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(work + 0x4004), 0x30411);
    u32 *root = *(u32 **)(work + 0x4000);
    if (root == NULL) return 0;
    for (u32 i = 7; i < 0x17; i++) root[i] = 0;
    return 1;
}

void fn_804900D4(u8 *work, u32 *node, s32 depth) {
    if (node == NULL) return;
    if (depth > 0) {
        for (u32 i = 0; i < 0x10; i++) fn_804900D4(work, (u32 *)node[i + 7], depth - 1);
    }
    RW_API(void (*)(void *, void *), 0x11C)(*(void **)(work + 0x4004), node);
}

void fn_804903F8(u8 *work) {
    fn_804900D4(work, *(u32 **)(work + 0x4000), lbl_805EDF20);
    *(u32 *)(work + 0x4000) = 0;
    fn_8047FCDC(*(void **)(work + 0x4004));
}

static void rwSetupProjection(u8 *object) {
    fn_8039494C();
    fn_80394500(9, 1);
    fn_80394984(0, 9, 1, 4, 0);
    fn_80394500(0xB, 1);
    fn_80394984(0, 0xB, 1, 5, 0);
    fn_803989A0(1);
    fn_80396C3C(1);
    fn_80396C78(4, 0, 0, 1, 0, 0, 2);
    fn_80396C78(5, 0, 0, 0, 0, 0, 2);
    if (*lbl_805F1664 == 0) {
        fn_803951BC(0);
        fn_80398804(0, 0xFF, 0xFF, 4);
        fn_803982A8(0, 4);
    } else {
        fn_80394500(0xD, 1);
        fn_80394984(0, 0xD, 1, 4, 0);
        fn_803982A8(0, 0);
        fn_803951BC(1);
        fn_80394F3C(0, 1, 4, 0x3C, 0, 0x7D);
        fn_80398804(0, 0, 0, 4);
        fn_80497804();
    }
    if (lbl_805F164C[0x18] == 0) fn_80399458();
    else fn_80399400(fn_8037F1BC() ^ 1);
    u32 *viewport = *(u32 **)(*(u32 **)lbl_805F15B4 + 0x60);
    *(f32 *)(lbl_8056E07C + 1) = lbl_805FB388 / (f32)*(u16 *)(lbl_805F164C + 4);
    *(f32 *)(lbl_8056E07C + 3) = lbl_805FB38C / (f32)*(u16 *)(lbl_805F164C + 8);
    fn_803991A0(lbl_805E3158);
    fn_80399114(lbl_8056E07C);
    *(f32 *)(lbl_8056E098 + 3) = lbl_805FB390 + (f32)*(s16 *)(viewport + 7);
    *(f32 *)(lbl_8056E098 + 7) = lbl_805FB390 + (f32)*(s16 *)((u8 *)viewport + 0x1E);
    fn_803991E8(lbl_8056E098, 0);
    fn_80399288(0);
}

void fn_8049043C(void) {
    rwSetupProjection(lbl_805F164C);
}

static void rwSetViewport(void) {
    u32 *viewport = *(u32 **)(*(u32 **)lbl_805F15B4 + 0x60);
    if (viewport != (u32 *)*viewport) {
        if (lbl_805F15F0 == 0) {
            if (lbl_805F164C[0x18] == 0) fn_80399458();
            else fn_80399400(fn_8037F1BC() ^ 1);
            fn_803994A0((s32)*(s16 *)(viewport + 7), (s32)*(s16 *)((u8 *)viewport + 0x1E),
                        viewport[3], viewport[4]);
        } else {
            if (lbl_805F164C[0x18] == 0) fn_80399458();
            else fn_80399400(fn_8037F1BC() ^ 1);
            if (lbl_805EDF28 == 0) {
                s32 height = (s32)*(s16 *)((u8 *)viewport + 0x1E) << 1;
                s32 limit = lbl_805F1648 - 2;
                if (height < limit) {
                    if (((s32)*(s16 *)((u8 *)viewport + 0x1E) + viewport[4]) * 2 < lbl_805F1648 + 2) {
                        fn_803994A0(0, limit, *(u16 *)(lbl_805F164C + 4));
                    } else {
                        fn_803994A0((s32)*(s16 *)(viewport + 7), limit,
                                    *(u16 *)(lbl_805F164C + 4), viewport[4] << 1);
                    }
                } else {
                    fn_803994A0((s32)*(s16 *)(viewport + 7), height,
                                *(u16 *)(lbl_805F164C + 4), viewport[4] << 1);
                }
                fn_80399518(0, lbl_805F1648 - 2);
            } else {
                s32 height = (s32)*(s16 *)((u8 *)viewport + 0x1E);
                if (lbl_805F1648 + 2 < (height + viewport[4]) * 2) {
                    if (lbl_805F1648 + 2 < (height << 1)) fn_803994A0(0, 0, *(u16 *)(lbl_805F164C + 4));
                    else fn_803994A0((s32)*(s16 *)(viewport + 7), height << 1, *(u16 *)(lbl_805F164C + 4));
                } else {
                    fn_803994A0((s32)*(s16 *)(viewport + 7), height << 1,
                                *(u16 *)(lbl_805F164C + 4), viewport[4] << 1);
                }
                fn_80399518(0, 0);
            }
        }
    }
    fn_80399114(lbl_805E3158);
}

void fn_80490724(void) {
    rwSetViewport();
}

static inline void rwWriteTexture(u32 *entry) {
    if (*lbl_805F1664 == 0) {
        *(volatile f32 *)0xCC008000 = *(f32 *)(entry + 2);
        *(volatile f32 *)0xCC008004 = *(f32 *)(entry + 3);
        *(volatile f32 *)0xCC008008 = *(f32 *)(entry + 4);
        *(volatile u8 *)0xCC00800C = ((u8 *)(entry + 3))[0];
        *(volatile u8 *)0xCC00800D = ((u8 *)(entry + 3))[1];
        *(volatile u8 *)0xCC00800E = ((u8 *)(entry + 3))[2];
        *(volatile u8 *)0xCC00800F = ((u8 *)(entry + 3))[3];
        *(volatile f32 *)0xCC008010 = *(f32 *)(entry + 4);
        *(volatile f32 *)0xCC008014 = *(f32 *)(entry + 5);
    } else {
        *(volatile u32 *)0xCC008000 = entry[5];
    }
}

u32 fn_80490AB8(u32 *entries, u32 first, u32 second, u32 third, u32 fourth) {
    u32 *a = entries + first * 6;
    u32 *b = entries + second * 6;
    u32 *c = entries + fourth * 6;
    fn_8049043C();
    fn_80395B28(0x90, 0, 3);
    rwWriteTexture(a);
    rwWriteTexture(b);
    rwWriteTexture(c);
    fn_80490724();
    return 1;
}

u32 fn_80490CBC(u32 *entries, u32 first, u32 second, u32 third) {
    u32 *a = entries + first * 6;
    u32 *b = entries + second * 6;
    fn_8049043C();
    fn_80395B28(0xA8, 0, 2);
    rwWriteTexture(a);
    rwWriteTexture(b);
    fn_80490724();
    return 1;
}
}
