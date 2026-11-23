/* RenderWare: RpSkin (plugin 0x116), continued from RwCpp11.cpp.
 *
 * 0x8044BBFC-0x8044D6E8: reading the split data, RpSkinCreate, the plugin's engine, atomic and
 * geometry callbacks and its attach function, and the first part of the GameCube instancing code
 * (resource entries, bone matrices, software skinning entry). Compiled as C.
 *
 * Not written: fn_8044D554 (positions only) sets GQR5/GQR6 in the middle of compiled code, which
 * needs inline assembly. Near misses: fn_8044BDF0 and fn_8044C8C0 (loop registers; the second
 * also has the zeroed-counter copy of docs/research-notes.md) and fn_8044D344 (retail gives each of
 * the three usedBones[i] reads of its first loop an induction pointer of its own; GC/2.7 does so
 * too with `const R2Skin *skin` and skin->usedBones[i] in the loop, probes/Game_RwCpp12_W40.py,
 * but then keeps the zero for boneToWorld.flags in a saved register where retail reloads it).
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

extern R2SkinGlobals lbl_8056D708;
extern s32 lbl_805F1520; // offset of the R2VertexDesc pointer in RpGeometry
extern s32 lbl_805F1578; // offset of R2ResGlobals in the engine globals
extern volatile u16 lbl_805EDF40; // the draw-sync token of the frame being built
extern u32 lbl_805EDD80;
extern u32 lbl_805EDD84;
extern u8 lbl_805E1538[];
extern R2Matrix lbl_805E31CC; // identity

R2Matrix *RwFrameGetLTM(void *frame);
u32 fn_8047D868(void *stream, void *buffer, u32 size);
u32 fn_8047D784(void *stream, void *buffer, u32 size);
u32 fn_80480D4C(void *stream, void *buffer, u32 size);
u32 fn_8047D458(void *stream, const void *buffer, u32 size);
u32 fn_8047D350(void *stream, const void *buffer, u32 size);
u32 fn_80480ED0(void *stream, const void *buffer, u32 size);
u32 fn_804810FC(void *stream, u32 size);
void *fn_8047FC04(u32 entrySize, u32 entriesPerBlock, u32 alignment, u32 blocks, void *list,
                  u32 hint);
void fn_8047FCDC(void *list);
s32 fn_80485DB8(u32 pluginId);
s32 fn_80485D78(u32 size, u32 pluginId, void *open, void *close);
s32 fn_80458B6C(u32 size, u32 pluginId, void *construct, void *destruct, void *copy);
s32 fn_80458BEC(u32 pluginId, void *read, void *write, void *getSize);
s32 fn_80458C28(u32 pluginId, void *alwaysCallback);
s32 fn_80458C5C(u32 pluginId, void *rightsCallback);
s32 fn_80458C90(u32 pluginId);
s32 fn_8045994C(u32 size, u32 pluginId, void *construct, void *destruct, void *copy);
s32 fn_8045998C(u32 pluginId, void *read, void *write, void *getSize);
void fn_8039561C(u16 token);
s32 fn_8049A78C(u16 token);
void fn_8048071C(R2ResEntry *entry);
R2ResEntry *fn_80480878(void *owner, void *ownerRef, u32 size, void *destroyNotify);
void fn_8047EBB8(R2Matrix *out, const R2Matrix *a, const R2Matrix *b);
void fn_8047F0B0(R2Matrix *out, const R2Matrix *in);

u32 fn_8044BAB4(R2Skin *skin);
void *fn_8044BB18(void *stream, R2Skin *skin);
void *fn_8044E6BC(R2Geometry *geometry);
void *fn_8044E758(R2Geometry *geometry);
u32 fn_8044E7D0(R2Geometry *geometry);
void *fn_8044E89C(void *stream, R2Geometry *geometry);
void *fn_8044EAD0(void *stream, R2Geometry *geometry);
u32 fn_8044EE3C();
u32 fn_80451D60(u32 flags);
u32 fn_80451E0C(void);
void *fn_80451E6C(R2Atomic *atomic, s32 type);
void fn_80482458(R2V3d *out, const R2V3d *in, const R2Matrix *matrix);
void fn_80450F68(const R2Matrix *matrices, R2Skin *skin, R2SkinBlendJob *job);
void fn_80451254(const R2Matrix *matrices, R2Skin *skin, R2SkinBlendJob *job);
void fn_80451618(const R2Matrix *matrices, R2Skin *skin, R2SkinBlendJob *job);
void DCFlushRange(void *start, u32 size);
void fn_80394F2C(void);
void fn_80450D6C(R2Matrix *out, const R2Matrix *skinToBone, const R2Matrix *bones,
                 const R2Matrix *last, const u8 *usedBones, u32 numUsedBones);

void *fn_8044C174(void *instance);
void *fn_8044C280(void *instance);
void *fn_8044CD4C(R2Skin *skin);
R2Skin *fn_8044CCAC(R2Geometry *geometry);
void *fn_8044CCC0(R2Geometry *geometry, R2Skin *skin);

#define SKIN_OF(geometry) R2_PLUGIN(R2Skin, geometry, lbl_8056D708.geometryOffset)
#define SKIN_ATOMIC(object) ((R2SkinAtomic *)((u8 *)(object) + lbl_8056D708.atomicOffset))
#define SKIN_HINT 0x30116

/* fn_8044BAB4 as the compiler inlined it into the split data's creation. */
static inline u32 SkinSplitDestroy(R2Skin *skin) {
    if (skin->split.remapIndices != NULL) {
        lbl_805F15B4->memFree(skin->split.remapIndices);
    }
    skin->split.boneLimit = 0;
    skin->split.numMeshes = 0;
    skin->split.numRLE = 0;
    skin->split.remapIndices = NULL;
    skin->split.meshRLECount = NULL;
    skin->split.meshRLE = NULL;
    return 1;
}

