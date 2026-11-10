/* RenderWare collision-tree serialization and dispatch, 0x80442210..0x80443A73.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F1428;

void *fn_80441188(void *, void *, void *, void *, void *);
void *fn_80441BF8(void *, void *, void *, void *, void *);
void *fn_80442210(void *, void *, void *, void *, void *);
u32 *fn_804566D0(u32);
int fn_8047CF50(void *, u32, void *, void *);
int fn_8047D350(void *, void *, u32);
int fn_8047D458(void *, void *, u32);
int fn_8047D784(void *, void *, u32);
int fn_8047D868(void *, void *, u32);
int fn_80480D4C(void *, void *, u32);
int fn_80480ED0(void *, void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
s32 fn_8045994C(u32, u32, void *, void *, void *);
s32 fn_8045998C(u32, void *, void *, void *);

static inline void *rwAlloc(u32 size, u32 tag) {
    return ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, tag);
}
static inline void rwFree(void *memory) {
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(memory);
}

/* fn_80442210: collision traversal, defined from the retail branch tree below. */

void *fn_80442AA4(void *context, u32 *shape, void *a, void *b, void *c) {
    switch (shape[6]) {
    case 1:
        fn_80441188(context, shape, a, b, c);
        break;
    case 3:
        fn_80442210(context, shape, shape, a, b);
        break;
    case 4:
        fn_80441BF8(context, shape, a, b, c);
        break;
    case 5: {
        u32 *matrix = fn_804566D0(shape[0]);
        u32 local[4];
        local[0] = matrix[0];
        local[1] = matrix[1];
        local[2] = matrix[2];
        local[3] = matrix[3];
        fn_80442210(context, local, shape, a, b);
        break;
    }
    default:
        break;
    }
    return context;
}

u32 fn_80442B98(u32 *tree) {
    u8 *entries = (u8 *)tree[1];
    for (u32 i = 0; i < tree[0]; i++) {
        if (*(u32 *)(entries + 4) != 0) rwFree(*(void **)(entries + 4));
        entries += 8;
    }
    rwFree(tree);
    return 0;
}

u32 *fn_80442C24(void *context) {
    u32 local[6];
    if (fn_8047CF50(context, 1, local + 4, local + 5) == 0) return NULL;
    if (fn_8047D868(context, local, 4) == 0 ||
        fn_8047D868(context, local + 1, 4) == 0 ||
        fn_8047D868(context, local + 2, 4) == 0 ||
        fn_8047D868(context, local + 3, 4) == 0) return NULL;
    if (local[0] >= 0x101) {
        local[4] = 0x122;
        local[5] = fn_8047DC28(0x80000004);
        fn_8047DB84(local + 4);
        return NULL;
    }
    u32 count = local[2];
    u32 *tree = (u32 *)rwAlloc(count * 8 + 8, 0x3001E);
    if (tree == NULL) {
        local[4] = 0x122;
        local[5] = fn_8047DC28(0x80000013, count * 8 + 8);
        fn_8047DB84(local + 4);
        return NULL;
    }
    tree[0] = count;
    tree[1] = (u32)(tree + 2);
    memset((void *)tree[1], 0, count * 8);
    for (u32 i = 0; i < count; i++) {
        u32 items;
        if (fn_8047D868(context, &items, 4) == 0) return NULL;
        u32 *entry = (u32 *)(tree[1] + i * 8);
        if (items != 0) {
            u32 bytes = items * 0x14;
            entry[1] = (u32)rwAlloc(bytes, 0x30122);
            if (entry[1] == 0) {
                local[4] = 0x122;
                local[5] = fn_8047DC28(0x80000013, bytes);
                fn_8047DB84(local + 4);
                return NULL;
            }
            memset((void *)entry[1], 0, bytes);
            for (u32 j = 0; j < items; j++) *(u32 *)(entry[1] + j * 0x14 + 0x10) = 0xFFFFFFFF;
            entry[0] = items;
            u8 *item = (u8 *)entry[1];
            for (u32 j = 0; j < items; j++) {
                if (fn_8047D784(context, item, 0x10) == 0 ||
                    fn_8047D868(context, item + 0x10, 4) == 0) return NULL;
                item += 0x14;
            }
        }
    }
    return tree;
}

