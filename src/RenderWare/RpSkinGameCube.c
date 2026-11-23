/* RenderWare: RpSkin (plugin 0x116), GameCube back end: the skinning resource entries, the bone
 * matrix palette, the skinned atomic pipelines (instance, reinstance and render callbacks, with and
 * without material effects), the native skin data (weight packing, stream size, write and read),
 * the display-list instancing of native skinned geometry, and RpSkinAtomicSetType.
 *
 * 0x8044CDC4-0x80451F48 (.rodata 0x8051BD10-0x8051BD28, .bss 0x805E1560-0x805E1580,
 * .sdata2 0x805FB0A8-0x805FB0D8). The platform-neutral RpSkin.c comes before it (see there for the
 * boundary); RpUserData.c follows. Built as C with cflags_rw_c (configure.py), the flags the skin
 * code was matched with in wave 40.
 *
 * Not written:
 * - fn_8044D554 (positions only) and fn_8044D6E8 (positions and normals) set GQR5-GQR7 in the
 *   middle of compiled code; they own the u8[5] initializers at 0x805FB0A8-0x805FB0D0.
 * - fn_80450D6C, fn_80450F68, fn_80451080, fn_80451254, fn_804513C4, fn_80451618, fn_804517C8:
 *   hand-written paired-single assembly (they save their own registers and use psq_l/psq_st),
 *   which stays unwritten.
 * Near misses: fn_8044DA3C (retail forms lbl_8056D708's address in r3 after the object's copy;
 * GC/2.7 schedules it first, into r5), fn_8044DD1C and fn_8044DE74 (retail keeps incrementing the
 * run's bone counter beside its strength-reduced matrix offset; GC/2.7 drops the dead counter),
 * fn_80451BCC, and fn_80451E6C (RpSkinAtomicSetType: its switch compares 2 first where retail
 * compares 1, 3 rows; probes/RenderWare_RpSkinGameCube_W41b.py). The display-list instancing functions
 * (fn_8044EEAC-fn_8045066C, fn_80450938) are drafts.
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

extern R2SkinGlobals lbl_8056D708;
extern s32 lbl_805F1520; /* offset of the R2VertexDesc pointer in RpGeometry */
extern s32 lbl_805F1578; /* offset of R2ResGlobals in the engine globals */
extern s32 lbl_805EDDD8; /* 1 selects fn_8044F9E4 for geometries flagged 0x02000000 */
extern s32 lbl_805F1658; /* offset of the GameCube raster extension */
extern volatile u16 lbl_805EDF40; /* the draw-sync token of the frame being built */
extern u16 lbl_805F1678;
extern u32 lbl_805F16B8;
extern R2Matrix lbl_805E31CC; /* identity */

/* The vertex format the display-list instancing builds (fn_8046FC1C and friends fill it in). */
static u32 skinVertexFormat[8];

R2Matrix *RwFrameGetLTM(void *frame);
void fn_803991E8(f32 mtx[3][4], u32 id);
void fn_80399238(f32 mtx[3][4], u32 id);
void fn_80399000(void *list, u32 size);
void fn_8039561C(u16 token);
s32 fn_8049A78C(u16 token);
void fn_8048071C(R2ResEntry *entry);
R2ResEntry *fn_80480878(void *owner, void *ownerRef, u32 size, void *destroyNotify);
void fn_8047EBB8(R2Matrix *out, const R2Matrix *a, const R2Matrix *b);
void fn_8047F0B0(R2Matrix *out, const R2Matrix *in);
R2Skin *fn_8044CCAC(const R2Geometry *geometry);
void *fn_8044CCC0(R2Geometry *geometry, R2Skin *skin);
u32 fn_8044BDBC(R2Skin *skin);
void *fn_8044BB18(void *stream, R2Skin *skin);
void *fn_8044BBFC(void *stream, R2Skin *skin);
s32 fn_8044CF68(R2Skin *skin, R2Atomic *atomic, R2SkinInstance *instance, R2ResEntry **repEntry);
void fn_8044D344(R2Matrix *out, R2Skin *skin, R2Matrix *ltm, R2HAnimHierarchy *hierarchy);
void fn_8044D554(R2Skin *skin, R2Matrix *matrices, void *srcPositions, void *dstPositions,
                 R2VertexDesc *desc, s32 numVertices);
void fn_8044D6E8(R2Skin *skin, R2Matrix *matrices, void *srcPositions, void *srcNormals,
                 void *dstPositions, void *dstNormals, R2VertexDesc *desc, s32 numVertices);
void fn_80450D6C(R2Matrix *out, const R2Matrix *skinToBone, const R2Matrix *bones,
                 const R2Matrix *last, const u8 *usedBones, u32 numUsedBones);
R2ResEntry *fn_8044F9E4(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef);
R2ResEntry *fn_8046C0D4(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef);
R2ResEntry *fn_80450938(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef);
void fn_80463840(R2Atomic *atomic, R2ResEntry **repEntry);
void fn_8046288C(R2ResEntry *entry);
void fn_80462C30(R2VertexDesc *desc, void *node);
void *fn_80462E1C(void *geometry);
void fn_804950AC(R2Matrix *ltm, s32 normals);
void fn_80394500(u32 a, u32 b);
void *fn_804655D4(u32 flags, u32 a, u32 b, u32 c);
void fn_80499C38(void *texture, u32 stage);
void fn_80497778(s32 enable);
s32 fn_8047CF50(void *stream, u32 type, u32 *length, u32 *version);
u32 fn_8047CEF0(void *stream, u32 type, u32 length, u32 version, u32 buildNum);
u32 fn_8047D868(void *stream, void *buffer, u32 size);
u32 fn_8047D784(void *stream, void *buffer, u32 size);
u32 fn_8047D458(void *stream, const void *buffer, u32 size);
u32 fn_8047D350(void *stream, const void *buffer, u32 size);
u32 fn_80480D4C(void *stream, void *buffer, u32 size);
u32 fn_80480ED0(void *stream, const void *buffer, u32 size);
s32 fn_8047DC28(s32 code, ...);
void fn_8047DB84(void *error);
void fn_8046FC1C(u32 *format);
void fn_8046FC48(u32 *format, u32 value);
void fn_8046FC50(u32 *format, u32 attr, u32 a, u32 b, u32 c);
void fn_8046FF34(u32 *format, u32 attr, u32 type);
u32 fn_8047046C(u32 *format, u32 count);
u32 *fn_80466194(u32 *, u32, u32);
u32 *fn_80465ED4(u32 *, u8 *, u32, u32);
u32 fn_80465CDC(u32 *, u32 *, u32, u32);
u32 fn_8046EE8C(u32);
u32 fn_8046EEA0(u32, u32 *);
u32 fn_8046B094(u32, u32, u32);
void fn_8046D8C8(u32, u32, u32 *, u32 *, u32);
void fn_8046B0E0(u8 *, u32, u32, u8 *);
void *fn_8046ACBC(u32);
void fn_8046E594(u32, u8 *, u32 *, u32, u32, void *, u32, u32, u32);
void fn_8046F208(u32, u8 *, u32 *, u32);
void fn_8046F6CC(u32, u8 *, u32 *, u32, u32);
void DCFlushRange(void *start, u32 size);
void fn_80394F2C(void);
void fn_8049B800(void *pipeline);
u32 fn_8046335C(u32, u32, void *, void *, void *, void *);
s32 fn_80458C90(u32 pluginId);
s32 fn_804A2064(void *atomic);
s32 fn_804A8478(void *node);
void fn_80463104(void *geometry, u32 desc);
s32 fn_80462C28(void);
void fn_80462C20(u32 state);
void fn_804A8120(void *node, void *format, u32 count);
void fn_804A2F08(void);
void fn_80463428(void);