static inline R2Skin *SkinSplitCreate(R2Skin *skin, u32 boneLimit, u32 numBones, u32 numMeshes,
                                      u32 numRLE) {
    s32 size;

    SkinSplitDestroy(skin);
    size = numBones + numMeshes * 2 + numRLE * 2;
    skin->split.remapIndices = (u8 *)lbl_805F15B4->memAlloc(size, SKIN_HINT);
    if (skin->split.remapIndices == NULL) {
        return NULL;
    }
    memset(skin->split.remapIndices, 0, size);
    skin->split.boneLimit = boneLimit;
    skin->split.numMeshes = numMeshes;
    skin->split.numRLE = numRLE;
    skin->split.meshRLECount = (u16 *)(skin->split.remapIndices + numBones);
    skin->split.meshRLE = (u16 *)((u8 *)skin->split.meshRLECount + (numMeshes << 1));
    return skin;
}

/* Reads a skin's split data from a stream. */
void *fn_8044BBFC(void *stream, R2Skin *skin) {
    u32 numMeshes;
    u32 numRLE;
    u32 boneLimit;
    R2SkinSplit *split = &skin->split;

    if (!fn_8047D868(stream, &boneLimit, sizeof(u32))) {
        return NULL;
    }
    if (!fn_8047D868(stream, &numMeshes, sizeof(u32))) {
        return NULL;
    }
    if (!fn_8047D868(stream, &numRLE, sizeof(u32))) {
        return NULL;
    }
    if ((s32)numMeshes > 0) {
        if (SkinSplitCreate(skin, boneLimit, skin->numBones, numMeshes, numRLE) == NULL) {
            return NULL;
        }
        if (!fn_80480D4C(stream, split->remapIndices,
                         skin->numBones + split->numMeshes * 2 + split->numRLE * 2)) {
            lbl_805F15B4->memFree(split);
            return NULL;
        }
    }
    return stream;
}

/* Size of a skin's split data in a stream. */
u32 fn_8044BDBC(R2Skin *skin) {
    u32 size = 12;

    if (skin->split.numMeshes != 0) {
        size += skin->numBones + skin->split.numMeshes * 2 + skin->split.numRLE * 2;
    }
    return size;
}

