/* RenderWare 3.7 world plugin: world sectors, 0x8045F2A0-0x8045F9E8 (wave 41 re-cut of the
 * size-cut Game/RwCpp23 tail and Game/RwCpp24 head). The file owns the world-sector plugin registry
 * (.data 0x8056D880; the binary-world reader and the world file use it too, so it keeps its address
 * name) and the sector module record (.sbss 0x805F14F8).
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_8045F2A0 / fn_8045F2B4  sector module open / close (plugin 0x50A)
 *   fn_8045F2C8  RpWorldSectorRender (the world's sector render callback)
 *   fn_8045F2F8  RpWorldSectorRegisterPlugin   fn_8045F338  RpWorldSectorRegisterPluginStream
 *   fn_8045F374  search callback (stops at the wanted object)
 *   fn_8045F390  default sector render callback: render the sector, then its atomics in the frustum
 *   fn_8045F458  build a world sector's mesh   fn_8045F6E0 / fn_8045F7D0  destroy a sector tree
 * Built with cflags_rw_c (configure.py): C, functions in definition order, no automatic inlining.
 * Retail calls RpWorldSectorRender from the default render callback and the recursive sector-tree
 * destroy from itself out of line; with automatic inlining (cflags_rw_r3) GC/2.7 expands both.
 * Matching. fn_8045F458 is the world-sector twin of RpGeometryUnlock (written the same way); its
 * polygon counter is declared first, as retail gives it the highest of the locals' saved registers.
 * The other functions are still untyped drafts. */
#include <dolphin/types.h>
void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
#include "Game/rw/r3.h"

/* The untyped functions below still index the engine block by offset. */
#define RWENGINEBYTES ((u8 *)lbl_805F15B4)

/* The plugin registry of world sectors (0x88 bytes each). */
RwPluginRegistry lbl_8056D880 = { 0x88, 0x88, 0, 0, NULL, NULL };

/* The module record (plugin 0x50A). */
static RwModuleInfo sectorModule;

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32); /* RwPluginRegistryAddPlugin */
void fn_804815F0(u8 *, u32, u32, u32, u32);      /* RwPluginRegistryAddPluginStream */
u32 fn_804566D0(u32 *atomic);                     /* RpAtomicGetWorldBoundingSphere */
s32 fn_804856C8(u32 camera, u32 sphere);          /* RwCameraFrustumTestSphere */

void *fn_8045F2A0(void *instance) {
    sectorModule.numInstances++;
    return instance;
}

void *fn_8045F2B4(void *instance) {
    sectorModule.numInstances--;
    return instance;
}

u32 fn_8045F2C8(u32 argument) {
    u32 *table = (u32 *)RWENGINEBYTES;
    table = (u32 *)table[1];
    return ((u32 (*)(u32))table[0x1A])(argument);
}

void fn_8045F2F8(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8((u8 *)&lbl_8056D880, first, second, third, fourth, fifth);
}

void fn_8045F338(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0((u8 *)&lbl_8056D880, first, second, third, fourth);
}

/* For-all callback of a search: stops (returns NULL) at the wanted item and flags it as found. */
void *fn_8045F374(void *item, u32 *search) {
    if ((void *)search[0] == item) {
        search[1] = 1;
        return NULL;
    }
    return item;
}

u32 *fn_8045F390(u32 *object) {
    if (fn_8045F2C8((u32)object) == 0) {
        goto done;
    }
    {
    u32 *node;
    u32 *sentinel = object + 14;
    u32 *current = (u32 *)object[14];
    while (current != sentinel) {
        node = (u32 *)current[2];
        if ((((u8 *)node)[2] & 4) != 0) {
            if (*(u16 *)((u8 *)node + 96) != *(u16 *)(RWENGINEBYTES + 8)) {
                u32 result = fn_804566D0(node);
                if (fn_804856C8(*(u32 *)RWENGINEBYTES, result) != 0) {
                    ((void (*)(u32 *))*(u32 *)((u8 *)node + 72))(node);
                }
                *(u16 *)((u8 *)node + 96) = *(u16 *)(RWENGINEBYTES + 8);
            }
        }
        current = (u32 *)current[0];
    }
done:
    return object;
}}

RpMeshHeader *fn_8045F1FC(RpBuildMesh *, s32); /* _rpMeshOptimise */
void fn_8045C26C(RpBuildMesh *);     /* _rpBuildMeshDestroy */
RpBuildMesh *fn_8045C340(RpBuildMesh *, RpMaterial *, u16, u16, u16, u16, u16, u16, u16);