R2Atomic *fn_8044D924(void *object, R2ResEntry **instance);
void fn_8044DC4C(R2Matrix *matrix, s32 id, s32 normals);
void fn_8044DD1C(R2Skin *skin, u32 meshIndex, s32 normals);
R2Atomic *fn_80451BCC(R2Atomic *atomic, R2GxNodeData *node);

#define SKIN_OF(geometry) R2_PLUGIN(R2Skin, geometry, lbl_8056D708.geometryOffset)
#define SKIN_ATOMIC(object) ((R2SkinAtomic *)((u8 *)(object) + lbl_8056D708.atomicOffset))
#define SKIN_GLOBALS (&lbl_8056D708)
#define VERTEX_DESC(geometry) R2_PLUGIN(R2VertexDesc, geometry, lbl_805F1520)
#define RASTER_EXT(raster) ((R2RasterExt *)((u8 *)(raster) + lbl_805F1658))
#define RES_OF(array) (((R2SkinResEntry **)(array))[-1])
#define SKIN_HINT 0x30116
#define WEIGHT(skin, vertex, n) (((f32 *)&(skin)->weights[vertex])[n])

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
            /* bytes per vertex by GX component type (u8, s8, u16, s16, f32); the initializer is this
             * file's .rodata */
            u32 sizes[5] = {3, 3, 6, 6, 12};
            size = ((geometry->numVertices * sizes[desc->type[0]] + 31) & ~31) + 6;
            if (geometry->flags & 0x10) {
                size += (geometry->numVertices *
                             (sizes[desc->type[1]] * (desc->frac[1] != 0 ? 3 : 1)) +
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

/* Instance callback of the skinned atomic pipeline. */
R2Atomic *fn_8044D924(void *object, R2ResEntry **instance) {
    R2Atomic *atomic = object;
    R2Geometry *geometry = atomic->geometry;
    R2Skin *skin = fn_8044CCAC(geometry);
    void *owner;
    R2ResEntry **ownerRef;

    if (geometry->numMorphTargets != 1) {
        owner = atomic;
        ownerRef = &atomic->repEntry;
    } else {
        owner = geometry;
        ownerRef = &geometry->repEntry;
    }
    if (skin->maxWeights > 1 || skin->unk2C == 3) {
        if (geometry->flags & 0x02000000) {
            if (lbl_805EDDD8 == 1) {
                *instance = fn_8044F9E4(geometry, owner, ownerRef);
            } else {
                *instance = fn_8046C0D4(geometry, owner, ownerRef);
            }
        } else {
            *instance = fn_8046C0D4(geometry, owner, ownerRef);
        }
    } else {
        if (geometry->flags & 0x02000000) {
            if (lbl_805EDDD8 == 1) {
                *instance = fn_8044F9E4(geometry, owner, ownerRef);
            } else {
                *instance = fn_80450938(geometry, owner, ownerRef);
            }
        } else {
            *instance = fn_80450938(geometry, owner, ownerRef);
        }
    }
    geometry->lockedSinceLastInst = 0;
    return atomic;
}

/* Reinstance callback: skins the vertices on the CPU (more than one weight) or uploads the
 * bone matrices. */
R2Atomic *fn_8044DA3C(void *object, R2ResEntry **repEntry) {
    R2SkinInstance *instance;
    R2Atomic *atomic = object;
    R2Geometry *geometry;
    R2Skin *skin;
    R2Geometry *owner;
    void **positions;
    void **normals;
    void *savedPositions;
    void *savedNormals;

    geometry = atomic->geometry;
    skin = SKIN_OF(geometry);
    if (skin->maxWeights > 1 || skin->unk2C == 3) {
        instance = &((R2SkinResEntry *)*repEntry)->instance;
        owner = (R2Geometry *)(*repEntry)->owner;

        if (owner->object.type == 1) {
            R2SkinAtomic *data = SKIN_ATOMIC(owner);
            positions = &data->positions;
            normals = &data->normals;
        } else {
            positions = &skin->positions;
            normals = &skin->normals;
        }
        if (*positions == NULL) {
            if (!(geometry->flags & 0x01000000)) {
                fn_80463840(atomic, repEntry);
            }
            *positions = instance->positions;
            instance->positions = NULL;
            if (geometry->flags & 0x10) {
                *normals = instance->normals;
                instance->normals = NULL;
            }
        } else if (!(geometry->flags & 0x01000000)) {
            savedPositions = instance->positions;
            instance->positions = *positions;
            if (geometry->flags & 0x10) {
                savedNormals = instance->normals;
                instance->normals = *normals;
                fn_80463840(atomic, repEntry);
                instance->normals = savedNormals;
            } else {
                fn_80463840(atomic, repEntry);
            }
            instance->positions = savedPositions;
        }
        if (!fn_8044CF68(skin, atomic, instance, repEntry)) {
            return NULL;
        }
        fn_8044D344(lbl_8056D708.matrixCache, skin, RwFrameGetLTM(atomic->object.parent),
                    SKIN_ATOMIC(atomic)->hierarchy);
        if (geometry->flags & 0x10) {
            fn_8044D6E8(skin, lbl_8056D708.matrixCache, *positions, *normals, instance->positions,
                        instance->normals, VERTEX_DESC(geometry), geometry->numVertices);
        } else {
            fn_8044D554(skin, lbl_8056D708.matrixCache, *positions, instance->positions,
                        VERTEX_DESC(geometry), geometry->numVertices);
        }
    } else {
        fn_8044D344(lbl_8056D708.matrixCache, skin, RwFrameGetLTM(atomic->object.parent),
                    SKIN_ATOMIC(atomic)->hierarchy);
    }
    return atomic;
}

/* fn_8044DC4C and fn_8044DD1C are inlined into fn_8044DE74 below and also emitted out of line. */
#pragma auto_inline on

/* Loads a matrix of the palette into GX position (and normal) matrix memory: transposed to GX's
 * three rows, with x and z negated. */
void fn_8044DC4C(R2Matrix *matrix, s32 id, s32 normals) {
    f32 mtx[3][4];

    mtx[0][0] = -matrix->right.x;
    mtx[0][1] = -matrix->up.x;
    mtx[0][2] = -matrix->at.x;
    mtx[0][3] = -matrix->pos.x;
    mtx[1][0] = matrix->right.y;
    mtx[1][1] = matrix->up.y;
    mtx[1][2] = matrix->at.y;
    mtx[1][3] = matrix->pos.y;
    mtx[2][0] = -matrix->right.z;
    mtx[2][1] = -matrix->up.z;
    mtx[2][2] = -matrix->at.z;
    mtx[2][3] = -matrix->pos.z;
    fn_803991E8(mtx, id);
    if (normals) {
        fn_80399238(mtx, id);
    }
}

/* Loads the bone matrices one mesh of a split skin uses, run by run. */
void fn_8044DD1C(R2Skin *skin, u32 meshIndex, s32 normals) {
    const u8 *counts = (const u8 *)skin->split.meshRLECount;
    u32 start = counts[meshIndex * 2] & 0x7F;
    u32 numRuns = counts[meshIndex * 2 + 1];
    u32 id = 0;
    u8 i;

    for (i = 0; i < numRuns; i++) {
        const R2SkinRLE *run = (const R2SkinRLE *)skin->split.meshRLE + i + start;
        u32 bone = run->start;
        u32 count = run->length;
        u32 j;

        for (j = 0; j < count; j++) {
            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + bone, id, normals);
            id += 3;
            bone++;
        }
    }
}

#pragma auto_inline reset

/* Render callback: sets up the vertex format and the lighting, then draws the display list of
 * every mesh, loading the bone matrices of split skins mesh by mesh. */
R2Atomic *fn_8044DE74(R2Atomic *atomic, R2GxNodeData *node) {
    R2SkinInstance *instance = &node->resEntry->instance;
    R2DisplayList *list;
    R2Matrix *ltm;
    R2Skin *skin;
    R2GxLightCallback callback;
    u32 remaining;
    R2Mesh *mesh;

    node->resEntry->instance.token = lbl_805EDF40;
    list = (R2DisplayList *)&instance->normals + (instance->numArrays - 1);
    ltm = RwFrameGetLTM(atomic->object.parent);
    fn_80462C30(VERTEX_DESC(atomic->geometry), node);
    skin = fn_8044CCAC(atomic->geometry);
    if (skin->maxWeights > 1 || skin->unk2C == 3) {
        fn_804950AC(ltm, (node->flags >> 4) & 1);
    } else {
        fn_80394500(0, 1);
    }
    if (VERTEX_DESC(atomic->geometry) == NULL) {
        fn_80462E1C(atomic->geometry);
    }
    callback = (R2GxLightCallback)fn_804655D4(node->flags, node->unk20, node->unk1C,
                                              instance->flags & 1);
    remaining = node->meshHeader->numMeshes;
    mesh = (R2Mesh *)(node->meshHeader + 1);
    if (skin->maxWeights == 1 && skin->split.numMeshes == 0) {
        u32 id = 0;
        u32 i;

        for (i = 0; i < skin->numUsedBones; i++) {
            fn_8044DC4C((R2Matrix *)lbl_8056D708.matrixCache + skin->usedBones[i], id,
                        (node->flags >> 4) & 1);
            id += 3;
        }
    }
    if (node->flags & 0x84) {
        while (remaining-- != 0) {
            R2Texture *texture = mesh->material->texture;

            fn_80499C38(texture, 0);
            if (texture != NULL && texture->raster != NULL) {
                fn_80497778((RASTER_EXT(texture->raster->parent)->flags & 1) ^ 1);
            } else {
                fn_80497778(1);
            }
            if (callback != NULL) {
                callback(node->lighting, &mesh->material->color,
                         mesh->material->surfaceProps.ambient);
            }
            if (skin->maxWeights == 1 && skin->split.numMeshes != 0) {
                fn_8044DD1C(skin, node->meshHeader->numMeshes - (remaining + 1),
                            (node->flags >> 4) & 1);
            }
            fn_80399000(list->data, list->size);
            list++;
            mesh++;
        }
    } else {
        fn_80497778(1);
        while (remaining-- != 0) {
            if (callback != NULL) {
                callback(node->lighting, &mesh->material->color,
                         mesh->material->surfaceProps.ambient);
            }
            if (skin->maxWeights == 1 && skin->split.numMeshes != 0) {
                fn_8044DD1C(skin, node->meshHeader->numMeshes - (remaining + 1),
                            (node->flags >> 4) & 1);
            }
            fn_80399000(list->data, list->size);
            list++;
            mesh++;
        }
    }
    return atomic;
}

/* Sorts every vertex's weights in decreasing order (with its bone indices) and, for more than one
 * weight per vertex, packs them for the CPU skinning: the weights scaled to 128 so that they sum
 * to 128, then the indices, a byte each. */
R2Skin *fn_8044E408(R2Skin *skin, u32 numVertices) {
    u32 i;
    u32 j;
    u32 k;
    u32 m;
    u8 *packed;
    u32 total;
    u32 n;

    skin->positions = NULL;
    skin->normals = NULL;
    skin->packedWeights = NULL;
    skin->packedIndices = NULL;
    for (i = 0; i < numVertices; i++) {
        for (j = 0; j < 3; j++) {
            for (k = 1; k < 4 - j; k++) {
                if (((f32 *)skin->weights + i * 4 + j)[k] > ((f32 *)skin->weights + i * 4 + j)[0]) {
                    f32 swap = ((f32 *)skin->weights + i * 4 + j)[0];
                    u32 a;
                    u32 b;

                    ((f32 *)skin->weights + i * 4 + j)[0] = ((f32 *)skin->weights + i * 4 + j)[k];
                    ((f32 *)skin->weights + i * 4 + j)[k] = swap;
                    a = (skin->indices[i] >> ((j + k) * 8)) & 0xFF;
                    b = (skin->indices[i] >> (j * 8)) & 0xFF;
                    skin->indices[i] &= ~(0xFF << (j * 8));
                    skin->indices[i] |= a << (j * 8);
                    skin->indices[i] &= ~(0xFF << ((j + k) * 8));
                    skin->indices[i] |= b << ((j + k) * 8);
                }
            }
        }
    }
    if (skin->maxWeights > 1) {
        skin->packedWeights =
            lbl_805F15B4->memAlloc(numVertices * (skin->maxWeights * 2) + 3, SKIN_HINT);
        packed = skin->packedWeights;
        for (i = 0; i < numVertices; i++) {
            total = 0;
            for (m = 0; m < skin->maxWeights; m++) {
                *packed = (u8)(128.0f * ((f32 *)skin->weights + i * 4)[m]);
                total += *packed++;
            }
            if ((u8)total < 0x80) {
                for (k = 0; k < skin->maxWeights; k++) {
                    total++;
                    *(packed - (skin->maxWeights - k)) += 1;
                    if ((u8)total == 0x80) {
                        break;
                    }
                }
            }
        }
        skin->packedIndices = skin->packedWeights + skin->maxWeights * numVertices;
        skin->packedIndices = (u8 *)(((u32)skin->packedIndices + 3) & ~3);
        packed = skin->packedIndices;
        for (i = 0; i < numVertices; i++) {
            for (n = 0; n < skin->maxWeights; n++) {
                *packed++ = skin->indices[i] >> (n * 8);
            }
        }
    }
    return skin;
}

/* Native skin data of a geometry being prepared. */
R2Geometry *fn_8044E6BC(R2Geometry *geometry) {
    R2Skin *skin = SKIN_OF(geometry);

    if (skin != NULL) {
        if (geometry->flags & 0x01000000) {
            if (geometry->flags & 0x10) {
                geometry->lockedSinceLastInst |= 6;
            } else {
                geometry->lockedSinceLastInst |= 2;
            }
            skin->positions = NULL;
            skin->normals = NULL;
        } else if (fn_8044E408(skin, geometry->numVertices) == NULL) {
            return NULL;
        }
    }
    return geometry;
}

/* Frees the packed weights of a geometry's skin. */
R2Geometry *fn_8044E758(R2Geometry *geometry) {
    R2Skin *skin = SKIN_OF(geometry);

    if (skin->packedWeights != NULL) {
        lbl_805F15B4->memFree(skin->packedWeights);
        skin->packedWeights = NULL;
        skin->packedIndices = NULL;
    }
    skin->positions = NULL;
    skin->normals = NULL;
    return geometry;
}

#pragma auto_inline on

/* Stream size of a geometry's native skin data. The vertex arrays the skin kept are handed back
 * to the geometry's instance first. */
s32 fn_8044E7D0(const R2Geometry *geometry) {
    R2Skin *skin = SKIN_OF(geometry);
    s32 size = skin->numUsedBones + 0x14;

    if (skin->maxWeights > 1) {
        size += geometry->numVertices * (skin->maxWeights * 2);
    }
    size += skin->numBones * 64;
    if (skin->positions != NULL) {
        R2SkinInstance *instance = &((R2SkinResEntry *)geometry->repEntry)->instance;

        RES_OF(instance->positions)->entry.ownerRef = NULL;
        instance->positions = skin->positions;
        if (geometry->flags & 0x10) {
            instance->normals = skin->normals;
        }
        geometry->repEntry->destroyNotify = fn_8046288C;
        skin->positions = NULL;
        skin->normals = NULL;
    }
    size += fn_8044BDBC(skin);
    return size;
}

#pragma auto_inline reset

/* Writes a geometry's native skin data. */
void *fn_8044E89C(void *stream, const R2Geometry *geometry) {
    s32 numVertices;
    u32 maxWeights;
    R2Skin *skin;
    s32 format = 6;
    u32 header;

    if (!fn_8047CEF0(stream, 1, fn_8044E7D0(geometry) - 12, 0x37002, 0x37)) {
        return NULL;
    }
    if (!fn_8047D458(stream, &format, sizeof(s32))) {
        return NULL;
    }
    numVertices = geometry->numVertices;
    skin = SKIN_OF(geometry);
    maxWeights = skin->maxWeights;
    header = ((maxWeights & 0xFF) << 16) | ((skin->numUsedBones & 0xFF) << 8) |
             (skin->numBones & 0xFF);
    if (!fn_8047D458(stream, &header, sizeof(u32))) {
        return NULL;
    }
    if (!fn_80480ED0(stream, skin->usedBones, skin->numUsedBones)) {
        return NULL;
    }
    if (skin->maxWeights > 1) {
        u32 size = maxWeights * numVertices;

        if (!fn_80480ED0(stream, skin->packedIndices, size)) {
            return NULL;
        }
        if (!fn_80480ED0(stream, skin->packedWeights, size)) {
            return NULL;
        }
    }
    if (!fn_80480ED0(stream, skin->matrices, skin->numBones << 6)) {
        return NULL;
    }
    if (!fn_8044BB18(stream, skin)) {
        return NULL;
    }
    return stream;
}

/* Reads a geometry's native skin data. */
void *fn_8044EAD0(void *stream, R2Geometry *geometry) {
    s32 numVertices;
    R2Skin *skin;
    R2Error error;
    u32 version;
    u32 length;
    u32 header;
    s32 format;

    if (!fn_8047CF50(stream, 1, &length, &version)) {
        return NULL;
    }
    if (version < 0x35000 || version > 0x37002) {
        error.pluginID = 0x116;
        error.errorCode = fn_8047DC28(0x80000004, version);
        fn_8047DB84(&error);
        return NULL;
    }
    if (!fn_8047D868(stream, &format, sizeof(s32))) {
        return NULL;
    }
    if (format != 6) {
        return NULL;
    }
    skin = lbl_805F15B4->freeListAlloc(lbl_8056D708.freeList, SKIN_HINT);
    memset(skin, 0, sizeof(R2Skin));
    if (!fn_8047D868(stream, &header, sizeof(u32))) {
        return NULL;
    }
    skin->numBones = header & 0xFF;
    skin->numUsedBones = (header >> 8) & 0xFF;
    skin->maxWeights = (header >> 16) & 0xFF;
    numVertices = geometry->numVertices;
    length -= 8;
    if (skin->maxWeights > 1) {
        skin->packedWeights = lbl_805F15B4->memAlloc(length + 5, SKIN_HINT);
        skin->packedIndices = skin->packedWeights + skin->maxWeights * numVertices;
        skin->packedIndices = (u8 *)(((u32)skin->packedIndices + 3) & ~3);
        skin->matrices = (R2Matrix *)(skin->packedIndices + skin->maxWeights * numVertices);
        skin->matrices = (R2Matrix *)(((u32)skin->matrices + 3) & ~3);
        skin->usedBones = (u8 *)(skin->matrices + skin->numBones);
        length = skin->numUsedBones;
        if (fn_80480D4C(stream, skin->usedBones, length) != length) {
            return NULL;
        }
        length = skin->maxWeights * numVertices;
        if (fn_80480D4C(stream, skin->packedIndices, length) != length) {
            return NULL;
        }
        length = skin->maxWeights * numVertices;
        if (fn_80480D4C(stream, skin->packedWeights, length) != length) {
            return NULL;
        }
        length = skin->numBones << 6;
        if (fn_80480D4C(stream, skin->matrices, length) != length) {
            return NULL;
        }
    } else {
        skin->packedWeights = lbl_805F15B4->memAlloc(length + 3, SKIN_HINT);
        skin->matrices = (R2Matrix *)skin->packedWeights;
        skin->matrices = (R2Matrix *)(((u32)skin->matrices + 3) & ~3);
        skin->usedBones = (u8 *)(skin->matrices + skin->numBones);
        length = skin->numUsedBones;
        if (fn_80480D4C(stream, skin->usedBones, length) != length) {
            return NULL;
        }
        length = skin->numBones << 6;
        if (fn_80480D4C(stream, skin->matrices, length) != length) {
            return NULL;
        }
    }
    if (!fn_8044BBFC(stream, skin)) {
        return NULL;
    }
    fn_8044CCC0(geometry, skin);
    return stream;
}

/* Stream size of a skinned atomic: nothing is written. The vertex arrays the atomic kept are
 * handed back to its instance first. */
s32 fn_8044EE3C(const R2Atomic *atomic) {
    R2SkinAtomic *data = SKIN_ATOMIC(atomic);

    if (data->positions != NULL) {
        R2SkinInstance *instance = &((R2SkinResEntry *)atomic->repEntry)->instance;

        RES_OF(instance->positions)->entry.ownerRef = NULL;
        instance->positions = data->positions;
        if (atomic->geometry->flags & 0x10) {
            instance->normals = data->normals;
        }
        atomic->repEntry->destroyNotify = fn_8046288C;
        data->positions = NULL;
        data->normals = NULL;
    }
    return 0;
}

struct RwCpp14Descriptor {
    u32 data;
    u32 type;
    s8 links[16];
};

static inline u32 RwCpp14ValueType(u32 value) {
    u32 type = 2;
    if (value >= 0xFF) {
        type = 3;
    }
    return type;
}

u32 fn_8044EEAC(u32 *object, u32 *outObject, u32 *outValues) {
    if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) fn_80462E1C(object);

    { struct RwCpp14Descriptor entries[16];
    u32 *context = (u32 *)object[0x17];
    u32 streams = 1;
    entries[0].data = context[5];
    entries[0].type = 8;
    entries[0].links[0] = -1;
    if ((object[2] & 0x10) != 0) {
        entries[1].data = context[6];
        entries[1].type = 8;
        entries[1].links[0] = -1;
        streams = 2;
    }
    if ((object[2] & 0x8) != 0) {
        entries[streams].data = object[0x0C];
        entries[streams].type = 4;
        entries[streams].links[0] = -1;
        streams++;
    }
    if ((object[2] & 0x84) != 0) {
        { u32 i; for (i = 0; (s32)i < (s32)object[7]; i++) {
            entries[streams + i].data = object[0x0D + i];
            entries[streams + i].type = 7;
            entries[streams + i].links[0] = -1;
        } }
        streams += object[7];
    }

    { u32 *record = (u32 *)fn_8044CCAC((const R2Geometry *)object);
    u32 base = streams;
    if (record[4] > 1) {
        entries[base].data = record[10];
        switch (record[4]) {
        case 1: entries[base].type = 1; break;
        case 2: entries[base].type = 2; break;
        case 3: entries[base].type = 3; break;
        case 4: entries[base].type = 4; break;
        default: entries[base].type = 0; break;
        }
        entries[base + 1].data = record[9];
        entries[base + 1].type = entries[base].type;
        entries[base].links[0] = -1;
        entries[base + 1].links[0] = -1;
    } else {
        entries[base].data = record[5];
        entries[base].type = 4;
        entries[base + 1].data = record[6];
        entries[base + 1].type = 9;
        entries[base].links[0] = -1;
        entries[base + 1].links[0] = -1;
    }
    streams += 2;

    if ((object[2] & 0x10) != 0) {
        entries[streams - 2].links[0] = 0;
        entries[streams - 2].links[1] = 1;
        entries[streams - 2].links[2] = (s8)(streams - 1);
        entries[streams - 2].links[3] = -1;
        entries[streams - 1].links[0] = 0;
        entries[streams - 1].links[1] = 1;
        entries[streams - 1].links[2] = (s8)(streams - 2);
        entries[streams - 1].links[3] = -1;
    } else {
        entries[streams - 2].links[0] = 0;
        entries[streams - 2].links[1] = (s8)(streams - 1);
        entries[streams - 2].links[2] = -1;
        entries[streams - 1].links[0] = 0;
        entries[streams - 1].links[1] = (s8)(streams - 2);
        entries[streams - 1].links[2] = -1;
    }

    { u32 *indices = fn_80466194((u32 *)entries, streams, object[5]);
    if (indices == NULL) return 0;
    { u32 *packed = fn_80465ED4(indices, (u8 *)entries, streams, object[5]);
    *outObject = (u32)packed;
    if (packed == NULL) {
        lbl_805F15B4->memFree(indices);
        return 0;
    }

    { u32 vertices = *(u16 *)((u8 *)object[0x15] + 4);
    u32 customStreams = streams - 2;
    u32 rowBytes = vertices * 4;
    u32 tableBytes = customStreams * rowBytes;
    u32 *table = (u32 *)lbl_805F15B4->memAlloc(tableBytes, 0x30116);
    if (table == NULL) {
        lbl_805F15B4->memFree(packed);
        lbl_805F15B4->memFree(indices);
        return 0;
    }
    memset(table, 0, tableBytes);
    { u32 vertex; for (vertex = 0; vertex < vertices; vertex++) {
        u32 *source = (u32 *)((u8 *)object[0x15] + vertex * 0xC);
        { u32 stream; for (stream = 0; stream < customStreams; stream++) {
            table[vertex * customStreams + stream] = source[4];
        } }
    } }

    { u32 *result = (u32 *)lbl_805F15B4->memAlloc(rowBytes, 0x30116);
    *outValues = (u32)result;
    if (result == NULL) {
        lbl_805F15B4->memFree(table);
        lbl_805F15B4->memFree(packed);
        lbl_805F15B4->memFree(indices);
        return 0;
    }

    { u32 vertex; for (vertex = 0; vertex < vertices; vertex++) {
        u32 *source = (u32 *)((u8 *)object[0x15] + vertex * 0xC);
        u32 value = fn_80465CDC(indices, table + vertex * customStreams, customStreams, source[5]);
        result[vertex] = value;
        if (value == 0) {
            { s32 i; for (i = (s32)vertex; i >= 0; i--) {
                lbl_805F15B4->memFree((void *)result[i]);
            } }
            lbl_805F15B4->memFree(table);
            lbl_805F15B4->memFree(packed);
            lbl_805F15B4->memFree(indices);
            return 0;
        }
    } }
    lbl_805F15B4->memFree(table);
    lbl_805F15B4->memFree(indices);
    return 1;
 } } } } } }}



