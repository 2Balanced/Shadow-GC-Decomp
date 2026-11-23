/* RenderWare core: the raster module (0x80489B2C-0x8048A178).
 * RwRasterGetOffset, the device's raster requests, RwRasterUnlock, RwRasterDestroy,
 * RwRasterRegisterPlugin, RwRasterGetNumLevels, RwRasterShowRaster, RwRasterSubRaster,
 * RwRasterCreate, RwRasterLock, module close and open (the context stack starts with an empty
 * raster). The file owns the module globals (.sbss), the free list parameters (.sdata), the free
 * list (.bss) and the raster plugin registry (.data). C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

void fn_804807CC(void);
RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);

static RwModuleInfo rasterModule;
static RwPluginRegistry rasterTKList = {sizeof(RwRaster), sizeof(RwRaster), 0, 0, NULL, NULL};
static s32 rasterFreeListBlockSize = 0x80;
static s32 rasterFreeListPreallocBlocks = 1;
static RwFreeList rasterFreeList;

#define RASTERGLOBAL(field) (RWGLOBALAT(rwRasterGlobals, rasterModule.globalsOffset).field)

/* RwRasterGetOffset. */
RwRaster *fn_80489B2C(RwRaster *raster, s16 *xOffset, s16 *yOffset) {
    *xOffset = raster->nOffsetX;
    *yOffset = raster->nOffsetY;
    return raster;
}

/* A raster request through the device's standard function 16. */
RwRaster *fn_80489B40(RwRaster *raster) {
    RwEngineInstance->stdFunc[16](NULL, raster, 0);
    return raster;
}

/* RwRasterUnlock: the device's standard function 24, then the lock bits go. */
RwRaster *fn_80489B88(RwRaster *raster) {
    RwEngineInstance->stdFunc[24](NULL, raster, 0);
    raster->privateFlags &= ~0x18;
    return raster;
}

/* RwRasterDestroy. */
RwBool fn_80489BDC(RwRaster *raster) {
    fn_80482144(&rasterTKList, raster);
    RwEngineInstance->stdFunc[5](NULL, raster, 0);
    RwEngineInstance->memoryFree(RASTERGLOBAL(rasterFreeList), raster);
    return TRUE;
}

/* RwRasterRegisterPlugin. */
s32 fn_80489C54(s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return fn_80481DC8(&rasterTKList, size, pluginID, constructCB, destructCB, copyCB);
}

/* A raster query through the device's standard function 23. */
s32 fn_80489C94(RwRaster *raster, s32 value) {
    s32 result;

    if (RwEngineInstance->stdFunc[23](&result, raster, value)) {
        return result;
    }
    return 0;
}

/* RwRasterGetNumLevels. */
s32 fn_80489CE4(RwRaster *raster) {
    s32 numLevels;

    if (!((raster->cFormat << 8) & 0x8000)) {
        return 1;
    }
    if (RwEngineInstance->stdFunc[28](&numLevels, raster, 0)) {
        return numLevels;
    }
    return -1;
}

/* RwRasterShowRaster. */
RwRaster *fn_80489D44(RwRaster *raster, void *device, u32 flags) {
    RwStandardFunc showRaster = RwEngineInstance->stdFunc[20];

    fn_804807CC();
    if (showRaster(raster, device, flags)) {
        return raster;
    }
    return NULL;
}

/* RwRasterSubRaster. */
RwRaster *fn_80489DC4(RwRaster *raster, RwRaster *parent, RwRect *rect) {
    if (!(raster->cFlags & 0x80)) {
        return NULL;
    }
    raster->width = rect->w;
    raster->height = rect->h;
    raster->nOffsetX = parent->nOffsetX + (s16)rect->x;
    raster->nOffsetY = parent->nOffsetY + (s16)rect->y;
    if (RwEngineInstance->stdFunc[12](raster, parent, 0)) {
        raster->parent = parent->parent;
        return raster;
    }
    return NULL;
}

/* RwRasterCreate. */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags) {
    RwRaster *raster =
        (RwRaster *)RwEngineInstance->memoryAlloc(RASTERGLOBAL(rasterFreeList), 0x30407);

    if (raster) {
        RwStandardFunc createRaster = RwEngineInstance->stdFunc[4];

        raster->cFlags = raster->privateFlags = 0;
        raster->width = width;
        raster->height = height;
        raster->nOffsetX = 0;
        raster->nOffsetY = 0;
        raster->depth = depth;
        raster->parent = raster;
        raster->cpPixels = NULL;
        raster->palette = NULL;
        if (!createRaster(NULL, raster, flags)) {
            RwEngineInstance->memoryFree(RASTERGLOBAL(rasterFreeList), raster);
            return NULL;
        }
        fn_8048209C(&rasterTKList, raster);
        return raster;
    }
    return NULL;
}

/* RwRasterLock. */
u8 *fn_80489F70(RwRaster *raster, u8 level, s32 lockMode) {
    u8 *pixels;

    if (RwEngineInstance->stdFunc[15](&pixels, raster, lockMode + (level << 8))) {
        return pixels;
    }
    return NULL;
}

/* _rwRasterClose. */
void *fn_80489FC0(void *instance, s32 offset, s32 size) {
    if (RASTERGLOBAL(rasterFreeList)) {
        fn_8047FCDC(RASTERGLOBAL(rasterFreeList));
        RASTERGLOBAL(rasterFreeList) = NULL;
    }
    rasterModule.numInstances--;
    return instance;
}

/* _rwRasterOpen: the context stack starts with an empty raster. */
void *fn_8048A02C(void *instance, s32 offset, s32 size) {
    rasterModule.globalsOffset = offset;
    memset(&RASTERGLOBAL(dummyRaster), 0, sizeof(RwRaster));
    RASTERGLOBAL(dummyRaster).width = 0;
    RASTERGLOBAL(dummyRaster).height = 0;
    RASTERGLOBAL(dummyRaster).depth = 0;
    RASTERGLOBAL(dummyRaster).cFlags = 0x80;
    RASTERGLOBAL(dummyRaster).cpPixels = NULL;
    RASTERGLOBAL(dummyRaster).palette = NULL;
    RASTERGLOBAL(dummyRaster).cType = 0;
    RASTERGLOBAL(rasterSP) = 0;
    RASTERGLOBAL(rasterStack)[0] = &RASTERGLOBAL(dummyRaster);
    RASTERGLOBAL(rasterFreeList) = fn_8047FC04(rasterTKList.sizeOfStruct, rasterFreeListBlockSize, 4,
                                               rasterFreeListPreallocBlocks, &rasterFreeList, 0x40407);
    if (!RASTERGLOBAL(rasterFreeList)) {
        return NULL;
    }
    rasterModule.numInstances++;
    return instance;
}