void fn_80443044(u8 *owner) {
    *(u32 *)(owner + lbl_805F1428) = 0;
}

void *fn_80443054(u8 *owner) {
    u32 *tree = *(u32 **)(owner + lbl_805F1428);
    if (tree != NULL && tree[0] != 0) {
        extern void *fn_80443908(u8 *);
        fn_80443908(owner);
    }
    *(u32 *)(owner + lbl_805F1428) = 0;
    return owner;
}

void fn_804430AC(void) {}

void *fn_804430B0(void *context, void *unused, u8 *owner) {
    u32 local[8];
    if (fn_8047D868(context, local, 4) == 0) return NULL;
    extern void *fn_8044382C(u8 *, u32);
    if (fn_8044382C(owner, local[0]) == NULL) return NULL;
    u32 *tree = *(u32 **)(owner + lbl_805F1428);
    for (u32 i = 0; i < tree[0]; i++) {
        u32 *entry = (u32 *)(tree[1] + i * 0x40);
        u32 nameBytes;
        if (fn_8047D868(context, &nameBytes, 4) == 0) return NULL;
        if (nameBytes == 0) {
            entry[1] = 0;
        } else {
            entry[1] = (u32)rwAlloc(nameBytes, 0x30122);
            if (entry[1] == 0) {
                local[4] = 0x122;
                local[5] = fn_8047DC28(0x80000013, nameBytes);
                fn_8047DB84(local + 4);
                return NULL;
            }
            if (fn_80480D4C(context, (void *)entry[1], nameBytes) == 0) return NULL;
        }
        if (fn_8047D868(context, local, 0x10) == 0) return NULL;
        entry[2] = local[0];
        entry[3] = local[1];
        entry[5] = local[2];
        entry[6] = local[3];
        u32 dataBytes = entry[5];
        entry[4] = (u32)rwAlloc(dataBytes, 0x30122);
        if (entry[4] == 0) {
            local[4] = 0x122;
            local[5] = fn_8047DC28(0x80000013, dataBytes);
            fn_8047DB84(local + 4);
            return NULL;
        }
        if (fn_80480D4C(context, (void *)entry[4], dataBytes) == 0) return NULL;
        if (entry[6] == 0) {
            entry[7] = 0;
        } else {
            u32 flags = entry[2];
            u32 stride = ((flags & 2) ? 0xC : 0) + ((flags & 0x10) ? 0xC : 0) +
                         ((flags & 8) ? 4 : 0) + ((flags & 4) ? 8 : 0);
            u32 bytes = stride * entry[6];
            u8 *data = (u8 *)rwAlloc(bytes, 0x30122);
            entry[7] = (u32)data;
            if (data == NULL) {
                local[4] = 0x122;
                local[5] = fn_8047DC28(0x80000013, bytes);
                fn_8047DB84(local + 4);
                return NULL;
            }
            if ((flags & 2) != 0) {
                if (fn_8047D784(context, data, entry[6] * 0xC) == 0) return NULL;
                entry[8] = (u32)data;
                data += entry[6] * 0xC;
            }
            if ((flags & 0x10) != 0) {
                if (fn_8047D784(context, data, entry[6] * 0xC) == 0) return NULL;
                entry[9] = (u32)data;
                data += entry[6] * 0xC;
            }
            if ((flags & 8) != 0) {
                if (fn_80480D4C(context, data, entry[6] * 4) == 0) return NULL;
                entry[10] = (u32)data;
                data += entry[6] * 4;
            }
            if ((flags & 4) != 0) {
                if (fn_8047D784(context, data, entry[6] * 8) == 0) return NULL;
                entry[11] = (u32)data;
            }
        }
        if (fn_8047D784(context, entry + 12, 0x10) == 0) return NULL;
        *entry = (u32)owner;
    }
    return context;
}