u32 fn_8044F4D0(u32 *object, u32 *table) {
    u32 *record = (u32 *)fn_8044CCAC((const R2Geometry *)object);
    u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
    if (format == NULL) format = (u8 *)fn_80462E1C(record);

    fn_8046FC1C(skinVertexFormat);
    fn_8046FC48(skinVertexFormat, 0);
    if (record[4] == 1) fn_8046FF34(skinVertexFormat, 0, 1);
    fn_8046FC50(skinVertexFormat, 9, 1, format[0], format[0xC]);

    { u32 value = table[1];
    fn_8046FF34(skinVertexFormat, 9, RwCpp14ValueType(value));
    { u32 index = 1;
    if ((object[2] & 0x10) != 0) {
        if (format[0xD] != 0) {
            fn_8046FC50(skinVertexFormat, 0x19, 1, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(skinVertexFormat, 0x19, RwCpp14ValueType(value));
        } else {
            fn_8046FC50(skinVertexFormat, 0xA, 0, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(skinVertexFormat, 0xA, RwCpp14ValueType(value));
        }
        index = 2;
    }

    if ((object[2] & 0x8) != 0) {
        u32 byte = format[0xA];
        fn_8046FC50(skinVertexFormat, 0xB, (2 - byte) >> 31, byte, 0);
        value = table[index * 2 + 1];
        fn_8046FF34(skinVertexFormat, 0xB, RwCpp14ValueType(value));
        index++;
    }

    if ((object[2] & 0x84) != 0) {
        u32 *entry = table + index * 2;
        { u32 i; for (i = 0; (s32)i < (s32)object[7]; i++) {
            fn_8046FC50(skinVertexFormat, i + 0xD, 1, format[i + 2], format[i + 0xE]);
            value = entry[1];
            fn_8046FF34(skinVertexFormat, i + 0xD, RwCpp14ValueType(value));
            entry += 2;
            index++;
        } }
    }
    fn_8047046C(skinVertexFormat, index & 0xFF);
    return (u32)skinVertexFormat;
 } }}

u32 fn_8044F740(u32 *object, u16 *tableA, u32 *tableB, s32 count, u32 index) {
    u16 *output;
    if (object[13] != 0) {
        u8 local[0x28];
        u32 written = 0;
        u8 *table = (u8 *)object[16];
        u32 pair = index * 2;
        u32 first = table[pair];
        u32 outerCount = table[pair + 1];
        u32 tableOffset = (first & 0x7F) * 2;
        { u32 outer; for (outer = 0; outer < outerCount; outer++) {
            u8 *entry = (u8 *)object[17] + outer * 2 + tableOffset;
            u32 base = entry[0];
            u32 innerCount = entry[1];
            u8 *cursor = local + written;
            { u32 i; for (i = 0; i < innerCount; i++) {
                *cursor++ = (u8)(base + i);
            } }
            written += innerCount;
        } }

        output = (u16 *)lbl_805F15B4->memAlloc(count * 2, 0x30116);
        { s32 i; for (i = 0; i < count; i++) {
            u32 expected = tableB[tableA[i]] & 0xFF;
            u32 candidate = 0;
            int found = 0;
            { u32 round; for (round = 0; round < 2 && !found; round++) {
                { u32 j; for (j = 0; j < 5; j++) {
                    if (local[round * 5 + j] == expected) {
                        output[i] = (u16)(candidate * 3);
                        found = 1;
                        break;
                    }
                    candidate++;
                } }
            } }
        } }
    } else {
        output = (u16 *)lbl_805F15B4->memAlloc(count * 2, 0x30116);
        { s32 i; for (i = 0; i < count; i++) {
            u32 expected = tableB[tableA[i]] & 0xFF;
            u8 *candidates = (u8 *)object[2];
            { u32 j; for (j = 0; j < object[1]; j++) {
                if (candidates[j] == expected) {
                    output[i] = (u16)(j * 3);
                    break;
                }
            } }
        } }
    }
    return (u32)output;
}

R2ResEntry *fn_8044F9E4(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef) {
    u32 *object = (u32 *)geometry;
    u32 arg2 = (u32)owner;
    u32 *out = (u32 *)ownerRef;
{
    u32 packedStorage;
    u32 valuesStorage;
    fn_8044EEAC(object, &packedStorage, &valuesStorage);
    { u32 *packed = (u32 *)packedStorage;
    u32 *values = (u32 *)valuesStorage;
    u32 context = fn_8044F4D0(object, packed);

    u32 second[52];
    u32 streams = 1;
    u32 optional = 1;
    second[9] = packed[1];
    second[35] = packed[0];
    if ((object[2] & 0x10) != 0) {
        second[9 + optional] = packed[optional * 2 + 1];
        second[35 + optional] = packed[optional * 2];
        optional++;
        streams++;
    }
    if ((object[2] & 0x8) != 0) {
        second[9 + optional] = packed[optional * 2 + 1];
        second[35 + optional] = packed[optional * 2];
        optional++;
        streams++;
    }
    if ((object[2] & 0x84) != 0) {
        u32 i = 0;
        u32 *packedPair = packed + 8;
        u32 *secondA = second + 13;
        u32 *secondB = second + 39;
        for (; i + 8 <= object[7]; i += 8) {
            { u32 j; for (j = 0; j < 8; j++) {
                secondA[j] = packedPair[j * 2 + 1];
                secondB[j] = packedPair[j * 2];
            } }
            secondA += 8;
            secondB += 8;
            packedPair += 16;
            streams += 8;
        }
        for (; i < object[7]; i++) {
            *secondA++ = packedPair[1];
            *secondB++ = packedPair[0];
            packedPair += 2;
            streams++;
        }
    }

    { u32 base = fn_8046EE8C(context);
    u32 vertices = *(u16 *)((u8 *)object[0x15] + 4);
    u32 stride = (object[2] & 1) != 0 ? 0x98 : 0x90;
    u32 size = base + vertices * 8 + 0x1F;
    { u32 i; for (i = 0; i < vertices; i++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + i * 0xC);
        if ((object[2] & 1) != 0) {
            u32 tempA;
            u32 tempB;
            fn_8046D8C8(*(u32 *)values[i], descriptor[5], &tempA, &tempB, 1);
            size += fn_8046B094(context, tempA, tempB);
        } else {
            size += fn_8046B094(context, 1, descriptor[5]);
        }
    } }
    size += fn_8046EEA0(context, second);

    { u32 *block = (u32 *)lbl_805F15B4->memAlloc(size + 0x18, 0x30116);
    block[0] = 0;
    block[1] = 0;
    block[2] = size;
    block[3] = arg2;
    block[4] = (u32)out;
    block[5] = (u32)fn_8046288C;
    *out = (u32)block;
    { u8 *payload = (u8 *)(block + 6);
    memset(payload, 0, size);
    *(u16 *)(payload + 0) = lbl_805F1678;
    *(u16 *)(payload + 2) = *(u16 *)((u8 *)object[0x15] + 6);
    *(u32 *)(payload + 4) = 0;

    if ((object[2] & 0x8) != 0) {
        u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
        if (format == NULL) {
            *(u32 *)(payload + 4) &= ~1;
            { u8 *bytes = (u8 *)object[0x0C];
            { s32 i; for (i = 0; i < (s32)object[5]; i++) {
                if (bytes[i * 4 + 3] < 0xFF) {
                    *(u32 *)(payload + 4) |= 1;
                    break;
                }
            } }
         }} else if (format[0xA] > 2) {
            *(u32 *)(payload + 4) |= 1;
        } else {
            *(u32 *)(payload + 4) &= ~1;
        }
    }

    { u8 *data = payload + base;
    u8 *end = (u8 *)(((u32)data + vertices * 8 + 0x1F) & ~0x1F);
    u32 *record = (u32 *)fn_8044CCAC((const R2Geometry *)object);
    u32 descriptorOffset = 0;
    u32 valuesIndex = 0;
    u8 *currentData = data;
    { u32 vertex; for (vertex = 0; vertex < vertices; vertex++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + descriptorOffset);
        u32 tempA;
        u32 tempB;
        if ((object[2] & 1) != 0) {
            fn_8046D8C8(*(u32 *)values[valuesIndex / 4], descriptor[5], &tempA, &tempB, 1);
        } else {
            tempA = descriptor[5];
            tempB = 1;
        }
        { u32 part = fn_8046B094(context, tempA, tempB);
        fn_8046B0E0(currentData, vertex, part, end);

        { u32 *vertexValues = (u32 *)values[valuesIndex / 4];
        u32 *lookup = NULL;
        if (record[4] <= 1) {
            u32 streamIndex = 1;
            if ((object[2] & 0x10) != 0) streamIndex++;
            if ((object[2] & 0x8) != 0) streamIndex++;
            if ((object[2] & 0x84) != 0) streamIndex += object[7];
            lookup = (u32 *)fn_8044F740(record, (u16 *)vertexValues[0],
                                        (u32 *)packed[streamIndex], descriptor[5], vertex);
        }
        if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
            fn_80462E1C(lookup);
        }
        if (lookup != NULL) {
            second[0] = (u32)lookup;
        }
        second[9] = vertexValues[0];
        if ((object[2] & 0x10) != 0) {
            second[10] = vertexValues[1];
        }
        if ((object[2] & 0x8) != 0) {
            second[11] = vertexValues[2];
        }
        if ((object[2] & 0x84) != 0) {
            u32 i = 0;
            u32 *valuesPair = vertexValues + 4;
            u32 *secondValues = second + 13;
            for (; i + 8 <= (u8)object[7]; i += 8) {
                { u32 j; for (j = 0; j < 8; j++) {
                    secondValues[j] = valuesPair[j];
                } }
                secondValues += 8;
                valuesPair += 8;
            }
            for (; i < (u8)object[7]; i++) {
                *secondValues++ = *valuesPair++;
            }
        }
        { void *acbc = fn_8046ACBC(context);
        fn_8046E594(context, currentData, second, descriptor[5], object[2] & 1, acbc, 1, stride, 0);
        if (lookup != NULL) {
            lbl_805F15B4->memFree(lookup);
        }
        end += part;
        currentData += 8;
        descriptorOffset += 0xC;
        valuesIndex += 4;
     } } }} }

    fn_8046F208(context, payload, second, (u32)end);
    fn_8046F6CC(context, payload, second, 0, 0);
    if (record[4] > 1) {
        u32 streamIndex = 1;
        if ((object[2] & 0x10) != 0) streamIndex++;
        if ((object[2] & 0x8) != 0) streamIndex++;
        if ((object[2] & 0x84) != 0) streamIndex += object[7];
        memcpy((void *)record[10], (void *)packed[streamIndex], record[4] * packed[streamIndex + 1]);
        memcpy((void *)record[9], (void *)packed[streamIndex + 2], record[4] * packed[streamIndex + 3]);
    }
    object[5] = packed[1];
    DCFlushRange(payload, size);
    fn_80394F2C();
    { u32 i; for (i = 0; i < vertices; i++) {
        lbl_805F15B4->memFree((void *)values[i]);
    } }
    lbl_805F15B4->memFree(values);
    lbl_805F15B4->memFree(packed);
    return (R2ResEntry *)block;
 } } } } }}
}

