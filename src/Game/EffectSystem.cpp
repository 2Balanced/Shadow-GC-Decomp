/* Effect::System::EffectSystem (class name from RTTI): the effect library's link to the game's
 * memory. The unit keeps the current effect system (set by Game/GameSystem.cpp through
 * fn_8042A5E0, read through fn_8042A5D0), the block pool the library's command executors live in,
 * and the system's functions that take blocks from and return them to the current pool. File name
 * from the class; member and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define EFFECT_SYSTEM_FULL

#include "Effect/System/EffectPool.h"
#include "Effect/System/EffectSystem.h"

namespace Effect {
namespace System {

static EffectSystem *s_system;

extern "C" {

/* Makes system the current effect system (name inferred). */
void fn_8042A5E0(EffectSystem *system) {
    s_system = system;
}

/* The current effect system (name inferred). */
EffectSystem *fn_8042A5D0(void) {
    return s_system;
}
}

UnkEffectPool_8042A344 *EffectSystem::SetPool(UnkEffectPool_8042A344 *pool) {
    UnkEffectPool_8042A344 *previous = m_pool;
    m_pool = pool;
    return previous;
}

void *EffectSystem::PoolAlloc() {
    UnkEffectPoolNode_8042A344 *node;
    if (m_pool == 0 || (node = m_pool->m_freeHead) == 0) {
        return 0;
    }
    m_pool->m_freeHead = node->m_next;
    if (m_pool->m_freeHead == 0) {
        m_pool->m_freeTail = 0;
    } else {
        m_pool->m_freeHead->m_prev = 0;
    }
    m_pool->m_freeCount--;
    if (m_pool->m_usedTail != 0) {
        m_pool->m_usedTail->m_next = node;
        node->m_prev = m_pool->m_usedTail;
    } else {
        m_pool->m_usedHead = node;
        node->m_prev = 0;
    }
    m_pool->m_usedTail = node;
    node->m_next = 0;
    m_pool->m_usedCount++;
    return node + 1;
}

void EffectSystem::PoolFree(void *p) {
    if (p == 0) {
        return;
    }
    UnkEffectPoolNode_8042A344 *node = static_cast<UnkEffectPoolNode_8042A344 *>(p) - 1;
    if (node->m_prev != 0) {
        node->m_prev->m_next = node->m_next;
    }
    if (node->m_next != 0) {
        node->m_next->m_prev = node->m_prev;
    }
    if (node == m_pool->m_usedHead) {
        m_pool->m_usedHead = node->m_next;
    }
    if (node == m_pool->m_usedTail) {
        m_pool->m_usedTail = node->m_prev;
    }
    m_pool->m_usedCount--;
    if (m_pool->m_freeTail != 0) {
        m_pool->m_freeTail->m_next = node;
        node->m_prev = m_pool->m_freeTail;
    } else {
        m_pool->m_freeHead = node;
        node->m_prev = 0;
    }
    m_pool->m_freeTail = node;
    node->m_next = 0;
    m_pool->m_freeCount++;
}

UnkEffectPool_8042A344::UnkEffectPool_8042A344(u32 size, s32 count) {
    m_stride = size + sizeof(UnkEffectPoolNode_8042A344);
    if (m_stride & 0xF) {
        m_stride += 0x10 - (m_stride & 0xF);
    }
    if (count != 0) {
        m_buffer = s_system->Allocate(m_stride * count);
    } else {
        m_buffer = 0;
    }
    if (m_buffer == 0) {
        count = 0;
    }
    m_freeHead = static_cast<UnkEffectPoolNode_8042A344 *>(m_buffer);
    m_freeTail = 0;
    m_usedTail = 0;
    m_usedHead = 0;
    m_usedCount = 0;
    m_freeCount = count;
    if (m_freeHead != 0) {
        UnkEffectPoolNode_8042A344 *node = m_freeHead;
        UnkEffectPoolNode_8042A344 *prev = 0;
        for (s32 i = 0; i < count; i++) {
            node->m_prev = prev;
            prev = node;
            node->m_next = reinterpret_cast<UnkEffectPoolNode_8042A344 *>(
                reinterpret_cast<u8 *>(node) + m_stride);
            node = node->m_next;
        }
        prev->m_next = 0;
        m_freeTail = prev;
    }
}

UnkEffectPool_8042A344::~UnkEffectPool_8042A344() {
    if (m_buffer != 0) {
        s_system->Free(m_buffer);
    }
}

} // namespace System
} // namespace Effect
