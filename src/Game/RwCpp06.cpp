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

static inline void *rwAlloc(u32 size, u32 tag) {
    return ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, tag);
}
static inline void rwFree(void *memory) {
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(memory);
}

extern s32 lbl_805F1420;
extern float lbl_805FAF88;
extern float lbl_805FAF8C;
extern float lbl_805FAF9C;

struct RwCollisionV3d {
    float x;
    float y;
    float z;
};

struct RwCollisionBBox {
    RwCollisionV3d sup;
    RwCollisionV3d inf;
};

struct RwCollisionSphere {
    RwCollisionV3d center;
    float radius;
};

struct RwCollisionLine {
    RwCollisionV3d start;
    RwCollisionV3d end;
};

struct RwCollisionIntersection {
    union {
        RwCollisionLine line;
        RwCollisionSphere sphere;
        RwCollisionBBox box;
    } t;
    s32 type;
};

struct RwCollisionTriangle {
    RwCollisionV3d normal;
    RwCollisionV3d point;
    s32 index;
    RwCollisionV3d *vertices[3];
};

struct RwCollisionWords3 {
    u32 x;
    u32 y;
    u32 z;
};

struct RwCollisionRpTriangle {
    u16 vertIndex[3];
    s16 matIndex;
};

struct RwCollisionNode {
    u8 type;
    u8 count;
    u16 index;
};

struct RwCollisionSplit {
    RwCollisionNode left;
    float leftValue;
    RwCollisionNode right;
    float rightValue;
};

struct RwCollisionTree {
    u32 flags;
    RwCollisionBBox bbox;
    u16 numTriangles;
    u16 numSplits;
    RwCollisionSplit *splits;
    u16 *map;
};

struct RwCollisionSector {
    s32 type;
};

struct RwCollisionPlaneSector : public RwCollisionSector {
    float value;
    RwCollisionSector *leftSubTree;
    RwCollisionSector *rightSubTree;
    float leftValue;
    float rightValue;
};

struct RwCollisionWorldSector : public RwCollisionSector {
    RwCollisionRpTriangle *triangles;
    RwCollisionV3d *vertices;
};

struct RwCollisionWorld {
    u8 field_0x0[0x1C];
    RwCollisionSector *rootSector;
};

typedef RwCollisionTriangle *(*RwCollisionCallback)(RwCollisionIntersection *, RwCollisionWorldSector *,
                                                     RwCollisionTriangle *, float, void *);

s32 fn_8047B720(void *, void *, void *, void *, void *, void *);
float fn_80482394(float);

struct RwCollisionSectorWalk {
    RwCollisionBBox box;
    RwCollisionSector *stack[64];
    u32 field_0x118;
    RwCollisionSector **sp;
};

struct RwCollisionTreeEntry {
    RwCollisionNode *node;
    u32 flags;
};

struct RwCollisionTreeWalk {
    RwCollisionTree *tree;
    RwCollisionBBox box;
    RwCollisionBBox outer;
    RwCollisionNode root;
    RwCollisionTreeEntry stack[33];
    RwCollisionTreeEntry *top;
};