u32 fn_80450448(u32 *object) {
    u32 value = object[5];
    u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
    if (format == NULL) format = (u8 *)fn_80462E1C(object);

    fn_8046FC1C(skinVertexFormat);
    fn_8046FC48(skinVertexFormat, 0);
    fn_8046FF34(skinVertexFormat, 0, 1);
    fn_8046FC50(skinVertexFormat, 9, 1, format[0], format[0xC]);
    fn_8046FF34(skinVertexFormat, 9, RwCpp14ValueType(value));

    { u32 index = 1;
    if ((object[2] & 0x10) != 0) {
        if (format[0xD] != 0) {
            fn_8046FC50(skinVertexFormat, 0x19, 1, format[1], 0);
            fn_8046FF34(skinVertexFormat, 0x19, RwCpp14ValueType(value));
        } else {
            fn_8046FC50(skinVertexFormat, 0xA, 0, format[1], 0);
            fn_8046FF34(skinVertexFormat, 0xA, RwCpp14ValueType(value));
        }
        index = 2;
    }
    if ((object[2] & 0x8) != 0) {
        u32 byte = format[0xA];
        fn_8046FC50(skinVertexFormat, 0xB, (2 - byte) >> 31, byte, 0);
        fn_8046FF34(skinVertexFormat, 0xB, RwCpp14ValueType(value));
        index++;
    }
    if ((object[2] & 0x84) != 0) {
        { u32 i; for (i = 0; (s32)i < (s32)object[7]; i++) {
            fn_8046FC50(skinVertexFormat, i + 0xD, 1, format[i + 2], format[i + 0xE]);
            fn_8046FF34(skinVertexFormat, i + 0xD, RwCpp14ValueType(value));
            index++;
        } }
    }
    fn_8047046C(skinVertexFormat, index & 0xFF);
    return (u32)skinVertexFormat;
 }}

