/* The constructor and destructor of the node pool of Game/PooledList.h (the pool itself is a
 * function-local static of fn_80028B30, Game/EffectSystemUSA.cpp), and a round-up helper other
 * units call by its fn_ name. The pool's nodes are PooledListNode<void>. The file name and the
 * names here are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/PooledList.h"
#include <dolphin/types.h>

PooledListPool::PooledListPool(s32 capacity) {
    if ((m_nodes = reinterpret_cast<PooledListPoolNode *>(new PooledListNode<void>[capacity])) == 0) {
        for (;;) {
        }
    }
    m_capacity = capacity;
    m_used = 0;
    m_free = m_nodes;
    PooledListPoolNode *node = m_free;
    for (s32 i = 0; i < capacity; i++, node++) {
        fn_80028A1C(node);
        if (i == 0) {
            node->m_prev = 0;
        } else {
            node->m_prev = node - 1;
        }
        if (i == capacity - 1) {
            node->m_next = 0;
        } else {
            node->m_next = node + 1;
        }
    }
}

PooledListPool::~PooledListPool() {
    if (m_nodes != 0) {
        delete[] reinterpret_cast<PooledListNode<void> *>(m_nodes);
        m_nodes = 0;
    }
}

/* Rounds size up to a multiple of align (a power of two). */
extern "C" s32 fn_8004E85C(s32 size, s32 align) {
    return (size + (align - 1)) & ~(align - 1);
}
