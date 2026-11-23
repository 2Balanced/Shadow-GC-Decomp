/* RenderWare core, 0x80485A58..0x804875AF (a size cut, not a source file):
 *   0x80485A58  RwCameraCreate, the last function of the camera module
 *   0x80485BCC  the engine: allocation without free lists, device requests, plugin registration,
 *               version, video mode queries, RwEngineStop/Start/Close/Open/Term/Init
 *   0x80486888  frames: module open/close, hierarchy root, clone, create, destroy, the dirty
 *               frame list, LTM, root, add/remove child, for-all-children
 * Functions keep their fn_ names because game code calls them by those; each comment gives the
 * public RenderWare function the behaviour corresponds to. Compiled as C with cflags_rw_r6
 * (configure.py): auto inlining without deferral, five passes deep.
 * NonMatching: fn_80485C3C (_rwDeviceSystemRequest), 2 rows: retail loads the 1 that request 13
 * stores separately from the TRUE it returns; GC/2.7 shares one register (nine spellings in
 * probes/Game_RwCpp48.py). */
#pragma cplusplus off
#include <dolphin/types.h>
#include "Game/rw/r6.h"

extern RwModuleInfo lbl_805F15A8;     /* camera module */
extern s32 lbl_805F15B0;              /* engines opened and not closed */
extern RwModuleInfo lbl_805F15B8;     /* frame module */
extern RwGlobals lbl_805E2B48;        /* the engine globals before RwEngineOpen and after close */
extern RwPluginRegistry lbl_8056DF70; /* camera plugins */
extern RwPluginRegistry lbl_8056DFB0; /* engine plugins */
extern char lbl_8056DFC8[];           /* the name of the one subsystem */
extern RwPluginRegistry lbl_8056DFE8; /* frame plugins */
extern s32 lbl_805EDEE0;
extern s32 lbl_805EDEE4;
extern u8 lbl_805E2C78[];
extern const f32 lbl_805FB2E0; /* 1.0f */
extern const f32 lbl_805FB2E4; /* 0.0001f */
extern const f32 lbl_805FB2E8; /* 0.5f */
extern const f32 lbl_805FB2F4; /* 0.0f */
extern const f32 lbl_805FB2F8; /* 0.05f */
extern const f32 lbl_805FB2FC; /* 10.0f */
extern const f32 lbl_805FB300; /* 5.0f */
extern const f32 lbl_805FB308; /* 1.0f */
extern const f32 lbl_805FB30C; /* 0.0f */

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        void *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
void fn_8047FA28(RwBool enable);
void *fn_8047FDB0(RwFreeList *list, u32 hint);
RwFreeList *fn_8047FF50(RwFreeList *list, void *entry);
RwBool fn_80480274(const RwMemoryFunctions *memFuncs);
void fn_8048038C(void);
RwBool fn_80481B98(void);
RwBool fn_80481C58(void);
s32 fn_80481D98(const RwPluginRegistry *reg, u32 pluginID);
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_804821B0(const RwPluginRegistry *reg, void *dst, const void *src);
RwBool fn_80482E20(void);
void fn_80482F60(void);
RwObjectHasFrame *fn_8048528C(RwObjectHasFrame *object);
RwCamera *fn_804852F0(RwCamera *camera);
RwCamera *fn_80485354(RwCamera *camera);
RwBool fn_80489830(f32 gamma);
void fn_8048B5D8(RwFrame *root);
s32 fn_80493E64(void);
RwDevice *fn_804950A0(void);
s32 fn_8049B140(void);