u32 fn_8045066C(u32 *object, u32 *descriptor, u32 index) {
    u32 count = descriptor[1];
    u16 *output;
    u16 *entries = (u16 *)descriptor[0];
    u32 *table = (u32 *)object[5];
    if (object[13] != 0) {
        u8 local[0x28];
        u32 written = 0;
        u8 *source = (u8 *)object[16];
        u32 pair = index * 2;
        u32 first = source[pair];
        u32 outerCount = source[pair + 1];
        u32 tableOffset = (first & 0x7F) * 2;
        { u32 outer; for (outer = 0; outer < outerCount; outer++) {
            u8 *entry = (u8 *)object[17] + outer * 2 + tableOffset;
            u32 base = entry[0];
            u32 innerCount = entry[1];
            u8 *cursor = local + written;
            { u32 i; for (i = 0; i < innerCount; i++) {
                *cursor++ = (u8)(base + i);
            } }
            written += innerCount;
        } }

        output = (u16 *)lbl_805F15B4->memAlloc(count * 2, 0x30116);
        { u32 i; for (i = 0; i < count; i++) {
            u32 expected = table[entries[i]] & 0xFF;
            u32 candidate = 0;
            int found = 0;
            { u32 round; for (round = 0; round < 2 && !found; round++) {
                { u32 j; for (j = 0; j < 5; j++) {
                    if (local[round * 5 + j] == expected) {
                        output[i] = (u16)(candidate * 3);
                        found = 1;
                        break;
                    }
                    candidate++;
                } }
            } }
        } }
    } else {
        u8 *candidates = (u8 *)object[2];
        output = (u16 *)lbl_805F15B4->memAlloc(count * 2, 0x30116);
        { u32 i; for (i = 0; i < count; i++) {
            u32 expected = table[entries[i]] & 0xFF;
            { u32 j; for (j = 0; j < object[1]; j++) {
                if (candidates[j] == expected) {
                    output[i] = (u16)(j * 3);
                    break;
                }
            } }
        } }
    }
    return (u32)output;
}

