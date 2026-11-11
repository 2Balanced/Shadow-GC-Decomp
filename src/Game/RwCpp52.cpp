/* RenderWare archive/group management and color quantization helpers, 0x8048C038..0x8048DBEB.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F15D4;
extern s32 DAT_805f15d8;
extern u8 *lbl_805F15D0;
extern u32 lbl_8056E030;
extern u32 lbl_8056E048;
extern u32 lbl_805EDF04;
extern u32 lbl_805EDF00;
extern u32 lbl_805EDF0C;
extern u32 lbl_805EDF10;
extern u32 lbl_805EDF14;
extern u32 lbl_805EDF18;
extern u32 lbl_805E2D10;
extern u32 lbl_805E2D34;
extern f32 lbl_805FB360;
extern f32 lbl_805FB364;
extern f32 lbl_805FB368;
extern f32 lbl_805FB36C;
extern f32 lbl_805FB370;
extern f32 lbl_805FB378;
extern f32 lbl_805FB37C;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_8048209C(void *, void *);
void fn_80482144(void *, void *);
void fn_80481DC8(void *, void *, void *, void *, void *, void *);
void *fn_8047FC04(u32, u32, u32, u32, u32, u32);
void fn_8047FCDC(void *);
void fn_8048722C(void *);
void *fn_8048BD84(char *, char *);
void *fn_8048BA30(char *, char *, u32, u32 *, u32 *, void **, void **);
void *fn_80489E74(u32, u32, void *, void *);
void fn_804894B8(void *);
void *fn_80489A1C(void *, void *);
void fn_80489BDC(void *);
u32 fn_80487DB0(void *);
u32 fn_80489CE4(void *);
u32 fn_80489F70(void *, u8, s32);
void fn_80489B40(void *);
u32 fn_8048B860(void *, u32, void **, s32, u32);
u32 *fn_8048CCDC(void *);
void *fn_8048CE54(u8 *, char *);
void *fn_8048CF00(u8 *, char *);
void *fn_80487D08(u32, u32, u32);
u32 *fn_80487E18(u32 *);
void *fn_804899A8(void *, void *);
void fn_804893B8(void *, void *);
void *fn_8048A96C(void *, u32, u32);
u32 fn_8048CD8C(u32 *);
void *fn_8048CFFC(u8 *, char *);
void fn_8048CBE0(u8 *, u32 (*)(u8 *, u32), u32);
void *fn_8048CFAC(u8 *, u8 *);
u32 fn_8048B6AC(void *, char *, u8);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))
#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)
#define RW_COPY_LIMIT(dest, src, max)                                                            \
    do {                                                                                         \
        RW_API(void (*)(void *, void *), 0xD0)((dest), (src));                                    \
        if (RW_API(u32 (*)(void *), 0xF4)(src) >= (max)) {                                       \
            RW_ERROR(0x8000001E);                                                                \
            *((u8 *)(dest) + (max) - 1) = 0;                                                     \
        }                                                                                        \
    } while (0)

void *fn_8048C038(char *name, char *suffix) {
    u32 kind = 4;
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x1C) != 0) kind = 0x8004;
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x20) != 0) kind = 0x9004;
    u32 width = 0;
    u32 height = 0;
    void *a = NULL;
    void *b = NULL;
    void **resources = (void **)fn_8048BA30(name, suffix, kind, &width, &height, &a, &b);
    if (resources == NULL) return NULL;
    void *converted = fn_80489E74(width, height, a, b);
    if (converted == NULL) {
        fn_80487DB0(resources);
        return NULL;
    }
    u32 flags = (u32)b;
    if ((flags & 0x8000) == 0) {
        fn_804894B8(resources);
        if (fn_80489A1C(converted, resources) == NULL) {
            fn_80489BDC(converted);
            fn_80487DB0(resources);
            return NULL;
        }
        fn_80487DB0(resources);
    } else if ((flags & 0x1000) == 0) {
        s32 count = fn_80489CE4(converted);
        void *group[16];
        group[0] = resources;
        for (s32 i = 1; i < count; i++) {
            fn_80489F70(converted, (u8)i, 5);
            fn_80489B40(converted);
            group[i] = fn_8048BA30(name, suffix, kind, &width, &height, &a, &b);
            if (group[i] == NULL) {
                while (--i >= 0) fn_80487DB0(group[i]);
                fn_80489BDC(converted);
                return NULL;
            }
        }
        if ((((u8 *)converted)[0x23] & 0x60) == 0) {
            for (s32 i = 0; i < count; i++) fn_804894B8(group[i]);
        } else {
            u8 palette[0x400];
            fn_8048B860(palette, 0, group, count, (((u8 *)converted)[0x23] & 0x40) == 0 ? 8 : 4);
            fn_804894B8(group[0]);
        }
        for (s32 i = 0; i < count; i++) {
            if (fn_80489F70(converted, (u8)i, 5) != 0) {
                if (fn_80489A1C(converted, group[i]) == NULL) {
                    for (; i < count; i++) fn_80487DB0(group[i]);
                    fn_80489BDC(converted);
                    return NULL;
                }
                fn_80489B40(converted);
            }
            fn_80487DB0(group[i]);
        }
    } else {
        if (fn_80489A1C(converted, resources) == NULL) {
            fn_80489BDC(converted);
            fn_80487DB0(resources);
            return NULL;
        }
        fn_80487DB0(resources);
    }
    void *archive = fn_8048CCDC(converted);
    if (archive == NULL) {
        fn_80489BDC(converted);
        return NULL;
    }
    fn_8048CE54((u8 *)archive, name);
    fn_8048CF00((u8 *)archive, suffix != NULL ? suffix : (char *)&lbl_805EDF00);
    return archive;
}

void *fn_8048C5F4(char *name, char *suffix) {
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x1C) == 0) {
        return fn_8048BD84(name, suffix);
    }
    return fn_8048C038(name, suffix);
}

void *fn_8048C634(u8 *object, void *existing) {
    u32 *resources = (u32 *)existing;
    if (existing == NULL) {
        resources = (u32 *)fn_80487D08(*(u32 *)(object + 0xC), *(u32 *)(object + 0x10), 0x20);
        if (resources == NULL) return NULL;
        if (fn_80487E18(resources) == NULL) return NULL;
        fn_804899A8(resources, object);
    } else if (resources[3] != 0x20) {
        resources = (u32 *)fn_80487D08(resources[1], resources[2], 0x20);
        if (resources == NULL) return NULL;
        if (fn_80487E18(resources) == NULL) return NULL;
        fn_804893B8(resources, existing);
    }
    if (resources == NULL) return NULL;
    u8 saved = object[0x23] & 0x10;
    object[0x23] &= ~saved;
    s32 count = fn_80489CE4(object);
    void **group = (void **)resources;
    for (s32 i = 1; i < count; i++) {
        group[i] = NULL;
        if (fn_80489F70(object, (u8)i, 2) != 0) {
            group[i] = fn_8048A96C(group[i - 1], *(u32 *)(object + 0xC), *(u32 *)(object + 0x10));
            fn_80489B40(object);
        }
        if (group[i] == NULL) {
            while (--i >= 0) {
                if (group[i] != existing) fn_80487DB0(group[i]);
            }
            object[0x23] |= saved;
            return NULL;
        }
    }
    if ((object[0x23] & 0x60) == 0) {
        for (s32 i = 0; i < count; i++) fn_804894B8(group[i]);
    } else {
        u8 palette[0x400];
        u32 result = fn_8048B860(palette, (u32)existing, group, count, (object[0x23] & 0x40) == 0 ? 8 : 4);
        if (result == 0 && count > 0) {
            if (group[0] != existing) fn_80487DB0(group[0]);
            object[0x23] |= saved;
            return NULL;
        }
        fn_804894B8(group[0]);
    }
    for (s32 i = 0; i < count; i++) {
        if (fn_80489F70(object, (u8)i, 5) != 0) {
            fn_80489A1C(object, group[i]);
            fn_80489B40(object);
        }
        if (group[i] != existing) fn_80487DB0(group[i]);
    }
    object[0x23] |= saved;
    return object;
}

void *fn_8048C91C(char *name) {
    u32 *base = (u32 *)(lbl_805F15B4 + lbl_805F15D4);
    if (base[4] == 0) {
        for (u32 *link = (u32 *)*base; link != base; link = (u32 *)*link) {
            void *found = fn_8048CFFC((u8 *)(link - 4), name);
            if (found != NULL) return found;
        }
        return NULL;
    }
    return fn_8048CFFC((u8 *)base[4], name);
}

u32 fn_8048C9A8(u32 value) {
    *(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x1C) = value;
    return 1;
}

u32 fn_8048C9C0(void) {
    return *(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x1C);
}

u32 fn_8048C9D4(u32 value) {
    *(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x20) = value;
    return 1;
}

u32 fn_8048C9EC(void) {
    return *(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x20);
}

void fn_8048CA00(u32 *object, s32 mode) {
    if (mode == 0) {
        object[0] = 0;
    } else {
        RW_API(void (*)(u32 *, s32), 0x68)(object, mode);
    }
}

void *fn_8048CA64(void) {
    u8 *object = (u8 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(lbl_805F15B4 + lbl_805F15D4 + 0xC), 0x30016);
    if (object == NULL) return NULL;
    object[0] = 6;
    object[1] = 0;
    object[2] = 0;
    object[3] = 0;
    *(u32 *)(object + 4) = 0;
    *(u32 *)(object + 0x10) = *(u32 *)(lbl_805F15B4 + lbl_805F15D4);
    *(u32 *)(object + 0x14) = (u32)(lbl_805F15B4 + lbl_805F15D4);
    **(u32 **)(lbl_805F15B4 + lbl_805F15D4 + 4) = (u32)(object + 0x10);
    *(u32 *)(lbl_805F15B4 + lbl_805F15D4) = (u32)(object + 0x10);
    *(u8 **)(object + 8) = object + 8;
    *(u8 **)(object + 0xC) = object + 8;
    fn_8048209C(&lbl_8056E048, object);
    return object;
}

u32 fn_8048CB38(u8 *object) {
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x10) == (u32)object) {
        *(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x10) = 0;
    }
    fn_8048CBE0(object, (u32 (*)(u8 *, u32))fn_8048CD8C, 0);
    fn_80482144(&lbl_8056E048, object);
    **(u32 **)(object + 0x14) = *(u32 *)(object + 0x10);
    *(u32 *)(*(u32 *)(object + 0x10) + 4) = *(u32 *)(object + 0x14);
    RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15D4 + 0xC), object);
    return 1;
}

void fn_8048CBE0(u8 *object, u32 (*callback)(u8 *, u32), u32 data) {
    u32 **list = (u32 **)(object + 8);
    for (u32 *link = *list; link != (u32 *)list; link = (u32 *)*link) {
        if (callback((u8 *)(link - 2), data) == 0) break;
    }
}

u32 fn_8048CC54(u32 (*callback)(u8 *, u32), u32 data) {
    u32 *base = (u32 *)(lbl_805F15B4 + lbl_805F15D4);
    for (u32 *link = (u32 *)*base; link != base; link = (u32 *)*link) {
        if (callback((u8 *)(link - 4), data) == 0) break;
    }
    return 1;
}

u32 *fn_8048CCDC(void *owner) {
    u32 *object = (u32 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(lbl_805F15B4 + lbl_805F15D4 + 8), 0x30006);
    if (object == NULL) return NULL;
    object[1] = 0;
    *(u8 *)(object + 4) = 0;
    *(u8 *)(object + 0xC) = 0;
    object[0] = (u32)owner;
    object[0x15] = 1;
    object[0x14] = 0;
    object[0x14] = (object[0x14] & 0xFFFF00FF) | 0x1100;
    object[0x14] = (object[0x14] & 0xFFFFFF00) | 1;
    fn_8048209C(&lbl_8056E030, object);
    return object;
}

u32 fn_8048CD8C(u32 *object) {
    object[0x15]--;
    if (object[0x15] < 1) {
        object[0x15]++;
        fn_80482144(&lbl_8056E030, object);
        if (object[1] != 0) {
            *(u32 *)object[3] = object[2];
            *(u32 *)(object[2] + 4) = object[3];
        }
        if (object[0] != 0) {
            fn_80489BDC((void *)object[0]);
            object[0] = 0;
        }
        object[0x15]--;
        RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15D4 + 8), object);
    }
    return 1;
}

void *fn_8048CE54(u8 *object, char *name) {
    RW_API(void (*)(void *, void *), 0xD0)(object + 0x10, name);
    if (RW_API(u32 (*)(void *), 0xF4)(name) > 0x1F) {
        RW_ERROR(0x8000001E);
        object[0x2F] = 0;
    }
    return object;
}

void *fn_8048CF00(u8 *object, char *name) {
    RW_API(void (*)(void *, void *), 0xD0)(object + 0x30, name);
    if (RW_API(u32 (*)(void *), 0xF4)(name) > 0x1F) {
        RW_ERROR(0x8000001E);
        object[0x4F] = 0;
    }
    return object;
}

void *fn_8048CFAC(u8 *parent, u8 *child) {
    if (*(u32 *)(child + 4) != 0) {
        **(u32 **)(child + 0xC) = *(u32 *)(child + 8);
        *(u32 *)(*(u32 *)(child + 8) + 4) = *(u32 *)(child + 0xC);
    }
    *(u8 **)(child + 4) = parent;
    *(u8 **)(child + 8) = *(u8 **)(parent + 8);
    *(u8 **)(child + 0xC) = parent + 8;
    **(u32 **)(parent + 8 + 4) = (u32)(child + 8);
    *(u32 *)(parent + 8) = (u32)(child + 8);
    return child;
}

void *fn_8048CFFC(u8 *node, char *name) {
    u32 **list = (u32 **)(node + 8);
    for (u32 *link = *list; link != (u32 *)list; link = (u32 *)*link) {
        u8 *text = (u8 *)(link + 2);
        char *other = name;
        u8 a;
        u8 b;
        do {
            a = *text;
            if (a == 0 || (b = *other) == 0) break;
            if (a >= '`' && a < '{') a -= 0x20;
            if (b >= '`' && b < '{') b -= 0x20;
            if (a != b) goto next;
            text++;
            other++;
        } while (true);
        if (a == *other) return link - 2;
    next:;
    }
    return NULL;
}

void fn_8048D0C4(void *object) {
    *(void **)(lbl_805F15B4 + lbl_805F15D4 + 0x10) = object;
}

void *fn_8048D0D8(void) {
    return *(void **)(lbl_805F15B4 + lbl_805F15D4 + 0x10);
}

u32 fn_8048D0EC(char *a, char *b, u32 c, u32 d) {
    u32 (*callback)(char *, char *, u32, u32) = RW_API(u32 (*)(char *, char *, u32, u32), 0x30);
    return callback != NULL ? callback(a, b, c, d) : 0;
}

void *fn_8048D130(char *name, u32 mode) {
    u32 *object = RW_API(u32 *(*)(char *, u32), lbl_805F15D4 + 0x18)(name, mode);
    if (object != NULL) {
        object[0x15]++;
        return object;
    }
    object = RW_API(u32 *(*)(char *, u32), lbl_805F15D4 + 0x14)(name, mode);
    if (object == NULL) {
        RW_ERROR(0x16);
        return NULL;
    }
    u8 *parent = *(u8 **)(lbl_805F15B4 + lbl_805F15D4 + 0x10);
    if (parent != NULL) fn_8048CFAC(parent, (u8 *)object);
    return object;
}

void fn_8048D280(void *a, void *b, void *c, void *d, void *e) {
    fn_80481DC8(&lbl_8056E030, a, b, c, d, e);
}

u32 fn_8048D2C0(void) {
    return RW_API(u32 (*)(void), 0x2C)() != 0;
}

void *fn_8048D300(void *object) {
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x24) != 0) {
        RW_API(void (*)(void), 0x10C)();
        *(u32 *)(lbl_805F15B4 + lbl_805F15D4 + 0x24) = 0;
        *(u16 *)(lbl_805F15B4 + lbl_805F15D4 + 0x28) = 0;
    }
    u32 *base = (u32 *)(lbl_805F15B4 + lbl_805F15D4);
    if (base[2] != 0 && base[3] != 0 && lbl_805F15D0 != NULL) {
        if (base[4] == (u32)lbl_805F15D0) base[4] = 0;
        fn_8048CB38((u8 *)lbl_805F15D0);
        lbl_805F15D0 = NULL;
    }
    if (base[2] != 0) {
        fn_8047FCDC((void *)base[2]);
        base[2] = 0;
    }
    if (base[3] != 0) {
        fn_8047FCDC((void *)base[3]);
        base[3] = 0;
    }
    DAT_805f15d8--;
    return object;
}

void *fn_8048D4DC(void *object, s32 offset) {
    lbl_805F15D4 = offset;
    void *memory = fn_8047FC04(lbl_8056E030, lbl_805EDF0C, 4, lbl_805EDF10, lbl_805E2D10, 0x40006);
    *(void **)(lbl_805F15B4 + offset + 8) = memory;
    if (memory == NULL) return NULL;
    memory = fn_8047FC04(lbl_8056E048, lbl_805EDF14, 4, lbl_805EDF18, lbl_805E2D34, 0x40408);
    *(void **)(lbl_805F15B4 + offset + 0xC) = memory;
    if (memory == NULL) {
        fn_8047FCDC(*(void **)(lbl_805F15B4 + offset + 8));
        *(u32 *)(lbl_805F15B4 + offset + 8) = 0;
        return NULL;
    }
    u8 *base = lbl_805F15B4 + offset;
    *(u32 *)base = (u32)base;
    *(u32 *)(base + 4) = (u32)base;
    DAT_805f15d8++;
    lbl_805F15D0 = (u8 *)fn_8048CA64();
    *(u8 **)(base + 0x10) = lbl_805F15D0;
    if (lbl_805F15D0 == NULL) {
        fn_8047FCDC(*(void **)(base + 0xC));
        *(u32 *)(base + 0xC) = 0;
        fn_8047FCDC(*(void **)(base + 8));
        *(u32 *)(base + 8) = 0;
        return NULL;
    }
    *(u32 *)(base + 0x1C) = 0;
    *(u32 *)(base + 0x20) = 0;
    *(void **)(base + 0x18) = fn_8048C91C;
    *(void **)(base + 0x14) = fn_8048C5F4;
    *(void **)(base + 0x2C) = fn_8048C634;
    *(void **)(base + 0x30) = fn_8048B6AC;
    *(u32 *)(base + 0x24) = 0;
    *(u16 *)(base + 0x28) = 0;
    return object;
}

void fn_8048D7C0(u8 *child, u8 *parent) {
    if (*(u32 *)(child + 4) != 0) {
        **(u32 **)(child + 0xC) = *(u32 *)(child + 8);
        *(u32 *)(*(u32 *)(child + 8) + 4) = *(u32 *)(child + 0xC);
    }
    *(u8 **)(child + 4) = parent;
    if (parent != NULL) {
        *(u8 **)(child + 8) = *(u8 **)(parent + 0x90);
        *(u8 **)(child + 0xC) = parent + 0x90;
        **(u32 **)(parent + 0x90 + 4) = (u32)(child + 8);
        *(u32 *)(parent + 0x90) = (u32)(child + 8);
        fn_8048722C(parent);
    }
}

void fn_8048D834(u8 *object) {
    if (*(u32 *)(object + 4) == 0) return;
    **(u32 **)(object + 0xC) = *(u32 *)(object + 8);
    *(u32 *)(*(u32 *)(object + 8) + 4) = *(u32 *)(object + 0xC);
}

void fn_8048D85C(f32 *color) {
    color[0] = color[0] / lbl_805FB360;
    color[2] = color[2] / lbl_805FB364;
}

void fn_8048D880(f32 *color) {
    ((u8 *)color)[0x18] = 0;
    f32 value = lbl_805FB368;
    color[0] = value;
    color[1] = value;
    color[2] = value;
    color[3] = value;
    color[4] = value;
    color[5] = value;
}

void fn_8048D8A8(f32 *color, u8 *rgba) {
    color[5] -= (color[4] * color[4] + color[3] * color[3] +
                 color[1] * color[1] + color[2] * color[2]) / color[0];
    if (color[5] < lbl_805FB368) color[5] = lbl_805FB368;
    f32 weight = color[0];
    color[1] += lbl_805FB36C * rgba[0] * lbl_805FB360 * weight;
    color[2] += lbl_805FB36C * rgba[1] * weight;
    color[3] += lbl_805FB36C * rgba[2] * lbl_805FB364 * weight;
    color[4] += lbl_805FB36C * rgba[3] * weight;
}

void fn_8048D9C8(f32 *out, f32 *a, f32 *b) {
    out[5] = a[5] + b[5];
    if (lbl_805FB368 < a[0] && lbl_805FB368 < b[0]) {
        f32 ia = lbl_805FB378 / a[0];
        f32 ib = lbl_805FB378 / b[0];
        f32 x = a[2] * ia - b[2] * ib;
        f32 y = a[1] * ia - b[1] * ib;
        f32 z = a[3] * ia - b[3] * ib;
        f32 w = a[4] * ia - b[4] * ib;
        out[5] += (w * w + z * z + y * y + x * x) / (ia + ib);
    }
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
    out[3] = a[3] + b[3];
    out[4] = a[4] + b[4];
    out[0] = a[0] + b[0];
}

void fn_8048DAC8(u8 *out, f32 *color) {
    f32 scale = lbl_805FB378 / color[0];
    s32 r = (s32)(lbl_805FB37C * (color[1] * scale / lbl_805FB360));
    s32 g = (s32)(lbl_805FB37C * color[2] * scale);
    s32 b = (s32)(lbl_805FB37C * (color[3] * scale / lbl_805FB364));
    s32 a = (s32)(lbl_805FB37C * color[4] * scale);
    out[0] = r < 0xFF ? (u8)r : 0xFF;
    out[1] = g < 0xFF ? (u8)g : 0xFF;
    out[2] = b < 0xFF ? (u8)b : 0xFF;
    out[3] = a < 0xFF ? (u8)a : 0xFF;
}

void fn_8048DBA4(u32 *object) {
    object[0] = 0;
    object[1] = 0;
    object[2] = 0;
    object[3] = 0;
    object[4] = 0;
    object[5] = 0;
    object[6] = 0;
    object[7] = 0;
    object[8] = 0;
    object[9] = 0;
    object[10] = 0;
    object[11] = 0;
    object[12] = 0;
    object[13] = 0;
    object[14] = 0;
    object[15] = 0;
}
}
