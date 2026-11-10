/* RenderWare frame hierarchy and stream lifecycle, 0x80485A58..0x804875AF.
 * Names are inferred from behavior; recursion replaces decompiler-unrolled traversals. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern u8 *lbl_805F15B0;
extern s32 lbl_805F15A8;
extern s32 lbl_805F15B8;
extern s32 DAT_805f15bc;
extern u8 lbl_805E2B48[300];
extern u32 lbl_8056DF70;
extern u32 lbl_8056DFB0;
extern u32 lbl_8056DFC8;
extern u32 lbl_8056DFE8;
extern u32 lbl_805EDEE0;
extern u32 lbl_805EDEE4;
extern u32 lbl_805E2C78;
extern f32 lbl_805FB2E0;
extern f32 lbl_805FB2E4;
extern f32 lbl_805FB2E8;
extern f32 lbl_805FB2F4;
extern f32 lbl_805FB2F8;
extern f32 lbl_805FB2FC;
extern f32 lbl_805FB300;
extern f32 lbl_805FB308;
extern f32 lbl_805FB30C;
extern f32 lbl_805FB310;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void *fn_8048209C(void *, void *);
void fn_80482144(void *, void *);
void fn_804821B0(void *, void *, void *);
u32 fn_80481DC8(void *, void *, void *, void *, void *, void *);
void fn_80481D98(void *, void *);
void *fn_8047FC04(u32, u32, u32, u32, u32, u32);
void fn_8047FCDC(void *);
void fn_80481C58(void);
void fn_8048038C(void);
u32 fn_804950A0(void);
void fn_8047FA28(u32);
u32 fn_80482E20(void);
u32 fn_80480274(void *);
u32 fn_80481B98(void);
void fn_80482F60(void);
void fn_8047DB38(void);
void fn_8047DB70(void);
void fn_804825EC(void);
void fn_80482548(void);
void fn_8047DB10(void);
void fn_8047DB24(void);
void fn_8047E88C(void);
void fn_8047E828(void);
void fn_80480C6C(void);
void fn_80480CF8(void);
void fn_80485424(void);
void fn_804853C0(void);
void fn_80487914(void);
void fn_80487BB0(void);
void fn_8048A02C(void);
void fn_80489FC0(void);
void fn_8048D4DC(void);
void fn_8048D300(void);
void fn_8049B0CC(void);
void fn_8049B110(void);
void fn_80482BC4(void);
void fn_80482C50(void);
u32 fn_8049B140(void);
void fn_8049AFF8(void);
void fn_8049AF94(void);
void fn_8048053C(void);
void fn_80480690(void);
u32 fn_80493E64(void);
void fn_80489830(f32);
void fn_8048528C(void);
void fn_80485354(void);
void fn_804852F0(void);
void fn_8047FDB0(void);
void fn_8047FF50(void);
void fn_8048B5D8(void *);
void *fn_80486888(void *, s32);
void *fn_8048692C(void *);
void *fn_80487448(u8 *);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))
#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)

void *fn_80485A58(void) {
    u8 *object = (u8 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(lbl_805F15B4 + lbl_805F15A8), 0x30005);
    if (object == NULL) return NULL;
    object[0] = 4;
    object[1] = 0;
    object[2] = 0;
    object[3] = 0;
    *(u32 *)(object + 4) = 0;
    *(void **)(object + 0x10) = fn_8048528C;
    *(void **)(object + 0x18) = fn_80485354;
    *(void **)(object + 0x1C) = fn_804852F0;
    *(f32 *)(object + 0x6C) = lbl_805FB2E0;
    *(f32 *)(object + 0x68) = lbl_805FB2E0;
    *(f32 *)(object + 0x74) = lbl_805FB2E0;
    *(f32 *)(object + 0x70) = lbl_805FB2E0;
    *(f32 *)(object + 0x7C) = lbl_805FB2F4;
    *(f32 *)(object + 0x78) = lbl_805FB2F4;
    *(f32 *)(object + 0x80) = lbl_805FB2F8;
    *(f32 *)(object + 0x84) = lbl_805FB2FC;
    *(f32 *)(object + 0x88) = lbl_805FB300;
    *(u32 *)(object + 0x60) = 0;
    *(u32 *)(object + 0x64) = 0;
    *(u32 *)(object + 0x14) = 1;
    f32 far;
    f32 near;
    if (*(u32 *)(object + 0x14) == 2) {
        far = *(f32 *)(object + 0x84);
        near = *(f32 *)(object + 0x80);
    } else {
        far = lbl_805FB2E0 / *(f32 *)(object + 0x84);
        near = lbl_805FB2E0 / *(f32 *)(object + 0x80);
    }
    f32 center = lbl_805FB2E4 * (*(f32 *)(lbl_805F15B4 + 0x1C) - *(f32 *)(lbl_805F15B4 + 0x18));
    f32 top = *(f32 *)(lbl_805F15B4 + 0x1C) - center;
    f32 bottom = *(f32 *)(lbl_805F15B4 + 0x18) + center;
    f32 scale = (top - bottom) / (far - near);
    *(f32 *)(object + 0x8C) = scale;
    *(f32 *)(object + 0x90) = lbl_805FB2E8 * -(scale * (far + near) - (top + bottom));
    *(u32 *)(object + 0x2C) = 0;
    fn_8048209C(&lbl_8056DF70, object);
    return object;
}

void fn_80485BCC(u32 *memory) {
    RW_API(void (*)(u32), 0x108)(*memory);
}

void *fn_80485BFC(void *object, void *memory) {
    RW_API(void (*)(void *), 0x10C)(memory);
    return object;
}

u32 fn_80485C3C(u8 *object, u32 command, u32 *output, u32 arg4, u32 arg5) {
    u32 result = (*(u32 (**)(u32, u32 *, u32, u32))(object + 4))(command, output, arg4, arg5);
    if (result == 0) {
        if (command == 0xF) {
            result = 1;
            *output = 0;
        } else if (command == 0xD) {
            result = 1;
            *output = 1;
        } else if (command > 0xC && command < 0xF && arg5 == 0) {
            result = 1;
            RW_API(void (*)(void *, void *), 0xCC)(output, &lbl_8056DFC8);
        } else if (command >= 0xF && command < 0x13) {
            result = command < 0x11 ? (arg5 == 0) : 1;
        }
    }
    if (result == 0) RW_ERROR(0x18);
    return result;
}

void *fn_80485D64(void) {
    return lbl_805F15B0;
}

u32 fn_80485D6C(void) {
    return 0x37002;
}

void fn_80485D78(void *a, void *b, void *c, void *d) {
    fn_80481DC8(&lbl_8056DFB0, a, b, c, d, NULL);
}

void fn_80485DB8(void *object) {
    fn_80481D98(&lbl_8056DFB0, object);
}

void *fn_80485DE4(void *a, void *b) {
    if (RW_API(u32 (*)(u32, void *, u32, void *), 0x14)(6, a, 0, b) == 0) {
        RW_ERROR(0x18);
        return NULL;
    }
    return a;
}

u32 fn_80485E70(void) {
    u32 result;
    if (RW_API(u32 (*)(u32, void *, u32, u32), 0x14)(10, &result, 0, 0) == 0) {
        RW_ERROR(0x18);
        result = 0xFFFFFFFF;
    }
    return result;
}

void *fn_80485EF4(void) {
    u8 *base = lbl_805F15B4;
    u32 result = RW_API(u32 (*)(u32, u32, u32, u32), 0x14)(0x12, 0, 0, 0);
    if (result == 0) result = 1;
    if (result == 0) RW_ERROR(0x18);
    fn_80482144(&lbl_8056DFB0, base);
    result = RW_API(u32 (*)(u32, u32, u32, u32), 0x14)(3, 0, 0, 0);
    if (result == 0) {
        RW_ERROR(0x18);
    } else {
        *(u32 *)(base + 0x124) = 2;
    }
    return (void *)result;
}

u32 fn_80485FE0(void) {
    u8 *base = lbl_805F15B4;
    if (RW_API(u32 (*)(u32, u32, u32, u32), 0x14)(2, 0, 0, 0) == 0) {
        RW_ERROR(0x18);
        return 0;
    }
    if (fn_8048209C(&lbl_8056DFB0, base) != NULL) {
        fn_80489830(lbl_805FB310);
        u32 result = RW_API(u32 (*)(u32, u32, u32, u32), 0x14)(0x11, 0, 0, 0);
        if (result == 0) result = 1;
        if (result == 0) RW_ERROR(0x18);
        *(u32 *)(base + 0x124) = 3;
        return 1;
    }
    if (RW_API(u32 (*)(u32, u32, u32, u32), 0x14)(3, 0, 0, 0) == 0) RW_ERROR(0x18);
    return 0;
}

u32 fn_80486138(void) {
    u32 result = RW_API(u32 (*)(u32, u32, u32, u32), 0x14)(1, 0, 0, 0);
    u8 *old = lbl_805F15B4;
    if (result == 0) {
        RW_ERROR(0x18);
        return result;
    }
    lbl_805F15B4 = lbl_805E2B48;
    memcpy(lbl_805E2B48, old, 300);
    RW_API(void (*)(void *), 0x10C)(old);
    lbl_805F15B0--;
    *(u32 *)(lbl_805F15B4 + 0x124) = 1;
    return result;
}

u32 fn_80486204(void *context) {
    if (lbl_805F15B4 == NULL) lbl_805F15B4 = lbl_805E2B48;
    u32 result = *(u32 *)(lbl_805F15B4 + 0x124) == 1;
    if (*(u32 *)(lbl_805F15B4 + 0x124) != 1) {
        RW_ERROR(0x80000001);
        return result;
    }
    if (context == NULL) {
        RW_ERROR(0x80000016);
        return 0;
    }
    u32 owner = fn_804950A0();
    if (owner == 0) return 0;
    u8 *object = (u8 *)RW_API(void *(*)(u32, u32), 0x108)((u32)&lbl_8056DFB0, 0x40000);
    if (object == NULL) {
        lbl_805F15B4 = lbl_805E2B48;
        RW_ERROR(0x80000013);
        return 0;
    }
    lbl_805F15B4 = object;
    memcpy(object, lbl_805E2B48, 300);
    if ((*(u32 (**)(u32, void *, void *, u32))(owner + 4))(4, object + 0x10, object + 0x108, 0) == 0) {
        RW_ERROR(0x18);
    }
    if ((*(u32 (**)(u32, u32, void *, u32))(owner + 4))(0, 0, context, 0) == 0) {
        RW_ERROR(0x18);
        lbl_805F15B4 = lbl_805E2B48;
        memcpy(lbl_805E2B48, object, 300);
        RW_API(void (*)(void *), 0x10C)(object);
        return 0;
    }
    if ((*(u32 (**)(u32, void *, u32, u32))(owner + 4))(0xB, object + 0x48, 0, 0x1D) == 0) {
        RW_ERROR(0x18);
    }
    lbl_805F15B0++;
    *(u32 *)(object + 0x124) = 2;
    return 1;
}

u32 fn_804864AC(void) {
    u32 empty = lbl_805F15B0 == 0;
    if (empty) {
        fn_80481C58();
        fn_8048038C();
        *(u32 *)(lbl_805F15B4 + 0x124) = 0;
    }
    return empty;
}

u32 fn_804864F8(void *context, u32 flags, u32 param) {
    lbl_805F15B4 = lbl_805E2B48;
    if ((flags & 1) == 0) {
        *(void **)(lbl_805F15B4 + 0x118) = fn_8047FDB0;
        *(void **)(lbl_805F15B4 + 0x11C) = fn_8047FF50;
        fn_8047FA28(1);
    } else {
        *(void **)(lbl_805F15B4 + 0x118) = fn_80485BCC;
        *(void **)(lbl_805F15B4 + 0x11C) = fn_80485BFC;
        fn_8047FA28(0);
    }
    *(u32 *)(lbl_805F15B4 + 0x128) = param;
    if (*(u32 *)(lbl_805F15B4 + 0x124) != 0) return 0;
    if (fn_80482E20() == 0) return 0;
    if (fn_80480274(context) == 0) return 0;
    if (fn_80481B98() == 0) return 0;
    u32 a = fn_80481DC8(&lbl_8056DFB0, (void *)8, (void *)0x40F, fn_8047DB38, fn_8047DB70, NULL);
    u32 b = fn_80481DC8(&lbl_8056DFB0, (void *)8, (void *)0x401, fn_804825EC, fn_80482548, NULL);
    u32 c = fn_80481DC8(&lbl_8056DFB0, (void *)0, (void *)0x40D, fn_8047DB10, fn_8047DB24, NULL);
    u32 d = fn_80481DC8(&lbl_8056DFB0, (void *)0x18, (void *)0x402, fn_8047E88C, fn_8047E828, NULL);
    u32 e = fn_80481DC8(&lbl_8056DFB0, (void *)4, (void *)0x403, fn_80486888, fn_8048692C, NULL);
    u32 f = fn_80481DC8(&lbl_8056DFB0, (void *)4, (void *)0x404, fn_80480C6C, fn_80480CF8, NULL);
    u32 g = fn_80481DC8(&lbl_8056DFB0, (void *)4, (void *)0x405, fn_80485424, fn_804853C0, NULL);
    u32 h = fn_80481DC8(&lbl_8056DFB0, (void *)0x220, (void *)0x406, fn_80487914, fn_80487BB0, NULL);
    u32 i = fn_80481DC8(&lbl_8056DFB0, (void *)100, (void *)0x407, fn_8048A02C, fn_80489FC0, NULL);
    u32 j = fn_80481DC8(&lbl_8056DFB0, (void *)0x34, (void *)0x408, fn_8048D4DC, fn_8048D300, NULL);
    u32 k = fn_80481DC8(&lbl_8056DFB0, (void *)0x60, (void *)0x409, fn_8049B0CC, fn_8049B110, NULL);
    u32 l = fn_80481DC8(&lbl_8056DFB0, (void *)4, (void *)0x412, fn_80482BC4, fn_80482C50, NULL);
    u32 m = fn_8049B140();
    u32 n = fn_80481DC8(&lbl_8056DFB0, (void *)0x74, (void *)0x40A, fn_8049AFF8, fn_8049AF94, NULL);
    u32 o = fn_80481DC8(&lbl_8056DFB0, (void *)0x28, (void *)0x40B, fn_8048053C, fn_80480690, NULL);
    if ((s32)(a | b | c | d | e | f | g | h | i | j | k | l | m | n | o) >= 0 && fn_80493E64() != 0) {
        *(u32 *)(lbl_805F15B4 + 0x124) = 1;
        return 1;
    }
    fn_80481C58();
    fn_8048038C();
    fn_80482F60();
    return 0;
}

void *fn_80486888(void *object, s32 offset) {
    lbl_805F15B8 = offset;
    void *memory = fn_8047FC04(lbl_8056DFE8, lbl_805EDEE0, 4, lbl_805EDEE4, lbl_805E2C78, 0x4000E);
    *(void **)(lbl_805F15B4 + offset) = memory;
    if (memory == NULL) return NULL;
    *(u32 *)(lbl_805F15B4 + 0xBC) = (u32)(lbl_805F15B4 + 0xBC);
    *(u32 *)(lbl_805F15B4 + 0xC0) = (u32)(lbl_805F15B4 + 0xBC);
    DAT_805f15bc++;
    return object;
}

void *fn_8048692C(void *object) {
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15B8) != 0) {
        fn_8047FCDC(*(void **)(lbl_805F15B4 + lbl_805F15B8));
        *(u32 *)(lbl_805F15B4 + lbl_805F15B8) = 0;
    }
    DAT_805f15bc--;
    return object;
}

void fn_80486990(u8 *node, u8 *root) {
    if (node == NULL) return;
    *(u8 **)(node + 0xA0) = root;
    for (node = *(u8 **)(node + 0x98); node != NULL; node = *(u8 **)(node + 0x9C)) {
        fn_80486990(node, root);
    }
}

u8 *fn_80486AA4(u8 *source, u8 *root) {
    u8 *object = (u8 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(lbl_805F15B4 + lbl_805F15B8), 0x3000E);
    if (object == NULL) return NULL;
    if (root == NULL) root = object;
    object[0] = source[0];
    object[1] = source[1];
    object[2] = source[2];
    object[3] = source[3];
    *(u32 *)(object + 4) = 0;
    *(u8 **)(object + 0x90) = object + 0x90;
    *(u8 **)(object + 0x94) = object + 0x90;
    for (u32 i = 0; i < 9; i++) *(u32 *)(object + 0x10 + i * 4) = *(u32 *)(source + 0x10 + i * 4);
    *(u32 *)(object + 0x98) = 0;
    *(u32 *)(object + 0x9C) = 0;
    *(u8 **)(object + 0xA0) = root;
    *(u8 **)(source + 0xA0) = object;
    for (u8 *child = *(u8 **)(source + 0x98); child != NULL; child = *(u8 **)(child + 0x9C)) {
        u8 *copy = fn_80486AA4(child, root);
        if (copy == NULL) {
            extern u32 fn_80487208(u8 *);
            fn_80487208(object);
            return NULL;
        }
        *(u8 **)(copy + 0x9C) = *(u8 **)(object + 0x98);
        *(u8 **)(object + 0x98) = copy;
        *(u8 **)(copy + 4) = object;
    }
    fn_8048209C(&lbl_8056DFE8, object);
    fn_804821B0(&lbl_8056DFE8, object, source);
    return object;
}

void *fn_80486C5C(u8 *source, u8 *root) {
    u8 *object = fn_80486AA4(source, root);
    if (object != NULL) {
        object[3] &= 0xFC;
        extern void fn_8048722C(u8 *);
        fn_8048722C(object);
    }
    return object;
}

void *fn_80486CA4(u8 *object) {
    u8 *parent = *(u8 **)(object + 4);
    fn_80486990(object, parent != NULL ? *(u8 **)(parent + 0xA0) : object);
    return object;
}

u32 fn_80486CF0(u8 *object) {
    return (*(u8 **)(object + 0xA0))[3] & 3;
}

void *fn_80486D00(void) {
    u8 *object = (u8 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(lbl_805F15B4 + lbl_805F15B8), 0x3000E);
    if (object == NULL) return NULL;
    object[0] = 0;
    object[1] = 0;
    object[2] = 0;
    object[3] = 0;
    *(u32 *)(object + 4) = 0;
    *(u8 **)(object + 0x90) = object + 0x90;
    *(u8 **)(object + 0x94) = object + 0x90;
    *(u32 *)(object + 0x1C) = 3;
    *(f32 *)(object + 0x38) = lbl_805FB308;
    *(f32 *)(object + 0x24) = lbl_805FB308;
    *(f32 *)(object + 0x10) = lbl_805FB308;
    *(f32 *)(object + 0x20) = lbl_805FB30C;
    *(f32 *)(object + 0x18) = lbl_805FB30C;
    *(f32 *)(object + 0x14) = lbl_805FB30C;
    *(f32 *)(object + 0x34) = lbl_805FB30C;
    *(f32 *)(object + 0x30) = lbl_805FB30C;
    *(f32 *)(object + 0x28) = lbl_805FB30C;
    *(f32 *)(object + 0x48) = lbl_805FB30C;
    *(f32 *)(object + 0x44) = lbl_805FB30C;
    *(f32 *)(object + 0x40) = lbl_805FB30C;
    *(u32 *)(object + 0x1C) |= 0x20003;
    *(u32 *)(object + 0x5C) = 3;
    *(f32 *)(object + 0x78) = lbl_805FB308;
    *(f32 *)(object + 0x64) = lbl_805FB308;
    *(f32 *)(object + 0x50) = lbl_805FB308;
    *(f32 *)(object + 0x60) = lbl_805FB30C;
    *(f32 *)(object + 0x58) = lbl_805FB30C;
    *(f32 *)(object + 0x54) = lbl_805FB30C;
    *(f32 *)(object + 0x74) = lbl_805FB30C;
    *(f32 *)(object + 0x70) = lbl_805FB30C;
    *(f32 *)(object + 0x68) = lbl_805FB30C;
    *(f32 *)(object + 0x88) = lbl_805FB30C;
    *(f32 *)(object + 0x84) = lbl_805FB30C;
    *(f32 *)(object + 0x80) = lbl_805FB30C;
    *(u32 *)(object + 0x5C) |= 0x20003;
    *(u32 *)(object + 0x98) = 0;
    *(u32 *)(object + 0x9C) = 0;
    *(u8 **)(object + 0xA0) = object;
    fn_8048209C(&lbl_8056DFE8, object);
    return object;
}

u32 fn_80486E2C(u8 *object) {
    fn_80482144(&lbl_8056DFE8, object);
    if (*(u32 *)(object + 4) != 0) {
        extern void *fn_80487448(u8 *);
        fn_80487448(object);
    }
    if ((object[3] & 3) != 0) {
        **(u32 **)(object + 0xC) = *(u32 *)(object + 8);
        *(u32 *)(*(u32 *)(object + 8) + 4) = *(u32 *)(object + 0xC);
    }
    for (u8 *child = *(u8 **)(object + 0x98); child != NULL; child = *(u8 **)(child + 0x9C)) {
        *(u32 *)(child + 4) = 0;
    }
    RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15B8), object);
    return 1;
}

void fn_80486ED8(u8 *object) {
    fn_80482144(&lbl_8056DFE8, object);
    if ((object[3] & 3) != 0) {
        **(u32 **)(object + 0xC) = *(u32 *)(object + 8);
        *(u32 *)(*(u32 *)(object + 8) + 4) = *(u32 *)(object + 0xC);
    }
}

void fn_80486F34(u8 *object) {
    fn_80482144(&lbl_8056DFE8, object);
    if ((object[3] & 3) != 0) {
        **(u32 **)(object + 0xC) = *(u32 *)(object + 8);
        *(u32 *)(*(u32 *)(object + 8) + 4) = *(u32 *)(object + 0xC);
    }
    RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15B8), object);
}

void fn_80486FAC(u8 *object) {
    if (object == NULL) return;
    for (u8 *child = *(u8 **)(object + 0x98); child != NULL;) {
        u8 *next = *(u8 **)(child + 0x9C);
        fn_80486FAC(child);
        child = next;
    }
    fn_80486ED8(object);
    RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15B8), object);
}

u32 fn_80487208(u8 *object) {
    fn_80486FAC(object);
    return 1;
}

void fn_8048722C(u8 *object) {
    u8 *root = *(u8 **)(object + 0xA0);
    u8 flags = root[3];
    if ((flags & 3) == 0) {
        *(u32 *)(root + 8) = *(u32 *)(lbl_805F15B4 + 0xBC);
        *(u32 *)(root + 0xC) = (u32)(lbl_805F15B4 + 0xBC);
        **(u32 **)(lbl_805F15B4 + 0xBC + 4) = (u32)(root + 8);
        *(u32 *)(lbl_805F15B4 + 0xBC) = (u32)(root + 8);
    }
    root[3] = flags | 3;
    object[3] |= 0xC;
}

void *RwFrameGetLTM(u8 *object) {
    u8 *root = *(u8 **)(object + 0xA0);
    if ((root[3] & 1) != 0) fn_8048B5D8(root);
    return object + 0x50;
}

void *fn_804872D8(u8 *object) {
    return *(void **)(object + 0xA0);
}

void *fn_804872E0(u8 *parent, u8 *child) {
    if (*(u32 *)(child + 4) != 0) fn_80487448(child);
    *(u8 **)(child + 0x9C) = *(u8 **)(parent + 0x98);
    *(u8 **)(parent + 0x98) = child;
    *(u8 **)(child + 4) = parent;
    fn_80486990(child, *(u8 **)(parent + 0xA0));
    return parent;
}

void *fn_80487348(u8 *parent, u8 *child) {
    fn_804872E0(parent, child);
    if ((child[3] & 3) != 0) {
        **(u32 **)(child + 0xC) = *(u32 *)(child + 8);
        *(u32 *)(*(u32 *)(child + 8) + 4) = *(u32 *)(child + 0xC);
        child[3] &= 0xFC;
    }
    fn_8048722C(child);
    return parent;
}

void *fn_80487448(u8 *object) {
    u8 *parent = *(u8 **)(object + 4);
    u8 *child = *(u8 **)(parent + 0x98);
    if (child == object) {
        *(u8 **)(parent + 0x98) = *(u8 **)(object + 0x9C);
    } else {
        u8 *previous;
        do {
            previous = child;
            child = *(u8 **)(previous + 0x9C);
        } while (child != object);
        *(u8 **)(previous + 0x9C) = *(u8 **)(object + 0x9C);
    }
    *(u32 *)(object + 4) = 0;
    *(u32 *)(object + 0x9C) = 0;
    fn_80486990(object, object);
    fn_8048722C(object);
    return object;
}

void *fn_8048752C(u8 *object, u32 (*callback)(u8 *, void *), void *data) {
    for (u8 *child = *(u8 **)(object + 0x98); child != NULL;) {
        u8 *next = *(u8 **)(child + 0x9C);
        if (callback(child, data) == 0) break;
        child = next;
    }
    return object;
}
}