R2ResEntry *fn_80450938(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef) {
    u32 *object = (u32 *)geometry;
    u32 arg2 = (u32)owner;
    u32 *out = (u32 *)ownerRef;
{
    u32 record = (u32)fn_8044CCAC((R2Geometry *)arg2);
    u32 context = fn_80450448(object);
    if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
        fn_80462E1C((void *)context);
    }

    { u32 second[52];
    second[9] = object[5];
    second[35] = ((u32 *)object[0x17])[5];
    if ((object[2] & 0x10) != 0) {
        second[10] = object[5];
        second[36] = ((u32 *)object[0x17])[6];
    }
    if ((object[2] & 0x8) != 0) {
        second[11] = object[5];
        second[37] = object[0x0C];
    }
    if ((object[2] & 0x84) != 0) {
        { s32 i; for (i = 0; i < (s32)object[7]; i++) {
            second[13 + i] = object[5];
            second[39 + i] = object[0x0D + i];
        } }
    }

    { u32 base = fn_8046EE8C(context);
    u32 vertices = *(u16 *)((u8 *)object[0x15] + 4);
    u32 stride = (object[2] & 1) != 0 ? 0x98 : 0x90;
    u32 size = base + vertices * 8 + 0x1F;
    u32 sizeOffset = 0;
    { u32 i; for (i = 0; i < vertices; i++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + sizeOffset);
        size += fn_8046B094(context, 1, descriptor[5]);
        sizeOffset += 0xC;
    } }
    size += fn_8046EEA0(context, second);

    { u32 *block;
    if ((object[2] & 0x02000000) != 0) {
        block = (u32 *)((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(size + 0x18, 0x30116);
        block[0] = 0;
        block[1] = 0;
        block[2] = size;
        block[3] = arg2;
        block[4] = (u32)out;
        block[5] = (u32)fn_8046288C;
        *out = (u32)block;
    } else {
        block = (u32 *)fn_80480878((void *)arg2, out, size, (void *)fn_8046288C);
    }

    { u8 *payload = (u8 *)(block + 6);
    memset(payload, 0, size);
    *(u16 *)(payload + 0) = lbl_805F1678;
    *(u16 *)(payload + 2) = *(u16 *)((u8 *)object[0x15] + 6);
    *(u32 *)(payload + 4) = 0;

    if ((object[2] & 0x8) != 0) {
        u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
        if (format == NULL) {
            fn_80462E1C(format);
            *(u32 *)(payload + 4) &= ~1;
            { u8 *bytes = (u8 *)object[0x0C];
            { s32 i; for (i = 0; i < (s32)object[5]; i++) {
                if (bytes[i * 4 + 3] < 0xFF) {
                    *(u32 *)(payload + 4) |= 1;
                    break;
                }
            } }
         }} else if (format[0xA] > 2) {
            *(u32 *)(payload + 4) |= 1;
        } else {
            *(u32 *)(payload + 4) &= ~1;
        }
    }

    { u8 *data = payload + base;
    u8 *end = (u8 *)(((u32)data + vertices * 8 + 0x1F) & ~0x1F);
    u32 descriptorOffset = 0;
    u8 *currentData = data;
    { u32 vertex; for (vertex = 0; vertex < vertices; vertex++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + descriptorOffset);
        u32 part = fn_8046B094(context, 1, descriptor[5]);
        fn_8046B0E0(currentData, vertex, part, end);
        { u32 lookup = fn_8045066C((u32 *)record, descriptor + 4, vertex);
        if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
            fn_80462E1C((void *)lookup);
        }

        { u32 values[26];
        values[0] = lookup;
        values[9] = descriptor[4];
        if ((object[2] & 0x10) != 0) {
            values[10] = descriptor[4];
        }
        if ((object[2] & 0x8) != 0) {
            values[11] = descriptor[4];
        }
        if ((object[2] & 0x84) != 0) {
            { s32 i; for (i = 0; i < (s32)object[7]; i++) {
                values[13 + i] = descriptor[4];
            } }
        }

        { void *acbc = fn_8046ACBC(context);
        fn_8046E594(context, currentData, values, descriptor[5], 0, acbc, 1, stride, 0);
        ((void (*)(void *))*(u32 *)((u8 *)lbl_805F15B4 + 0x10C))((void *)lookup);
        end += part;
        currentData += 8;
        descriptorOffset += 0xC;
     } } }} }

    fn_8046F208(context, payload, second, (u32)end);
    fn_8046F6CC(context, payload, second, 0, 0);
    DCFlushRange(payload, size);
    fn_80394F2C();
    return (R2ResEntry *)block;
 } } } } }}
}