/* The module open and close functions the engine registers as plugins. */
void *fn_8047DB10(void *instance, s32 offset, s32 size);
void *fn_8047DB24(void *instance, s32 offset, s32 size);
void *fn_8047DB38(void *instance, s32 offset, s32 size);
void *fn_8047DB70(void *instance, s32 offset, s32 size);
void *fn_8047E828(void *instance, s32 offset, s32 size);
void *fn_8047E88C(void *instance, s32 offset, s32 size);
void *fn_80480C6C(void *instance, s32 offset, s32 size);
void *fn_80480CF8(void *instance, s32 offset, s32 size);
void *fn_8048053C(void *instance, s32 offset, s32 size);
void *fn_80480690(void *instance, s32 offset, s32 size);
void *fn_80482548(void *instance, s32 offset, s32 size);
void *fn_804825EC(void *instance, s32 offset, s32 size);
void *fn_80482BC4(void *instance, s32 offset, s32 size);
void *fn_80482C50(void *instance, s32 offset, s32 size);
void *fn_804853C0(void *instance, s32 offset, s32 size);
void *fn_80485424(void *instance, s32 offset, s32 size);
void *fn_80487914(void *instance, s32 offset, s32 size);
void *fn_80487BB0(void *instance, s32 offset, s32 size);
void *fn_80489FC0(void *instance, s32 offset, s32 size);
void *fn_8048A02C(void *instance, s32 offset, s32 size);
void *fn_8048D300(void *instance, s32 offset, s32 size);
void *fn_8048D4DC(void *instance, s32 offset, s32 size);
void *fn_8049AF94(void *instance, s32 offset, s32 size);
void *fn_8049AFF8(void *instance, s32 offset, s32 size);
void *fn_8049B0CC(void *instance, s32 offset, s32 size);
void *fn_8049B110(void *instance, s32 offset, s32 size);

void *fn_80486888(void *instance, s32 offset, s32 size);
void *fn_8048692C(void *instance, s32 offset, s32 size);
void fn_80486990(RwFrame *frame, RwFrame *root);
RwBool fn_80487208(RwFrame *frame);
RwFrame *fn_80487448(RwFrame *child);

#define RWCAMERAGLOBAL (RWGLOBALAT(RwFreeList *, lbl_805F15A8.globalsOffset))
#define RWFRAMEGLOBAL (RWGLOBALAT(RwFreeList *, lbl_805F15B8.globalsOffset))
#define FrameParent(frame) ((RwFrame *)(frame)->object.parent)

static inline void CameraUpdateZShiftAndScale(RwCamera *camera) {
    f32 zNear = RwEngineInstance->dOpenDevice.zBufferNear;
    f32 zFar = RwEngineInstance->dOpenDevice.zBufferFar;
    f32 nearPlane;
    f32 farPlane;
    f32 margin;
    f32 zScale;

    switch (camera->projectionType) {
    case 2:
        farPlane = camera->farPlane;
        nearPlane = camera->nearPlane;
        break;
    case 1:
    default:
        farPlane = lbl_805FB2E0 / camera->farPlane;
        nearPlane = lbl_805FB2E0 / camera->nearPlane;
        break;
    }
    margin = lbl_805FB2E4 * (zFar - zNear);
    zFar -= margin;
    zNear += margin;
    zScale = (zFar - zNear) / (farPlane - nearPlane);
    camera->zScale = zScale;
    camera->zShift = lbl_805FB2E8 * ((zFar + zNear) - zScale * (farPlane + nearPlane));
}

/* RwCameraCreate: a perspective camera with a unit view window, clip planes 0.05 and 10. */
RwCamera *fn_80485A58(void) {
    RwCamera *camera = (RwCamera *)RwEngineInstance->memoryAlloc(RWCAMERAGLOBAL, 0x30005);

    if (!camera) {
        return NULL;
    }
    camera->object.object.type = 4;
    camera->object.object.subType = 0;
    camera->object.object.flags = 0;
    camera->object.object.privateFlags = 0;
    camera->object.object.parent = NULL;
    camera->object.sync = fn_8048528C;
    camera->beginUpdate = fn_80485354;
    camera->endUpdate = fn_804852F0;
    camera->viewWindow.x = camera->viewWindow.y = lbl_805FB2E0;
    camera->recipViewWindow.x = camera->recipViewWindow.y = lbl_805FB2E0;
    camera->viewOffset.x = camera->viewOffset.y = lbl_805FB2F4;
    camera->nearPlane = lbl_805FB2F8;
    camera->farPlane = lbl_805FB2FC;
    camera->fogPlane = lbl_805FB300;
    camera->frameBuffer = NULL;
    camera->zBuffer = NULL;
    camera->projectionType = 1;
    CameraUpdateZShiftAndScale(camera);
    camera->viewMatrix.flags = 0;
    fn_8048209C(&lbl_8056DF70, camera);
    return camera;
}

