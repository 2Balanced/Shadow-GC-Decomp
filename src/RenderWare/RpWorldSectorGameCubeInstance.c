/* RenderWare world plugin, GameCube: instancing a world sector into display lists,
 * 0x8046C4D0..0x8046D8C8. The twin of RpGeometryGameCubeInstance.c for the sectors of a world,
 * whose format is the world's (plugin data at lbl_805F1524): fn_8046C4D0 merges a sector's
 * attribute arrays, fn_8046C94C and fn_8046D2B8 fill the file's scratch vertex descriptor (.bss
 * 0x805E18C0), fn_8046CB94 and fn_8046D4C8 build the instance. Positions are quantised from the
 * sector's offset (+0x6C). Names are inferred from behaviour.
 *
 * Source file: its own scratch descriptor; the small list-size functions are called out of
 * line as in the geometry file. The display-list file after it starts with the strip counter
 * fn_8046D8C8, which both instancing files and the skin plugin call. Old units Game/RwCpp32
 * (end) and RwCpp33 (first part).
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw (cflags_rw_132).
 * Matching: fn_8046C4D0 (the sector a typed parameter), fn_8046C94C, fn_8046D2B8. fn_8046CB94
 * and fn_8046D4C8 have retail's instruction count but other saved-register assignments. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

/* The scratch vertex descriptor of a world sector's instance. */
static RwGameCubeVtxDesc worldSectorVtxDesc;

/* An array of 255 entries or more needs 16-bit indices (0xFF is not used as a byte index).
 * The enumerators keep the choice a branch, as in retail. */
#define INDEX_TYPE(num) (((num) >= 0xFF) ? GX_INDEX16 : GX_INDEX8)

/* fn_8046B110 for a world sector: the sector's arrays (its packed normals get their pad byte
 * cleared first), merged by the index optimiser, and one index block per mesh. */
s32 fn_8046C4D0(RpWorld *world, RpWorldSector *sector, void **vertexData, void ***meshData) {
    RwGameCubeVtxStream streams[16];
    s32 i;
    u32 total;
    u32 size;
    void *table;
    u16 **indices;
    s32 numStreams;
    s32 numMeshes;

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

/* The world's version of fn_8046B554 (it has its own scratch descriptor, worldSectorVtxDesc). */
RwGameCubeVtxDesc *fn_8046C94C(RpWorld *world, RwGameCubeVtxArray *arrays) {
    RwGameCubeGeomFormat *format = RWGAMECUBEWORLDFORMAT(world);
    s32 numArrays;
    s32 i;

    if (format == NULL) {
        format = fn_80462E1C();
    }
    fn_8046FC1C(&worldSectorVtxDesc);
    fn_8046FC48(&worldSectorVtxDesc, 0);
    numArrays = 0;

    fn_8046FC50(&worldSectorVtxDesc, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&worldSectorVtxDesc, GX_VA_POS, INDEX_TYPE(arrays[numArrays].num));
    numArrays++;
    if (world->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&worldSectorVtxDesc, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&worldSectorVtxDesc, GX_VA_NBT, INDEX_TYPE(arrays[numArrays].num));
        } else {
            fn_8046FC50(&worldSectorVtxDesc, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&worldSectorVtxDesc, GX_VA_NRM, INDEX_TYPE(arrays[numArrays].num));
        }
        numArrays++;
    }
    if (world->flags & 8) {
        fn_8046FC50(&worldSectorVtxDesc, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&worldSectorVtxDesc, GX_VA_CLR0, INDEX_TYPE(arrays[numArrays].num));
        numArrays++;
    }
    if (world->flags & 0x84) {
        for (i = 0; i < world->numTexCoordSets; i++) {
            fn_8046FC50(&worldSectorVtxDesc, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&worldSectorVtxDesc, GX_VA_TEX0 + i, INDEX_TYPE(arrays[numArrays].num));
            numArrays++;
        }
    }
    fn_8047046C(&worldSectorVtxDesc, numArrays);
    return &worldSectorVtxDesc;
}

