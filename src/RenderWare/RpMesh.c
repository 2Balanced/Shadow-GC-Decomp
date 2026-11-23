/* RenderWare 3.7 world plugin: meshes, 0x8045BF30-0x8045CA38 (wave 41 re-cut of the size-cut
 * Game/RwCpp21 tail and Game/RwCpp22 head). The file owns the build-mesh free list pointer (.sdata
 * 0x805EDDC0) and the mesh module record (.sbss 0x805F14F0), which the triangle-strip file behind it
 * and the instancing code of other files (fn_80448850) read, so it keeps its address name. Its
 * mesh reader expands the allocation helper that starts the file; the strip builder calls it.
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_8045BF30  mesh allocation helper   fn_8045BF64 / fn_8045BFBC  mesh module close / open
 *   fn_8045C144  _rpBuildMeshCreate       fn_8045C26C  _rpBuildMeshDestroy
 *   fn_8045C2D4  _rpMeshDestroy           fn_8045C340  _rpBuildMeshAddTriangle
 *   fn_8045C4E4  _rpMeshHeaderForAllMeshes  fn_8045C56C  _rpMeshWrite
 *   fn_8045C770  _rpMeshRead              fn_8045C9C8  _rpMeshSize
 *   fn_8045CA20  _rpMeshGetNextSerialNumber
 * Built with cflags_rw_r3 (configure.py): C, functions in definition order.
 * Status: fn_8045C770 has retail's instructions except that GC/2.7 copies the chunk count to r3
 * before the unrolled index copy (retail unrolls on the count itself); 15 loop spellings were
 * measured (probes/Game_RwCpp22.py). */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

RwFreeList *fn_8047FBD8(s32, s32, s32, u32);              /* RwFreeListCreate */
s32 fn_8047FCDC(RwFreeList *);                            /* RwFreeListDestroy */

/* The mesh module record (plugin 0x502): read by the strip builder and other files. */
RwModuleInfo lbl_805F14F0;

/* The build-mesh free list (explicitly zeroed: it sits in .sdata). */
#pragma explicit_zero_data on
static RwFreeList *buildMeshFreeList = NULL;
#pragma explicit_zero_data reset

void *fn_8045BF30(u32 size) {
    return RwMalloc(size, 0x30502);
}

void *fn_8045BF64(void *instance) {
    lbl_805F14F0.numInstances--;
    if (lbl_805F14F0.numInstances == 0) {
        if (buildMeshFreeList != NULL) {
            fn_8047FCDC(buildMeshFreeList);
            buildMeshFreeList = NULL;
        }
    }
    return instance;
}

#define RPMESHGLOBAL(member) (RWPLUGINGLOBAL(rpMeshGlobals, lbl_805F14F0).member)

void *fn_8045BFBC(void *instance, s32 offset) {
    lbl_805F14F0.globalsOffset = offset;
    if (lbl_805F14F0.numInstances == 0) {
        buildMeshFreeList = fn_8047FBD8(sizeof(RpBuildMesh), 50, 4, 0x40502);
        if (buildMeshFreeList == NULL) {
            if (buildMeshFreeList != NULL) {
                fn_8047FCDC(buildMeshFreeList);
                buildMeshFreeList = NULL;
            }
            return NULL;
        }
    }
    RPMESHGLOBAL(nextSerialNum) = 1;
    lbl_805F14F0.numInstances++;
    RPMESHGLOBAL(meshFlagsToPrimType)[0] = 3;
    RPMESHGLOBAL(meshFlagsToPrimType)[1] = 4;
    RPMESHGLOBAL(meshFlagsToPrimType)[2] = 5;
    RPMESHGLOBAL(meshFlagsToPrimType)[4] = 1;
    RPMESHGLOBAL(meshFlagsToPrimType)[8] = 2;
    RPMESHGLOBAL(meshFlagsToPrimType)[16] = 6;
    RPMESHGLOBAL(primTypeToMeshFlags)[1] = 4;
    RPMESHGLOBAL(primTypeToMeshFlags)[2] = 8;
    RPMESHGLOBAL(primTypeToMeshFlags)[3] = 0;
    RPMESHGLOBAL(primTypeToMeshFlags)[4] = 1;
    RPMESHGLOBAL(primTypeToMeshFlags)[5] = 2;
    RPMESHGLOBAL(primTypeToMeshFlags)[6] = 16;
    return instance;
}

RpBuildMesh *fn_8045C144(u32 bufferSize) {
    RpBuildMesh *mesh = (RpBuildMesh *)RwFreeListAlloc(buildMeshFreeList, 0x30502);
    if (mesh != NULL) {
        mesh->numTriangles = 0;
        if (bufferSize != 0) {
            u32 size = bufferSize * sizeof(RpBuildMeshTriangle);
            mesh->meshTriangles = (RpBuildMeshTriangle *)RwMalloc(size, 0x1030502);
            if (mesh->meshTriangles == NULL) {
                RwFreeListFree(buildMeshFreeList, mesh);
                RWERROR((E_RW_NOMEM, size));
                return NULL;
            }
            mesh->triangleBufferSize = bufferSize;
        } else {
            mesh->meshTriangles = NULL;
            mesh->triangleBufferSize = 0;
        }
        return mesh;
    }
    RWERROR((E_RW_NOMEM, sizeof(RpBuildMesh)));
    return NULL;
}