/* The engine's allocation when it runs without free lists: straight from the memory functions. */
void *fn_80485BCC(RwFreeList *list, u32 hint) {
    return RwEngineInstance->memoryFuncs.rwmalloc(list->entrySize, hint);
}

RwFreeList *fn_80485BFC(RwFreeList *list, void *entry) {
    RwEngineInstance->memoryFuncs.rwfree(entry);
    return list;
}

/* _rwDeviceSystemRequest: the device's system function, with defaults for the subsystem and
 * start/stop requests a device does not implement. */
static inline RwBool DeviceSystemRequest(RwDevice *device, s32 request, void *out, void *inOut,
                                         s32 number) {
    RwBool result = device->fpSystem(request, out, inOut, number);

    if (!result) {
        switch (request) {
        case 17:
        case 18:
            result = TRUE;
            break;
        case 13:
            *(s32 *)out = 1;
            result = TRUE;
            break;
        case 14:
            result = (number == 0);
            if (result) {
                RwEngineInstance->stringFuncs.vecStrcpy((char *)out, lbl_8056DFC8);
            }
            break;
        case 15:
            *(s32 *)out = 0;
            result = TRUE;
            break;
        case 16:
            result = (number == 0);
            break;
        }
    }
    if (!result) {
        RWERROR((0x18, request));
    }
    return result;
}

RwBool fn_80485C3C(RwDevice *device, s32 request, void *out, void *inOut, s32 number) {
    return DeviceSystemRequest(device, request, out, inOut, number);
}

/* The number of engines opened and not yet closed. */
s32 fn_80485D64(void) {
    return lbl_805F15B0;
}

/* RwEngineGetVersion. */
u32 fn_80485D6C(void) {
    return 0x37002;
}

/* RwEngineRegisterPlugin. */
s32 fn_80485D78(s32 size, u32 pluginID, RwPluginObjectConstructor initCB,
                RwPluginObjectDestructor termCB) {
    return fn_80481DC8(&lbl_8056DFB0, size, pluginID, initCB, termCB, NULL);
}

/* RwEngineGetPluginOffset. */
s32 fn_80485DB8(u32 pluginID) {
    return fn_80481D98(&lbl_8056DFB0, pluginID);
}

/* RwEngineGetVideoModeInfo. */
void *fn_80485DE4(void *modeInfo, s32 modeIndex) {
    if (!DeviceSystemRequest(&RwEngineInstance->dOpenDevice, 6, modeInfo, NULL, modeIndex)) {
        modeInfo = NULL;
    }
    return modeInfo;
}

/* RwEngineGetCurrentVideoMode. */
s32 fn_80485E70(void) {
    s32 mode;

    if (DeviceSystemRequest(&RwEngineInstance->dOpenDevice, 10, &mode, NULL, 0)) {
        return mode;
    }
    return -1;
}

/* RwEngineStop. */
RwBool fn_80485EF4(void) {
    RwDevice *device = &RwEngineInstance->dOpenDevice;
    RwBool result;

    DeviceSystemRequest(device, 18, NULL, NULL, 0);
    fn_80482144(&lbl_8056DFB0, RwEngineInstance);
    result = DeviceSystemRequest(device, 3, NULL, NULL, 0);
    if (result) {
        RwEngineInstance->engineStatus = 2;
    }
    return result;
}

