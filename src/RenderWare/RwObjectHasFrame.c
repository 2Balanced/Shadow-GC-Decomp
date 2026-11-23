/* RenderWare core: objects attached to frames (0x8048D7C0-0x8048D85C):
 * _rwObjectHasFrameSetFrame and _rwObjectHasFrameReleaseFrame (cameras, lights and atomics use
 * them). No data. Boundary: the texture module ends at 0x8048D7C0 with its module open; the
 * palette quantiser's helpers start at 0x8048D85C with their own constant pool.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwFrame *fn_8048722C(RwFrame *frame);

/* _rwObjectHasFrameSetFrame: the object leaves its old frame's list and joins the new one's. */
void fn_8048D7C0(void *object, RwFrame *frame) {
    RwObjectHasFrame *ohf = (RwObjectHasFrame *)object;

    if (ohf->object.parent) {
        ohf->lFrame.prev->next = ohf->lFrame.next;
        ohf->lFrame.next->prev = ohf->lFrame.prev;
    }
    ohf->object.parent = frame;
    if (frame) {
        ohf->lFrame.next = frame->objectList.link.next;
        ohf->lFrame.prev = &frame->objectList.link;
        frame->objectList.link.next->prev = &ohf->lFrame;
        frame->objectList.link.next = &ohf->lFrame;
        fn_8048722C(frame);
    }
}

/* _rwObjectHasFrameReleaseFrame. */
void fn_8048D834(void *object) {
    RwObjectHasFrame *ohf = (RwObjectHasFrame *)object;

    if (ohf->object.parent) {
        ohf->lFrame.prev->next = ohf->lFrame.next;
        ohf->lFrame.next->prev = ohf->lFrame.prev;
    }
}