void *fn_80451A6C(u32 *context, u32 *node) {
    if (fn_804A8478(node) != 0) {
        u32 *owner = (u32 *)context[6];
        if (*(u32 *)((u8 *)owner + lbl_805F1520) == 0) {
            fn_80463104(owner, lbl_805F16B8);
        }
        { s32 state = fn_80462C28();
        if (state == 1) fn_80462C20(0);
        context = (u32 *)fn_8044D924(context, (R2ResEntry **)node);
        if (state == 1) fn_80462C20(1);
        return context;
     }}
    return fn_8044D924(context, (R2ResEntry **)node);
}

void *fn_80451B20(u32 *context, u32 *node) {
    if (fn_8044DA3C(context, (R2ResEntry **)node) == NULL) return NULL;
    { u32 *owner = (u32 *)context[6];
    u8 *format = *(u8 **)((u8 *)owner + lbl_805F1520);
    u32 *record = (u32 *)fn_8044CCAC((R2Geometry *)owner);
    if (record[4] > 1 && format != NULL && format[0xD] != 0 && (owner[2] & 0x10) != 0) {
        fn_804A8120(node, format, owner[5]);
    }
    return context;
 }}

/* Retail calls fn_8044DC4C and fn_8044DD1C out of line here, which the inlining above would not do
 * in one translation unit: the material-effects pipeline (from fn_80451A6C) may be a file of its own,
 * though no data shows it. */