static inline s32 SkinAllocate(R2Skin *skin, u32 numBones, u32 numUsedBones, u32 numVertices,
                               const u8 *usedBones, const R2MatrixWeights *weights,
                               const u32 *indices, const R2Matrix *matrices) {
    s32 size;
    u32 i;

    size = numVertices * 20 + numBones * 64 + numUsedBones + 15;
    skin->memory = lbl_805F15B4->memAlloc(size, SKIN_HINT);
    if (skin->memory == NULL) {
        return 0;
    }
    memset(skin->memory, 0, size);
    skin->numBones = numBones;
    skin->numUsedBones = numUsedBones;
    skin->usedBones = (u8 *)skin->memory;
    skin->matrices = (R2Matrix *)(((u32)skin->usedBones + numUsedBones + 15) & ~15);
    skin->indices = (u32 *)((u8 *)skin->matrices + (numBones << 6));
    skin->weights = (R2MatrixWeights *)((u8 *)skin->indices + (numVertices << 2));
    if (usedBones != NULL && numUsedBones != 0) {
        memcpy(skin->usedBones, usedBones, numUsedBones);
    }
    if (matrices != NULL) {
        i = skin->numBones;
        while (i-- != 0) {
            skin->matrices[i] = matrices[i];
        }
    }
    if (indices != NULL) {
        memcpy(skin->indices, indices, numVertices * sizeof(u32));
    }
    if (weights != NULL) {
        memcpy(skin->weights, weights, numVertices * sizeof(R2MatrixWeights));
    }
    return 1;
}

/* RpSkinCreate, with the used-bone list and the weight count worked out when not given. */
R2Skin *fn_8044BDF0(u32 numVertices, u32 numBones, u32 numUsedBones, u32 maxWeights,
                    const R2MatrixWeights *weights, const u32 *indices, const R2Matrix *matrices) {
    u8 usedBones[256];
    R2Skin *skin;

    skin = (R2Skin *)lbl_805F15B4->freeListAlloc(lbl_8056D708.freeList, SKIN_HINT);
    memset(skin, 0, sizeof(R2Skin));

    if (maxWeights == 0) {
        u32 j;
        u32 i;
        const u32 *weight;
        const R2MatrixWeights *row;

        skin->maxWeights = 1;
        row = weights;
        for (i = 0; i < numVertices; i++) {
            j = skin->maxWeights;
            weight = &row->w[j];
            for (; j < 4; j++) {
                if (*weight == 0) {
                    break;
                }
                skin->maxWeights++;
                if (skin->maxWeights == 4) {
                    goto counted;
                }
                weight++;
            }
            row++;
        }
    }
counted:

    if (numUsedBones == 0) {
        s32 isNew;
        u8 bone;
        u32 j;
        u32 i;
        u32 k;

        numUsedBones = 0;
        for (i = 0; i < numVertices; i++) {
            for (j = 0; j < skin->maxWeights; j++) {
                if (weights[i].w[j] != 0) {
                    bone = (u8)(indices[i] >> (j * 8));
                    isNew = 1;
                    for (k = 0; k < numUsedBones; k++) {
                        if (bone == usedBones[k]) {
                            isNew = 0;
                            break;
                        }
                    }
                    if (isNew) {
                        usedBones[numUsedBones] = bone;
                        numUsedBones++;
                    }
                }
            }
        }
    }

    if (!SkinAllocate(skin, numBones, numUsedBones, numVertices, usedBones, weights, indices,
                      matrices)) {
        lbl_805F15B4->freeListFree(lbl_8056D708.freeList, skin);
        return NULL;
    }
    return skin;
}

/* The plugin's engine open callback. */
void *fn_8044C174(void *instance) {
    s32 flags;

    if (lbl_8056D708.numInstances == 0) {
        flags = 1;
        if (fn_80485DB8(0x120) != -1) {
            flags |= 2;
        }
        if (fn_80485DB8(0x12E) != -1) {
            flags |= 4;
        }
        fn_80451D60(flags);
        lbl_8056D708.freeList =
            fn_8047FC04(sizeof(R2Skin), lbl_805EDD80, 4, lbl_805EDD84, lbl_805E1538, 0x40116);
        lbl_8056D708.matrixCacheMemory = lbl_805F15B4->memAlloc(0x400F, 0x40116);
        memset(lbl_8056D708.matrixCacheMemory, 0, 0x400F);
        lbl_8056D708.matrixCache = (void *)(((u32)lbl_8056D708.matrixCacheMemory + 15) & ~15);
    }
    lbl_8056D708.numInstances++;
    return instance;
}