/* Sphere intersection against every triangle in the world's collision trees. */
void *fn_80442210(void *worldArg, void *sphereArg, void *intersectionArg, void *callbackArg, void *data) {
    RwCollisionWorld *world = (RwCollisionWorld *)worldArg;
    RwCollisionSphere *sphere = (RwCollisionSphere *)sphereArg;
    RwCollisionIntersection *intersection = (RwCollisionIntersection *)intersectionArg;
    RwCollisionCallback callback = (RwCollisionCallback)callbackArg;
    const float radius = sphere->radius;
    const float inverseRadius = lbl_805FAF88 / radius;
    const float halfRadius = lbl_805FAF9C * radius;

    RwCollisionBBox box;
    box.sup.x = sphere->center.x + radius;
    box.sup.y = sphere->center.y + radius;
    box.sup.z = sphere->center.z + radius;
    box.inf.x = sphere->center.x - radius;
    box.inf.y = sphere->center.y - radius;
    box.inf.z = sphere->center.z - radius;

    RwCollisionBBox outer;
    outer.sup.x = sphere->center.x + halfRadius;
    outer.sup.y = sphere->center.y + halfRadius;
    outer.sup.z = sphere->center.z + halfRadius;
    outer.inf.x = sphere->center.x - halfRadius;
    outer.inf.y = sphere->center.y - halfRadius;
    outer.inf.z = sphere->center.z - halfRadius;

    RwCollisionSectorWalk sectors;
    sectors.box = box;
    sectors.stack[0] = world->rootSector;
    sectors.sp = &sectors.stack[1];

    while (sectors.sp > sectors.stack) {
        sectors.sp--;
        while ((*sectors.sp)->type >= 0) {
            RwCollisionPlaneSector *plane = (RwCollisionPlaneSector *)*sectors.sp;
            u32 axis = (u32)plane->type;
            if (plane->rightValue > *(float *)((u8 *)&sectors.box.sup + axis)) {
                *sectors.sp = plane->leftSubTree;
            } else {
                *sectors.sp = plane->rightSubTree;
                if (plane->leftValue >= *(float *)((u8 *)&sectors.box.inf + axis)) {
                    sectors.sp++;
                    *sectors.sp = plane->leftSubTree;
                }
            }
        }

        RwCollisionWorldSector *sector = (RwCollisionWorldSector *)*sectors.sp;
        RwCollisionTree *tree = *(RwCollisionTree **)((u8 *)sector + lbl_805F1420);
        if (tree == NULL) continue;

        float dx;
        if (sphere->center.x < tree->bbox.inf.x) {
            dx = tree->bbox.inf.x - sphere->center.x;
        } else if (sphere->center.x > tree->bbox.sup.x) {
            dx = sphere->center.x - tree->bbox.sup.x;
        } else {
            dx = lbl_805FAF8C;
        }
        float dy;
        if (sphere->center.y < tree->bbox.inf.y) {
            dy = tree->bbox.inf.y - sphere->center.y;
        } else if (sphere->center.y > tree->bbox.sup.y) {
            dy = sphere->center.y - tree->bbox.sup.y;
        } else {
            dy = lbl_805FAF8C;
        }
        float dz;
        if (sphere->center.z < tree->bbox.inf.z) {
            dz = tree->bbox.inf.z - sphere->center.z;
        } else if (sphere->center.z > tree->bbox.sup.z) {
            dz = sphere->center.z - tree->bbox.sup.z;
        } else {
            dz = lbl_805FAF8C;
        }
        if (dx * dx + dy * dy + dz * dz > radius * radius) continue;
        if (tree->bbox.inf.x > box.sup.x || tree->bbox.inf.y > box.sup.y || tree->bbox.inf.z > box.sup.z ||
            tree->bbox.sup.x < box.inf.x || tree->bbox.sup.y < box.inf.y || tree->bbox.sup.z < box.inf.z) {
            continue;
        }

        RwCollisionTreeWalk walk;
        walk.tree = tree;
        walk.box = box;
        walk.outer = outer;
        walk.root.index = 0;
        walk.root.count = 0xFF;
        if (tree->numSplits == 0) walk.root.count = (u8)tree->numTriangles;
        walk.stack[0].node = &walk.root;
        walk.stack[0].flags = (tree->bbox.inf.y < outer.inf.y ? 4 : 0) |
                              (tree->bbox.inf.x < outer.inf.x ? 1 : 0) |
                              (tree->bbox.inf.z < outer.inf.z ? 0x10 : 0) |
                              (tree->bbox.sup.x > outer.sup.x ? 2 : 0) |
                              (tree->bbox.sup.y > outer.sup.y ? 8 : 0) |
                              (tree->bbox.sup.z > outer.sup.z ? 0x20 : 0);
        walk.top = &walk.stack[1];

        while (walk.top > walk.stack) {
            walk.top--;
            u16 start = 0;
            u16 count = 0;
            u32 leafFlags = 0;
            for (;;) {
                RwCollisionTreeEntry *entry = walk.top;
                RwCollisionNode *node = entry->node;
                if (node->count != 0xFF) {
                    start = node->index;
                    count = node->count;
                    leafFlags = entry->flags;
                    break;
                }
                if (entry->flags == 0) {
                    RwCollisionSplit *split = &tree->splits[node->index];
                    RwCollisionSplit *first = split;
                    while (first->left.count == 0xFF) first = &tree->splits[first->left.index];
                    RwCollisionNode *last = &split->right;
                    while (last->count == 0xFF) last = &tree->splits[last->index].right;
                    start = first->left.index;
                    count = (u16)(last->index + last->count - first->left.index);
                    leafFlags = 0;
                    break;
                }
                RwCollisionSplit *split = &tree->splits[node->index];
                u32 axis = split->left.type & 0xC;
                if (split->rightValue > *(float *)((u8 *)&walk.box.sup + axis)) {
                    entry->node = &split->left;
                    continue;
                }
                entry->node = &split->right;
                if (split->leftValue >= *(float *)((u8 *)&walk.box.inf + axis)) {
                    walk.top++;
                    walk.top->node = &split->left;
                    walk.top->flags = walk.top[-1].flags;
                    if (split->leftValue <= *(float *)((u8 *)&walk.outer.sup + axis)) {
                        walk.top->flags &= ~(2 << (axis >> 1));
                    }
                    if (split->rightValue >= *(float *)((u8 *)&walk.outer.inf + axis)) {
                        walk.top[-1].flags &= ~(1 << (axis >> 1));
                    }
                }
            }

            s32 end = (s32)start + (s32)count;
            for (u16 i = start; (s32)i < end; i++) {
                u16 triangleIndex = tree->map != NULL ? tree->map[i] : i;
                RwCollisionRpTriangle *triangle = &sector->triangles[triangleIndex];
                RwCollisionV3d *v0 = &sector->vertices[triangle->vertIndex[0]];
                RwCollisionV3d *v1 = &sector->vertices[triangle->vertIndex[1]];
                RwCollisionV3d *v2 = &sector->vertices[triangle->vertIndex[2]];
                RwCollisionTriangle collision;
                float distance;
                if (leafFlags != 0) {
                    if (fn_8047B720(sphere, v0, v1, v2, &collision.normal, &distance) == 0) continue;
                } else {
                    float ax = v1->x - v0->x;
                    float ay = v1->y - v0->y;
                    float az = v1->z - v0->z;
                    float bx = v2->x - v0->x;
                    float by = v2->y - v0->y;
                    float bz = v2->z - v0->z;
                    collision.normal.x = ay * bz - az * by;
                    collision.normal.y = az * bx - ax * bz;
                    collision.normal.z = ax * by - ay * bx;
                    float scale = fn_80482394(collision.normal.x * collision.normal.x +
                                              collision.normal.y * collision.normal.y +
                                              collision.normal.z * collision.normal.z);
                    collision.normal.x *= scale;
                    collision.normal.y *= scale;
                    collision.normal.z *= scale;
                    distance = (sphere->center.x - v0->x) * collision.normal.x +
                               (sphere->center.y - v0->y) * collision.normal.y +
                               (sphere->center.z - v0->z) * collision.normal.z;
                }
                distance *= inverseRadius;
                *(RwCollisionWords3 *)&collision.point = *(const RwCollisionWords3 *)v0;
                collision.index = triangleIndex;
                collision.vertices[0] = v0;
                collision.vertices[1] = v1;
                collision.vertices[2] = v2;
                if (callback(intersection, sector, &collision, distance, data) == NULL) return NULL;
            }
        }
    }
    return world;
}


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
    u32 count = local[2];
    u32 *tree = (u32 *)rwAlloc(count * 8 + 8, 0x3001E);
    if (tree == NULL) {
        local[4] = 0x122;
        local[5] = fn_8047DC28(0x80000013, count * 8 + 8);
        fn_8047DB84(local + 4);
    } else {
        tree[0] = count;
        tree[1] = (u32)(tree + 2);
        for (u32 i = 0; i < count; i++) {
            ((u64 *)tree[1])[i] = 0;
        }
    }
    if (tree == NULL) return NULL;
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
        u32 stride = ((flags & 0x10) ? 0xC : 0) + ((flags & 0x8) ? 0x4 : 0) +
                     ((flags & 0x2) ? 0xC : 0) + ((flags & 0x4) ? 0x8 : 0);
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