#pragma dont_inline on
/* Render callback of the skinned atomic pipeline with material effects: as fn_8044DE74, with each
 * mesh drawn by the material effects' renderer. */
R2Atomic *fn_80451BCC(R2Atomic *atomic, R2GxNodeData *node) {
    R2DisplayList *list;
    R2MeshHeader *meshHeader;
    R2Skin *skin;
    R2Matrix *ltm;
    R2SkinInstance *instance = &node->resEntry->instance;

    node->resEntry->instance.token = lbl_805EDF40;
    list = (R2DisplayList *)&instance->normals + (instance->numArrays - 1);
    meshHeader = atomic->geometry->mesh;
    ltm = RwFrameGetLTM(atomic->object.parent);
    fn_80462C30(R2_PLUGIN(R2VertexDesc, atomic->geometry, lbl_805F1520), node);
    skin = fn_8044CCAC(atomic->geometry);
    if (skin->maxWeights > 1) {
        fn_804950AC(ltm, (node->flags >> 4) & 1);
    } else {
        fn_80394500(0, 1);
    }
    if (skin->maxWeights == 1 && skin->split.numMeshes == 0) {
        u32 i;
        s32 id = 0;

        for (i = 0; i < skin->numUsedBones; i++) {
            fn_8044DC4C((R2Matrix *)SKIN_GLOBALS->matrixCache + skin->usedBones[i], id,
                        (node->flags >> 4) & 1);
            id += 3;
        }
    }
    fn_804A2F08();
    {
        R2Mesh *mesh = (R2Mesh *)(meshHeader + 1);
        s32 remaining = meshHeader->numMeshes;

        while (remaining-- != 0) {
            if (skin->maxWeights == 1 && skin->split.numMeshes != 0) {
                fn_8044DD1C(skin, node->meshHeader->numMeshes - (remaining + 1),
                            (node->flags >> 4) & 1);
            }
            fn_804A457C(mesh, list, atomic, ltm, node);
            list++;
            mesh++;
        }
    }
    return atomic;
}
#pragma dont_inline reset


u32 fn_80451D60(u32 mode) {
    u32 *callbacks = (u32 *)&lbl_8056D708;
    if ((mode & 1) != 0) {
        callbacks[10] = fn_8046335C(0x116, 1, (void *)fn_8044D924, (void *)fn_8044DA3C,
                                    (void *)fn_80463428, (void *)fn_8044DE74);
    }
    if ((mode & 2) != 0) {
        callbacks[11] = fn_8046335C(0x116, 2, (void *)fn_80451A6C, (void *)fn_80451B20,
                                    (void *)fn_80463428, (void *)fn_80451BCC);
    }
    return 1;
}

u32 fn_80451E0C(void) {
    u32 *callbacks = (u32 *)&lbl_8056D708;
    if (callbacks[10] != 0) {
        fn_8049B800((void *)callbacks[10]);
        callbacks[10] = 0;
    }
    if (callbacks[11] != 0) {
        fn_8049B800((void *)callbacks[11]);
        callbacks[11] = 0;
    }
    return 1;
}

u32 *fn_80451E6C(u32 *object, s32 mode) {
    u32 *record = (u32 *)fn_8044CCAC((R2Geometry *)object[6]);
    s32 value;

    switch (mode) {
    case 1:
        record[11] = 1;
        value = 1;
        break;
    case 2:
        record[11] = 2;
        value = 2;
        break;
    default:
        if (fn_80458C90(0x120) > 0 && fn_804A2064(object) != 0) {
            record[11] = 2;
            value = 2;
        } else {
            record[11] = 1;
            value = 1;
        }
        break;
    }
    object[0x1B] = (u32)lbl_8056D708.pipelines[value];
    return object;
}