/* RwEngineStart. */
RwBool fn_80485FE0(void) {
    RwDevice *device = &RwEngineInstance->dOpenDevice;

    if (DeviceSystemRequest(device, 2, NULL, NULL, 0)) {
        if (fn_8048209C(&lbl_8056DFB0, RwEngineInstance)) {
            fn_80489830(RwEngineInstance->dOpenDevice.gammaCorrection);
            DeviceSystemRequest(device, 17, NULL, NULL, 0);
            RwEngineInstance->engineStatus = 3;
            return TRUE;
        }
        DeviceSystemRequest(device, 3, NULL, NULL, 0);
    }
    return FALSE;
}

/* RwEngineClose: the globals go back to the static block. */
RwBool fn_80486138(void) {
    RwBool result = DeviceSystemRequest(&RwEngineInstance->dOpenDevice, 1, NULL, NULL, 0);

    if (result) {
        RwGlobals *globals = RwEngineInstance;

        RwEngineInstance = &lbl_805E2B48;
        memcpy(RwEngineInstance, globals, sizeof(RwGlobals));
        RwEngineInstance->memoryFuncs.rwfree(globals);
        lbl_805F15B0--;
        RwEngineInstance->engineStatus = 1;
    }
    return result;
}

/* The device part of RwEngineOpen: the globals move to a block of the size the engine plugins
 * need, the device registers and opens, and its standard functions are fetched. */
static inline RwBool EngineOpenDevice(RwDevice *device, void *initParams) {
    RwGlobals *globals;

    globals = RwEngineInstance = (RwGlobals *)RwEngineInstance->memoryFuncs.rwmalloc(
        lbl_8056DFB0.sizeOfStruct, 0x40000);
    if (globals) {
        memcpy(RwEngineInstance, &lbl_805E2B48, sizeof(RwGlobals));
        DeviceSystemRequest(device, 4, &RwEngineInstance->dOpenDevice, &RwEngineInstance->memoryFuncs,
                            0);
        if (DeviceSystemRequest(device, 0, NULL, initParams, 0)) {
            DeviceSystemRequest(device, 11, RwEngineInstance->stdFunc, NULL, 29);
            lbl_805F15B0++;
            return TRUE;
        }
        RwEngineInstance = &lbl_805E2B48;
        memcpy(RwEngineInstance, globals, sizeof(RwGlobals));
        RwEngineInstance->memoryFuncs.rwfree(globals);
        return FALSE;
    }
    RwEngineInstance = &lbl_805E2B48;
    RWERROR((0x80000013, lbl_8056DFB0.sizeOfStruct));
    return FALSE;
}

/* RwEngineOpen. */
RwBool fn_80486204(void *initParams) {
    RwBool result;

    if (!RwEngineInstance) {
        RwEngineInstance = &lbl_805E2B48;
    }
    result = (RwEngineInstance->engineStatus == 1);
    if (result) {
        result = (initParams != NULL);
        if (result) {
            RwDevice *device = fn_804950A0();

            result = (device != NULL);
            if (result) {
                result = EngineOpenDevice(device, initParams);
                if (result) {
                    RwEngineInstance->engineStatus = 2;
                }
            }
        } else {
            RWERROR((0x80000016));
        }
    } else {
        RWERROR((0x80000001));
    }
    return result;
}

/* RwEngineTerm. */
RwBool fn_804864AC(void) {
    RwBool result = (lbl_805F15B0 == 0);

    if (result) {
        fn_80481C58();
        fn_8048038C();
        RwEngineInstance->engineStatus = 0;
    }
    return result;
}

/* The core modules, registered as engine plugins (their open and close functions). */
static inline RwBool EngineRegisterModules(void) {
    s32 status;

    status = fn_80481DC8(&lbl_8056DFB0, 8, 0x40F, fn_8047DB38, fn_8047DB70, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 8, 0x401, fn_804825EC, fn_80482548, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 0, 0x40D, fn_8047DB10, fn_8047DB24, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 0x18, 0x402, fn_8047E88C, fn_8047E828, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 4, 0x403, fn_80486888, fn_8048692C, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 4, 0x404, fn_80480C6C, fn_80480CF8, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 4, 0x405, fn_80485424, fn_804853C0, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 0x220, 0x406, fn_80487914, fn_80487BB0, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 0x64, 0x407, fn_8048A02C, fn_80489FC0, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 0x34, 0x408, fn_8048D4DC, fn_8048D300, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 0x60, 0x409, fn_8049B0CC, fn_8049B110, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 4, 0x412, fn_80482BC4, fn_80482C50, NULL);
    status |= fn_8049B140();
    status |= fn_80481DC8(&lbl_8056DFB0, 0x74, 0x40A, fn_8049AFF8, fn_8049AF94, NULL);
    status |= fn_80481DC8(&lbl_8056DFB0, 0x28, 0x40B, fn_8048053C, fn_80480690, NULL);
    return (status >= 0);
}