s32 fn_8045C26C(RpBuildMesh *mesh) {
    if (mesh->meshTriangles != NULL) {
        RwFree(mesh->meshTriangles);
        mesh->meshTriangles = NULL;
    }
    RwFreeListFree(buildMeshFreeList, mesh);
    return 1;
}

s32 fn_8045C2D4(RpMeshHeader *header) {
    if (header->flags != 0 || header->numMeshes != 0 || header->serialNum != 0 ||
        header->totalIndicesInMesh != 0 || header->firstMeshOffset != 0) {
        RwFree(header);
    }
    return 1;
}

/* _rpBuildMeshAddTriangle: appends one triangle, growing the buffer by one record when full. */
RpBuildMesh *fn_8045C340(RpBuildMesh *mesh, RpMaterial *material, u16 vert1, u16 vert2, u16 vert3,
                         u16 matIndex, u16 textureIndex, u16 rasterIndex, u16 pipelineIndex) {
    if (mesh->numTriangles >= mesh->triangleBufferSize) {
        RpBuildMeshTriangle *triangles;
        u32 size = (mesh->numTriangles + 1) * sizeof(RpBuildMeshTriangle);
        if (mesh->numTriangles != 0) {
            triangles = (RpBuildMeshTriangle *)RwRealloc(mesh->meshTriangles, size, 0x1030502);
        } else {
            triangles = (RpBuildMeshTriangle *)RwMalloc(size, 0x1030502);
        }
        if (triangles == NULL) {
            RWERROR((E_RW_NOMEM, size));
            return NULL;
        }
        mesh->meshTriangles = triangles;
        mesh->triangleBufferSize = mesh->numTriangles + 1;
    }
    mesh->meshTriangles[mesh->numTriangles].material = material;
    mesh->meshTriangles[mesh->numTriangles].vertIndex[0] = vert1;
    mesh->meshTriangles[mesh->numTriangles].vertIndex[1] = vert2;
    mesh->meshTriangles[mesh->numTriangles].vertIndex[2] = vert3;
    mesh->meshTriangles[mesh->numTriangles].matIndex = matIndex;
    mesh->meshTriangles[mesh->numTriangles].textureIndex = textureIndex;
    mesh->meshTriangles[mesh->numTriangles].rasterIndex = rasterIndex;
    mesh->meshTriangles[mesh->numTriangles].pipelineIndex = pipelineIndex;
    mesh->numTriangles++;
    return mesh;
}

u32 *fn_8045C4E4(u32 *object, u32 (*callback)(u8 *, u32 *, u32), u32 argument) {
    s32 count = *(u16 *)((u8 *)object + 4);
    u8 *entry = (u8 *)object + object[3];
    entry += 16;
    while (count-- != 0) {
        if (callback(entry, object, argument) == 0) {
            return object;
        }
        entry += 12;
    }
    return object;
}
RwStream *fn_8047D458(RwStream *stream, const void *data, u32 size); /* RwStreamWriteInt32 */
RwStream *fn_8047D868(RwStream *stream, void *data, u32 size);       /* RwStreamReadInt32 */
s32 fn_8045B8EC(const RpMaterialList *matList, const RpMaterial *material);
RpMaterial *fn_8045B79C(const RpMaterialList *matList, s32 index);

/* Mesh chunk records. */
typedef struct RpMeshHeaderChunk {
    u32 flags;
    u32 numMeshes;
    u32 totalIndicesInMesh;
} RpMeshHeaderChunk;

typedef struct RpMeshChunk {
    u32 numIndices;
    s32 matIndex;
} RpMeshChunk;

/* The object that owns the meshes is a geometry (type 8) or a world sector's world (type 7);
 * both keep their flags at 0x08, and a native one has no indices in the stream. */
typedef struct RpMeshOwner {
    RwObject object;
    u32 flags;
} RpMeshOwner;

#define RPMESHOWNERHASINDICES(type, owner)                                 \
    (((type) == 8 && !((owner)->flags & rpGEOMETRYNATIVE)) ||              \
     ((type) == 7 && !((owner)->flags & rpGEOMETRYNATIVE)))

