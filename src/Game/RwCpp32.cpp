/* RenderWare GameCube driver, geometry instancing into display lists, 0x8046B554..0x8046CB94.
 * Names are inferred from behaviour. Compiled by CodeWarrior GC 1.3.2 with -O4,p.
 * Matching: fn_8046B554, fn_8046BEC4, fn_8046C94C. fn_8046B79C, fn_8046C0D4 and fn_8046C4D0
 * (the world-sector twin of fn_8046B110) have retail's instructions but other saved-register
 * assignments; fn_8046B79C and fn_8046C0D4 also miss retail's copy of the instance size kept for
 * DCFlushRange. */
#include <dolphin/types.h>
#include <string.h>
#include "Game/rw/r4.h"

extern "C" {
extern RwGameCubeVtxDesc lbl_805E18A0;
extern RwGameCubeVtxDesc lbl_805E18C0;

/* RwCpp34.cpp and RwCpp35.cpp: the vertex descriptor's setters. */
void fn_8046FC1C(RwGameCubeVtxDesc *desc);
void fn_8046FC48(RwGameCubeVtxDesc *desc, u32 value);
void fn_8046FC50(RwGameCubeVtxDesc *desc, s32 attr, s32 count, u32 type, u32 frac);
void fn_8046FF34(RwGameCubeVtxDesc *desc, s32 attr, GXAttrType type);
void fn_8047046C(RwGameCubeVtxDesc *desc, u8 numArrays);

/* An array of 255 entries or more needs 16-bit indices (0xFF is not used as a byte index).
 * The enumerators keep the choice a branch, as in retail. */
#define INDEX_TYPE(num) (((num) >= 0xFF) ? GX_INDEX16 : GX_INDEX8)

/* Fills the driver's scratch vertex descriptor for a geometry whose merged attribute arrays
 * are `arrays`: every attribute is indexed, by bytes while its array is short enough. */
RwGameCubeVtxDesc *fn_8046B554(RpGeometry *geom, RwGameCubeVtxArray *arrays) {
    RwGameCubeGeomFormat *format = RWGAMECUBEGEOMFORMAT(geom);
    s32 numArrays;
    s32 i;

    if (format == NULL) {
        format = fn_80462E1C();
    }
    fn_8046FC1C(&lbl_805E18A0);
    fn_8046FC48(&lbl_805E18A0, 0);
    numArrays = 0;

    fn_8046FC50(&lbl_805E18A0, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&lbl_805E18A0, GX_VA_POS, INDEX_TYPE(arrays[numArrays].num));
    numArrays++;
    if (geom->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&lbl_805E18A0, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18A0, GX_VA_NBT, INDEX_TYPE(arrays[numArrays].num));
        } else {
            fn_8046FC50(&lbl_805E18A0, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18A0, GX_VA_NRM, INDEX_TYPE(arrays[numArrays].num));
        }
        numArrays++;
    }
    if (geom->flags & 8) {
        fn_8046FC50(&lbl_805E18A0, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&lbl_805E18A0, GX_VA_CLR0, INDEX_TYPE(arrays[numArrays].num));
        numArrays++;
    }
    if (geom->flags & 0x84) {
        for (i = 0; i < geom->numTexCoordSets; i++) {
            fn_8046FC50(&lbl_805E18A0, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&lbl_805E18A0, GX_VA_TEX0 + i, INDEX_TYPE(arrays[numArrays].num));
            numArrays++;
        }
    }
    fn_8047046C(&lbl_805E18A0, numArrays);
    return &lbl_805E18A0;
}

/* The attribute arrays of an indexed instance: the merged arrays of fn_8046B110, one per
 * attribute the geometry has, in the order fn_8046B554 declares them. */
static inline void FillArrays(RpGeometry *geom, RwGameCubeVtxArray *vtx, RwGameCubeArrays *arrays) {
    RwGameCubeGeomFormat *format = RWGAMECUBEGEOMFORMAT(geom);
    u8 numTexCoords = geom->numTexCoordSets;
    u32 flags = geom->flags;
    u32 n;
    u32 i;

    if (format == NULL) {
        format = fn_80462E1C();
    }
    n = 0;
    arrays->num[GX_VA_POS] = vtx[n].num;
    arrays->data[GX_VA_POS] = vtx[n].data;
    n++;
    if (flags & 0x10) {
        arrays->num[GX_VA_NRM] = vtx[n].num;
        arrays->data[GX_VA_NRM] = vtx[n].data;
        n++;
    }
    if (flags & 8) {
        arrays->num[GX_VA_CLR0] = vtx[n].num;
        arrays->data[GX_VA_CLR0] = vtx[n].data;
        n++;
    }
    if (flags & 0x84) {
        for (i = 0; i < numTexCoords; i++) {
            arrays->num[GX_VA_TEX0 + i] = vtx[n].num;
            arrays->data[GX_VA_TEX0 + i] = vtx[n].data;
            n++;
        }
    }
}

