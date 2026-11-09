/* The motion library's memory hooks: Game/MotionHeap.cpp installs an allocation, an aligned
 * allocation and a release callback (fn_804110EC); the library allocates and releases through the
 * last two, falling back to the default allocator when none is installed. Other units call them by
 * their fn_ names, so they keep them; the file name is invented and everything here is named from
 * what the code does. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <dolphin/types.h>

typedef void *(*UnkMotionAlloc_804110EC)(u32 size, int arg);
typedef void (*UnkMotionRelease_804110EC)(void *data);

extern "C" {
/* The default allocation and release (behaviour inferred). */
void *fn_803A2C40(u32 size, int arg);
void fn_803A2D54(void *data);

void *fn_80411134(u32 size, int arg);
void fn_804110FC(void *data);
void fn_804110EC(UnkMotionAlloc_804110EC alloc, UnkMotionAlloc_804110EC allocAligned,
                 UnkMotionRelease_804110EC release);
}

UnkMotionAlloc_804110EC lbl_805F1398;
UnkMotionAlloc_804110EC lbl_805F139C;
UnkMotionRelease_804110EC lbl_805F13A0;

void *fn_80411134(u32 size, int arg) {
    if (lbl_805F139C != 0) {
        return lbl_805F139C(size, arg);
    }
    return fn_803A2C40(size, arg);
}

void fn_804110FC(void *data) {
    if (lbl_805F13A0 != 0) {
        lbl_805F13A0(data);
    } else {
        fn_803A2D54(data);
    }
}

void fn_804110EC(UnkMotionAlloc_804110EC alloc, UnkMotionAlloc_804110EC allocAligned,
                 UnkMotionRelease_804110EC release) {
    lbl_805F1398 = alloc;
    lbl_805F139C = allocAligned;
    lbl_805F13A0 = release;
}