/* RwEngineInit: the memory functions and free list mode, then the core modules as engine
 * plugins. */
RwBool fn_804864F8(const RwMemoryFunctions *memFuncs, u32 initFlags, u32 resArenaSize) {
    RwBool result = FALSE;

    RwEngineInstance = &lbl_805E2B48;
    if (initFlags & 1) {
        RwEngineInstance->memoryAlloc = fn_80485BCC;
        RwEngineInstance->memoryFree = fn_80485BFC;
        fn_8047FA28(FALSE);
    } else {
        RwEngineInstance->memoryAlloc = fn_8047FDB0;
        RwEngineInstance->memoryFree = fn_8047FF50;
        fn_8047FA28(TRUE);
    }
    RwEngineInstance->resArenaInitSize = resArenaSize;
    if (RwEngineInstance->engineStatus == 0) {
        result = fn_80482E20();
        if (result) {
            result = fn_80480274(memFuncs);
            if (result) {
                result = fn_80481B98();
                if (result) {
                    result = EngineRegisterModules();
                    if (result) {
                        result = fn_80493E64();
                        if (result) {
                            RwEngineInstance->engineStatus = 1;
                            return result;
                        }
                    }
                    fn_80481C58();
                }
                fn_8048038C();
            }
            fn_80482F60();
        }
    }
    return result;
}

/* _rwFrameOpen. */
void *fn_80486888(void *instance, s32 offset, s32 size) {
    lbl_805F15B8.globalsOffset = offset;
    RWFRAMEGLOBAL = fn_8047FC04(lbl_8056DFE8.sizeOfStruct, lbl_805EDEE0, 4, lbl_805EDEE4,
                                lbl_805E2C78, 0x4000E);
    if (!RWFRAMEGLOBAL) {
        return NULL;
    }
    RwEngineInstance->dirtyFrameList.link.next = &RwEngineInstance->dirtyFrameList.link;
    RwEngineInstance->dirtyFrameList.link.prev = &RwEngineInstance->dirtyFrameList.link;
    lbl_805F15B8.numInstances++;
    return instance;
}

/* _rwFrameClose. */
void *fn_8048692C(void *instance, s32 offset, s32 size) {
    if (RWFRAMEGLOBAL) {
        fn_8047FCDC(RWFRAMEGLOBAL);
        RWFRAMEGLOBAL = NULL;
    }
    lbl_805F15B8.numInstances--;
    return instance;
}

/* Sets the root of a frame and of everything below it. Retail inlines the recursion eight levels
 * deep here but only five in fn_80486FAC, so the depth is raised for this function alone. */
#pragma inline_depth(8)
void fn_80486990(RwFrame *frame, RwFrame *root) {
    frame->root = root;
    frame = frame->child;
    while (frame) {
        fn_80486990(frame, root);
        frame = frame->next;
    }
}
#pragma inline_depth(5)