/* The index arrays of one mesh of an indexed instance (one per attribute array). */
static inline void FillIndices(RpGeometry *geom, u16 **idx, u16 **indices) {
    RwGameCubeGeomFormat *format = RWGAMECUBEGEOMFORMAT(geom);
    u8 numTexCoords = geom->numTexCoordSets;
    u32 flags = geom->flags;
    u32 n;
    u32 i;

    if (format == NULL) {
        format = fn_80462E1C();
    }
    n = 0;
    indices[GX_VA_POS] = idx[n];
    n++;
    if (flags & 0x10) {
        indices[GX_VA_NRM] = idx[n];
        n++;
    }
    if (flags & 8) {
        indices[GX_VA_CLR0] = idx[n];
        n++;
    }
    if (flags & 0x84) {
        for (i = 0; i < numTexCoords; i++) {
            indices[GX_VA_TEX0 + i] = idx[n];
            n++;
        }
    }
}

/* Whether a geometry's prelight colours are all opaque, into bit 0 of the instance's flags:
 * the native format says so when there is one, else the colours are searched. */
#define SET_ALPHA_FLAG(header, geom)                                                     \
    if ((geom)->flags & 8) {                                                             \
        RwGameCubeGeomFormat *format = RWGAMECUBEGEOMFORMAT(geom);                       \
        if (format == NULL) {                                                            \
            s32 v;                                                                       \
            (header)->flags &= ~1;                                                       \
            for (v = 0; v < (geom)->numVertices; v++) {                                  \
                if ((geom)->preLitLum[v].alpha < 0xFF) {                                 \
                    (header)->flags |= 1;                                                \
                    break;                                                               \
                }                                                                        \
            }                                                                            \
        } else if (format->clrType > 2) {                                                \
            (header)->flags |= 1;                                                        \
        } else {                                                                         \
            (header)->flags &= ~1;                                                       \
        }                                                                                \
    }

/* Instances a geometry into display lists that index each attribute array separately (the
 * arrays and index blocks of fn_8046B110), one list per mesh and, for triangle strips, one
 * strip per list record, in a resource entry of `owner`. Returns the entry. */
RwResEntry *fn_8046B79C(RpGeometry *geom, void *owner, RwResEntry **ownerRef) {
    RwGameCubeArrays arrays;
    u16 *indices[26];
    void *vertexData;
    void **meshData;
    s32 numLists;
    s32 numVertices;
    RwGameCubeVtxDesc *desc;
    RwResEntry *entry;
    RwGameCubeInstHeader *header;
    u32 headerSize;
    u32 listsSize;
    u32 size;
    u32 primitive;
    u32 listSize;
    RwGameCubeDisplayList *lists;
    u8 *instData;
    u8 *data;
    u32 i;

    fn_8046B110(geom, &vertexData, &meshData);
    desc = fn_8046B554(geom, (RwGameCubeVtxArray *)vertexData);
    FillArrays(geom, (RwGameCubeVtxArray *)vertexData, &arrays);

    headerSize = fn_8046EE8C(desc);
    listsSize = geom->mesh->numMeshes * 8;
    size = headerSize + listsSize;
    size += 31;
    primitive = (geom->flags & 1) ? 0x98 : 0x90;
    for (i = 0; i < geom->mesh->numMeshes; i++) {
        if (geom->flags & 1) {
            fn_8046D8C8(*(u16 **)meshData[i], RPMESHES(geom->mesh)[i].numIndices, &numLists,
                        &numVertices, 1);
            size += fn_8046B094(desc, numLists, numVertices);
        } else {
            numVertices = RPMESHES(geom->mesh)[i].numIndices;
            size += fn_8046B094(desc, 1, numVertices);
        }
    }
    size += fn_8046EEA0(desc, &arrays);

    entry = (RwResEntry *)RwMalloc(size + sizeof(RwResEntry), 0x3050D);
    entry->link.next = NULL;
    entry->link.prev = NULL;
    entry->owner = owner;
    entry->size = size;
    entry->ownerRef = ownerRef;
    entry->destroyNotify = fn_8046288C;
    *ownerRef = entry;

    instData = (u8 *)(entry + 1);
    memset(instData, 0, size);
    header = (RwGameCubeInstHeader *)instData;
    header->serial = lbl_805F1678;
    header->meshSerial = geom->mesh->serialNum;
    header->flags = 0;
    SET_ALPHA_FLAG(header, geom);

    lists = (RwGameCubeDisplayList *)((u8 *)header + headerSize);
    data = (u8 *)(((u32)lists + listsSize + 31) & ~31);
    for (i = 0; i < geom->mesh->numMeshes; i++) {
        if (geom->flags & 1) {
            fn_8046D8C8(*(u16 **)meshData[i], RPMESHES(geom->mesh)[i].numIndices, &numLists,
                        &numVertices, 1);
        } else {
            numVertices = RPMESHES(geom->mesh)[i].numIndices;
            numLists = 1;
        }
        listSize = fn_8046B094(desc, numLists, numVertices);
        fn_8046B0E0((void **)&lists[i], i, listSize, data);
        FillIndices(geom, (u16 **)meshData[i], indices);
        fn_8046E594(desc, &lists[i], indices, RPMESHES(geom->mesh)[i].numIndices, geom->flags & 1,
                    fn_8046ACBC(desc), 1, primitive, NULL);
        data += listSize;
    }
    fn_8046F208(desc, header, &arrays, data);
    fn_8046F6CC(desc, header, &arrays, 0, 0);
    DCFlushRange(header, size);
    fn_80394F2C();

    for (i = 0; i < geom->mesh->numMeshes; i++) {
        RwFree(meshData[i]);
    }
    RwFree(meshData);
    RwFree(vertexData);
    return entry;
}