/* The plugin's engine close callback. */
void *fn_8044C280(void *instance) {
    if (--lbl_8056D708.numInstances == 0) {
        fn_80451E0C();
        fn_8047FCDC(lbl_8056D708.freeList);
        lbl_8056D708.freeList = NULL;
        lbl_805F15B4->memFree(lbl_8056D708.matrixCacheMemory);
        lbl_8056D708.matrixCacheMemory = NULL;
    }
    return instance;
}

/* Geometry constructor. */
void *fn_8044C30C(R2Geometry *geometry) {
    SKIN_OF(geometry) = NULL;
    return geometry;
}

/* Geometry destructor. */
void *fn_8044C324(R2Geometry *geometry) {
    R2Skin *skin = SKIN_OF(geometry);

    if (skin != NULL) {
        fn_8044E758(geometry);
        SKIN_OF(geometry) = (R2Skin *)fn_8044CD4C(skin);
    }
    return geometry;
}

/* Geometry copy callback: nothing is copied. */
void *fn_8044C38C(void *dst) {
    return dst;
}

/* Atomic constructor. */
void *fn_8044C390(R2Atomic *atomic) {
    memset(SKIN_ATOMIC(atomic), 0, sizeof(R2SkinAtomic));
    return atomic;
}

/* Atomic destructor. */
void *fn_8044C3D8(R2Atomic *atomic) {
    if (SKIN_ATOMIC(atomic)->hierarchy != NULL) {
        SKIN_ATOMIC(atomic)->hierarchy = NULL;
    }
    return atomic;
}

/* Atomic copy callback. */
void *fn_8044C3FC(R2Atomic *dst, const R2Atomic *src) {
    SKIN_ATOMIC(dst)->hierarchy = SKIN_ATOMIC(src)->hierarchy;
    return dst;
}

/* Atomic always callback: gives a skinned atomic its pipeline after a read. */
u32 fn_8044C414(R2Atomic *atomic) {
    s32 type = 1;

    if (fn_80485DB8(0x120) != -1) {
        if (*((u8 *)atomic + fn_80458C90(0x120)) != 0) {
            type = 2;
        }
    }
    if (atomic->geometry != NULL && fn_8044CCAC(atomic->geometry) != NULL) {
        if (fn_80485DB8(0x120) == -1 && type == 2) {
            type = 1;
        } else if (fn_80485DB8(0x12E) == -1 && type == 3) {
            type = 1;
        }
        fn_80451E6C(atomic, type);
    }
    return 1;
}

/* Atomic rights callback: the pipeline type comes from the stream. */
u32 fn_8044C4D4(R2Atomic *atomic, void *stream, s32 length, s32 type) {
    if (atomic->geometry != NULL && fn_8044CCAC(atomic->geometry) != NULL) {
        if (fn_80485DB8(0x120) == -1 && type == 2) {
            type = 1;
        } else if (fn_80485DB8(0x12E) == -1 && type == 3) {
            type = 1;
        }
        fn_80451E6C(atomic, type);
    }
    return 1;
}

/* Geometry stream callbacks: size, write and read. Native (pre-instanced) geometry goes through
 * the platform functions in RwCpp13.cpp. */
s32 fn_8044C56C(void *object) {
    R2Geometry *geometry = (R2Geometry *)object;
    u32 size = 0;
    R2Skin *skin = SKIN_OF(geometry);

    if (skin != NULL) {
        if (!(geometry->flags & 0x01000000)) {
            s32 numVertices = geometry->numVertices;
            size = sizeof(u32) + skin->numUsedBones;
            size += numVertices * sizeof(u32);
            size += numVertices * sizeof(R2MatrixWeights);
            size += skin->numBones * sizeof(R2Matrix);
            size += fn_8044BDBC(skin);
        } else {
            size = fn_8044E7D0(geometry);
        }
    }
    return size;
}

