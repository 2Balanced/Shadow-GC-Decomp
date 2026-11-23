/* RenderWare world plugin, GameCube: instancing a geometry into display lists,
 * 0x8046B110..0x8046C4D0. fn_8046B110 merges the geometry's attribute arrays through the index
 * optimiser, fn_8046B554 and fn_8046BEC4 fill the file's scratch vertex descriptor (.bss
 * 0x805E18A0), fn_8046B79C and fn_8046C0D4 build the instance: indexed per attribute array, or
 * with one index for all. Names are inferred from behaviour.
 *
 * Source file: fn_8046B79C and fn_8046C0D4 call the small fn_8046B094 and fn_8046B0E0 out of
 * line (they are in the file before), and fn_8046C4D0 starts the world-sector twin of this file,
 * which has a scratch descriptor of its own (.bss 0x805E18C0), as the atomic and world-sector
 * instance nodes of range r3 are files of their own. Old units Game/RwCpp31 (end) and RwCpp32.
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw (cflags_rw_132).
 * Matching: fn_8046B110 (the geometry is the typed first parameter: a void * parameter copied
 * into a typed local costs three saved registers), fn_8046B554, fn_8046BEC4. fn_8046B79C and
 * fn_8046C0D4 have retail's instruction count but other saved-register assignments; retail also
 * keeps a copy of the instance data as the header (mr r21,r25 in fn_8046C0D4) that no spelling
 * tried here reproduces (probes/RenderWare_RpGeometryGameCubeInstance.py). */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

/* The scratch vertex descriptor of a geometry's instance. */
static RwGameCubeVtxDesc geometryVtxDesc;

/* Builds the indexed vertex data of a geometry: one stream per attribute array (positions,
 * normals, prelight colours, texture coordinate sets), merged by the index optimiser into
 * `*vertexData`, and one index block per mesh in `*meshData`. Returns 0 when out of memory.
 * The failure path of the last loop leaks as retail does. */
s32 fn_8046B110(RpGeometry *geom, void **vertexData, void ***meshData) {
    RwGameCubeVtxStream streams[16];
    s32 i;
    u32 total;
    u32 size;
    void *table;
    u16 **indices;
    s32 numStreams;
    s32 numMeshes;

    if (RWGAMECUBEGEOMFORMAT(geom) == NULL) {
        fn_80462E1C();
    }

    numStreams = 0;
    streams[numStreams].data = geom->morphTarget->verts;
    streams[numStreams].type = 8;
    streams[numStreams].link = -1;
    numStreams++;
    if (geom->flags & 0x10) {
        streams[numStreams].data = geom->morphTarget->normals;
        streams[numStreams].type = 8;
        streams[numStreams].link = -1;
        numStreams++;
    }
    if (geom->flags & 8) {
        streams[numStreams].data = geom->preLitLum;
        streams[numStreams].type = 4;
        streams[numStreams].link = -1;
        numStreams++;
    }
    if (geom->flags & 0x84) {
        for (i = 0; i < geom->numTexCoordSets; i++) {
            streams[numStreams].data = geom->texCoords[i];
            streams[numStreams].type = 7;
            streams[numStreams].link = -1;
            numStreams++;
        }
    }

    total = numStreams;
    table = fn_80466194(streams, total, geom->numVertices);
    if (table == NULL) {
        return 0;
    }
    *vertexData = fn_80465ED4(table, streams, numStreams, geom->numVertices);
    if (*vertexData == NULL) {
        RwFree(table);
        return 0;
    }

    numMeshes = geom->mesh->numMeshes;
    size = numMeshes * 4;
    indices = (u16 **)RwMalloc(numStreams * size, 0x1050D);
    if (indices == NULL) {
        RwFree(*vertexData);
        RwFree(table);
        return 0;
    }
    memset(indices, 0, numStreams * size);
    for (i = 0; i < numMeshes; i++) {
        RpMesh *mesh = &RPMESHES(geom->mesh)[i];
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
                                     RPMESHES(geom->mesh)[i].numIndices);
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
    fn_8046FC1C(&geometryVtxDesc);
    fn_8046FC48(&geometryVtxDesc, 0);
    numArrays = 0;

    fn_8046FC50(&geometryVtxDesc, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&geometryVtxDesc, GX_VA_POS, INDEX_TYPE(arrays[numArrays].num));
    numArrays++;
    if (geom->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&geometryVtxDesc, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&geometryVtxDesc, GX_VA_NBT, INDEX_TYPE(arrays[numArrays].num));
        } else {
            fn_8046FC50(&geometryVtxDesc, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&geometryVtxDesc, GX_VA_NRM, INDEX_TYPE(arrays[numArrays].num));
        }
        numArrays++;
    }
    if (geom->flags & 8) {
        fn_8046FC50(&geometryVtxDesc, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&geometryVtxDesc, GX_VA_CLR0, INDEX_TYPE(arrays[numArrays].num));
        numArrays++;
    }
    if (geom->flags & 0x84) {
        for (i = 0; i < geom->numTexCoordSets; i++) {
            fn_8046FC50(&geometryVtxDesc, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&geometryVtxDesc, GX_VA_TEX0 + i, INDEX_TYPE(arrays[numArrays].num));
            numArrays++;
        }
    }
    fn_8047046C(&geometryVtxDesc, numArrays);
    return &geometryVtxDesc;
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
    fn_8046FC1C(&geometryVtxDesc);
    fn_8046FC48(&geometryVtxDesc, 0);
    numArrays = 0;

    fn_8046FC50(&geometryVtxDesc, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&geometryVtxDesc, GX_VA_POS, INDEX_TYPE(numVertices));
    numArrays++;
    if (geom->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&geometryVtxDesc, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&geometryVtxDesc, GX_VA_NBT, INDEX_TYPE(numVertices));
        } else {
            fn_8046FC50(&geometryVtxDesc, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&geometryVtxDesc, GX_VA_NRM, INDEX_TYPE(numVertices));
        }
        numArrays++;
    }
    if (geom->flags & 8) {
        fn_8046FC50(&geometryVtxDesc, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&geometryVtxDesc, GX_VA_CLR0, INDEX_TYPE(numVertices));
        numArrays++;
    }
    if (geom->flags & 0x84) {
        for (i = 0; i < geom->numTexCoordSets; i++) {
            fn_8046FC50(&geometryVtxDesc, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&geometryVtxDesc, GX_VA_TEX0 + i, INDEX_TYPE(numVertices));
            numArrays++;
        }
    }
    fn_8047046C(&geometryVtxDesc, numArrays);
    return &geometryVtxDesc;
}

/* Instances a geometry drawn with its own arrays into display lists, one per mesh, in a
 * resource entry of `owner`: allocated here for rpGEOMETRYNATIVEINSTANCE geometries, from
 * the resource arena otherwise. Returns the entry. */
RwResEntry *fn_8046C0D4(RpGeometry *geom, void *owner, RwResEntry **ownerRef) {
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