/* The same for a geometry drawn with its own arrays: one index, of the size the vertex count
 * needs, addresses every attribute. */
RwGameCubeVtxDesc *fn_8046BEC4(RpGeometry *geom) {
    RwGameCubeGeomFormat *format = RWGAMECUBEGEOMFORMAT(geom);
    u32 numVertices = geom->numVertices;
    s32 numArrays;
    s32 i;

    if (format == NULL) {
        format = fn_80462E1C();
    }
    fn_8046FC1C(&lbl_805E18A0);
    fn_8046FC48(&lbl_805E18A0, 0);
    numArrays = 0;

    fn_8046FC50(&lbl_805E18A0, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&lbl_805E18A0, GX_VA_POS, INDEX_TYPE(numVertices));
    numArrays++;
    if (geom->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&lbl_805E18A0, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18A0, GX_VA_NBT, INDEX_TYPE(numVertices));
        } else {
            fn_8046FC50(&lbl_805E18A0, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18A0, GX_VA_NRM, INDEX_TYPE(numVertices));
        }
        numArrays++;
    }
    if (geom->flags & 8) {
        fn_8046FC50(&lbl_805E18A0, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&lbl_805E18A0, GX_VA_CLR0, INDEX_TYPE(numVertices));
        numArrays++;
    }
    if (geom->flags & 0x84) {
        for (i = 0; i < geom->numTexCoordSets; i++) {
            fn_8046FC50(&lbl_805E18A0, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&lbl_805E18A0, GX_VA_TEX0 + i, INDEX_TYPE(numVertices));
            numArrays++;
        }
    }
    fn_8047046C(&lbl_805E18A0, numArrays);
    return &lbl_805E18A0;
}

/* Instances a geometry drawn with its own arrays into display lists, one per mesh, in a
 * resource entry of `owner`: allocated here for rpGEOMETRYNATIVEINSTANCE geometries, from
 * the resource arena otherwise. Returns the entry. */
