/* RenderWare 3.7 world plugin: RpGeometry, 0x80458CF8-0x8045A760 (wave 41 re-cut of the size-cut
 * Game/RwCpp20). The file owns its plugin registry (.data 0x8056D810), its module record (.sbss
 * 0x805F14D0) and its one literal (.sdata2 0x805FB118, 0.0f plus padding). Its first function is the
 * module open behind the clump file's tail (0x80458C28-0x80458CF8, RpAtomicRegisterPlugin and its
 * neighbours, which use the atomic registry at 0x8056D7E0 and belong to the clump file before it).
 *
 * Names are inferred from behaviour and from the public RenderWare API:
 *   fn_80458CF8 / fn_80458D10  geometry module open / close (plugin 0x503 of RpWorldPluginAttach)
 *   fn_80458D24  RpGeometryCreateSpace          fn_80458DB0  RpGeometryAddMorphTargets
 *   fn_8045905C  RpGeometryTriangleSetMaterial  fn_804590E8  RpGeometryForAllMaterials
 *   fn_8045916C  RpGeometryLock                 fn_804591C4  RpGeometryUnlock
 *   fn_80459464  RpGeometryCreate               fn_8045984C  RpGeometryAddRef
 *   fn_8045985C  RpGeometryDestroy              fn_8045994C  RpGeometryRegisterPlugin
 *   fn_8045998C  RpGeometryRegisterPluginStream fn_804599C8  RpGeometryStreamRead
 * Evidence for the file: RpGeometryStreamRead expands RpGeometryDestroy in every failure path (one
 * file, automatic inlining), the module record and registry are used by no other file, and the
 * literal pool of the light file starts on the next 8-byte boundary.
 * Built with cflags_rw_r3 (configure.py): C, functions in definition order.
 * Matching. RpGeometryStreamRead reaches each morph target through RpGeometryGetMorphTarget as an
 * inline function: written as a plain array access, the morph target and the strength-reduced
 * offset trade saved registers (r28/r29). */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

void *memset(void *, int, u32);

s32 fn_804815F0(RwPluginRegistry *, u32, void *, void *, void *);       /* RwPluginRegistryAddPluginStream */
s32 fn_80481DC8(RwPluginRegistry *, s32, u32, void *, void *, void *); /* RwPluginRegistryAddPlugin */
void fn_8048209C(RwPluginRegistry *, void *); /* rwPluginRegistryInitObject */
void fn_80482144(RwPluginRegistry *, void *); /* rwPluginRegistryDeInitObject */
void *fn_804816AC(RwPluginRegistry *, RwStream *, void *); /* rwPluginRegistryReadDataChunks */
void fn_8048071C(RwResEntry *);               /* RwResourcesFreeResEntry */

s32 fn_8047CF50(RwStream *, u32, u32 *, u32 *); /* RwStreamFindChunk */
u32 fn_80480D4C(RwStream *, void *, u32);       /* RwStreamRead */
void *fn_8047D254(void *, u32);                 /* RwMemNative32 */
RwStream *fn_8047D784(RwStream *, void *, u32); /* RwStreamReadReal */

RpMaterialList *fn_8045B788(RpMaterialList *);            /* _rpMaterialListInitialize */
RpMaterialList *fn_8045B6F0(RpMaterialList *);            /* _rpMaterialListDeinitialize */
RpMaterial *fn_8045B79C(RpMaterialList *, s32);           /* _rpMaterialListGetMaterial */
s32 fn_8045B7AC(RpMaterialList *, RpMaterial *);          /* _rpMaterialListAppendMaterial */
s32 fn_8045B8EC(RpMaterialList *, RpMaterial *);          /* _rpMaterialListFindMaterialIndex */
RpMaterialList *fn_8045B924(RwStream *, RpMaterialList *); /* _rpMaterialListStreamRead */

