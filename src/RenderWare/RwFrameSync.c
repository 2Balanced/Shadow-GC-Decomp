/* RenderWare core: frame hierarchy synchronisation (0x8048ADE0-0x8048B6AC).
 * The recursive walks that bring a dirty hierarchy's LTMs and attached objects up to date, the
 * root step, _rwFrameSyncDirty (the dirty frame list), and the LTM-only walk and root step that
 * RwFrameGetLTM uses. Retail inlines each recursive walk into itself three, five and six levels
 * deep (#pragma inline_depth per function). No data.
 * Boundary: image resampling ends at 0x8048ADE0 with its own constant pool; the texture module
 * starts at 0x8048B6AC with the mipmap name callback its module open installs.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwMatrix *fn_8047EBB8(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b);

void fn_8048B208(RwFrame *frame);

#define FrameParent(frame) ((RwFrame *)(frame)->object.parent)

/* Each object attached to the frame (camera, light, atomic) syncs to it. */
#define FRAMESYNCOBJECTS(frame)                                                                    \
    do {                                                                                           \
        if ((frame)->objectList.link.next != &(frame)->objectList.link) {                          \
            RwLLLink *cur_ = (frame)->objectList.link.next;                                        \
            RwLLLink *end_ = &(frame)->objectList.link;                                            \
                                                                                                   \
            while (cur_ != end_) {                                                                 \
                RwObjectHasFrame *object_ = (RwObjectHasFrame *)((u8 *)cur_ - 8);                  \
                object_->sync(object_);                                                            \
                cur_ = cur_->next;                                                                 \
            }                                                                                      \
        }                                                                                          \
    } while (0)

/* The children of a frame whose LTM changed: new LTMs where the hierarchy flags ask for one,
 * then the attached objects (retail inlines the walk three levels deep). */
#pragma inline_depth(3)
void fn_8048ADE0(RwFrame *frame, s32 flags) {
    while (frame) {
        s32 accumFlags = flags | frame->object.privateFlags;

        if (accumFlags & 4) {
            fn_8047EBB8(&frame->ltm, &frame->modelling, &FrameParent(frame)->ltm);
        }
        FRAMESYNCOBJECTS(frame);
        frame->object.privateFlags &= ~0xC;
        fn_8048ADE0(frame->child, accumFlags);
        frame = frame->next;
    }
}

/* The children of a frame whose objects only need their sync (five levels deep). */
#pragma inline_depth(5)
void fn_8048AFF0(RwFrame *frame) {
    while (frame) {
        FRAMESYNCOBJECTS(frame);
        frame->object.privateFlags &= ~0x8;
        fn_8048AFF0(frame->child);
        frame = frame->next;
    }
}

/* One dirty hierarchy from its root. */
void fn_8048B208(RwFrame *frame) {
    s32 ltmFlags;
    s32 flags;

    flags = frame->object.privateFlags;
    if (flags & 1) {
        ltmFlags = flags & 4;
        if (ltmFlags) {
            frame->ltm = frame->modelling;
        }
        FRAMESYNCOBJECTS(frame);
        fn_8048ADE0(frame->child, ltmFlags);
    } else {
        FRAMESYNCOBJECTS(frame);
        fn_8048AFF0(frame->child);
    }
    frame->object.privateFlags = flags & 0xF0;
}

/* _rwFrameSyncDirty: every hierarchy on the dirty list, then the list is emptied. */
RwBool fn_8048B354(void) {
    RwLLLink *cur = RwEngineInstance->dirtyFrameList.link.next;
    RwLLLink *end = &RwEngineInstance->dirtyFrameList.link;

    while (cur != end) {
        fn_8048B208((RwFrame *)((u8 *)cur - 8));
        cur = cur->next;
    }
    RwEngineInstance->dirtyFrameList.link.next = &RwEngineInstance->dirtyFrameList.link;
    RwEngineInstance->dirtyFrameList.link.prev = &RwEngineInstance->dirtyFrameList.link;
    return TRUE;
}

/* The LTMs below a frame, without the objects (six levels deep). */
#pragma inline_depth(6)
void fn_8048B3C0(RwFrame *frame, s32 flags) {
    while (frame) {
        s32 accumFlags = flags | frame->object.privateFlags;

        if (accumFlags & 4) {
            fn_8047EBB8(&frame->ltm, &frame->modelling, &FrameParent(frame)->ltm);
            frame->object.privateFlags &= ~0x4;
        }
        fn_8048B3C0(frame->child, accumFlags);
        frame = frame->next;
    }
}

#pragma inline_depth(5)

/* _rwFrameSyncHierarchyLTM: the LTMs of a hierarchy from its root. */
void fn_8048B5D8(RwFrame *frame) {
    u8 flags = frame->object.privateFlags;

    if (flags & 4) {
        frame->ltm = frame->modelling;
    }
    fn_8048B3C0(frame->child, flags);
    frame->object.privateFlags = flags & ~5;
}
