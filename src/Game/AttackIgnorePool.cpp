/* The constructor and destructor of the pool the attack collisions' ignore lists take their nodes
 * from (Game/AttackIgnoreList.h; the pool itself is a function-local static there, built by
 * Game/BulletUtil.cpp's copy of AttackIgnorePool). The layout and the code follow Game/PooledList.h's
 * pool (Game/PooledListPool.cpp). The file name and the names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define ATTACK_IGNORE_LIST_MEMBERS
#include "Game/AttackIgnoreList.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

/* A node as the pool builds it: the layout of UnkAttackIgnoreNode_80053644, with an untyped value
 * (retail has this unit's own copies of the node's constructor and destructor, 0x80050B4C and
 * 0x800509E0, and of the weak_ptr's, apart from those of the list's node type). */
struct UnkAttackIgnorePoolNode_80050B4C {
    UnkAttackIgnorePoolNode_80050B4C() : m_raw(0), m_prev(0), m_next(0), m_allocated(false) {}

    /* 0x00 */ boost::weak_ptr<void> m_value;
    /* 0x08 */ void *m_raw;
    /* 0x0C */ UnkAttackIgnorePoolNode_80050B4C *m_prev;
    /* 0x10 */ UnkAttackIgnorePoolNode_80050B4C *m_next;
    /* 0x14 */ bool m_allocated;
};

UnkAttackIgnorePool_800535A4::UnkAttackIgnorePool_800535A4(int capacity) {
    if ((m_nodes = new UnkAttackIgnorePoolNode_80050B4C[capacity]) == 0) {
        for (;;) {
        }
    }
    m_capacity = capacity;
    m_used = 0;
    m_free = (UnkAttackIgnoreNode_80053644 *)m_nodes;
    UnkAttackIgnorePoolNode_80050B4C *node = (UnkAttackIgnorePoolNode_80050B4C *)m_free;
    for (s32 i = 0; i < capacity; i++, node++) {
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

UnkAttackIgnorePool_800535A4::~UnkAttackIgnorePool_800535A4() {
    if (m_nodes != 0) {
        delete[] (UnkAttackIgnorePoolNode_80050B4C *)m_nodes;
        m_nodes = 0;
    }
}