RpBuildMesh *fn_8045C144(s32);       /* _rpBuildMeshCreate */
void fn_8045C26C(RpBuildMesh *);     /* _rpBuildMeshDestroy */
void fn_8045C2D4(RpMeshHeader *);    /* _rpMeshDestroy */
RpBuildMesh *fn_8045C340(RpBuildMesh *, RpMaterial *, u16, u16, u16, u16, u16, u16, u16);
RpMeshHeader *fn_8045F1FC(RpBuildMesh *, s32); /* _rpMeshOptimise */

RpGeometry *fn_80459464(s32 numVertices, s32 numTriangles, u32 format);
RpGeometry *fn_804591C4(RpGeometry *geometry);
s32 fn_8045985C(RpGeometry *geometry);

/* The plugin registry of geometries (0x60 bytes each). */
RwPluginRegistry geometryTKList = { sizeof(RpGeometry), sizeof(RpGeometry), 0, 0, NULL, NULL };

/* The module record (plugin 0x503). */
static RwModuleInfo geometryModule;

void *fn_80458CF8(void *instance, s32 offset) {
    geometryModule.globalsOffset = offset;
    geometryModule.numInstances++;
    return instance;
}

void *fn_80458D10(void *instance) {
    geometryModule.numInstances--;
    return instance;
}

RpGeometry *fn_80458D24(f32 radius) {
    RpGeometry *geometry = fn_80459464(0, 0, 0);
    if (geometry != NULL) {
        RpMorphTarget *morphTarget = geometry->morphTarget;
        morphTarget->boundingSphere.center.x = 0.0f;
        morphTarget->boundingSphere.center.y = 0.0f;
        morphTarget->boundingSphere.center.z = 0.0f;
        morphTarget->boundingSphere.radius = radius;
    }
    if (fn_804591C4(geometry) == NULL) {
        fn_8045985C(geometry);
        return NULL;
    }
    return geometry;
}

s32 fn_80458DB0(RpGeometry *geometry, s32 count) {
    RpMorphTarget *morphTargets;
    u32 bytes;
    u32 size;
    s32 i;
    u8 *data;

    if (geometry->flags & rpGEOMETRYNATIVE) {
        bytes = sizeof(RpMorphTarget);
    } else {
        bytes = sizeof(RpMorphTarget) + geometry->numVertices * sizeof(RwV3d);
        if (geometry->flags & rpGEOMETRYNORMALS) {
            bytes += geometry->numVertices * sizeof(RwV3d);
        }
    }
    size = bytes * (geometry->numMorphTargets + count);

    if (geometry->morphTarget != NULL) {
        u8 *source;
        u8 *dest;
        u32 remaining;
        u32 oldSize;
        morphTargets = (RpMorphTarget *)RwRealloc(geometry->morphTarget, size, 0x3000F);
        if (morphTargets == NULL) {
            RWERROR((E_RW_NOMEM, size));
            return -1;
        }
        /* The vertex data behind the old records moves up behind the new ones. */
        oldSize = bytes * geometry->numMorphTargets;
        source = (u8 *)morphTargets + oldSize - 1;
        dest = source + count * sizeof(RpMorphTarget);
        remaining = oldSize - geometry->numMorphTargets * sizeof(RpMorphTarget);
        while (remaining--) {
            *dest-- = *source--;
        }
    } else {
        morphTargets = (RpMorphTarget *)RwMalloc(size, 0x3000F);
        if (morphTargets == NULL) {
            RWERROR((E_RW_NOMEM, size));
            return -1;
        }
    }

    geometry->numMorphTargets += count;
    geometry->morphTarget = morphTargets;
    data = (u8 *)&morphTargets[geometry->numMorphTargets];
    for (i = 0; i < geometry->numMorphTargets; i++) {
        RpMorphTarget *morphTarget = &geometry->morphTarget[i];
        morphTarget->verts = NULL;
        morphTarget->normals = NULL;
        if (!(geometry->flags & rpGEOMETRYNATIVE)) {
            if (geometry->numVertices != 0) {
                morphTarget->verts = (RwV3d *)data;
                data += geometry->numVertices * sizeof(RwV3d);
                if (geometry->flags & rpGEOMETRYNORMALS) {
                    morphTarget->normals = (RwV3d *)data;
                    data += geometry->numVertices * sizeof(RwV3d);
                }
            }
        }
    }

    for (i = geometry->numMorphTargets - count; i < geometry->numMorphTargets; i++) {
        RpMorphTarget *morphTarget = &geometry->morphTarget[i];
        morphTarget->boundingSphere.center.x = 0.0f;
        morphTarget->boundingSphere.center.y = 0.0f;
        morphTarget->boundingSphere.center.z = 0.0f;
        morphTarget->boundingSphere.radius = 0.0f;
        morphTarget->parentGeom = geometry;
    }
    return geometry->numMorphTargets - count;
}

