/* RenderWare GameCube driver, world sector instancing into display lists, 0x8046CB94..0x8046E594.
 * Names are inferred from behaviour. Compiled by CodeWarrior GC 1.3.2 with -O4,p.
 * Matching: fn_8046D2B8. fn_8046CB94 and fn_8046D4C8 are the world twins of fn_8046B79C and
 * fn_8046C0D4 (RwCpp32.cpp), written the same way and with the same saved-register difference.
 * fn_8046DA24 is not written yet. */
#include <dolphin/types.h>
#include <string.h>
#include "Game/rw/r4.h"

extern "C" {
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
                    fn_8046ACBC(desc), 1, primitive, sector->unk6C);
        data += listSize;
    }
    fn_8046F208(desc, header, &arrays, data);
    fn_8046F6CC(desc, header, &arrays, 1, sector->unk6C);
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
    fn_8046FC1C(&lbl_805E18C0);
    fn_8046FC48(&lbl_805E18C0, 0);
    numArrays = 0;

    fn_8046FC50(&lbl_805E18C0, GX_VA_POS, 1, format->posType, format->posFrac);
    fn_8046FF34(&lbl_805E18C0, GX_VA_POS, INDEX_TYPE(numVertices));
    numArrays++;
    if (world->flags & 0x10) {
        if (format->nbt) {
            fn_8046FC50(&lbl_805E18C0, GX_VA_NBT, 1, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18C0, GX_VA_NBT, INDEX_TYPE(numVertices));
        } else {
            fn_8046FC50(&lbl_805E18C0, GX_VA_NRM, 0, format->nrmType, 0);
            fn_8046FF34(&lbl_805E18C0, GX_VA_NRM, INDEX_TYPE(numVertices));
        }
        numArrays++;
    }
    if (world->flags & 8) {
        fn_8046FC50(&lbl_805E18C0, GX_VA_CLR0, format->clrType > 2, format->clrType, 0);
        fn_8046FF34(&lbl_805E18C0, GX_VA_CLR0, INDEX_TYPE(numVertices));
        numArrays++;
    }
    if (world->flags & 0x84) {
        for (i = 0; i < world->numTexCoordSets; i++) {
            fn_8046FC50(&lbl_805E18C0, GX_VA_TEX0 + i, 1, format->texType[i], format->texFrac[i]);
            fn_8046FF34(&lbl_805E18C0, GX_VA_TEX0 + i, INDEX_TYPE(numVertices));
            numArrays++;
        }
    }
    fn_8047046C(&lbl_805E18C0, numArrays);
    return &lbl_805E18C0;
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
                    primitive, NULL);
        data += listSize;
    }
    fn_8046F208(desc, header, &arrays, data);
    fn_8046F6CC(desc, header, &arrays, 0, 0);
    DCFlushRange(header, instSize);
    fn_80394F2C();
    return entry;
}

#define CLOSE_STRIP()            \
    do {                         \
        *numVertices += run + 2; \
        run = 0;                 \
        *numStrips += 1;         \
    } while (0)

/* Counts the strips and strip vertices a triangle strip's index list becomes once it is cut
 * at its degenerate triangles. With `swap` set a strip may only start on an even triangle, so
 * that its winding stays: an odd start takes one index more.
 * NonMatching in its first six instructions only: retail zeroes the two results through r9
 * and loads the counter's zero after them, GC 1.3.2 keeps the counter in r9 from the start
 * and takes r10 for the stores (ten source orders tried). */
void fn_8046D8C8(u16 *indices, u32 numIndices, s32 *numStrips, s32 *numVertices, s32 swap) {
    u32 run;
    u32 i;

    *numStrips = 0;
    *numVertices = 0;
    run = 0;
    for (i = 0; i < numIndices - 2; i++) {
        if (indices[0] != indices[1] && indices[0] != indices[2] && indices[1] != indices[2]) {
            if (run == 0 && (i & 1) && swap) {
                run = 2;
            } else {
                run++;
            }
        } else if (run != 0) {
            if (swap && i < numIndices - 3) {
                if (indices[1] != indices[2] && indices[1] != indices[3] &&
                    indices[2] != indices[3]) {
                    if (((i + 1) & 1) == 0) {
                        CLOSE_STRIP();
                    } else {
                        run++;
                    }
                } else {
                    CLOSE_STRIP();
                }
            } else {
                CLOSE_STRIP();
            }
        }
        indices++;
    }
    if (run != 0) {
        CLOSE_STRIP();
    }
}

#define WRITE_STRIP()                                              \
    do {                                                           \
        u8 *head = &out[numStrips * 3 + numVertices * vertexSize]; \
        head[0] = command;                                         \
        *(u16 *)(head + 1) = run + 2;                              \
    } while (0)
#define EMIT_STRIP()            \
    do {                        \
        WRITE_STRIP();          \
        numVertices += run + 2; \
        run = 0;                \
        numStrips++;            \
    } while (0)

/* Writes the primitive headers (GX command byte and 16-bit vertex count) of the strips
 * fn_8046D8C8 counts into the display list at `out`, leaving `vertexSize` bytes for each
 * vertex after them. Without `split` the whole list is one primitive.
 * NonMatching by registers only: retail keeps the run and the vertex count in r11 and r10 and
 * the loaded indices in r30 and r31, GC 1.3.2 gives this source the opposite. */
void fn_8046E3E0(u16 *indices, u32 numIndices, s32 split, u32 vertexSize, u8 command, s32 swap,
                 u8 *out) {
    u32 numStrips = 0;
    u32 numVertices = 0;
    u32 run = 0;
    u32 i;

    if (split) {
        for (i = 0; i < numIndices - 2; i++) {
            if (indices[0] != indices[1] && indices[0] != indices[2] &&
                indices[1] != indices[2]) {
                if (run == 0 && (i & 1) && swap) {
                    run = 2;
                } else {
                    run++;
                }
            } else if (run != 0) {
                if (swap && i < numIndices - 3) {
                    if (indices[1] != indices[2] && indices[1] != indices[3] &&
                        indices[2] != indices[3]) {
                        if (((i + 1) & 1) == 0) {
                            EMIT_STRIP();
                        } else {
                            run++;
                        }
                    } else {
                        EMIT_STRIP();
                    }
                } else {
                    EMIT_STRIP();
                }
            }
            indices++;
        }
        if (run != 0) {
            WRITE_STRIP();
        }
    } else {
        out[0] = command;
        *(u16 *)(out + 1) = numIndices;
    }
}
}
