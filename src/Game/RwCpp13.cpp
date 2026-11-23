/* RenderWare: RpSkin (plugin 0x116), GameCube back end, continued from RwCpp12.cpp.
 *
 * 0x8044D6E8-0x8044EEAC: the skinning of positions and normals, the skinned atomic pipeline's
 * instance, reinstance and render callbacks, the bone matrix upload, and the native skin data:
 * weight sorting and packing, its stream size, write and read, and the hand-back of the kept
 * vertex arrays. Compiled as C.
 *
 * Not written: fn_8044D6E8 (positions and normals) sets GQR5-GQR7 in the middle of compiled code,
 * which needs inline assembly (see fn_8044D554 in RwCpp12.cpp).
 * Near misses: fn_8044DA3C (retail forms lbl_8056D708's address in r3 after the object's copy;
 * GC/2.7 schedules it first, into r5), fn_8044DD1C and fn_8044DE74 (retail keeps incrementing the
 * run's bone counter beside its strength-reduced matrix offset; GC/2.7 drops the dead counter, two
 * instructions per loop; DE74 also has other saved registers).
 * Levers found here (probes/Game_RwCpp13*.py): the stream callbacks read the object through a
 * pointer to const; the pipeline callbacks take `void *object` and make the typed atomic a local
 * (it then gets its register among the locals); a signed id parameter; the loops' own counters.
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

extern R2SkinGlobals lbl_8056D708;
extern s32 lbl_805F1520; // offset of the R2VertexDesc pointer in RpGeometry
extern s32 lbl_805EDDD8; // 1 selects fn_8044F9E4 for geometries flagged 0x02000000
extern s32 lbl_805F1658; // offset of the GameCube raster extension
extern volatile u16 lbl_805EDF40; // the draw-sync token of the frame being built
extern const f32 lbl_805FB0D0; // 128.0f

R2Matrix *RwFrameGetLTM(void *frame);
void fn_803991E8(f32 mtx[3][4], u32 id);
void fn_80399238(f32 mtx[3][4], u32 id);
void fn_80399000(void *list, u32 size);
R2Skin *fn_8044CCAC(const R2Geometry *geometry);
void *fn_8044CCC0(R2Geometry *geometry, R2Skin *skin);
s32 fn_8044CF68(R2Skin *skin, R2Atomic *atomic, R2SkinInstance *instance, R2ResEntry **repEntry);
void fn_8044D344(R2Matrix *out, R2Skin *skin, R2Matrix *ltm, R2HAnimHierarchy *hierarchy);
void fn_8044D554(R2Skin *skin, R2Matrix *matrices, void *srcPositions, void *dstPositions,
                 R2VertexDesc *desc, s32 numVertices);
void fn_8044D6E8(R2Skin *skin, R2Matrix *matrices, void *srcPositions, void *srcNormals,
                 void *dstPositions, void *dstNormals, R2VertexDesc *desc, s32 numVertices);
R2ResEntry *fn_8044F9E4(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef);
R2ResEntry *fn_8046C0D4(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef);
R2ResEntry *fn_80450938(R2Geometry *geometry, void *owner, R2ResEntry **ownerRef);
void fn_80463840(R2Atomic *atomic, R2ResEntry **repEntry);
void fn_8046288C(R2ResEntry *entry);
void fn_80462C30(R2VertexDesc *desc, void *node);
void fn_80462E1C(R2Geometry *geometry);
void fn_804950AC(R2Matrix *ltm, s32 normals);
void fn_80394500(u32 a, u32 b);
void *fn_804655D4(u32 flags, u32 a, u32 b, u32 c);
void fn_80499C38(void *texture, u32 stage);
void fn_80497778(s32 enable);
u32 fn_8044BDBC(R2Skin *skin);
void *fn_8044BB18(void *stream, R2Skin *skin);
void *fn_8044BBFC(void *stream, R2Skin *skin);
s32 fn_8047CF50(void *stream, u32 type, u32 *length, u32 *version);
u32 fn_8047CEF0(void *stream, u32 type, u32 length, u32 version, u32 buildNum);
u32 fn_8047D868(void *stream, void *buffer, u32 size);
u32 fn_8047D458(void *stream, const void *buffer, u32 size);
u32 fn_80480D4C(void *stream, void *buffer, u32 size);
u32 fn_80480ED0(void *stream, const void *buffer, u32 size);
s32 fn_8047DC28(s32 code, ...);
void fn_8047DB84(void *error);

#define SKIN_OF(geometry) R2_PLUGIN(R2Skin, geometry, lbl_8056D708.geometryOffset)
#define SKIN_ATOMIC(object) ((R2SkinAtomic *)((u8 *)(object) + lbl_8056D708.atomicOffset))
#define VERTEX_DESC(geometry) R2_PLUGIN(R2VertexDesc, geometry, lbl_805F1520)
#define RASTER_EXT(raster) ((R2RasterExt *)((u8 *)(raster) + lbl_805F1658))
#define RES_OF(array) (((R2SkinResEntry **)(array))[-1])
#define SKIN_HINT 0x30116
#define WEIGHT(skin, vertex, n) (((f32 *)&(skin)->weights[vertex])[n])

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
                *packed = (u8)(lbl_805FB0D0 * ((f32 *)skin->weights + i * 4)[m]);
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
