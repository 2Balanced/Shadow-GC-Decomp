/* RenderWare core: the frame module (0x80486888-0x80487914).
 * Module open and close, the hierarchy root walk, RwFrameClone, RwFrameDirty, RwFrameCreate,
 * RwFrameDestroy and the hierarchy destructor, RwFrameUpdateObjects, RwFrameGetLTM, RwFrameGetRoot,
 * adding and removing children, RwFrameForAllChildren, translate, scale, transform, rotate,
 * RwFrameSetIdentity and RwFrameRegisterPlugin. The file owns the module globals (.sbss), the free
 * list parameters (.sdata), the free list (.bss), the frame plugin registry (.data) and its
 * constant pool (.sdata2 0x805FB308-0x805FB310).
 * Functions come out in definition order and only helpers defined before a caller are inlined
 * (RwFrameClone calls RwFrameUpdateObjects, defined after it, out of line, while RwFrameAddChild
 * and RwFrameRemoveChild have it inline); five passes give the hierarchy destructor (fn_80486FAC)
 * its five inlined levels. C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwMatrix *fn_8047EFC8(RwMatrix *matrix, const RwV3d *axis, f32 angle, s32 combine);
RwMatrix *fn_8047F398(RwMatrix *matrix, const RwV3d *scale, s32 combine);
RwMatrix *fn_8047F59C(RwMatrix *matrix, const RwV3d *translation, s32 combine);
RwMatrix *fn_8047F72C(RwMatrix *matrix, const RwMatrix *transform, s32 combine);
RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_804821B0(const RwPluginRegistry *reg, void *dst, const void *src);
void fn_8048B5D8(RwFrame *root);

void fn_80486990(RwFrame *frame, RwFrame *root);
RwBool fn_80487208(RwFrame *frame);
RwFrame *fn_80487448(RwFrame *child);

static RwModuleInfo frameModule;
RwPluginRegistry frameTKList = {sizeof(RwFrame), sizeof(RwFrame), 0, 0, NULL, NULL};
static s32 frameFreeListBlockSize = 0x32;
static s32 frameFreeListPreallocBlocks = 1;
static RwFreeList frameFreeList;

#define RWFRAMEGLOBAL (RWGLOBALAT(RwFreeList *, frameModule.globalsOffset))
#define FrameParent(frame) ((RwFrame *)(frame)->object.parent)

/* _rwFrameOpen. */
void *fn_80486888(void *instance, s32 offset, s32 size) {
    frameModule.globalsOffset = offset;
    RWFRAMEGLOBAL = fn_8047FC04(frameTKList.sizeOfStruct, frameFreeListBlockSize, 4, frameFreeListPreallocBlocks,
                                &frameFreeList, 0x4000E);
    if (!RWFRAMEGLOBAL) {
        return NULL;
    }
    RwEngineInstance->dirtyFrameList.link.next = &RwEngineInstance->dirtyFrameList.link;
    RwEngineInstance->dirtyFrameList.link.prev = &RwEngineInstance->dirtyFrameList.link;
    frameModule.numInstances++;
    return instance;
}

/* _rwFrameClose. */
void *fn_8048692C(void *instance, s32 offset, s32 size) {
    if (RWFRAMEGLOBAL) {
        fn_8047FCDC(RWFRAMEGLOBAL);
        RWFRAMEGLOBAL = NULL;
    }
    frameModule.numInstances--;
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
    fn_8048209C(&frameTKList, newFrame);
    fn_804821B0(&frameTKList, newFrame, frame);
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
    matrix->right.x = matrix->up.y = matrix->at.z = 1.0f;
    matrix->right.y = matrix->right.z = matrix->up.x = 0.0f;
    matrix->up.z = matrix->at.x = matrix->at.y = 0.0f;
    matrix->pos.x = matrix->pos.y = matrix->pos.z = 0.0f;
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
    fn_8048209C(&frameTKList, frame);
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

    fn_80482144(&frameTKList, frame);
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
    fn_80482144(&frameTKList, frame);
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

/* RwFrameTranslate. */
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, s32 combine) {
    fn_8047F59C(&frame->modelling, translation, combine);
    FrameUpdateObjects(frame);
    return frame;
}

/* RwFrameScale. */
RwFrame *fn_8048764C(RwFrame *frame, const RwV3d *scale, s32 combine) {
    fn_8047F398(&frame->modelling, scale, combine);
    FrameUpdateObjects(frame);
    return frame;
}

/* RwFrameTransform. */
RwFrame *fn_804876E8(RwFrame *frame, const RwMatrix *transform, s32 combine) {
    fn_8047F72C(&frame->modelling, transform, combine);
    FrameUpdateObjects(frame);
    return frame;
}

/* RwFrameRotate. */
RwFrame *fn_80487784(RwFrame *frame, const RwV3d *axis, f32 angle, s32 combine) {
    fn_8047EFC8(&frame->modelling, axis, angle, combine);
    FrameUpdateObjects(frame);
    return frame;
}

/* RwFrameSetIdentity. */
RwFrame *fn_80487820(RwFrame *frame) {
    frame->modelling.right.x = frame->modelling.up.y = frame->modelling.at.z = 1.0f;
    frame->modelling.right.y = frame->modelling.right.z = frame->modelling.up.x = 0.0f;
    frame->modelling.up.z = frame->modelling.at.x = frame->modelling.at.y = 0.0f;
    frame->modelling.pos.x = frame->modelling.pos.y = frame->modelling.pos.z = 0.0f;
    frame->modelling.flags |= 0x20003;
    FrameUpdateObjects(frame);
    return frame;
}

/* RwFrameRegisterPlugin. */
s32 fn_804878D4(s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return fn_80481DC8(&frameTKList, size, pluginID, constructCB, destructCB, copyCB);
}
