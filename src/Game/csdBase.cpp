/* csdBase.cpp (file name inferred from csdBase.h, the header whose class operator new calls the
 * allocator, see Game/CsdRCPtr.h): the Chao CSD library's memory allocator, which the game
 * installs. The functions keep their placeholder names: game units call them by these names. */
#include <dolphin/types.h>

#include "Game/CsdRCPtr.h"

#pragma pool_data off

/* The installed allocator (pool_data on: the library reaches its statics from a base register; see
 * Game/csdMatrix.cpp). */
#pragma pool_data on
static UnkCsdAllocator_803605AC *s_allocator;
#pragma pool_data off

/* Frees p. */
extern "C" void fn_80360528(void *p) {
    s_allocator->FreeDebug(p);
}

/* Allocates size bytes, recording the source file and line. */
extern "C" void *fn_80360564(u32 size, const char *file, int line) {
    return s_allocator->AllocDebug(size, file, line);
}

/* The installed allocator. */
extern "C" UnkCsdAllocator_803605AC *fn_803605AC(void) {
    return s_allocator;
}

/* Installs allocator unless it is zero; returns the one installed before (or the current one). */
extern "C" UnkCsdAllocator_803605AC *fn_803605BC(UnkCsdAllocator_803605AC *allocator) {
    if (allocator != 0) {
        UnkCsdAllocator_803605AC *old = s_allocator;
        s_allocator = allocator;
        return old;
    }
    return s_allocator;
}