void *fn_80443474(void *context, void *unused, u8 *owner) {
    u32 *tree = *(u32 **)(owner + lbl_805F1428);
    if (fn_8047D458(context, tree, 4) == 0) return NULL;
    for (u32 i = 0; i < tree[0]; i++) {
        u32 *entry = (u32 *)(tree[1] + i * 0x40);
        u32 nameBytes = 0;
        if (entry[1] != 0) nameBytes = ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)entry[1]) + 1;
        if (fn_8047D458(context, &nameBytes, 4) == 0) return NULL;
        if (entry[1] != 0 && fn_80480ED0(context, (void *)entry[1], nameBytes) == 0) return NULL;
        u32 local[4] = {entry[2], entry[3], entry[5], entry[6]};
        if (fn_8047D458(context, local, 0x10) == 0) return NULL;
        if (fn_80480ED0(context, (void *)entry[4], entry[5]) == 0) return NULL;
        if (entry[6] != 0) {
            if ((entry[2] & 2) != 0 && fn_8047D350(context, (void *)entry[8], entry[6] * 0xC) == 0) return NULL;
            if ((entry[2] & 0x10) != 0 && fn_8047D350(context, (void *)entry[9], entry[6] * 0xC) == 0) return NULL;
            if ((entry[2] & 8) != 0 && fn_80480ED0(context, (void *)entry[10], entry[6] * 4) == 0) return NULL;
            if ((entry[2] & 4) != 0 && fn_8047D350(context, (void *)entry[11], entry[6] * 8) == 0) return NULL;
        }
        if (fn_8047D350(context, entry + 12, 0x10) == 0) return NULL;
    }
    return context;
}

u32 fn_804436B0(u8 *owner) {
    u32 *tree = *(u32 **)(owner + lbl_805F1428);
    if (tree == NULL) return 0;
    u32 size = 4;
    for (u32 i = 0; i < tree[0]; i++) {
        u32 *entry = (u32 *)(tree[1] + i * 0x40);
        size += 4;
        if (entry[1] != 0) size += ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)entry[1]) + 1;
        u32 flags = entry[2];
        u32 stride = ((flags & 2) ? 0xC : 0) + ((flags & 0x10) ? 0xC : 0) +
                     ((flags & 8) ? 4 : 0) + ((flags & 4) ? 8 : 0);
        size += entry[5] + entry[6] * stride + 0x20;
    }
    return size;
}

u32 fn_804437B0(void) {
    lbl_805F1428 = fn_8045994C(4, 0x122, fn_80443044, fn_80443054, fn_804430AC);
    if (lbl_805F1428 < 0) return 0;
    return ((u32)fn_8045998C(0x122, fn_804430B0, fn_80443474, fn_804436B0) >> 31) ^ 1;
}

void *fn_8044382C(u8 *owner, u32 count) {
    u32 *tree = (u32 *)rwAlloc(0x1C, 0x30122);
    *(u32 **)(owner + lbl_805F1428) = tree;
    if (tree == NULL) return NULL;
    memset(tree, 0, 0x1C);
    tree[0] = count;
    u32 bytes = count * 0x44;
    void *entries = rwAlloc(bytes, 0x30122);
    if (entries == NULL) {
        rwFree(tree);
        return NULL;
    }
    memset(entries, 0, bytes);
    tree[1] = (u32)entries;
    tree[2] = (u32)entries + count * 0x40;
    return owner;
}

void *fn_80443908(u8 *owner) {
    u32 *tree = *(u32 **)(owner + lbl_805F1428);
    for (u32 i = 0; i < tree[0]; i++) {
        extern void *fn_804439A8(u8 *, u32);
        fn_804439A8(owner, i);
    }
    rwFree((void *)tree[1]);
    rwFree(tree);
    *(u32 *)(owner + lbl_805F1428) = 0;
    return owner;
}

void *fn_804439A8(u8 *owner, u32 index) {
    u8 *entry = (u8 *)(*(u32 *)(*(u32 *)(owner + lbl_805F1428) + 4) + index * 0x40);
    if (*(u32 *)(entry + 4) != 0) rwFree(*(void **)(entry + 4));
    if (*(u32 *)(entry + 0x10) != 0) rwFree(*(void **)(entry + 0x10));
    if (*(u32 *)(entry + 0x1C) != 0) rwFree(*(void **)(entry + 0x1C));
    memset(entry, 0, 0x40);
    return owner;
}

u32 fn_80443A54(u8 *owner) {
    u32 *tree = *(u32 **)(owner + lbl_805F1428);
    return tree != NULL ? tree[0] : 0;
}
}
