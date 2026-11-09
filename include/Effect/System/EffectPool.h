#ifndef EFFECT_SYSTEM_EFFECTPOOL_H
#define EFFECT_SYSTEM_EFFECTPOOL_H

/* A fixed-size block pool of the effect library (no RTTI, so the class name is a placeholder;
 * layout from its constructor, 0x8042A344, and Game/EffectSystem.cpp). The elements make one for
 * their command executors, which allocate from the effect system's current pool
 * (EffectSystem::PoolAlloc). Each block starts with a 0x10-byte node that links it into the free or
 * the used list. Member names are inferred. */
#include "Effect/System/EffectSystem.h"
#include <dolphin/types.h>

namespace Effect {
namespace System {

struct UnkEffectPoolNode_8042A344 {
    /* 0x0 */ UnkEffectPoolNode_8042A344 *m_next;
    /* 0x4 */ UnkEffectPoolNode_8042A344 *m_prev;
    /* 0x8 */ u8 field_0x8[0x10 - 0x8];
};

class UnkEffectPool_8042A344 {
  public:
    /* Makes count blocks of size bytes (0x8042A344), and frees them again (0x8042A2D8). */
    UnkEffectPool_8042A344(u32 size, s32 count);
    ~UnkEffectPool_8042A344();
    /* Pools live in the effect system's memory. */
    static void *operator new(unsigned long size) {
        return fn_8042A5D0()->Allocate(size);
    }
    /* Calls the inline FreeMemory, so it is never inlined: its one copy (0x80421F1C) is emitted
     * with the exception tables of Game/EffectObject.cpp, and the deleting destructor calls it. */
    static void operator delete(void *p) {
        FreeMemory(p);
    }
    /* Hands memory back to the effect system. */
    static void FreeMemory(void *p) {
        fn_8042A5D0()->Free(p);
    }

    /* The memory the blocks take. */
    u32 GetSize() const {
        return m_stride * (m_usedCount + m_freeCount);
    }

    /* 0x00 */ UnkEffectPoolNode_8042A344 *m_freeHead;
    /* 0x04 */ UnkEffectPoolNode_8042A344 *m_freeTail;
    /* 0x08 */ UnkEffectPoolNode_8042A344 *m_usedHead;
    /* 0x0C */ UnkEffectPoolNode_8042A344 *m_usedTail;
    /* 0x10 */ s32 m_freeCount;
    /* 0x14 */ s32 m_usedCount;
    /* 0x18 */ void *m_buffer;
    /* 0x1C */ u32 m_stride; /* the block size with its node, rounded up to 0x10 bytes */
};

} // namespace System
} // namespace Effect

#endif
