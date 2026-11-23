/* RenderWare: RpDMorph (plugin 0x122, rwID_DMORPHPLUGIN), 0x80442B98..0x80443A74: a delta morph
 * animation's destroy and stream read functions, then the plugin's geometry extension (named delta
 * targets: vertex, normal, colour and texture-coordinate delta arrays with a bounding sphere), its
 * geometry callbacks, the geometry half of the attach and the target array's create and destroy.
 * Its .sbss is the geometry plugin offset alone (0x805F1428, padded to 8 bytes); the atomic half
 * (Game/../RenderWare/RpDMorph.c) has its own offset after it. The animation functions may be a
 * file of their own: they reach no data, so nothing in the binary places a cut there.
 * Built as C without lmw/stmw at -O4,p, inlining only what is declared inline, in definition order
 * (cflags_rw_c; cflags_rw_r3, _r5 and _r6 give the same code here, 10 of 13 functions). Names are inferred from behavior. NonMatching: fn_80442C24, fn_804430B0,
 * fn_804436B0 (drafts). */
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern u8 *lbl_805F15B4;

s32 lbl_805F1428;

int fn_8047CF50(void *, u32, void *, void *);
u32 fn_8047D350(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D868(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
s32 fn_8045994C(u32, u32, void *, void *, void *);
s32 fn_8045998C(u32, void *, void *, void *);
void *fn_8044382C(u8 *, u32);
void *fn_80443908(u8 *);
void *fn_804439A8(u8 *, u32);

static inline void *rwAlloc(u32 size, u32 tag) {
    return ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, tag);
}
static inline void rwFree(void *memory) {
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(memory);
}

u32 fn_80442B98(u32 *tree) {
    u32 i;
    u8 *entries = (u8 *)tree[1];
    for (i = 0; i < tree[0]; i++) {
        if (*(u32 *)(entries + 4) != 0) rwFree(*(void **)(entries + 4));
        entries += 8;
    }
    rwFree(tree);
    return 0;
}

u32 *fn_80442C24(void *context) {
    u32 local[6];
    u32 count;
    u32 *tree;
    u32 i;
    if (fn_8047CF50(context, 1, local + 4, local + 5) == 0) return NULL;
    if (fn_8047D868(context, local + 3, 4) == 0 ||
        fn_8047D868(context, local + 2, 4) == 0 ||
        fn_8047D868(context, local + 1, 4) == 0 ||
        fn_8047D868(context, local, 4) == 0) return NULL;
    if (local[0] > 0x100) {
        local[4] = 0x122;
        local[5] = fn_8047DC28(0x80000004);
        fn_8047DB84(local + 4);
        return NULL;
    }
    count = local[2];
    tree = (u32 *)rwAlloc(count * 8 + 8, 0x3001E);
    if (tree == NULL) {
        local[4] = 0x122;
        local[5] = fn_8047DC28(0x80000013, count * 8 + 8);
        fn_8047DB84(local + 4);
    } else {
        tree[0] = count;
        tree[1] = (u32)(tree + 2);
        for (i = 0; i < count; i++) {
            ((u64 *)tree[1])[i] = 0;
        }
    }
    if (tree == NULL) return NULL;
    for (i = 0; i < count; i++) {
        u32 items;
        u32 *entry;
        if (fn_8047D868(context, &items, 4) == 0) return NULL;
        entry = (u32 *)(tree[1] + i * 8);
        if (items != 0) {
            u32 bytes = items * 0x14;
            u32 j;
            u8 *item;
            entry[1] = (u32)rwAlloc(bytes, 0x30122);
            if (entry[1] == 0) {
                local[4] = 0x122;
                local[5] = fn_8047DC28(0x80000013, bytes);
                fn_8047DB84(local + 4);
                return NULL;
            }
            memset((void *)entry[1], 0, bytes);
            for (j = 0; j < items; j++) *(u32 *)(entry[1] + j * 0x14 + 0x10) = 0xFFFFFFFF;
            entry[0] = items;
            item = (u8 *)entry[1];
            for (j = 0; j < items; j++) {
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
        fn_80443908(owner);
    }
    *(u32 *)(owner + lbl_805F1428) = 0;
    return owner;
}

void fn_804430AC(void) {}

void *fn_804430B0(void *context, void *unused, u8 *owner) {
    u32 local[8];
    u32 *tree;
    u32 i;
    if (fn_8047D868(context, local, 4) == 0) return NULL;
    if (fn_8044382C(owner, local[0]) == NULL) return NULL;
    tree = *(u32 **)(owner + lbl_805F1428);
    for (i = 0; i < tree[0]; i++) {
        u32 *entry = (u32 *)(tree[1] + i * 0x40);
        u32 nameBytes;
        u32 dataBytes;
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
        dataBytes = entry[5];
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

/* Plugin 0x122's geometry stream write callback: every target's name, header, vertex arrays and
 * bounding sphere. */
void *fn_80443474(void *stream, s32 length, const void *object, s32 offset, s32 size) {
    u32 header[4];
    u32 nameBytes;
    u32 *tree = *(u32 **)((const u8 *)object + lbl_805F1428);
    u32 i;
    if (fn_8047D458(stream, tree, 4) == 0) {
        return NULL;
    }
    for (i = 0; i < tree[0]; i++) {
        u32 *entry = (u32 *)(tree[1] + i * 0x40);
        nameBytes = entry[1] != 0 ? ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)entry[1]) + 1 : 0;
        if (fn_8047D458(stream, &nameBytes, 4) == 0) {
            return NULL;
        }
        if (entry[1] != 0 && fn_80480ED0(stream, (void *)entry[1], nameBytes) == 0) {
            return NULL;
        }
        header[0] = entry[2];
        header[1] = entry[3];
        header[2] = entry[5];
        header[3] = entry[6];
        if (fn_8047D458(stream, header, 0x10) == 0) {
            return NULL;
        }
        if (fn_80480ED0(stream, (void *)entry[4], entry[5]) == 0) {
            return NULL;
        }
        if (entry[6] != 0) {
            if ((entry[2] & 2) != 0 && fn_8047D350(stream, (void *)entry[8], entry[6] * 0xC) == 0) {
                return NULL;
            }
            if ((entry[2] & 0x10) != 0 && fn_8047D350(stream, (void *)entry[9], entry[6] * 0xC) == 0) {
                return NULL;
            }
            if ((entry[2] & 8) != 0 && fn_80480ED0(stream, (void *)entry[10], entry[6] * 4) == 0) {
                return NULL;
            }
            if ((entry[2] & 4) != 0 && fn_8047D350(stream, (void *)entry[11], entry[6] * 8) == 0) {
                return NULL;
            }
        }
        if (fn_8047D350(stream, entry + 12, 0x10) == 0) {
            return NULL;
        }
    }
    return stream;
}

u32 fn_804436B0(u8 *owner) {
    u32 *tree;
    u32 size;
    u32 i;
    u32 *entry;
    u32 flags;
    size = 0;
    tree = *(u32 **)(owner + lbl_805F1428);
    if (tree != NULL) {
        size = 4;
        for (i = 0; i < tree[0]; i++) {
            entry = (u32 *)(tree[1] + i * 0x40);
            size += 4;
            flags = entry[2];
            if (entry[1] != 0) {
                size += ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)entry[1]) + 1;
            }
            size += 0x10;
            size += entry[5];
            size += entry[6] * (((flags & 0x10) ? 0xC : 0) + ((flags & 0x8) ? 0x4 : 0) +
                                ((flags & 0x4) ? 0x8 : 0) + ((flags & 0x2) ? 0xC : 0));
            size += 0x10;
        }
    }
    return size;
}

u32 fn_804437B0(void) {
    lbl_805F1428 = fn_8045994C(4, 0x122, fn_80443044, fn_80443054, fn_804430AC);
    if (lbl_805F1428 < 0) return 0;
    return ((u32)fn_8045998C(0x122, fn_804430B0, fn_80443474, fn_804436B0) >> 31) ^ 1;
}

void *fn_8044382C(u8 *owner, u32 count) {
    s32 offset = lbl_805F1428;
    u32 bytes;
    u32 *tree = *(u32 **)(owner + offset) = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(0x1C, 0x30122);
    void *entries;
    if (tree == NULL) return NULL;
    memset(tree, 0, 0x1C);
    tree[0] = count;
    bytes = count * 0x44;
    entries = rwAlloc(bytes, 0x30122);
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
    s32 offset = lbl_805F1428;
    u32 *tree = *(u32 **)(owner + offset);
    u32 i;
    for (i = 0; i < tree[0]; i++) {
        fn_804439A8(owner, i);
    }
    rwFree((void *)tree[1]);
    rwFree(tree);
    *(u32 *)(owner + offset) = 0;
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