RpGeometry *fn_8045905C(RpGeometry *geometry, RpTriangle *triangle, RpMaterial *material) {
    if (material != NULL) {
        s32 index = fn_8045B8EC(&geometry->matList, material);
        if (index < 0) {
            index = fn_8045B7AC(&geometry->matList, material);
            if (index < 0) {
                return NULL;
            }
        }
        triangle->matIndex = index;
    } else {
        triangle->matIndex = 0xFFFF;
    }
    return geometry;
}

RpGeometry *fn_804590E8(RpGeometry *geometry, RpMaterial *(*callback)(RpMaterial *, void *),
                        void *data) {
    s32 count = geometry->matList.numMaterials;
    s32 i;
    for (i = 0; i < count; i++) {
        if (callback(geometry->matList.materials[i], data) == NULL) {
            return geometry;
        }
    }
    return geometry;
}

RpGeometry *fn_8045916C(RpGeometry *geometry, s32 lockMode) {
    geometry->lockedSinceLastInst |= lockMode;
    if (lockMode & 1) {
        if (geometry->mesh != NULL) {
            fn_8045C2D4(geometry->mesh);
            geometry->mesh = NULL;
        }
    }
    return geometry;
}

RpGeometry *fn_804591C4(RpGeometry *geometry) {
    RwTexture **textures;
    RwRaster **rasters;
    RxPipeline **pipelines;
    u16 numTextures = 0;
    u16 numRasters = 0;
    u16 numPipelines = 0;

    if (geometry->mesh == NULL) {
        RpBuildMesh *buildMesh = fn_8045C144(geometry->numTriangles);
        RpMeshHeader *mesh;

        if (buildMesh == NULL) {
            goto fail;
        }
        if (geometry->matList.numMaterials > 0) {
            u32 size = geometry->matList.numMaterials * sizeof(void *);
            s32 i;

            textures = (RwTexture **)RwMalloc(size, 0x10006);
            rasters = (RwRaster **)RwMalloc(size, 0x10503);
            pipelines = (RxPipeline **)RwMalloc(size, 0x10503);

            for (i = 0; i < geometry->numTriangles; i++) {
                RpTriangle *triangle = &geometry->triangles[i];
                RwRaster *raster = NULL;
                RpMaterial *material = fn_8045B79C(&geometry->matList, triangle->matIndex);
                RwTexture *texture = material->texture;
                u16 textureIndex;
                u16 rasterIndex;
                u16 pipelineIndex;
                RxPipeline *pipeline;

                for (textureIndex = 0; textureIndex < numTextures; textureIndex++) {
                    if (textures[textureIndex] == texture) {
                        break;
                    }
                }
                if (textureIndex == numTextures) {
                    textures[textureIndex] = texture;
                    numTextures++;
                }

                if (texture != NULL) {
                    raster = *(RwRaster **)texture;
                }
                for (rasterIndex = 0; rasterIndex < numRasters; rasterIndex++) {
                    if (rasters[rasterIndex] == raster) {
                        break;
                    }
                }
                if (rasterIndex == numRasters) {
                    rasters[rasterIndex] = raster;
                    numRasters++;
                }

                pipeline = material->pipeline;
                for (pipelineIndex = 0; pipelineIndex < numPipelines; pipelineIndex++) {
                    if (pipelines[pipelineIndex] == pipeline) {
                        break;
                    }
                }
                if (pipelineIndex == numPipelines) {
                    pipelines[pipelineIndex] = pipeline;
                    numPipelines++;
                }

                fn_8045C340(buildMesh, material, triangle->vertIndex[0], triangle->vertIndex[1],
                            triangle->vertIndex[2], triangle->matIndex, textureIndex, rasterIndex,
                            pipelineIndex);
            }
            RwFree(textures);
            RwFree(rasters);
            RwFree(pipelines);
        }

        if (geometry->flags & rpGEOMETRYTRISTRIP) {
            mesh = fn_8045F1FC(buildMesh, 1);
        } else {
            mesh = fn_8045F1FC(buildMesh, 0);
        }
        if (mesh != NULL) {
            geometry->mesh = mesh;
            return geometry;
        }
        fn_8045C26C(buildMesh);
        return NULL;
    fail:
        return NULL;
    }
    return geometry;
}