RwResEntry *fn_8046C0D4(void *object, void *owner, RwResEntry **ownerRef) {
    RpGeometry *geom = (RpGeometry *)object;
    RwGameCubeArrays arrays;
    u16 *indices[26];
    s32 instSize;
    RwGameCubeInstHeader *header;
    u32 headerSize;
    u32 listsSize;
    u32 primitive;
    u32 listSize;
    u32 size;
    RwResEntry *entry;
    RwGameCubeVtxDesc *desc;
    RwGameCubeDisplayList *lists;
    u8 *instData;
    u8 *data;
    u32 i;
    s32 j;
    RpMorphTarget *morph;

    desc = fn_8046BEC4(geom);
    if (RWGAMECUBEGEOMFORMAT(geom) == NULL) {
        fn_80462E1C();
    }
    morph = geom->morphTarget;
    arrays.num[GX_VA_POS] = geom->numVertices;
    arrays.data[GX_VA_POS] = morph->verts;
    if (geom->flags & 0x10) {
        arrays.num[GX_VA_NRM] = geom->numVertices;
        arrays.data[GX_VA_NRM] = morph->normals;
    }
    if (geom->flags & 8) {
        arrays.num[GX_VA_CLR0] = geom->numVertices;
        arrays.data[GX_VA_CLR0] = geom->preLitLum;
    }
    if (geom->flags & 0x84) {
        for (j = 0; j < geom->numTexCoordSets; j++) {
            arrays.num[GX_VA_TEX0 + j] = geom->numVertices;
            arrays.data[GX_VA_TEX0 + j] = geom->texCoords[j];
        }
    }

    headerSize = fn_8046EE8C(desc);
    listsSize = geom->mesh->numMeshes * 8;
    size = headerSize + listsSize;
    size += 31;
    primitive = (geom->flags & 1) ? 0x98 : 0x90;
    for (i = 0; i < geom->mesh->numMeshes; i++) {
        size += fn_8046B094(desc, 1, RPMESHES(geom->mesh)[i].numIndices);
    }
    size += fn_8046EEA0(desc, &arrays);

    if (geom->flags & 0x02000000) {
        entry = (RwResEntry *)RwMalloc(size + sizeof(RwResEntry), 0x3050D);
        entry->link.next = NULL;
        entry->link.prev = NULL;
        entry->owner = owner;
        entry->size = size;
        entry->ownerRef = ownerRef;
        entry->destroyNotify = fn_8046288C;
        *ownerRef = entry;
    } else {
        entry = fn_80480878(owner, ownerRef, size, fn_8046288C);
    }

    instSize = size;
    instData = (u8 *)(entry + 1);
    memset(instData, 0, size);
    header = (RwGameCubeInstHeader *)instData;
    header->serial = lbl_805F1678;
    header->meshSerial = geom->mesh->serialNum;
    header->flags = 0;
    SET_ALPHA_FLAG(header, geom);

    lists = (RwGameCubeDisplayList *)((u8 *)header + headerSize);
    data = (u8 *)(((u32)lists + listsSize + 31) & ~31);
    for (i = 0; i < geom->mesh->numMeshes; i++) {
        u16 *meshIndices;
        listSize = fn_8046B094(desc, 1, RPMESHES(geom->mesh)[i].numIndices);
        fn_8046B0E0((void **)&lists[i], i, listSize, data);
        meshIndices = RPMESHES(geom->mesh)[i].indices;
        if (RWGAMECUBEGEOMFORMAT(geom) == NULL) {
            fn_80462E1C();
        }
        indices[GX_VA_POS] = meshIndices;
        if (geom->flags & 0x10) {
            indices[GX_VA_NRM] = meshIndices;
        }
        if (geom->flags & 8) {
            indices[GX_VA_CLR0] = meshIndices;
        }
        if (geom->flags & 0x84) {
            for (j = 0; j < geom->numTexCoordSets; j++) {
                indices[GX_VA_TEX0 + j] = meshIndices;
            }
        }
        fn_8046E594(desc, &lists[i], indices, RPMESHES(geom->mesh)[i].numIndices, 0, fn_8046ACBC(desc), 1,
                    primitive, NULL);
        data += listSize;
    }
    fn_8046F208(desc, header, &arrays, data);
    fn_8046F6CC(desc, header, &arrays, 0, 0);
    DCFlushRange(header, instSize);
    fn_80394F2C();
    return entry;
}

void *fn_80466194(RwGameCubeVtxStream *streams, s32 numStreams, s32 numVertices);
void *fn_80465ED4(void *table, RwGameCubeVtxStream *streams, s32 numStreams, s32 numVertices);
void *fn_80465CDC(void *table, u16 **indices, s32 numStreams, u32 numIndices);

/* fn_8046B110 for a world sector: the sector's arrays (its packed normals get their pad byte
 * cleared first), merged by the index optimiser, and one index block per mesh. */
