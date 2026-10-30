/* The motion heap (no class with RTTI; file name and all names inferred): a singleton whose
 * constructor sets up the motion library's memory (fn_804110EC takes an allocation, a reallocation
 * and a release callback) and its working heap, and whose destructor tears them down again.
 * fn_800150FC switches the callbacks between heap 1 and heap 2. The object itself has no data;
 * its accessor (fn_80170234) is emitted in Game/PjsEventActionScene.cpp and keeps its address in
 * lbl_805EDFF8. The unit ends where the next unit's .sbss (the bound-sphere flag at 0x805EE000)
 * begins. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <dolphin/types.h>

class UnkMotionHeap_800151EC;

/* The 0xC-byte object the constructor hands to the motion library (layout unknown). */
struct UnkMotionHeapBlock_8040C2B8 {
    /* 0x00 */ u32 field_0x0[0xC / 4];
};

extern "C" {
/* Memory of heap 1 and heap 2: allocation, aligned allocation and release (behaviour inferred). */
void *fn_80049B90(u32 size, int heap);
void *fn_80049B60(u32 size, int align, int heap);
void fn_80049B34(void *data);
/* The motion library: its memory callbacks, its heap block, setup and teardown (behaviour
 * inferred). */
void fn_804110EC(void *(*alloc)(u32, int), void *(*allocAligned)(u32, int),
                 void (*release)(void *));
UnkMotionHeapBlock_8040C2B8 *fn_8040C2B8(UnkMotionHeapBlock_8040C2B8 *block, int size);
void fn_8040C25C(void *block, int flags);
void fn_804116F8(UnkMotionHeapBlock_8040C2B8 *block);
void fn_80420C50(void);
void fn_80420C30(void);
void fn_804107C0(int arg0, int arg1, int arg2, int arg3);
void fn_80410774(void);
void fn_804211D4(int arg0);

void *fn_80015320(u32 size, int align);
void *fn_800152FC(u32 size, int align);
void *fn_800152D8(u32 size, int align);
void *fn_800152B4(u32 size, int align);
void fn_80015294(void *data);
}

/* The heap block the motion library uses (set by the constructor). */
extern void *lbl_805F13A8;

/* The heap's address once created (see fn_80170234 in Game/PjsEventActionScene.cpp). */
UnkMotionHeap_800151EC *lbl_805EDFF8;

extern "C" {

void *fn_80015320(u32 size, int align) {
    return fn_80049B90(size, 1);
}

void *fn_800152FC(u32 size, int align) {
    return fn_80049B60(size, align, 1);
}

void *fn_800152D8(u32 size, int align) {
    return fn_80049B90(size, 2);
}

void *fn_800152B4(u32 size, int align) {
    return fn_80049B60(size, align, 2);
}

void fn_80015294(void *data) {
    fn_80049B34(data);
}

UnkMotionHeap_800151EC *fn_800151EC(UnkMotionHeap_800151EC *heap) {
    fn_804110EC(fn_80015320, fn_800152FC, fn_80015294);
    UnkMotionHeapBlock_8040C2B8 *block = new UnkMotionHeapBlock_8040C2B8;
    if (block != 0) {
        block = fn_8040C2B8(block, 100);
    }
    fn_804116F8(block);
    fn_80420C50();
    fn_804107C0(0x80, 0xA00, 0x100, 0x1400);
    fn_804110EC(fn_800152D8, fn_800152B4, fn_80015294);
    fn_804211D4(1);
    return heap;
}

UnkMotionHeap_800151EC *fn_80015160(UnkMotionHeap_800151EC *heap, short flags) {
    if (heap != 0) {
        fn_804110EC(fn_80015320, fn_800152FC, fn_80015294);
        fn_80410774();
        fn_80420C30();
        fn_8040C25C(lbl_805F13A8, 1);
        fn_804116F8(0);
        fn_804110EC(0, 0, 0);
        if (flags > 0) {
            operator delete(heap);
        }
    }
    return heap;
}

void fn_800150FC(UnkMotionHeap_800151EC *heap, int which) {
    switch (which) {
    case 1:
        fn_804110EC(fn_80015320, fn_800152FC, fn_80015294);
        break;
    default:
        fn_804110EC(fn_800152D8, fn_800152B4, fn_80015294);
        break;
    }
}

}
