/* Small wrappers that many units call by their fn_ names (file and function meanings inferred):
 * two around a setting of the skeleton's main unit (0x805E39C8), and RenderWare's free, calloc and
 * malloc through the engine instance's memory functions, which add their own amounts to the hint.
 * No data places the unit: it is split off on its own between the rotation helpers
 * (Game/UnkAxisRotation_80049778.cpp, whose .sdata2 ends it) and the message display that starts
 * at 0x80049BC0. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <dolphin/types.h>

/* The engine instance's memory functions (RwMemoryFunctions). */
struct UnkRwMemoryFunctions {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *p);
    void *(*rwrealloc)(void *p, u32 size, u32 hint);
    void *(*rwcalloc)(u32 count, u32 size, u32 hint);
};

/* The engine instance (RwEngineInstance; see Game/LandManager.cpp). */
struct RwGlobals {
    /* 0x000 */ u8 field_0x0[0x108];
    /* 0x108 */ UnkRwMemoryFunctions memoryFuncs;
};
extern RwGlobals *lbl_805F15B4;

extern "C" {
/* Set the value at 0x805E39C8: to 0x20, and to value. */
void fn_80051AB4(void);
void fn_80051AC0(int value);
}

extern "C" void *fn_80049B90(u32 size, u32 hint) {
    return lbl_805F15B4->memoryFuncs.rwmalloc(size, hint + 0x32);
}

extern "C" void *fn_80049B60(u32 count, u32 size, u32 hint) {
    return lbl_805F15B4->memoryFuncs.rwcalloc(count, size, hint + 0x28);
}

extern "C" void fn_80049B34(void *p) {
    lbl_805F15B4->memoryFuncs.rwfree(p);
}

extern "C" void fn_80049B14(int value) {
    fn_80051AC0(value);
}

extern "C" void fn_80049AF4(void) {
    fn_80051AB4();
}