void *fn_8044C5FC(void *stream, s32 length, void *object) {
    R2Geometry *geometry = (R2Geometry *)object;
    R2Skin *skin = SKIN_OF(geometry);
    s32 numVertices;
    u32 header;

    if (skin != NULL) {
        if (!(geometry->flags & 0x01000000)) {
            numVertices = geometry->numVertices;
            header = ((skin->maxWeights & 0xFF) << 16) | ((skin->numUsedBones & 0xFF) << 8) |
                     (skin->numBones & 0xFF);
            if (!fn_8047D458(stream, &header, sizeof(u32))) {
                return NULL;
            }
            if (!fn_80480ED0(stream, skin->usedBones, skin->numUsedBones)) {
                return NULL;
            }
            if (!fn_8047D458(stream, skin->indices, numVertices * sizeof(u32))) {
                return NULL;
            }
            if (!fn_8047D350(stream, skin->weights, numVertices * sizeof(R2MatrixWeights))) {
                return NULL;
            }
            if (!fn_8047D350(stream, skin->matrices, skin->numBones * sizeof(R2Matrix))) {
                return NULL;
            }
            if (!fn_8044BB18(stream, skin)) {
                return NULL;
            }
        } else {
            if (!fn_8044E89C(stream, geometry)) {
                return NULL;
            }
        }
    }
    return stream;
}

void *fn_8044C754(void *stream, s32 length, void *object) {
    R2Geometry *geometry = (R2Geometry *)object;
    u32 maxWeights;
    u32 numUsedBones;
    u32 numVertices;
    R2Skin *skin;
    u32 header;

    if (!(geometry->flags & 0x01000000)) {
        if (!fn_8047D868(stream, &header, sizeof(u32))) {
            return NULL;
        }
        numVertices = geometry->numVertices;
        numUsedBones = (header >> 8) & 0xFF;
        maxWeights = (header >> 16) & 0xFF;
        skin = fn_8044BDF0(numVertices, header & 0xFF, numUsedBones, maxWeights, NULL, NULL, NULL);
        if (skin == NULL) {
            return NULL;
        }
        if (fn_80480D4C(stream, skin->usedBones, numUsedBones) != numUsedBones) {
            return NULL;
        }
        if (!fn_8047D868(stream, skin->indices, numVertices * sizeof(u32))) {
            return NULL;
        }
        if (!fn_8047D784(stream, skin->weights, numVertices * sizeof(R2MatrixWeights))) {
            return NULL;
        }
        skin->maxWeights = maxWeights;
        if (!fn_8047D784(stream, skin->matrices, skin->numBones * sizeof(R2Matrix))) {
            return NULL;
        }
        if (!fn_8044BBFC(stream, skin)) {
            return NULL;
        }
        fn_8044CCC0(geometry, skin);
    } else {
        if (!fn_8044EAD0(stream, geometry)) {
            return NULL;
        }
    }
    return stream;
}

/* Atomic stream read callback: the skin chunk older files kept on the atomic. */
void *fn_8044C8C0(void *stream, s32 length, void *object, s32 offset, s32 size) {
    R2Atomic *atomic = (R2Atomic *)object;
    R2Skin *skin;
    R2Geometry *geometry = atomic->geometry;
    u32 numVertices;
    u32 numBones;
    u32 i;

    if (fn_8044CCAC(geometry) == NULL) {
        if (!fn_8047D868(stream, &numBones, sizeof(u32))) {
            return NULL;
        }
        numVertices = geometry->numVertices;
        skin = fn_8044BDF0(numVertices, numBones, numBones, 4, NULL, NULL, NULL);
        if (!fn_804810FC(stream, 4)) {
            return NULL;
        }
        if (!fn_8047D868(stream, skin->indices, numVertices * sizeof(u32))) {
            return NULL;
        }
        if (!fn_8047D784(stream, skin->weights, numVertices * sizeof(R2MatrixWeights))) {
            return NULL;
        }
        for (i = 0; i < skin->numBones; i++) {
            if (!fn_804810FC(stream, 12)) {
                return NULL;
            }
            if (!fn_8047D784(stream, &skin->matrices[i], sizeof(R2Matrix))) {
                return NULL;
            }
        }

        {
        u32 j;
        u32 i;
        const u32 *weight;
        const R2MatrixWeights *weights;

        weights = skin->weights;
        skin->maxWeights = 1;
        for (i = 0; i < numVertices; i++) {
            j = skin->maxWeights;
            weight = &weights->w[j];
            for (; j < 4; j++) {
                if (*weight == 0) {
                    break;
                }
                skin->maxWeights++;
                if (skin->maxWeights == 4) {
                    goto counted;
                }
                weight++;
            }
            weights++;
        }
        }
counted:
        {
        s32 isNew;
        u8 bone;
        u32 k;
        u32 j;
        u32 i;
        u8 *usedBones;
        const u32 *weight;
        u32 shift;
        const R2MatrixWeights *weights;
        u32 numUsedBones;
        const u32 *indices;

        weights = skin->weights;
        indices = skin->indices;
        usedBones = skin->usedBones;
        skin->numUsedBones = 0;
        for (i = 0; i < numVertices; i++) {
            weight = &weights->w[0];
            for (j = 0, shift = 0; j < skin->maxWeights; j++) {
                if (*weight != 0) {
                    bone = (u8)(*indices >> (shift));
                    numUsedBones = skin->numUsedBones;
                    isNew = 1;
                    for (k = 0; k < numUsedBones; k++) {
                        if (bone == usedBones[k]) {
                            isNew = 0;
                            break;
                        }
                    }
                    if (isNew) {
                        usedBones[numUsedBones] = bone;
                        skin->numUsedBones++;
                    }
                }
                weight++;
                shift += 8;
            }
            weights++;
            indices++;
        }
        }
        fn_8044CCC0(geometry, skin);
    } else {
        if (!fn_804810FC(stream, size)) {
            return NULL;
        }
    }
    return stream;
}