RpGeometry *fn_80459464(s32 numVertices, s32 numTriangles, u32 format) {
    u32 numTexCoordSets;
    u32 native;
    u32 flags;
    RpGeometry *geometry;
    u32 size;
    u32 baseFlags;
    u32 texFlag;

    if (numVertices < 0 || numVertices >= 0x10000 || numTriangles < 0) {
        if (numVertices >= 0 && numVertices >= 0x10000) {
            RWERROR((E_RP_GEOMETRY_TOOMANYVERTS));
        }
        return NULL;
    }

    size = geometryTKList.sizeOfStruct;
    baseFlags = format & 0xFF;
    if (format & 0x00FF0000) {
        numTexCoordSets = (format & 0x00FF0000) >> 16;
    } else if (format & rpGEOMETRYTEXTURED2) {
        numTexCoordSets = 2;
    } else {
        numTexCoordSets = (format & rpGEOMETRYTEXTURED) ? 1 : 0;
    }
    baseFlags &= ~(rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2);
    if (numTexCoordSets == 1) {
        texFlag = rpGEOMETRYTEXTURED;
    } else {
        texFlag = (numTexCoordSets > 1) ? rpGEOMETRYTEXTURED2 : 0;
    }
    flags = baseFlags | texFlag;

    native = format & rpGEOMETRYNATIVE;
    if (!native) {
        if (flags & rpGEOMETRYPRELIT) {
            size += numVertices * sizeof(RwRGBA);
        }
        if (numTexCoordSets != 0) {
            size += numTexCoordSets * (numVertices * sizeof(RwTexCoords));
        }
        size += numTriangles * sizeof(RpTriangle);
    }

    geometry = (RpGeometry *)RwMalloc(size, 0x3000F);
    if (geometry == NULL) {
        return NULL;
    }
    if (fn_8045B788(&geometry->matList) == NULL) {
        return NULL;
    }

    geometry->morphTarget = NULL;
    geometry->numMorphTargets = 0;
    geometry->object.type = 8;
    geometry->object.subType = 0;
    geometry->object.flags = 0;
    geometry->object.privateFlags = 0;
    geometry->object.parent = NULL;
    geometry->repEntry = NULL;
    geometry->lockedSinceLastInst = 0;
    geometry->refCount = 1;
    geometry->mesh = NULL;
    geometry->numTexCoordSets = numTexCoordSets;
    memset(geometry->texCoords, 0, sizeof(geometry->texCoords));
    geometry->preLitLum = NULL;
    geometry->triangles = NULL;
    geometry->numTriangles = numTriangles;
    geometry->flags = flags | (format & 0x0F000000);
    geometry->numVertices = numVertices;

    if (!native) {
        u8 *data = (u8 *)geometry + geometryTKList.sizeOfStruct;
        u32 i;

        if ((flags & rpGEOMETRYPRELIT) && numVertices != 0) {
            geometry->preLitLum = (RwRGBA *)data;
            data += numVertices * sizeof(RwRGBA);
        }
        if (numTexCoordSets != 0 && numVertices != 0) {
            for (i = 0; i < numTexCoordSets; i++) {
                geometry->texCoords[i] = (RwTexCoords *)data;
                data += numVertices * sizeof(RwTexCoords);
            }
        }
        if (numTriangles != 0) {
            s32 j;
            geometry->triangles = (RpTriangle *)data;
            for (j = 0; j < numTriangles; j++) {
                geometry->triangles[j].matIndex = 0xFFFF;
            }
        }
    }

    if (fn_80458DB0(geometry, 1) < 0) {
        fn_8045B6F0(&geometry->matList);
        RwFree(geometry);
        return NULL;
    }
    fn_8048209C(&geometryTKList, geometry);
    return geometry;
}

