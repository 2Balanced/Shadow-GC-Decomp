/* RenderWare octree color quantization, 0x8048DBEC..0x8048F5CF.
 * Node fields: weight, four color sums, error, index/flags, and sixteen child pointers.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805EDF20;
extern f32 lbl_805FB360;
extern f32 lbl_805FB364;
extern f32 lbl_805FB368;
extern f32 lbl_805FB36C;
extern f32 lbl_805FB378;
extern f32 lbl_805FB37C;

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))

static u8 *rwQuantWork;

static inline void rwNodeClear(u32 *node) {
    for (u32 i = 0; i < 0x10; i++) node[i] = 0;
}

static inline f32 rwClampByte(f32 value) {
    return value < 0.0f ? 0.0f : value;
}

u32 *fn_8048DBEC(u8 *work, u32 *node, u32 path, s32 depth) {
    if (depth == 0) return node;
    u32 *child = (u32 *)node[(path & 0xF) + 7];
    if (child == NULL) {
        child = (u32 *)RW_API(void *(*)(void *, u32), 0x118)(*(void **)(work + 0x4004), 0x30411);
        node[(path & 0xF) + 7] = (u32)child;
        rwNodeClear(child);
        if (depth == 1) {
            ((u8 *)child)[0x18] = 0;
            for (u32 i = 0; i < 6; i++) ((f32 *)child)[i] = lbl_805FB368;
        }
    }
    return fn_8048DBEC(work, child, path >> 2, depth - 1);
}

void fn_8048D8A8(f32 *node, const u8 *rgba) {
    node[5] -= (node[4] * node[4] + node[3] * node[3] +
                node[1] * node[1] + node[2] * node[2]) / node[0];
    if (node[5] < lbl_805FB368) node[5] = lbl_805FB368;
    f32 weight = node[0];
    node[1] += lbl_805FB36C * rgba[0] * lbl_805FB360 * weight;
    node[2] += lbl_805FB36C * rgba[1] * weight;
    node[3] += lbl_805FB36C * rgba[2] * lbl_805FB364 * weight;
    node[4] += lbl_805FB36C * rgba[3] * weight;
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

void fn_8048D85C(f32 *color) {
    color[0] /= lbl_805FB360;
    color[2] /= lbl_805FB364;
}

void fn_8048D880(f32 *node) {
    ((u8 *)node)[0x18] = 0;
    for (u32 i = 0; i < 6; i++) node[i] = lbl_805FB368;
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

void fn_8048DEB0(u8 *work, f32 scale, u32 *texture) {
    rwQuantWork = work;
    u32 *root = (u32 *)RW_API(void *(*)(void *, u32), 0x118)(*(void **)(work + 0x4004), 0x30411);
    *(u32 **)(work + 0x4000) = root;
    if (root == NULL) return;
    rwNodeClear(root);
    root[0] = 0;
    root[1] = 0;
    root[2] = 0;
    root[3] = 0;
    root[4] = 0;
    root[5] = 0;
    u32 kind = texture[3];
    if (kind == 4 || kind == 8) {
        u8 *palette = (u8 *)texture[6];
        for (u32 i = 0; i < (1u << kind); i++) {
            extern void fn_8048EB24(u32 *, const u8 *, s32);
            fn_8048EB24(root, palette + i * 4, 8);
        }
    } else if (kind == 0x20) {
        u8 *pixels = (u8 *)texture[5];
        for (u32 row = 0; row < texture[2]; row++) {
            u8 *pixel = pixels;
            for (u32 x = 0; x < texture[1]; x++) {
                extern void fn_8048EB24(u32 *, const u8 *, s32);
                fn_8048EB24(root, pixel, 8);
                pixel += 4;
            }
            pixels += texture[4];
        }
    }
    root[5] *= scale;
}

void fn_8048E300(u32 *node, u32 *point, u32 depth, u32 *bounds, u32 value) {
    if (node == NULL) return;
    u32 size = 1 << depth;
    if ((s32)(point[0] + size - bounds[0]) <= 0 || (s32)(point[1] + size - bounds[1]) <= 0 ||
        (s32)(point[2] + size - bounds[2]) <= 0 || (s32)(point[3] + size - bounds[3]) <= 0) return;
    if ((s32)(point[0] - bounds[4]) >= 0 || (s32)(point[1] - bounds[5]) >= 0 ||
        (s32)(point[2] - bounds[6]) >= 0 || (s32)(point[3] - bounds[7]) >= 0) return;
    if (depth == 0) {
        ((u8 *)node)[0x18] = (u8)value;
        return;
    }
    for (u32 i = 0; i < 0x10; i++) {
        u32 next[4] = {
            point[0] + ((i >> 3 & 1) << (depth - 1)),
            point[1] + ((i >> 2 & 1) << (depth - 1)),
            point[2] + ((i >> 1 & 1) << (depth - 1)),
            point[3] + ((i & 1) << (depth - 1)),
        };
        fn_8048E300((u32 *)node[i + 7], next, depth - 1, bounds, value);
    }
}

void fn_8048E47C(f32 *node, u32 *point, u32 depth, u32 *bounds, f32 *out) {
    if (node == NULL) return;
    u32 size = 1 << depth;
    if ((s32)(point[0] + size - bounds[0]) <= 0 || (s32)(point[1] + size - bounds[1]) <= 0 ||
        (s32)(point[2] + size - bounds[2]) <= 0 || (s32)(point[3] + size - bounds[3]) <= 0) return;
    if ((s32)(point[0] - bounds[4]) >= 0 || (s32)(point[1] - bounds[5]) >= 0 ||
        (s32)(point[2] - bounds[6]) >= 0 || (s32)(point[3] - bounds[7]) >= 0) return;
    if ((s32)(point[0] - bounds[0]) >= 0 && (s32)(point[1] - bounds[1]) >= 0 &&
        (s32)(point[2] - bounds[2]) >= 0 && (s32)(point[3] - bounds[3]) >= 0 &&
        (s32)(bounds[4] - point[0] - size) >= 0 && (s32)(bounds[5] - point[1] - size) >= 0 &&
        (s32)(bounds[6] - point[2] - size) >= 0 && (s32)(bounds[7] - point[3] - size) >= 0) {
        fn_8048D9C8(out, out, node);
        return;
    }
    if (depth == 0) return;
    for (u32 i = 0; i < 0x10; i++) {
        u32 next[4] = {
            point[0] + ((i >> 3 & 1) << (depth - 1)),
            point[1] + ((i >> 2 & 1) << (depth - 1)),
            point[2] + ((i >> 1 & 1) << (depth - 1)),
            point[3] + ((i & 1) << (depth - 1)),
        };
        fn_8048E47C((f32 *)((u32 *)node)[i + 7], next, depth - 1, bounds, out);
    }
}

f32 fn_8048E748(f32 *node, u32 *point, u32 depth, u32 *indices, f32 *target) {
    if (node == NULL) return lbl_805FB368;
    if (depth == 0) {
        indices[0] = ((u8 *)node)[0x18];
        if (node[0] == 0.0f) return lbl_805FB368;
        f32 inv = lbl_805FB378 / node[0];
        f32 x = node[1] * inv / lbl_805FB360 - target[0];
        f32 y = node[2] * inv - target[1];
        f32 z = node[3] * inv / lbl_805FB364 - target[2];
        f32 w = node[4] * inv - target[3];
        return x * x + y * y + z * z + w * w + node[5];
    }
    f32 best = lbl_805FB368;
    for (u32 i = 0; i < 0x10; i++) {
        u32 next[4] = {
            point[0] + ((i >> 3 & 1) << (depth - 1)),
            point[1] + ((i >> 2 & 1) << (depth - 1)),
            point[2] + ((i >> 1 & 1) << (depth - 1)),
            point[3] + ((i & 1) << (depth - 1)),
        };
        u32 candidate[4];
        f32 error = fn_8048E748((f32 *)((u32 *)node)[i + 7], next, depth - 1, candidate, target);
        if (error < best) {
            best = error;
            indices[0] = candidate[0];
        }
    }
    return best;
}

u32 fn_8048E988(f32 *node, f32 *candidate, u32 *bounds, u32 *best) {
    f32 distances[4];
    u32 indices[4];
    distances[0] = fn_8048E748(node, bounds, 0, indices, candidate);
    distances[1] = fn_8048E748(node, bounds, 1, indices + 1, candidate);
    distances[2] = fn_8048E748(node, bounds, 2, indices + 2, candidate);
    distances[3] = fn_8048E748(node, bounds, 3, indices + 3, candidate);
    u32 which = distances[1] < distances[0] ? 1 : 0;
    if (distances[2] < distances[which]) which = 2;
    if (distances[3] < distances[which]) which = 3;
    if (distances[which] >= candidate[5]) return 0;
    for (u32 i = 0; i < 8; i++) best[i] = bounds[i];
    best[which] = indices[which] & 0xFF;
    bounds[which + 4] = indices[which];
    return 1;
}

void fn_8048EB24(f32 *node, const u8 *rgba, s32 depth) {
    if (node == NULL) return;
    if (depth < 1) {
        fn_8048D8A8(node, rgba);
        return;
    }
    u32 shift = depth + 7 - lbl_805EDF20;
    u8 shifted[4] = {
        (u8)(rgba[0] + (1 << shift)),
        (u8)(rgba[1] + (1 << shift)),
        (u8)(rgba[2] + (1 << shift)),
        (u8)(rgba[3] + (1 << shift)),
    };
    u32 index = ((shifted[0] >> shift & 1) << 3) | ((shifted[1] >> shift & 1) << 2) |
                ((shifted[2] >> shift & 1) << 1) | (shifted[3] >> shift & 1);
    f32 *child = (f32 *)((u32 *)node)[index + 7];
    if (child == NULL) child = (f32 *)fn_8048DBEC(rwQuantWork, (u32 *)node, index, 1);
    if (child == NULL) return;
    fn_8048EB24(child, shifted, depth - 1);
    fn_8048D9C8(node, node, child);
}

u32 fn_8048EEFC(u32 *node, s32 depth) {
    if (node == NULL) return 0;
    if (depth < 1) return 1;
    u32 count = 0;
    for (u32 i = 0; i < 0x10; i++) count += fn_8048EEFC((u32 *)node[i + 7], depth - 1);
    return count;
}

u32 fn_8048F1E4(f32 *node, u8 *out, u32 index, s32 depth) {
    if (node == NULL) return index;
    if (depth < 1) {
        f32 inv = lbl_805FB378 / node[0];
        s32 r = (s32)(lbl_805FB37C * (node[1] * inv / lbl_805FB360));
        s32 g = (s32)(lbl_805FB37C * node[2] * inv);
        s32 b = (s32)(lbl_805FB37C * (node[3] * inv / lbl_805FB364));
        s32 a = (s32)(lbl_805FB37C * node[4] * inv);
        out[index * 4] = r < 0xFF ? (u8)r : 0xFF;
        out[index * 4 + 1] = g < 0xFF ? (u8)g : 0xFF;
        out[index * 4 + 2] = b < 0xFF ? (u8)b : 0xFF;
        out[index * 4 + 3] = a < 0xFF ? (u8)a : 0xFF;
        ((u8 *)node)[0x18] = (u8)index;
        return index + 1;
    }
    for (u32 i = 0; i < 0x10; i++) index = fn_8048F1E4((f32 *)((u32 *)node)[i + 7], out, index, depth - 1);
    return index;
}

u32 fn_8048F4B4(u32 *node, u32 path, s32 depth) {
    if (depth == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[(path & 0xF) + 7];
    if (depth - 1 == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[((path >> 2) & 0xF) + 7];
    if (depth - 2 == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[((path >> 6) & 0xF) + 7];
    if (depth - 3 == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[((path >> 10) & 0xF) + 7];
    if (depth - 4 == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[((path >> 14) & 0xF) + 7];
    if (depth - 5 == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[((path >> 18) & 0xF) + 7];
    if (depth - 6 == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[((path >> 22) & 0xF) + 7];
    if (depth - 7 == 0) return ((u8 *)node)[0x18];
    node = (u32 *)node[((path >> 26) & 0xF) + 7];
    if (depth - 8 == 0) return ((u8 *)node)[0x18];
    return fn_8048F4B4((u32 *)node[7], 0, depth - 9);
}
}