/* Atomic stream write callback: nothing is written. */
void *fn_8044CB6C(void *stream) {
    return stream;
}

/* Atomic stream size callback. */
u32 fn_8044CB70() {
    return fn_8044EE3C();
}

/* RpSkinPluginAttach */
u32 fn_8044CB90(void) {
    lbl_8056D708.engineOffset = fn_80485D78(0, 0x116, fn_8044C174, fn_8044C280);
    lbl_8056D708.atomicOffset =
        fn_80458B6C(sizeof(R2SkinAtomic), 0x116, fn_8044C390, fn_8044C3D8, fn_8044C3FC);
    fn_80458BEC(0x116, fn_8044C8C0, fn_8044CB6C, fn_8044CB70);
    fn_80458C28(0x116, fn_8044C414);
    fn_80458C5C(0x116, fn_8044C4D4);
    lbl_8056D708.geometryOffset =
        fn_8045994C(sizeof(R2Skin *), 0x116, fn_8044C30C, fn_8044C324, fn_8044C38C);
    fn_8045998C(0x116, fn_8044C754, fn_8044C5FC, fn_8044C56C);
    return 1;
}

/* RpSkinAtomicSetHAnimHierarchy */
R2Atomic *fn_8044CC98(R2Atomic *atomic, void *hierarchy) {
    SKIN_ATOMIC(atomic)->hierarchy = hierarchy;
    return atomic;
}

/* RpSkinGeometryGetSkin */
R2Skin *fn_8044CCAC(R2Geometry *geometry) {
    return SKIN_OF(geometry);
}

/* RpSkinGeometrySetSkin */
void *fn_8044CCC0(R2Geometry *geometry, R2Skin *skin) {
    if (skin != SKIN_OF(geometry)) {
        if (SKIN_OF(geometry) != NULL) {
            fn_8044E758(geometry);
        }
        SKIN_OF(geometry) = skin;
        if (skin != NULL) {
            if (!fn_8044E6BC(geometry)) {
                return NULL;
            }
        }
    }
    return geometry;
}

/* RpSkinDestroy */
void *fn_8044CD4C(R2Skin *skin) {
    if (skin->memory != NULL) {
        lbl_805F15B4->memFree(skin->memory);
    }
    fn_8044BAB4(skin);
    lbl_805F15B4->freeListFree(lbl_8056D708.freeList, skin);
    return NULL;
}

/* RpSkinGetVertexBoneIndices */
const u32 *fn_8044CDBC(R2Skin *skin) {
    return skin->indices;
}

/* Waits until the graphics processor is done with the arrays of a resource entry: a draw-sync
 * token is sent for the frame being built if the entry was used in it. */
static inline void SkinWaitForToken(R2SkinResEntry *entry) {
    if (entry->instance.token == lbl_805EDF40) {
        fn_8039561C(lbl_805EDF40);
        lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
    }
    while (!fn_8049A78C(entry->instance.token)) {
    }
}