/* Clones a frame and its children; each original's root points at its clone afterwards. */
RwFrame *fn_80486AA4(RwFrame *frame, RwFrame *root) {
    RwFrame *newFrame = (RwFrame *)RwEngineInstance->memoryAlloc(RWFRAMEGLOBAL, 0x3000E);
    RwFrame *child;

    if (!newFrame) {
        return NULL;
    }
    if (!root) {
        root = newFrame;
    }
    newFrame->object.type = frame->object.type;
    newFrame->object.subType = frame->object.subType;
    newFrame->object.flags = frame->object.flags;
    newFrame->object.privateFlags = frame->object.privateFlags;
    newFrame->object.parent = NULL;
    newFrame->objectList.link.next = &newFrame->objectList.link;
    newFrame->objectList.link.prev = &newFrame->objectList.link;
    newFrame->object.parent = NULL;
    newFrame->modelling = frame->modelling;
    newFrame->child = NULL;
    newFrame->next = NULL;
    newFrame->root = root;
    frame->root = newFrame;
    for (child = frame->child; child; child = child->next) {
        RwFrame *childClone = fn_80486AA4(child, root);

        if (!childClone) {
            fn_80487208(newFrame);
            return NULL;
        }
        childClone->next = newFrame->child;
        newFrame->child = childClone;
        childClone->object.parent = newFrame;
    }
    fn_8048209C(&lbl_8056DFE8, newFrame);
    fn_804821B0(&lbl_8056DFE8, newFrame, frame);
    return newFrame;
}

static inline RwFrame *FrameUpdateObjects(RwFrame *frame) {
    u8 oldFlags = frame->root->object.privateFlags;

    if (!(oldFlags & 3)) {
        frame->root->inDirtyList.next = RwEngineInstance->dirtyFrameList.link.next;
        frame->root->inDirtyList.prev = &RwEngineInstance->dirtyFrameList.link;
        RwEngineInstance->dirtyFrameList.link.next->prev = &frame->root->inDirtyList;
        RwEngineInstance->dirtyFrameList.link.next = &frame->root->inDirtyList;
    }
    frame->root->object.privateFlags = oldFlags | 3;
    frame->object.privateFlags |= 0xC;
    return frame;
}

/* RwFrameUpdateObjects. */
RwFrame *fn_8048722C(RwFrame *frame);

/* RwFrameClone. */
RwFrame *fn_80486C5C(RwFrame *frame) {
    RwFrame *clone = fn_80486AA4(frame, NULL);

    if (clone) {
        clone->object.privateFlags &= ~3;
        fn_8048722C(clone);
    }
    return clone;
}

/* Sets the hierarchy root of a frame from its parent. */
RwFrame *fn_80486CA4(RwFrame *frame) {
    if (FrameParent(frame)) {
        fn_80486990(frame, FrameParent(frame)->root);
    } else {
        fn_80486990(frame, frame);
    }
    return frame;
}

/* RwFrameDirty. */
RwBool fn_80486CF0(const RwFrame *frame) {
    return frame->root->object.privateFlags & 3;
}

static inline void MatrixSetIdentity(RwMatrix *matrix) {
    matrix->flags = 3;
    matrix->right.x = matrix->up.y = matrix->at.z = lbl_805FB308;
    matrix->right.y = matrix->right.z = matrix->up.x = lbl_805FB30C;
    matrix->up.z = matrix->at.x = matrix->at.y = lbl_805FB30C;
    matrix->pos.x = matrix->pos.y = matrix->pos.z = lbl_805FB30C;
    matrix->flags |= 0x20003;
}

/* RwFrameCreate. */
RwFrame *fn_80486D00(void) {
    RwFrame *frame = (RwFrame *)RwEngineInstance->memoryAlloc(RWFRAMEGLOBAL, 0x3000E);

    if (!frame) {
        return NULL;
    }
    frame->object.type = 0;
    frame->object.subType = 0;
    frame->object.flags = 0;
    frame->object.privateFlags = 0;
    frame->object.parent = NULL;
    frame->objectList.link.next = &frame->objectList.link;
    frame->objectList.link.prev = &frame->objectList.link;
    MatrixSetIdentity(&frame->modelling);
    MatrixSetIdentity(&frame->ltm);
    frame->child = NULL;
    frame->next = NULL;
    frame->root = frame;
    fn_8048209C(&lbl_8056DFE8, frame);
    return frame;
}