RpGeometry *fn_8045984C(RpGeometry *geometry) {
    geometry->refCount++;
    return geometry;
}

s32 fn_8045985C(RpGeometry *geometry) {
    s32 result = 1;

    if (geometry->refCount - 1 <= 0) {
        if (geometry->repEntry != NULL) {
            fn_8048071C(geometry->repEntry);
        }
        /* RpGeometryLock(geometry, rpGEOMETRYLOCKALL) under a temporary reference. */
        geometry->refCount--;
        geometry->refCount++;
        geometry->lockedSinceLastInst |= rpGEOMETRYLOCKALL;
        if (geometry->mesh != NULL) {
            fn_8045C2D4(geometry->mesh);
            geometry->mesh = NULL;
        }
        fn_80482144(&geometryTKList, geometry);
        if (geometry->morphTarget != NULL) {
            RwFree(geometry->morphTarget);
            geometry->morphTarget = NULL;
        }
        fn_8045B6F0(&geometry->matList);
        geometry->refCount--;
        RwFree(geometry);
        result = 1;
    } else {
        geometry->refCount--;
    }
    return result;
}

s32 fn_8045994C(s32 size, u32 pluginID, void *constructCB, void *destructCB, void *copyCB) {
    return fn_80481DC8(&geometryTKList, size, pluginID, constructCB, destructCB, copyCB);
}

s32 fn_8045998C(u32 pluginID, void *readCB, void *writeCB, void *getSizeCB) {
    return fn_804815F0(&geometryTKList, pluginID, readCB, writeCB, getSizeCB);
}

typedef struct RpGeometryChunkInfo {
    u32 format;
    s32 numTriangles;
    s32 numVertices;
    s32 numMorphTargets;
} RpGeometryChunkInfo;

typedef struct RpMorphTargetChunkInfo {
    RwSphere boundingSphere;
    s32 pointsPresent;
    s32 normalsPresent;
} RpMorphTargetChunkInfo;

static inline RpMorphTarget *RpGeometryGetMorphTarget(const RpGeometry *geometry, s32 index) {
    return &geometry->morphTarget[index];
}