/* Destroy notify of an atomic's or geometry's skinning resource entry. */
void fn_8044CDC4(R2SkinResEntry *entry) {
    R2Geometry *owner;
    u32 flags;

    SkinWaitForToken(entry);
    if (entry->instance.positions != NULL) {
        fn_8048071C(((R2ResEntry **)entry->instance.positions)[-1]);
    }
    owner = (R2Geometry *)entry->entry.owner;
    flags = owner->flags;
    if (owner->object.type == 1) {
        R2SkinAtomic *data = SKIN_ATOMIC(owner);
        entry->instance.positions = data->positions;
        if (flags & 0x10) {
            entry->instance.normals = data->normals;
        }
        data->positions = NULL;
        data->normals = NULL;
    } else {
        R2Skin *skin = SKIN_OF(owner);
        entry->instance.positions = skin->positions;
        if (flags & 0x10) {
            entry->instance.normals = skin->normals;
        }
        skin->positions = NULL;
        skin->normals = NULL;
    }
}

/* Destroy notify of the vertex arrays' own resource entry. */
void fn_8044CEE0(R2SkinResEntry *entry) {
    SkinWaitForToken(entry);
}

#define LL_REMOVE(link) \
    ((link)->prev->next = (link)->next, (link)->next->prev = (link)->prev)
#define LL_ADD_FRONT(list, link) \
    ((link)->next = (list)->next, (link)->prev = (list), (list)->next->prev = (link), \
     (list)->next = (link))
#define RES_USED (((R2ResGlobals *)((u8 *)lbl_805F15B4 + lbl_805F1578))->usedEntries)
#define RES_OF(array) (((R2SkinResEntry **)(array))[-1])

/* Bytes per vertex of a position or normal by its GX component type (u8, s8, u16, s16, f32). The
 * source surely had a local array with an initializer; its anonymous .rodata copy (lbl_8051BD10)
 * lies outside this unit's split, so the table is copied from the retail symbol by name. */
typedef struct R2ComponentSizes {
    u32 size[5];
} R2ComponentSizes;
extern const R2ComponentSizes lbl_8051BD10;

/* Instance step of a skinned atomic: keeps the geometry's resource entry in use and makes sure
 * the skinned position (and normal) arrays exist; they live in a resource entry of their own,
 * whose address is kept in the word in front of the positions. */
s32 fn_8044CF68(R2Skin *skin, R2Atomic *atomic, R2SkinInstance *instance, R2ResEntry **repEntry) {
    R2Geometry *geometry = atomic->geometry;
    R2Geometry *owner;
    R2ResEntry *entry;
    void *source;
    u32 size;

    if ((*repEntry)->link.next != NULL) {
        LL_REMOVE(&(*repEntry)->link);
        LL_ADD_FRONT(RES_USED, &(*repEntry)->link);
    }
    (*repEntry)->destroyNotify = (void (*)(R2ResEntry *))fn_8044CDC4;

    owner = (R2Geometry *)(*repEntry)->owner;
    if (owner->object.type == 1) {
        R2SkinAtomic *data = SKIN_ATOMIC(owner);
        if (data->positions == NULL) {
            data->positions = instance->positions;
            instance->positions = NULL;
            if (geometry->flags & 0x10) {
                data->normals = instance->normals;
                instance->normals = NULL;
            }
        }
    } else {
        if (skin->positions == NULL) {
            skin->positions = instance->positions;
            instance->positions = NULL;
            if (geometry->flags & 0x10) {
                skin->normals = instance->normals;
                instance->normals = NULL;
            }
        }
    }

    if (instance->positions != NULL) {
        if (!fn_8049A78C(RES_OF(instance->positions)->instance.token)) {
            RES_OF(instance->positions)->entry.ownerRef = NULL;
            instance->positions = NULL;
            if (geometry->flags & 0x10) {
                instance->normals = NULL;
            }
        }
    }

    if (instance->positions == NULL) {
        R2VertexDesc *desc = R2_PLUGIN(R2VertexDesc, geometry, lbl_805F1520);
        if (desc != NULL) {
            R2ComponentSizes sizes = lbl_8051BD10;
            size = ((geometry->numVertices * sizes.size[desc->type[0]] + 31) & ~31) + 6;
            if (geometry->flags & 0x10) {
                size += (geometry->numVertices *
                             (sizes.size[desc->type[1]] * (desc->frac[1] != 0 ? 3 : 1)) +
                         31) &
                        ~31;
            }
        } else {
            size = ((geometry->numVertices * 12 + 31) & ~31) + 6;
            if (geometry->flags & 0x10) {
                size += (geometry->numVertices * 12 + 31) & ~31;
            }
        }
        entry = fn_80480878(atomic, &instance->positions, size, fn_8044CEE0);
        if (geometry->numMorphTargets == 1) {
            source = geometry->repEntry;
        } else {
            source = atomic->repEntry;
        }
        if (entry == NULL || source == NULL) {
            if (entry != NULL) {
                fn_8048071C(entry);
            }
            return 0;
        }
        instance->positions = (void *)(((u32)instance->positions + 0x3D) & ~0x1F);
        if (geometry->flags & 0x10) {
            instance->normals = (u8 *)instance->positions +
                                ((geometry->numVertices * instance->normalStride + 31) & ~31);
        }
        RES_OF(instance->positions) = (R2SkinResEntry *)entry;
    } else {
        if (RES_OF(instance->positions)->entry.link.next != NULL) {
            LL_REMOVE(&RES_OF(instance->positions)->entry.link);
            LL_ADD_FRONT(RES_USED, &RES_OF(instance->positions)->entry.link);
        }
    }
    RES_OF(instance->positions)->instance.token = lbl_805EDF40;
    return 1;
}