/* rwLinkListRemoveLLLink on the frame's dirty list link. */
#define FrameRemoveFromDirtyList(frame)                                                            \
    do {                                                                                           \
        (frame)->inDirtyList.prev->next = (frame)->inDirtyList.next;                               \
        (frame)->inDirtyList.next->prev = (frame)->inDirtyList.prev;                               \
    } while (0)

/* RwFrameDestroy: the children are left without a parent. */
RwBool fn_80486E2C(RwFrame *frame) {
    RwFrame *child;

    fn_80482144(&lbl_8056DFE8, frame);
    if (FrameParent(frame)) {
        fn_80487448(frame);
    }
    if (frame->object.privateFlags & 3) {
        FrameRemoveFromDirtyList(frame);
    }
    for (child = frame->child; child; child = child->next) {
        child->object.parent = NULL;
    }
    RwEngineInstance->memoryFree(RWFRAMEGLOBAL, frame);
    return TRUE;
}

/* The plugin data and dirty list part of destroying a frame. */
void fn_80486ED8(RwFrame *frame) {
    fn_80482144(&lbl_8056DFE8, frame);
    if (frame->object.privateFlags & 3) {
        FrameRemoveFromDirtyList(frame);
    }
}

void fn_80486F34(RwFrame *frame) {
    fn_80486ED8(frame);
    RwEngineInstance->memoryFree(RWFRAMEGLOBAL, frame);
}

/* Destroys a frame and everything below it. */
void fn_80486FAC(RwFrame *frame) {
    if (frame) {
        RwFrame *child = frame->child;

        while (child) {
            RwFrame *next = child->next;

            fn_80486FAC(child);
            child = next;
        }
        fn_80486F34(frame);
    }
}

/* RwFrameDestroyHierarchy. */
RwBool fn_80487208(RwFrame *frame) {
    fn_80486FAC(frame);
    return TRUE;
}

RwFrame *fn_8048722C(RwFrame *frame) {
    return FrameUpdateObjects(frame);
}

/* RwFrameGetLTM: syncs the hierarchy first when its LTMs are dirty. */
RwMatrix *RwFrameGetLTM(RwFrame *frame) {
    if (frame->root->object.privateFlags & 1) {
        fn_8048B5D8(frame->root);
    }
    return &frame->ltm;
}

/* RwFrameGetRoot. */
RwFrame *fn_804872D8(const RwFrame *frame) {
    return frame->root;
}

/* Adds a child without marking anything dirty. */
RwFrame *fn_804872E0(RwFrame *parent, RwFrame *child) {
    if (FrameParent(child)) {
        fn_80487448(child);
    }
    child->next = parent->child;
    parent->child = child;
    child->object.parent = parent;
    fn_80486990(child, parent->root);
    return parent;
}

/* RwFrameAddChild. */
RwFrame *fn_80487348(RwFrame *parent, RwFrame *child) {
    if (FrameParent(child)) {
        fn_80487448(child);
    }
    child->next = parent->child;
    parent->child = child;
    child->object.parent = parent;
    fn_80486990(child, parent->root);
    if (child->object.privateFlags & 3) {
        FrameRemoveFromDirtyList(child);
        child->object.privateFlags &= ~3;
    }
    FrameUpdateObjects(child);
    return parent;
}

/* RwFrameRemoveChild. */
RwFrame *fn_80487448(RwFrame *child) {
    RwFrame *curFrame = FrameParent(child)->child;

    if (curFrame == child) {
        FrameParent(child)->child = child->next;
    } else {
        while (curFrame->next != child) {
            curFrame = curFrame->next;
        }
        curFrame->next = child->next;
    }
    child->object.parent = NULL;
    child->next = NULL;
    fn_80486990(child, child);
    FrameUpdateObjects(child);
    return child;
}

/* RwFrameForAllChildren. */
RwFrame *fn_8048752C(RwFrame *frame, RwFrame *(*callBack)(RwFrame *frame, void *data),
                     void *data) {
    RwFrame *child = frame->child;

    while (child) {
        RwFrame *next = child->next;

        if (!callBack(child, data)) {
            return frame;
        }
        child = next;
    }
    return frame;
}