/* Builds a world sector's mesh from its polygons: like RpGeometryUnlock, each polygon goes into the
 * build mesh with the indices of its texture, raster and pipeline among those seen so far. */
RpWorld *fn_8045F458(RpWorld *world, RpBuildMesh *buildMesh, RpWorldSector *sector,
                     RpMaterial **materials) {
    s32 i;
    RwTexture **textures;
    RwRaster **rasters;
    RxPipeline **pipelines;
    u16 numTextures = 0;
    u16 numRasters = 0;
    u16 numPipelines = 0;
    RpMeshHeader *mesh;

    if ((u32)world->matList.numMaterials != 0) {
        u32 size = world->matList.numMaterials * sizeof(void *);

        textures = (RwTexture **)RwMalloc(size, 0x10006);
        rasters = (RwRaster **)RwMalloc(size, 0x10507);
        pipelines = (RxPipeline **)RwMalloc(size, 0x10507);

        for (i = 0; i < sector->numPolygons; i++) {
            RpTriangle *polygon = &((RpTriangle *)sector->polygons)[i];
            RwRaster *raster = NULL;
            RpMaterial *material = materials[polygon->matIndex];
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

            fn_8045C340(buildMesh, material, polygon->vertIndex[0], polygon->vertIndex[1],
                        polygon->vertIndex[2], polygon->matIndex, textureIndex, rasterIndex,
                        pipelineIndex);
        }
        RwFree(textures);
        RwFree(rasters);
        RwFree(pipelines);
    }

    if (world->flags & 1) {
        mesh = fn_8045F1FC(buildMesh, 1);
    } else {
        mesh = fn_8045F1FC(buildMesh, 0);
    }
    if (mesh != NULL) {
        sector->mesh = mesh;
    } else {
        fn_8045C26C(buildMesh);
        return NULL;
    }
    return world;
}

void fn_8048071C(u32 *);

void fn_8045F6E0(u32 *object) {
    switch (*(s32 *)object) {
    case -1:
        if (object[13] != 0) {
            fn_8048071C((u32 *)object[13]);
        }
        {
            u32 *sentinel = object + 14;
            u32 *current = (u32 *)object[14];
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460DF0(item);
            }
        }
        {
            u32 *sentinel = object + 16;
            u32 *current = (u32 *)object[16];
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460D80(item);
            }
        }
        fn_80482144((u8 *)&lbl_8056D880, object);
        if (object[30] != 0) {
            fn_8045C2D4((u32 *)object[30]);
            object[30] = 0;
        }
        break;
    case -2:
        break;
    default:
        if (object[2] != 0) {
            fn_8045F6E0((u32 *)object[2]);
        }
        if (object[3] != 0) {
            fn_8045F6E0((u32 *)object[3]);
        }
        break;
    }
}

void fn_8045F7D0(void *input) {
    switch (*(s32 *)input) {
    case -1:
        {
        u32 *object = (u32 *)input;
        if (object[13] != 0) {
            fn_8048071C((u32 *)object[13]);
        }
        {
            u32 *sentinel = object + 14;
            u32 *current = (u32 *)object[14];
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460DF0(item);
            }
        }
        {
            u32 *current = (u32 *)object[16];
            u32 *sentinel = object + 16;
            while (current != sentinel) {
                u32 *item = current;
                current = (u32 *)current[0];
                fn_80460D80(item);
            }
        }
        fn_80482144((u8 *)&lbl_8056D880, object);
        if (object[2] != 0) {
            ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))((u32 *)object[2]);
            object[2] = 0;
        }
        if (object[3] != 0) {
            ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))((u32 *)object[3]);
            object[3] = 0;
        }
        if (object[12] != 0) {
            ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))((u32 *)object[12]);
            object[12] = 0;
        }
        if (object[1] != 0) {
            ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))((u32 *)object[1]);
            object[1] = 0;
        }
        {
            u32 *field = object;
            u32 zero = 0;
            { s32 i; for (i = 0; i < 8; i++) {
                if (field[4] != 0) {
                    ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))((u32 *)field[4]);
                    field[4] = zero;
                }
                field++;
            }}
        }
        if (object[30] != 0) {
            fn_8045C2D4((u32 *)object[30]);
            object[30] = 0;
        }
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(object);
        break;
    case -2:
        ((void (*)(void *))*(u32 *)(RWENGINEBYTES + 0x10C))(input);
        break;
    default: {
        u32 *target = (u32 *)input;
        if (target[2] != 0) {
            fn_8045F7D0((u32 *)target[2]);
        }
        target[2] = 0;
        if (target[3] != 0) {
            fn_8045F7D0((u32 *)target[3]);
        }
        target[3] = 0;
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))(target);
        break;
    }
    }}
}