s32 fn_8046C4D0(RpWorld *world, void *object, void **vertexData, void *meshDataOut) {
    void ***meshData = (void ***)meshDataOut;
    RwGameCubeVtxStream streams[16];
    s32 i;
    u32 total;
    u32 size;
    void *table;
    u16 **indices;
    s32 numStreams;
    s32 numMeshes;
    RpWorldSector *sector = (RpWorldSector *)object;

    if (RWGAMECUBEWORLDFORMAT(world) == NULL) {
        fn_80462E1C();
    }

    numStreams = 0;
    streams[numStreams].data = sector->vertices;
    streams[numStreams].type = 8;
    streams[numStreams].link = -1;
    numStreams++;
    if (world->flags & 0x10) {
        for (i = 0; i < sector->numVertices; i++) {
            sector->normals[i].pad = 0;
        }
        streams[numStreams].data = sector->normals;
        streams[numStreams].type = 4;
        streams[numStreams].link = -1;
        numStreams++;
    }
    if (world->flags & 8) {
        streams[numStreams].data = sector->preLitLum;
        streams[numStreams].type = 4;
        streams[numStreams].link = -1;
        numStreams++;
    }
    if (world->flags & 0x84) {
        for (i = 0; i < world->numTexCoordSets; i++) {
            streams[numStreams].data = sector->texCoords[i];
            streams[numStreams].type = 7;
            streams[numStreams].link = -1;
            numStreams++;
        }
    }

    total = numStreams;
    table = fn_80466194(streams, total, sector->numVertices);
    if (table == NULL) {
        return 0;
    }
    *vertexData = fn_80465ED4(table, streams, numStreams, sector->numVertices);
    if (*vertexData == NULL) {
        RwFree(table);
        return 0;
    }

    numMeshes = sector->mesh->numMeshes;
    size = numMeshes * 4;
    indices = (u16 **)RwMalloc(numStreams * size, 0x1050D);
    if (indices == NULL) {
        RwFree(*vertexData);
        RwFree(table);
        return 0;
    }
    memset(indices, 0, numStreams * size);
    for (i = 0; i < numMeshes; i++) {
        RpMesh *mesh = &RPMESHES(sector->mesh)[i];
        s32 j;
        for (j = 0; j < numStreams; j++) {
            indices[i * numStreams + j] = mesh->indices;
        }
    }

    *meshData = (void **)RwMalloc(size, 0x1050D);
    if (*meshData == NULL) {
        RwFree(indices);
        RwFree(*vertexData);
        RwFree(table);
        return 0;
    }
    for (i = 0; i < numMeshes; i++) {
        (*meshData)[i] = fn_80465CDC(table, &indices[i * numStreams], total,
                                     RPMESHES(sector->mesh)[i].numIndices);
        if ((*meshData)[i] == NULL) {
            while (--i > 0) {
                RwFree((*meshData)[i]);
            }
            return 0;
        }
    }
    RwFree(indices);
    RwFree(table);
    return 1;
}

/* The world's version of fn_8046B554 (it has its own scratch descriptor, lbl_805E18C0). */
RwGameCubeVtxDesc *fn_8046C94C(RpWorld *world, RwGameCubeVtxArray *arrays) {
    RwGameCubeGeomFormat *format = RWGAMECUBEWORLDFORMAT(world);
    s32 numArrays;
    s32 i;

    if (format == NULL) {
        format = fn_80462E1C();
    }
    fn_8046FC1C(&lbl_805E18C0);
    fn_8046FC48(&lbl_805E18C0, 0);
    numArrays = 0;

    fn_8046FC50(&lbl_805E18C0, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&lbl_805E18C0, GX_VA_POS, INDEX_TYPE(arrays[numArrays].num));
    numArrays++;
    if (world->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&lbl_805E18C0, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18C0, GX_VA_NBT, INDEX_TYPE(arrays[numArrays].num));
        } else {
            fn_8046FC50(&lbl_805E18C0, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18C0, GX_VA_NRM, INDEX_TYPE(arrays[numArrays].num));
        }
        numArrays++;
    }
    if (world->flags & 8) {
        fn_8046FC50(&lbl_805E18C0, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&lbl_805E18C0, GX_VA_CLR0, INDEX_TYPE(arrays[numArrays].num));
        numArrays++;
    }
    if (world->flags & 0x84) {
        for (i = 0; i < world->numTexCoordSets; i++) {
            fn_8046FC50(&lbl_805E18C0, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&lbl_805E18C0, GX_VA_TEX0 + i, INDEX_TYPE(arrays[numArrays].num));
            numArrays++;
        }
    }
    fn_8047046C(&lbl_805E18C0, numArrays);
    return &lbl_805E18C0;
}
}
