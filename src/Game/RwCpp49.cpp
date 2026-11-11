/* RenderWare texture allocation, conversion, and dictionary loading, 0x804875B0..0x8048915B.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F15C0;
extern s32 DAT_805f15c4;
extern u32 lbl_8056DFE8[];
extern u32 lbl_8056E000[];
extern u32 lbl_805EDEE8;
extern u32 lbl_805EDEEC;
extern u32 lbl_805EDEF0;
extern u32 lbl_805E2CA0;
extern u32 lbl_805E2CC4;
extern f32 lbl_805FB308;
extern f32 lbl_805FB30C;
extern f32 lbl_805FB310;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_8047F59C(void *);
void fn_8047F398(void *);
void fn_8047F72C(void *);
void fn_8047EFC8(void *);
void fn_80481DC8(void *, void *, void *, void *, void *, void *);
void *fn_8047FC04(u32, u32, u32, u32, u32, u32);
void fn_8047FCDC(void *);
void fn_8048209C(void *, void *);
void fn_80482144(void *, void *);
u32 fn_8047A67C(void *);
u32 fn_80483444(void);
u32 fn_80489830(f32);
void fn_804893B8(void *, void *);
void fn_804886CC(char *, u32, void *, void *);
u32 fn_80488A04(char *, u32 *);
u32 fn_80488A70(char *, u32 *);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))
#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)

static inline void rwLinkNode(u8 *object) {
    u8 *node = *(u8 **)(object + 0xA0);
    u8 flags = node[3];
    if ((flags & 3) == 0) {
        *(u32 *)(node + 8) = *(u32 *)(lbl_805F15B4 + 0xBC);
        *(u32 *)(node + 0xC) = (u32)(lbl_805F15B4 + 0xBC);
        **(u32 **)(lbl_805F15B4 + 0xBC + 4) = (u32)(node + 8);
        *(u32 *)(lbl_805F15B4 + 0xBC) = (u32)(node + 8);
    }
    node[3] = flags | 3;
    object[3] |= 0xC;
}

void *fn_804875B0(u8 *object) {
    fn_8047F59C(object + 0x10);
    rwLinkNode(object);
    return object;
}

void *fn_8048764C(u8 *object) {
    fn_8047F398(object + 0x10);
    rwLinkNode(object);
    return object;
}

void *fn_804876E8(u8 *object) {
    fn_8047F72C(object + 0x10);
    rwLinkNode(object);
    return object;
}

void *fn_80487784(u8 *object) {
    fn_8047EFC8(object + 0x10);
    rwLinkNode(object);
    return object;
}

void fn_80487820(u8 *object) {
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
    rwLinkNode(object);
}

void fn_804878D4(void *a, void *b, void *c, void *d, void *e) {
    fn_80481DC8(&lbl_8056DFE8, a, b, c, d, e);
}

void *fn_80487914(void *object, s32 offset) {
    lbl_805F15C0 = offset;
    void *memory = fn_8047FC04((u32)lbl_8056E000, lbl_805EDEE8, 4, lbl_805EDEEC, lbl_805E2CA0, 0x40018);
    *(void **)(lbl_805F15B4 + offset) = memory;
    if (memory == NULL) return NULL;
    memory = fn_8047FC04(0x34, lbl_805EDEE8, 4, lbl_805EDEF0, lbl_805E2CC4, 0x40406);
    *(void **)(lbl_805F15B4 + offset + 0x218) = memory;
    if (memory == NULL) {
        fn_8047FCDC(*(void **)(lbl_805F15B4 + offset));
        *(u32 *)(lbl_805F15B4 + offset) = 0;
        return NULL;
    }
    *(u32 *)(lbl_805F15B4 + offset + 8) = 0x100;
    void *buffer = RW_API(void *(*)(u32, u32), 0x108)(0x100, 0x1040406);
    *(void **)(lbl_805F15B4 + offset + 4) = buffer;
    if (buffer == NULL) {
        fn_8047FCDC(*(void **)(lbl_805F15B4 + offset + 0x218));
        *(u32 *)(lbl_805F15B4 + offset + 0x218) = 0;
        fn_8047FCDC(*(void **)(lbl_805F15B4 + offset));
        *(u32 *)(lbl_805F15B4 + offset) = 0;
        return NULL;
    }
    *(u8 *)buffer = 0;
    DAT_805f15c4++;
    fn_80489830(lbl_805FB310);
    *(u32 *)(lbl_805F15B4 + offset + 0x21C) = 0;
    *(u32 *)(lbl_805F15B4 + offset + 0x214) = 0x100;
    buffer = RW_API(void *(*)(u32, u32), 0x108)(0x100, 0x1040018);
    *(void **)(lbl_805F15B4 + offset + 0x210) = buffer;
    if (buffer == NULL) {
        RW_API(void (*)(void *), 0x10C)(*(void **)(lbl_805F15B4 + offset + 4));
        *(u32 *)(lbl_805F15B4 + offset + 4) = 0;
        *(u32 *)(lbl_805F15B4 + offset + 8) = 0;
        fn_8047FCDC(*(void **)(lbl_805F15B4 + offset + 0x218));
        *(u32 *)(lbl_805F15B4 + offset + 0x218) = 0;
        fn_8047FCDC(*(void **)(lbl_805F15B4 + offset));
        *(u32 *)(lbl_805F15B4 + offset) = 0;
        return NULL;
    }
    return object;
}

void *fn_80487BB0(void *object) {
    void *memory = *(void **)(lbl_805F15B4 + lbl_805F15C0 + 0x210);
    if (memory != NULL) {
        RW_API(void (*)(void *), 0x10C)(memory);
        *(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 0x210) = 0;
        *(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 0x214) = 0;
    }
    memory = *(void **)(lbl_805F15B4 + lbl_805F15C0 + 4);
    if (memory != NULL) {
        RW_API(void (*)(void *), 0x10C)(memory);
        *(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 4) = 0;
        *(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 8) = 0;
    }
    u8 *base = lbl_805F15B4 + lbl_805F15C0;
    while (*(u32 *)(base + 0x21C) != 0) {
        u32 *record = *(u32 **)(base + 0x21C);
        *(u32 *)(base + 0x21C) = *(u32 *)((u8 *)record + 0x30);
        RW_API(void (*)(void *, void *), 0x11C)(*(void **)(base + 0x218), record);
    }
    if (*(u32 *)(base + 0x218) != 0) {
        fn_8047FCDC(*(void **)(base + 0x218));
        *(u32 *)(base + 0x218) = 0;
    }
    if (*(u32 *)base != 0) {
        fn_8047FCDC(*(void **)base);
        *(u32 *)base = 0;
    }
    DAT_805f15c4--;
    return object;
}

u32 *fn_80487D08(u32 width, u32 height, u32 kind) {
    u32 *texture = (u32 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(lbl_805F15B4 + lbl_805F15C0), 0x30018);
    if (texture == NULL) return NULL;
    texture[1] = width;
    texture[2] = height;
    texture[3] = kind;
    texture[5] = 0;
    texture[6] = 0;
    texture[0] = 0;
    fn_8048209C(&lbl_8056E000, texture);
    return texture;
}

u32 fn_80487DB0(u32 *texture) {
    if ((texture[0] & 1) != 0) {
        extern u32 *fn_80487F50(u32 *);
        fn_80487F50(texture);
    }
    fn_80482144(&lbl_8056E000, texture);
    RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15C0), texture);
    return 1;
}

u32 *fn_80487E18(u32 *texture) {
    u32 kind = texture[3];
    u32 indexed = kind == 4 || kind == 8;
    texture[4] = (((kind + 7) >> 3) * texture[1] + 3) & ~3;
    u32 pixelBytes = texture[4] * texture[2];
    u32 bytes = pixelBytes + (indexed ? (1 << kind) * 4 : 0);
    texture[5] = (u32)RW_API(void *(*)(u32, u32), 0x108)(bytes, 0x30018);
    if (texture[5] == 0) {
        RW_ERROR(0x80000013);
        return NULL;
    }
    texture[6] = indexed ? texture[5] + pixelBytes : 0;
    texture[0] |= 1;
    return texture;
}

u32 *fn_80487F50(u32 *texture) {
    RW_API(void (*)(void *), 0x10C)((void *)texture[5]);
    texture[5] = 0;
    texture[6] = 0;
    texture[0] &= ~1;
    return texture;
}

void fn_80487FA8(u32 *texture) {
    s32 kind = (s32)texture[3];
    switch (kind) {
    case 4:
    case 8: {
        u8 *palette = (u8 *)texture[6];
        for (s32 i = 0; i < (1 << kind); i++) {
            s32 alpha = palette[0];
            if (palette[1] > alpha) alpha = palette[1];
            if (palette[2] > alpha) alpha = palette[2];
            palette[3] = alpha;
            palette += 4;
        }
        break;
    }
    case 0x20: {
        u8 *pixels = (u8 *)texture[5];
        for (s32 row = 0; row < (s32)texture[2]; row++) {
            u8 *pixel = pixels;
            for (s32 x = 0; x < (s32)texture[1]; x++) {
                s32 alpha = pixel[0];
                if (pixel[1] > alpha) alpha = pixel[1];
                if (pixel[2] > alpha) alpha = pixel[2];
                pixel[3] = alpha;
                pixel += 4;
            }
            pixels += texture[4];
        }
        break;
    }
    }
}

u32 *fn_80488088(u32 *dest, u32 *source) {
    if (dest[1] != source[1] || dest[2] != source[2]) {
        RW_ERROR(0x8000000A);
        return NULL;
    }
    u32 kind = dest[3];
    if (kind != 4 && kind != 8 && kind != 0x20) {
        RW_ERROR(0x80000009);
        return NULL;
    }
    if (kind != 0x20) {
        u32 *converted = fn_80487D08(dest[1], dest[2], kind);
        if (converted == NULL) return NULL;
        if (fn_80487E18(converted) == NULL) {
            fn_80487DB0(converted);
            return NULL;
        }
        fn_804893B8(converted, dest);
        fn_80487F50(dest);
        dest[3] = 0x20;
        if (fn_80487E18(dest) == NULL) {
            fn_80487DB0(converted);
            return NULL;
        }
        fn_804893B8(dest, converted);
        fn_80487DB0(converted);
    }
    u8 *input = (u8 *)source[5];
    u8 *output = (u8 *)dest[5];
    for (u32 row = 0; row < dest[2]; row++) {
        u8 *in = input;
        u8 *out = output;
        for (u32 x = 0; x < dest[1]; x++) {
            if (source[3] == 0x20) {
                out[3] = in[3];
                in += 4;
            } else {
                out[3] = *(u8 *)(source[6] + *in * 4 + 3);
                in++;
            }
            out += 4;
        }
        input += source[4];
        output += dest[4];
    }
    return dest;
}

void *fn_804885DC(char *name) {
    u32 bytes = RW_API(u32 (*)(void *), 0xF4)(name) + 1;
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 8) < bytes) {
        void *buffer = RW_API(void *(*)(void *, u32, u32), 0x110)(
            *(void **)(lbl_805F15B4 + lbl_805F15C0 + 4), bytes, 0x1040406);
        if (buffer == NULL) {
            RW_ERROR(0x80000013);
            return NULL;
        }
        *(void **)(lbl_805F15B4 + lbl_805F15C0 + 4) = buffer;
        *(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 8) = bytes;
    }
    memcpy(*(void **)(lbl_805F15B4 + lbl_805F15C0 + 4), name, bytes);
    return name;
}

void fn_804886CC(char *prefix, u32 extra, void *callback, void *data) {
    fn_80483444();
    char *source = *(char **)(lbl_805F15B4 + lbl_805F15C0 + 4);
    while (source != NULL && source[0] != 0) {
        char *separator = RW_API(char *(*)(char *, u32), 0xE0)(source, 0x3B);
        char *next = separator;
        u32 segment;
        if (separator == NULL) {
            segment = RW_API(u32 (*)(void *), 0xF4)(source);
            next = NULL;
        } else {
            segment = (u32)(separator - source);
            next++;
        }
        u32 required = segment + RW_API(u32 (*)(void *), 0xF4)(prefix) + extra;
        if (*(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 0x214) < required) {
            void *old = *(void **)(lbl_805F15B4 + lbl_805F15C0 + 0x210);
            void *buffer = old == NULL ? RW_API(void *(*)(u32, u32), 0x108)(required, 0x1040018)
                                       : RW_API(void *(*)(void *, u32, u32), 0x110)(old, required, 0x1040018);
            if (buffer == NULL) {
                RW_ERROR(0x80000013);
                return;
            }
            *(void **)(lbl_805F15B4 + lbl_805F15C0 + 0x210) = buffer;
            *(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 0x214) = required;
        }
        char *dest = *(char **)(lbl_805F15B4 + lbl_805F15C0 + 0x210);
        memcpy(dest, source, segment);
        RW_API(void (*)(char *, char *), 0xCC)(dest + segment, prefix);
        u32 (*call)(char *, void *) = (u32 (*)(char *, void *))callback;
        if (call(dest, data) == 0) return;
        source = next;
    }
    u32 required = RW_API(u32 (*)(void *), 0xF4)(prefix) + extra;
    if (*(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 0x214) < required) {
        void *old = *(void **)(lbl_805F15B4 + lbl_805F15C0 + 0x210);
        void *buffer = old == NULL ? RW_API(void *(*)(u32, u32), 0x108)(required, 0x1040018)
                                   : RW_API(void *(*)(void *, u32, u32), 0x110)(old, required, 0x1040018);
        if (buffer == NULL) {
            RW_ERROR(0x80000013);
            return;
        }
        *(void **)(lbl_805F15B4 + lbl_805F15C0 + 0x210) = buffer;
        *(u32 *)(lbl_805F15B4 + lbl_805F15C0 + 0x214) = required;
    }
    char *dest = *(char **)(lbl_805F15B4 + lbl_805F15C0 + 0x210);
    RW_API(void (*)(char *, char *), 0xCC)(dest, prefix);
    ((u32 (*)(char *, void *))callback)(dest, data);
}

u32 fn_80488A04(char *name, u32 *entry) {
    if (fn_8047A67C(name) != 0) {
        entry[1] = ((u32 (*)(char *))entry[0])(name);
        if (entry[1] != 0) return 0;
    }
    return (u32)name;
}

u32 fn_80488A70(char *name, u32 *entry) {
    u32 length = RW_API(u32 (*)(void *), 0xF4)(name);
    u32 *record = *(u32 **)(lbl_805F15B4 + lbl_805F15C0 + 0x21C);
    while (record != NULL) {
        RW_API(void (*)(char *, char *), 0xCC)(name + length, (char *)record);
        if (fn_8047A67C(name) != 0) {
            *entry = (u32)record;
            return 0;
        }
        RW_API(void (*)(char *, char *), 0xCC)(name + length, (char *)record + 0x14);
        if (fn_8047A67C(name) != 0) {
            *entry = (u32)record + 0x14;
            return 0;
        }
        record = *(u32 **)((u8 *)record + 0x30);
    }
    return (u32)name;
}

void *fn_80488B58(char *name) {
    u32 found = 0;
    fn_804886CC(name, 0x14, fn_80488A70, &found);
    return (void *)found;
}

static char *rwLastComponent(char *name) {
    char *part = RW_API(char *(*)(char *, u32), 0xDC)(name, 0x2F);
    if (part != NULL) name = part;
    part = RW_API(char *(*)(char *, u32), 0xDC)(name, 0x5C);
    if (part != NULL) name = part;
    return RW_API(char *(*)(char *, u32), 0xDC)(name, 0x2E);
}

u32 *fn_80488B94(char *name, char *suffix) {
    char *extension = RW_API(char *(*)(char *, u32), 0xDC)(name, 0x2E);
    if (extension != NULL) name = extension;
    extension = rwLastComponent(name);
    u32 *resource = NULL;
    if (extension != NULL) {
        for (u32 *record = *(u32 **)(lbl_805F15B4 + lbl_805F15C0 + 0x21C); record != NULL;
             record = *(u32 **)((u8 *)record + 0x30)) {
            if (RW_API(u32 (*)(void *, void *), 0xE8)(record, extension) == 0 ||
                RW_API(u32 (*)(void *, void *), 0xE8)((u8 *)record + 0x14, extension) == 0) {
                if (record[10] == 0) {
                    resource = NULL;
                } else {
                    u32 size = record[10];
                    fn_804886CC(name, 5, fn_80488A04, &size);
                    resource = (u32 *)size;
                }
                break;
            }
        }
    }
    if (resource == NULL || suffix == NULL || suffix[0] == 0) return resource;
    char *part = RW_API(char *(*)(char *, u32), 0xDC)(suffix, 0x3A);
    if (part != NULL) suffix = part;
    extension = rwLastComponent(suffix);
    u32 *other = NULL;
    if (extension != NULL) {
        for (u32 *record = *(u32 **)(lbl_805F15B4 + lbl_805F15C0 + 0x21C); record != NULL;
             record = *(u32 **)((u8 *)record + 0x30)) {
            if (RW_API(u32 (*)(void *, void *), 0xE8)(record, extension) == 0 ||
                RW_API(u32 (*)(void *, void *), 0xE8)((u8 *)record + 0x14, extension) == 0) {
                if (record[10] == 0) {
                    other = NULL;
                } else {
                    u32 size = record[10];
                    fn_804886CC(suffix, 5, fn_80488A04, &size);
                    other = (u32 *)size;
                }
                break;
            }
        }
    }
    if (other == NULL) {
        fn_80487DB0(resource);
        return NULL;
    }
    fn_80487FA8(other);
    if (fn_80488088(resource, other) == NULL) {
        fn_80487DB0(resource);
        fn_80487DB0(other);
        return NULL;
    }
    fn_80487DB0(other);
    return resource;
}

void *fn_80489118(void *object, void *arg, void *arg2) {
    RW_API(void (*)(void *, void *, void *), 0x54)(object, &arg, arg2);
    return object;
}
}