/* Builds the matrix palette: skin-to-bone times bone matrix for every used bone, in the space the
 * skinning needs (the hierarchy keeps no matrices, local-space matrices or world-space ones). */
void fn_8044D344(R2Matrix *out, R2Skin *skin, R2Matrix *ltm, R2HAnimHierarchy *hierarchy) {
    if (hierarchy != NULL) {
        if (hierarchy->flags & 0x2) {
            R2Matrix inverse;
            R2Matrix boneToWorld;
            const R2Matrix *last;
            const u8 *usedBones;
            u32 i;
            const R2Matrix *matrices;
            u32 numUsedBones;
            const R2Matrix *boneLTM;

            if (skin->maxWeights > 1 || skin->unk2C == 3) {
                inverse.flags = 0;
                fn_8047F0B0(&inverse, ltm);
                last = &inverse;
            } else {
                last = &lbl_805E31CC;
            }
            usedBones = skin->usedBones;
            matrices = skin->matrices;
            numUsedBones = skin->numUsedBones;
            for (i = 0; i < numUsedBones; i++) {
                boneToWorld.flags = 0;
                boneLTM = RwFrameGetLTM(hierarchy->nodeInfo[usedBones[i]].frame);
                fn_8047EBB8(&boneToWorld, &matrices[usedBones[i]], boneLTM);
                fn_8047EBB8(&out[usedBones[i]], &boneToWorld, last);
            }
        } else if (hierarchy->flags & 0x4000) {
            if (skin->maxWeights > 1 || skin->unk2C == 3) {
                const u8 *usedBones = skin->usedBones;
                u32 i;
                const R2Matrix *matrices = skin->matrices;
                u32 numUsedBones = skin->numUsedBones;

                for (i = 0; i < numUsedBones; i++) {
                    u32 bone = usedBones[i];
                    fn_8047EBB8(&out[bone], &matrices[bone], &hierarchy->matrixArray[bone]);
                }
            } else {
                R2Matrix last;
                last.flags = 0;
                fn_8047EBB8(&last, ltm, &lbl_805E31CC);
                fn_80450D6C(out, skin->matrices, hierarchy->matrixArray, &last, skin->usedBones,
                            skin->numUsedBones);
            }
        } else {
            R2Matrix inverse;
            const R2Matrix *last;

            if (skin->maxWeights > 1 || skin->unk2C == 3) {
                inverse.flags = 0;
                fn_8047F0B0(&inverse, ltm);
                last = &inverse;
            } else {
                last = &lbl_805E31CC;
            }
            fn_80450D6C(out, skin->matrices, hierarchy->matrixArray, last, skin->usedBones,
                        skin->numUsedBones);
        }
    }
}