/* The attribute arrays of an indexed instance: the merged arrays of fn_8046C4D0, one per
 * attribute the geometry has, in the order fn_8046C94C declares them. */
static inline void FillWorldArrays(RpWorld *world, RpWorldSector *sector, RwGameCubeVtxArray *vtx, RwGameCubeArrays *arrays) {
    RwGameCubeGeomFormat *format = RWGAMECUBEWORLDFORMAT(world);
    u8 numTexCoords = world->numTexCoordSets;
    u32 flags = world->flags;
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
static inline void FillWorldIndices(RpWorld *world, RpWorldSector *sector, u16 **idx, u16 **indices) {
    RwGameCubeGeomFormat *format = RWGAMECUBEWORLDFORMAT(world);
    u8 numTexCoords = world->numTexCoordSets;
    u32 flags = world->flags;
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

/* Whether a sector's prelight colours are all opaque, into bit 0 of the instance's flags:
 * the native format says so when there is one, else the colours are searched. */
#define SET_ALPHA_FLAG(header, world, sector)                                                     \
    if ((world)->flags & 8) {                                                             \
        RwGameCubeGeomFormat *format = RWGAMECUBEWORLDFORMAT(world);                       \
        if (format == NULL) {                                                            \
            s32 v;                                                                       \
            (header)->flags &= ~1;                                                       \
            for (v = 0; v < (sector)->numVertices; v++) {                                  \
                if ((sector)->preLitLum[v].alpha < 0xFF) {                                 \
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

/* fn_8046B79C for a world sector: the sector's display lists, indexed per attribute array,
 * in a resource entry of the sector (its repEntry). */
RwResEntry *fn_8046CB94(RpWorld *world, RpWorldSector *sector) {
    RwGameCubeArrays arrays;
    u16 *indices[26];
    void *vertexData;
    void **meshData;
    s32 numLists;
    s32 numVertices;
    u32 listSize;
    u32 headerSize;
    u32 listsSize;
    u8 *data;
    u32 primitive;
    RwResEntry *entry;
    RwGameCubeInstHeader *header;
    RwGameCubeVtxDesc *desc;
    RwGameCubeDisplayList *lists;
    u8 *instData;
    u32 size;
    u32 i;

    fn_8046C4D0(world, sector, &vertexData, &meshData);
    desc = fn_8046C94C(world, (RwGameCubeVtxArray *)vertexData);
    FillWorldArrays(world, sector, (RwGameCubeVtxArray *)vertexData, &arrays);

    headerSize = fn_8046EE8C(desc);
    listsSize = sector->mesh->numMeshes * 8;
    size = headerSize + listsSize;
    size += 31;
    primitive = (world->flags & 1) ? 0x98 : 0x90;
    for (i = 0; i < sector->mesh->numMeshes; i++) {
        if (world->flags & 1) {
            fn_8046D8C8(*(u16 **)meshData[i], RPMESHES(sector->mesh)[i].numIndices, &numLists,
                        &numVertices, 1);
            size += fn_8046B094(desc, numLists, numVertices);
        } else {
            size += fn_8046B094(desc, 1, RPMESHES(sector->mesh)[i].numIndices);
        }
    }
    size += fn_8046EEA0(desc, &arrays);

    entry = (RwResEntry *)RwMalloc(size + sizeof(RwResEntry), 0x3050D);
    entry->link.next = NULL;
    entry->link.prev = NULL;
    entry->owner = sector;
    entry->size = size;
    entry->ownerRef = &sector->repEntry;
    entry->destroyNotify = fn_8046288C;
    sector->repEntry = entry;

    instData = (u8 *)(entry + 1);
    memset(instData, 0, size);
    header = (RwGameCubeInstHeader *)instData;
    header->serial = lbl_805F1678;
    header->meshSerial = sector->mesh->serialNum;
    header->flags = 0;
    SET_ALPHA_FLAG(header, world, sector);

    lists = (RwGameCubeDisplayList *)((u8 *)header + headerSize);
    data = (u8 *)(((u32)lists + listsSize + 31) & ~31);
    for (i = 0; i < sector->mesh->numMeshes; i++) {
        s32 numLists;
        s32 numVertices;

        if (world->flags & 1) {
            fn_8046D8C8(*(u16 **)meshData[i], RPMESHES(sector->mesh)[i].numIndices, &numLists,
                        &numVertices, 1);
        } else {
            numVertices = RPMESHES(sector->mesh)[i].numIndices;
            numLists = 1;
        }
        listSize = fn_8046B094(desc, numLists, numVertices);
        fn_8046B0E0((void **)&lists[i], i, listSize, data);
        FillWorldIndices(world, sector, (u16 **)meshData[i], indices);
        fn_8046E594(desc, &lists[i], indices, RPMESHES(sector->mesh)[i].numIndices, world->flags & 1,
                    fn_8046ACBC(desc), 1, primitive, &sector->unk6C);
        data += listSize;
    }
    fn_8046F208(desc, header, &arrays, data);
    fn_8046F6CC(desc, header, &arrays, 1, &sector->unk6C);
    DCFlushRange(header, size);
    fn_80394F2C();

    for (i = 0; i < sector->mesh->numMeshes; i++) {
        RwFree(meshData[i]);
    }
    RwFree(meshData);
    RwFree(vertexData);
    return entry;
}

/* Fills the world's scratch vertex descriptor for a sector drawn with its own arrays: one
 * index, of the size the vertex count needs, addresses every attribute. */
RwGameCubeVtxDesc *fn_8046D2B8(RpWorld *world, RpWorldSector *sector) {
    RwGameCubeGeomFormat *format = RWGAMECUBEWORLDFORMAT(world);
    u32 numVertices = sector->numVertices;
    s32 numArrays;
    s32 i;

    if (format == NULL) {
        format = fn_80462E1C();
    }
    fn_8046FC1C(&worldSectorVtxDesc);
    fn_8046FC48(&worldSectorVtxDesc, 0);
    numArrays = 0;

    fn_8046FC50(&worldSectorVtxDesc, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&worldSectorVtxDesc, GX_VA_POS, INDEX_TYPE(numVertices));
    numArrays++;
    if (world->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&worldSectorVtxDesc, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&worldSectorVtxDesc, GX_VA_NBT, INDEX_TYPE(numVertices));
        } else {
            fn_8046FC50(&worldSectorVtxDesc, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&worldSectorVtxDesc, GX_VA_NRM, INDEX_TYPE(numVertices));
        }
        numArrays++;
    }
    if (world->flags & 8) {
        fn_8046FC50(&worldSectorVtxDesc, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&worldSectorVtxDesc, GX_VA_CLR0, INDEX_TYPE(numVertices));
        numArrays++;
    }
    if (world->flags & 0x84) {
        for (i = 0; i < world->numTexCoordSets; i++) {
            fn_8046FC50(&worldSectorVtxDesc, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&worldSectorVtxDesc, GX_VA_TEX0 + i, INDEX_TYPE(numVertices));
            numArrays++;
        }
    }
    fn_8047046C(&worldSectorVtxDesc, numArrays);
    return &worldSectorVtxDesc;
}

/* fn_8046C0D4 for a world sector drawn with its own arrays. */
RwResEntry *fn_8046D4C8(RpWorld *world, RpWorldSector *sector, void *owner, RwResEntry **ownerRef) {
    RwGameCubeArrays arrays;
    u16 *indices[26];
    RwGameCubeInstHeader *header;
    u32 headerSize;
    u32 listsSize;
    u32 primitive;
    u32 listSize;
    RwResEntry *entry;
    u8 *data;
    s32 instSize;
    RwGameCubeVtxDesc *desc;
    RwGameCubeDisplayList *lists;
    u8 *instData;
    u32 size;
    u32 i;
    s32 j;

    desc = fn_8046D2B8(world, sector);
    if (RWGAMECUBEWORLDFORMAT(world) == NULL) {
        fn_80462E1C();
    }
    arrays.num[GX_VA_POS] = sector->numVertices;
    arrays.data[GX_VA_POS] = sector->vertices;
    if (world->flags & 0x10) {
        arrays.num[GX_VA_NRM] = sector->numVertices;
        arrays.data[GX_VA_NRM] = sector->normals;
    }
    if (world->flags & 8) {
        arrays.num[GX_VA_CLR0] = sector->numVertices;
        arrays.data[GX_VA_CLR0] = sector->preLitLum;
    }
    if (world->flags & 0x84) {
        for (j = 0; j < world->numTexCoordSets; j++) {
            arrays.num[GX_VA_TEX0 + j] = sector->numVertices;
            arrays.data[GX_VA_TEX0 + j] = sector->texCoords[j];
        }
    }

    headerSize = fn_8046EE8C(desc);
    listsSize = sector->mesh->numMeshes * 8;
    size = headerSize + listsSize;
    size += 31;
    primitive = (world->flags & 1) ? 0x98 : 0x90;
    for (i = 0; i < sector->mesh->numMeshes; i++) {
        size += fn_8046B094(desc, 1, RPMESHES(sector->mesh)[i].numIndices);
    }
    size += fn_8046EEA0(desc, &arrays);

    if (world->flags & 0x02000000) {
        entry = (RwResEntry *)RwMalloc(size + sizeof(RwResEntry), 0x3050D);
        entry->link.next = NULL;
        entry->link.prev = NULL;
        entry->owner = sector;
        entry->size = size;
        entry->ownerRef = &sector->repEntry;
        entry->destroyNotify = fn_8046288C;
        sector->repEntry = entry;
    } else {
        entry = fn_80480878(owner, ownerRef, size, fn_8046288C);
    }

    instSize = size;
    instData = (u8 *)(entry + 1);
    memset(instData, 0, size);
    header = (RwGameCubeInstHeader *)instData;
    header->serial = lbl_805F1678;
    header->meshSerial = sector->mesh->serialNum;
    header->flags = 0;
    SET_ALPHA_FLAG(header, world, sector);

    lists = (RwGameCubeDisplayList *)((u8 *)header + headerSize);
    data = (u8 *)(((u32)lists + listsSize + 31) & ~31);
    for (i = 0; i < sector->mesh->numMeshes; i++) {
        u16 *meshIndices;
        listSize = fn_8046B094(desc, 1, RPMESHES(sector->mesh)[i].numIndices);
        fn_8046B0E0((void **)&lists[i], i, listSize, data);
        meshIndices = RPMESHES(sector->mesh)[i].indices;
        if (RWGAMECUBEWORLDFORMAT(world) == NULL) {
            fn_80462E1C();
        }
        indices[GX_VA_POS] = meshIndices;
        if (world->flags & 0x10) {
            indices[GX_VA_NRM] = meshIndices;
        }
        if (world->flags & 8) {
            indices[GX_VA_CLR0] = meshIndices;
        }
        if (world->flags & 0x84) {
            for (j = 0; j < world->numTexCoordSets; j++) {
                indices[GX_VA_TEX0 + j] = meshIndices;
            }
        }
        fn_8046E594(desc, &lists[i], indices, RPMESHES(sector->mesh)[i].numIndices, 0, fn_8046ACBC(desc), 1,
                    primitive, &sector->unk6C);
        data += listSize;
    }
    fn_8046F208(desc, header, &arrays, data);
    fn_8046F6CC(desc, header, &arrays, 1, &sector->unk6C);
    DCFlushRange(header, instSize);
    fn_80394F2C();
    return entry;
}
