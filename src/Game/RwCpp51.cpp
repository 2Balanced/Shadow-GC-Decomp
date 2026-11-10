/* RenderWare scene-graph traversal and palette/resource management, 0x8048A96C..0x8048C037.
 * Names are inferred from behavior; recursion replaces the decompiler's unrolled traversals. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern u8 lbl_805EDF00[];
extern const char lbl_8051C2D0[];

void fn_8047EBB8(void *, const void *, const void *);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
int fn_8048FED8(void *);
void fn_8048DEB0(void *, f32, void *);
void fn_8048F5D0(void *, u32, void *);
void fn_8048FAB0(u32, u32, u32, u32, void *, void *);
void *fn_80487D08(u32, u32, u32);
u32 fn_80487E18(void);
void fn_80487DB0(void *);
void fn_804903F8(void *);
void fn_804893B8(void *, void *);
void fn_8048A790(void *, void *);
char *fn_80488B58(char *);
void *fn_80488B94(char *, char *);
void *fn_80489A90(void *, u32, u32 *, u32 *, void *, void *);
void *fn_80489E74(u32, u32, void *, void *);
void fn_804894B8(void *);
void *fn_80489A1C(void *, void *);
void fn_80489BDC(void *);
void *fn_8048CCDC(void *);
void fn_8048CE54(void *, char *);
void fn_8048CF00(void *, char *);
void fn_8048D0EC(char *, char *, u32, u32);

static inline void rwCallbacks(u8 *node) {
    u32 **list = (u32 **)(node + 0x90);
    u32 *link = *list;
    if (link != (u32 *)list) {
        for (; link != (u32 *)list; link = (u32 *)*link) {
            (*(void (**)(u8 *))(link + 2))((u8 *)(link - 2));
        }
    }
}

static inline void rwCopyTransform(u8 *node) {
    u32 *source = (u32 *)(node + 0x10);
    u32 *dest = (u32 *)(node + 0x50);
    for (u32 i = 0; i < 0x10; i++) dest[i] = source[i];
}

static inline void rwUpdateTransform(u8 *node, u32 inherited) {
    if (((inherited | node[3]) & 4) != 0) {
        fn_8047EBB8(node + 0x50, node + 0x10, *(u8 **)(node + 4) + 0x50);
    }
}

extern f32 lbl_805FB348;
extern f32 lbl_805FB34C;
extern f32 lbl_805FB350;
extern f32 lbl_805FB358;
void fn_80487F50(void *);
void fn_8048A50C(u32 *, u32, u32, u32, u32, f32 *);

void *fn_8048A96C(u32 *source, u32 width, u32 height) {
    u32 *target = (u32 *)fn_80487D08(width, height, 0x20);
    if (target == NULL) return NULL;
    if (fn_80487E18() == 0) {
        fn_80487DB0(target);
        return NULL;
    }
    u32 *sample = source;
    bool converted = false;
    if (source[3] != 0x20) {
        sample = (u32 *)fn_80487D08(source[1], source[2], 0x20);
        if (sample == NULL) {
            fn_80487F50(target);
            fn_80487DB0(target);
            return NULL;
        }
        if (fn_80487E18() == 0) {
            fn_80487DB0(sample);
            fn_80487F50(target);
            fn_80487DB0(target);
            return NULL;
        }
        fn_804893B8(sample, source);
        converted = true;
    }
    target[0] |= sample[0] & 2;
    s32 stepX = (s32)(lbl_805FB348 * ((f32)target[1] / (f32)sample[1]));
    s32 stepY = (s32)(lbl_805FB348 * ((f32)target[2] / (f32)sample[2]));
    s32 y = 0;
    for (u32 row = 0; row < target[2]; row++) {
        u8 *output = (u8 *)target[5] + target[4] * row;
        s32 x = 0;
        for (u32 column = 0; column < target[1]; column++) {
            f32 color[4];
            fn_8048A50C(sample, x, x + stepX - 1, y, y + stepY - 1, color);
            output[0] = (u8)(lbl_805FB350 * color[0] + lbl_805FB34C);
            output[1] = (u8)(lbl_805FB350 * color[1] + lbl_805FB34C);
            output[2] = (u8)(lbl_805FB350 * color[2] + lbl_805FB34C);
            output[3] = (u8)(lbl_805FB350 * color[3] + lbl_805FB34C);
            output += 4;
            x += stepX;
        }
        y += stepY;
    }
    if (converted) {
        fn_80487F50(sample);
        fn_80487DB0(sample);
    }
    return target;
}

void fn_8048ADE0(u8 *node, u32 inherited) {
    for (; node != NULL; node = *(u8 **)(node + 0x9C)) {
        u8 flags = node[3];
        rwUpdateTransform(node, inherited);
        rwCallbacks(node);
        node[3] &= 0xF3;
        fn_8048ADE0(*(u8 **)(node + 0x98), inherited | flags);
    }
}

void fn_8048AFF0(u8 *node) {
    for (; node != NULL; node = *(u8 **)(node + 0x9C)) {
        rwCallbacks(node);
        node[3] &= 0xF7;
        fn_8048AFF0(*(u8 **)(node + 0x98));
    }
}

void fn_8048B208(u8 *node) {
    u8 flags = node[3];
    if ((flags & 1) == 0) {
        rwCallbacks(node);
        fn_8048AFF0(*(u8 **)(node + 0x98));
    } else {
        if ((flags & 4) != 0) rwCopyTransform(node);
        rwCallbacks(node);
        fn_8048ADE0(*(u8 **)(node + 0x98), flags & 4);
    }
    node[3] = flags & 0xF0;
}

u32 fn_8048B354(void) {
    u8 *base = lbl_805F15B4;
    u32 *list = (u32 *)(base + 0xBC);
    for (u32 *link = *(u32 **)list; link != list; link = (u32 *)*link) {
        fn_8048B208((u8 *)(link - 2));
    }
    *(u32 *)(base + 0xBC) = (u32)(base + 0xBC);
    *(u32 *)(base + 0xC0) = (u32)(base + 0xBC);
    return 1;
}

void fn_8048B3C0(u8 *node, u32 inherited) {
    while (node != NULL) {
        u8 flags = node[3];
        if (((inherited | flags) & 4) != 0) {
            rwUpdateTransform(node, inherited);
            node[3] &= 0xFB;
        }
        fn_8048B3C0(*(u8 **)(node + 0x98), inherited | flags);
        node = *(u8 **)(node + 0x9C);
    }
}

void fn_8048B5D8(u8 *node) {
    u8 flags = node[3];
    if ((flags & 4) != 0) rwCopyTransform(node);
    fn_8048B3C0(*(u8 **)(node + 0x98), flags);
    node[3] = flags & 0xFA;
}

u32 fn_8048B6AC(void *context, char *name, u8 value) {
    struct {
        u8 kind;
        s8 digit;
        u8 terminator;
    } output;
    output.kind = 0x6D;
    int valid = value != 0 && value < 0x10;
    output.digit = valid ? lbl_8051C2D0[value] : 0;
    output.terminator = 0;
    if (output.digit != 0) {
        ((void (*)(void *, void *))*(u32 *)(lbl_805F15B4 + 0xD4))(context, &output);
        if (name != NULL && name[0] != 0) {
            ((void (*)(void *, void *))*(u32 *)(lbl_805F15B4 + 0xD4))(name, &output);
        }
    }
    return 1;
}

u32 fn_8048B774(u32 **texture, u32 flags) {
    u32 palette[256];
    u8 work[0x4000];
    if (fn_8048FED8(work) == 0) return 0;
    fn_8048DEB0(work, lbl_805FB358, *texture);
    u32 count = 1 << flags;
    fn_8048F5D0(palette, count, work);
    u32 *replacement = (u32 *)fn_80487D08(*(u32 *)(*texture + 1), *(u32 *)(*texture + 2), flags);
    if (replacement == NULL) return 0;
    fn_80487E18();
    fn_8048FAB0(replacement[5], replacement[4], replacement[3], 0, work, *texture);
    memcpy((void *)replacement[6], palette, count * 4);
    fn_80487DB0(*texture);
    *texture = replacement;
    fn_804903F8(work);
    return 1;
}

u32 fn_8048B860(void *dest, u32 keep, u32 **textures, s32 count, u32 flags) {
    u32 *first = textures[0];
    u32 *palette = (u32 *)first[6];
    s32 index;
    if (palette != NULL) {
        u32 entries = 1 << flags;
        for (index = 1; index < count; index++) {
            u32 *other = (u32 *)textures[index][6];
            if (palette == NULL || other == NULL) {
                index = 0x40;
                break;
            }
            for (u32 i = 0; i < entries; i++) {
                if (palette[i] != other[i]) {
                    index = 0x40;
                    break;
                }
            }
            if (index == 0x40) break;
        }
        if (index == count) {
            memcpy(dest, palette, (1 << first[3]) * 4);
            return 1;
        }
    }
    u8 work[0x4000];
    if (fn_8048FED8(work) == 0) return 0;
    for (index = 0; index < count; index++) fn_8048DEB0(work, lbl_805FB358, textures[index]);
    fn_8048F5D0(dest, 1 << flags, work);
    for (index = 0; index < count; index++) {
        u32 *old = textures[index];
        u32 *replacement = (u32 *)fn_80487D08(old[1], old[2], flags);
        if (replacement == NULL) return 0;
        fn_80487E18();
        fn_8048FAB0(replacement[5], replacement[4], replacement[3], 0, work, old);
        replacement[6] = (u32)dest;
        textures[index] = replacement;
        if ((u32)old != keep) fn_80487DB0(old);
    }
    fn_804903F8(work);
    return 1;
}

void *fn_8048BA30(char *name, char *suffix, u32 kind, u32 *width, u32 *height, void *a, void *b) {
    char first[0x100];
    char second[0x100];
    ((void (*)(void *, void *, u32))*(u32 *)(lbl_805F15B4 + 0xD0))(first, name, 0x100);
    u32 length = ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))(name);
    if (length > 0xFF) {
        u32 error[2] = {1, fn_8047DC28(0x8000001E, name, 0x100, 0xFF, name[0xFF])};
        fn_8047DB84(error);
        first[0xFF] = 0;
    }
    char *found = fn_80488B58(name);
    if (found != NULL) ((void (*)(void *, void *))*(u32 *)(lbl_805F15B4 + 0xD4))(first, found);
    second[0] = 0;
    if (suffix != NULL && suffix[0] != 0) {
        ((void (*)(void *, void *, u32))*(u32 *)(lbl_805F15B4 + 0xD0))(second, suffix, 0x100);
        length = ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))(suffix);
        if (length > 0xFF) {
            u32 error[2] = {1, fn_8047DC28(0x8000001E, suffix, 0x100, 0xFF, suffix[0xFF])};
            fn_8047DB84(error);
            second[0xFF] = 0;
        }
        found = fn_80488B58(suffix);
        if (found != NULL) ((void (*)(void *, void *))*(u32 *)(lbl_805F15B4 + 0xD4))(second, found);
    }
    u32 *resource = (u32 *)fn_80488B94(first, second);
    if (resource == NULL) return NULL;
    if ((*width == 0 || *height == 0) && fn_80489A90(resource, kind, width, height, a, b) == NULL) {
        fn_80487DB0(resource);
        u32 error[2] = {1, fn_8047DC28(0x80000009)};
        fn_8047DB84(error);
        return NULL;
    }
    if (resource[1] != *width || resource[2] != *height) {
        u32 oldKind = resource[3];
        if (oldKind != 0x20) {
            u32 *original = resource;
            resource = (u32 *)fn_80487D08(original[1], original[2], 0x20);
            if (resource == NULL) {
                fn_80487DB0(original);
                return NULL;
            }
            if (fn_80487E18() == 0) {
                fn_80487DB0(resource);
                fn_80487DB0(original);
                return NULL;
            }
            fn_804893B8(resource, original);
            fn_80487DB0(original);
        }
        u32 *converted = (u32 *)fn_80487D08(*width, *height, 0x20);
        if (converted == NULL) {
            fn_80487DB0(resource);
            return NULL;
        }
        if (fn_80487E18() == 0) {
            fn_80487DB0(converted);
            fn_80487DB0(resource);
            return NULL;
        }
        fn_8048A790(converted, resource);
        fn_80487DB0(resource);
        resource = converted;
        if (oldKind == 4 || oldKind == 8) fn_8048B774(&resource, oldKind);
    }
    return resource;
}

void *fn_8048BD84(char *name, char *suffix) {
    char first[0x100];
    char second[0x100];
    u8 palette[0x400];
    ((void (*)(void *, void *, u32))*(u32 *)(lbl_805F15B4 + 0xD0))(first, name, 0x100);
    u32 length = ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))(name);
    if (length > 0xFF) {
        u32 error[2] = {1, fn_8047DC28(0x8000001E, name, 0x100, 0xFF, name[0xFF])};
        fn_8047DB84(error);
        first[0xFF] = 0;
    }
    second[0] = 0;
    if (suffix != NULL && suffix[0] != 0) {
        ((void (*)(void *, void *, u32))*(u32 *)(lbl_805F15B4 + 0xD0))(second, suffix, 0x100);
        length = ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))(suffix);
        if (length > 0xFF) {
            u32 error[2] = {1, fn_8047DC28(0x8000001E, suffix, 0x100, 0xFF, suffix[0xFF])};
            fn_8047DB84(error);
            second[0xFF] = 0;
        }
    }
    fn_8048D0EC(first, second, 0, 4);
    u32 width = 0;
    u32 height = 0;
    void *a = NULL;
    void *b = NULL;
    u32 *resource = (u32 *)fn_8048BA30(first, second, 4, &width, &height, &a, &b);
    if (resource == NULL) return NULL;
    void *converted = fn_80489E74(width, height, a, b);
    if (converted == NULL) {
        fn_80487DB0(resource);
        return NULL;
    }
    if ((((u8 *)converted)[0x23] & 0x60) != 0) {
        if ((((u8 *)converted)[0x23] & 0x40) == 0) {
            fn_8048B860(palette, 0, &resource, 1, 8);
        } else {
            fn_8048B860(palette, 0, &resource, 1, 4);
        }
        resource[6] = (u32)palette;
    }
    fn_804894B8(resource);
    void *result = fn_80489A1C(converted, resource);
    if (result == NULL) {
        fn_80489BDC(converted);
        fn_80487DB0(resource);
        return NULL;
    }
    fn_80487DB0(resource);
    result = fn_8048CCDC(converted);
    if (result == NULL) {
        fn_80489BDC(converted);
        return NULL;
    }
    fn_8048CE54(result, name);
    fn_8048CF00(result, suffix != NULL ? suffix : (char *)lbl_805EDF00);
    return result;
}
}