RpGeometry *fn_804599C8(RwStream *stream) {
    RpGeometry *geometry;
    u32 version;
    RpGeometryChunkInfo info;
    RpMorphTargetChunkInfo morphInfo;
    RpMorphTarget *morphTarget;
    u32 size;
    s32 i;
    s32 j;

    if (!fn_8047CF50(stream, 1, NULL, &version)) {
        return NULL;
    }
    if (version < 0x35000 || version > 0x37002) {
        RWERROR((E_RW_BADVERSION));
        return NULL;
    }
    {
        if (fn_80480D4C(stream, &info, sizeof(info)) != sizeof(info)) {
            return NULL;
        }
        fn_8047D254(&info, sizeof(info));
        geometry = fn_80459464(info.numVertices, info.numTriangles, info.format);
        if (geometry == NULL) {
            return NULL;
        }
        if (info.numMorphTargets > 1) {
            if (fn_80458DB0(geometry, info.numMorphTargets - 1) < 0) {
                fn_8045985C(geometry);
                return NULL;
            }
        }

        if (!(geometry->flags & rpGEOMETRYNATIVE)) {
            if (geometry->numVertices != 0) {
                if (info.format & rpGEOMETRYPRELIT) {
                    size = geometry->numVertices * sizeof(RwRGBA);
                    if (fn_80480D4C(stream, geometry->preLitLum, size) != size) {
                        fn_8045985C(geometry);
                        return NULL;
                    }
                }
                if (geometry->numTexCoordSets > 0) {
                    size = geometry->numVertices * sizeof(RwTexCoords);
                    for (i = 0; i < geometry->numTexCoordSets; i++) {
                        if (fn_8047D784(stream, geometry->texCoords[i], size) == NULL) {
                            fn_8045985C(geometry);
                            return NULL;
                        }
                    }
                }
                if (geometry->numTriangles != 0) {
                    RpTriangle *triangle = geometry->triangles;
                    s32 remaining = geometry->numTriangles;
                    u32 size = geometry->numTriangles * sizeof(RpTriangle);

                    if (fn_80480D4C(stream, triangle, size) != size) {
                        fn_8045985C(geometry);
                        return NULL;
                    }
                    fn_8047D254(triangle, size);
                    /* Stream triangles are two words: vertices 0 and 1, vertex 2 and material. */
                    while (remaining--) {
                        u32 first = ((u32 *)triangle)[0];
                        u32 second;
                        triangle->vertIndex[0] = first >> 16;
                        triangle->vertIndex[1] = first;
                        second = ((u32 *)triangle)[1];
                        triangle->vertIndex[2] = second >> 16;
                        triangle->matIndex = second;
                        triangle++;
                    }
                }
            }
        }

        for (j = 0; j < geometry->numMorphTargets; j++) {
            morphTarget = RpGeometryGetMorphTarget(geometry, j);
            if (fn_80480D4C(stream, &morphInfo, sizeof(morphInfo)) != sizeof(morphInfo)) {
                fn_8045985C(geometry);
                return NULL;
            }
            fn_8047D254(&morphInfo, sizeof(morphInfo));
            morphTarget->boundingSphere = morphInfo.boundingSphere;
            if (morphInfo.pointsPresent && morphInfo.normalsPresent) {
                /* Vertices and normals are adjacent: one read. */
                if (fn_8047D784(stream, morphTarget->verts,
                                geometry->numVertices * (2 * sizeof(RwV3d))) == NULL) {
                    fn_8045985C(geometry);
                    return NULL;
                }
            } else {
                if (morphInfo.pointsPresent) {
                    if (fn_8047D784(stream, morphTarget->verts,
                                    geometry->numVertices * sizeof(RwV3d)) == NULL) {
                        fn_8045985C(geometry);
                        return NULL;
                    }
                }
                if (morphInfo.normalsPresent) {
                    if (fn_8047D784(stream, morphTarget->normals,
                                    geometry->numVertices * sizeof(RwV3d)) == NULL) {
                        fn_8045985C(geometry);
                        return NULL;
                    }
                }
            }
        }

        if (!fn_8047CF50(stream, 8, NULL, &version)) {
            return NULL;
        }
        if (version < 0x35000 || version > 0x37002) {
            fn_8045985C(geometry);
            RWERROR((E_RW_BADVERSION));
            return NULL;
        }
        {
            if (fn_8045B924(stream, &geometry->matList) == NULL) {
                fn_8045985C(geometry);
                return NULL;
            }
            if (fn_804816AC(&geometryTKList, stream, geometry) == NULL) {
                fn_8045985C(geometry);
                return NULL;
            }
            if (fn_804591C4(geometry) == NULL) {
                fn_8045985C(geometry);
                return NULL;
            }
        }
    }
    return geometry;
}