/* _rpMeshWrite: the header, then per mesh its size and material index and its indices as words. */
RwStream *fn_8045C56C(const RpMeshHeader *meshHeader, const RpMeshOwner *owner, RwStream *stream,
                      const RpMaterialList *matList) {
    u32 buffer[256];
    RpMeshHeaderChunk headerChunk;
    RpMeshChunk meshChunk;
    u32 count;
    u32 type;
    u32 numMeshes;
    const RpMesh *mesh;
    u32 remaining;
    const u16 *indices;

    headerChunk.flags = meshHeader->flags;
    headerChunk.numMeshes = meshHeader->numMeshes;
    headerChunk.totalIndicesInMesh = meshHeader->totalIndicesInMesh;
    if (fn_8047D458(stream, &headerChunk, sizeof(headerChunk)) == NULL) {
        return NULL;
    }
    numMeshes = meshHeader->numMeshes;
    mesh = (const RpMesh *)(meshHeader + 1);
    type = owner->object.type;
    while (numMeshes--) {
        meshChunk.numIndices = mesh->numIndices;
        meshChunk.matIndex = fn_8045B8EC(matList, mesh->material);
        if (meshChunk.matIndex < 0) {
            meshChunk.matIndex = 0;
        }
        if (fn_8047D458(stream, &meshChunk, sizeof(meshChunk)) == NULL) {
            return NULL;
        }
        if (RPMESHOWNERHASINDICES(type, owner)) {
            remaining = mesh->numIndices;
            indices = mesh->indices;
            while (remaining != 0) {
                u32 i;
                count = 256;
                if (remaining < 256) {
                    count = remaining;
                }
                for (i = 0; i < count; i++) {
                    buffer[i] = *indices++;
                }
                if (fn_8047D458(stream, buffer, count * sizeof(u32)) == NULL) {
                    return NULL;
                }
                remaining -= count;
            }
        }
        mesh++;
    }
    return stream;
}

/* The allocation helper that starts this file (fn_8045BF30), which the reader expands. */
static inline RpMeshHeader *RpMeshHeaderAlloc(u32 size) {
    return (RpMeshHeader *)RwMalloc(size, 0x30502);
}

/* _rpMeshRead: the inverse of the writer; the header gets the module's next serial number. */
RpMeshHeader *fn_8045C770(RwStream *stream, const RpMeshOwner *owner, const RpMaterialList *matList) {
    u32 buffer[256];
    RpMeshHeaderChunk headerChunk;
    RpMeshChunk meshChunk;
    RpMeshHeader *meshHeader;
    u32 type;
    RpMesh *mesh;
    u16 *indices;
    u32 count;
    u32 numMeshes;
    u32 remaining;
    u32 *source;
    u32 size;

    if (fn_8047D868(stream, &headerChunk, sizeof(headerChunk)) == NULL) {
        return NULL;
    }
    size = sizeof(RpMeshHeader) + headerChunk.numMeshes * (sizeof(RpMesh) + sizeof(u32));
    if (RPMESHOWNERHASINDICES(owner->object.type, owner)) {
        size += headerChunk.totalIndicesInMesh * sizeof(u16);
    }
    meshHeader = RpMeshHeaderAlloc(size);
    if (meshHeader != NULL) {
        mesh = (RpMesh *)(meshHeader + 1);
        indices = (u16 *)(mesh + headerChunk.numMeshes);
        type = owner->object.type;
        meshHeader->flags = headerChunk.flags;
        meshHeader->numMeshes = headerChunk.numMeshes;
        meshHeader->serialNum = RWPLUGINGLOBAL(rpMeshGlobals, lbl_805F14F0).nextSerialNum;
        meshHeader->totalIndicesInMesh = headerChunk.totalIndicesInMesh;
        meshHeader->firstMeshOffset = 0;
        RWPLUGINGLOBAL(rpMeshGlobals, lbl_805F14F0).nextSerialNum++;
        numMeshes = meshHeader->numMeshes;
        while (numMeshes--) {
            if (fn_8047D868(stream, &meshChunk, sizeof(meshChunk)) == NULL) {
                return NULL;
            }
            mesh->numIndices = meshChunk.numIndices;
            mesh->material = fn_8045B79C(matList, meshChunk.matIndex);
            mesh->indices = indices;
            if (RPMESHOWNERHASINDICES(type, owner)) {
                remaining = mesh->numIndices;
                while (remaining != 0) {
                    source = buffer;
                    count = 256;
                    if (remaining < 256) {
                        count = remaining;
                    }
                    if (fn_8047D868(stream, source, count * sizeof(u32)) == NULL) {
                        return NULL;
                    }
                    remaining -= count;
                    while (count != 0) {
                        *indices++ = *source++;
                        count--;
                    }
                }
            }
            mesh++;
        }
    }
    return meshHeader;
}

u32 fn_8045C9C8(u32 *object, u8 *entry) {
    if (entry[0] == 8 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    if (entry[0] == 7 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    goto variable_size;
fixed_size:
    return *(u16 *)((u8 *)object + 4) * 8 + 12;
variable_size:
    {
        u32 count = *(u16 *)((u8 *)object + 4);
        u32 result = count * 8;
        result += object[2] * 4;
        result += 12;
        return result;
    }
}

s16 fn_8045CA20(void) {
    return RWPLUGINGLOBAL(rpMeshGlobals, lbl_805F14F0).nextSerialNum++;
}
