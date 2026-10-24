#ifndef GAME_ATTACKIGNORELIST_H
#define GAME_ATTACKIGNORELIST_H

/* The inline helpers of CharaColliAttack that the bullet units (Game/BulletCannon.cpp first) emit
 * out of line, and the list of attack collisions an attack collision ignores. Other units call
 * several of them by their addresses (fn_800532F8, fn_80053538, fn_80053B40, fn_80054C6C), so
 * they keep those names here. Class and member names are inferred.
 *
 * Include this header before anything that instantiates boost::weak_ptr<CharaColliAttack> (such as
 * Game/CharaColliAttack.h): it specializes weak_ptr<CharaColliAttack>::lock, which calls the
 * expired test under its address name. The list's constructor, destructor and pool are seen only
 * where ATTACK_IGNORE_LIST_MEMBERS is defined (Game/BulletUtil.cpp, which emits them with
 * CharaColliAttack's constructor and destructor). */

#include "boost/weak_ptr.hpp"
#include "dolphin/types.h"
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliAttack;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* Whether a weak_ptr to an attack collision has expired: boost's weak_ptr::expired, which other
 * units call by its address. */
extern "C" inline bool fn_80053538(const boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> *attack) {
    return attack->pn.use_count() == 0;
}

namespace boost {
template <>
inline shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack>
weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack>::lock() const {
    return fn_80053538(this) ? shared_ptr<element_type>() : shared_ptr<element_type>(*this);
}

#ifdef ATTACK_IGNORE_LIST_MEMBERS
/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line (boost 1.32's
 * definition). */
template <> inline void weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack>::reset() {
    this_type().swap(*this);
}
#endif
} // namespace boost

/* A node of the list (struct name unknown). */
struct UnkAttackIgnoreNode_80053644 {
    UnkAttackIgnoreNode_80053644() : m_raw(0), m_prev(0), m_next(0), m_allocated(false) {}

    boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> Get() const {
        return m_value;
    }

    /* 0x00 */ boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> m_value;
    /* 0x08 */ SonicteamUSA::System::CharaColli::CharaColliAttack *m_raw;
    /* 0x0C */ UnkAttackIgnoreNode_80053644 *m_prev;
    /* 0x10 */ UnkAttackIgnoreNode_80053644 *m_next;
    /* 0x14 */ bool m_allocated; /* from the heap rather than the pool */
};
SIZE_ASSERT(UnkAttackIgnoreNode_80053644, 0x18);

/* The pool the nodes of the lists come from (struct name unknown; layout as in
 * Game/PooledList.h). */
struct UnkAttackIgnorePool_800535A4 {
#ifdef ATTACK_IGNORE_LIST_MEMBERS
    /* Builds capacity linked nodes (0x80050A7C), and frees them (0x8005097C). */
    UnkAttackIgnorePool_800535A4(int capacity);
    ~UnkAttackIgnorePool_800535A4();

    /* Gives node back to the pool. */
    void Free(UnkAttackIgnoreNode_80053644 *node) {
        m_free->m_prev = node;
        node->m_next = m_free;
        node->m_prev = 0;
        m_free = node;
        node->m_value.reset();
        m_used--;
    }
#endif

    /* 0x0 */ void *m_nodes;
    /* 0x4 */ UnkAttackIgnoreNode_80053644 *m_free;
    /* 0x8 */ s32 m_capacity;
    /* 0xC */ s32 m_used;
};

#ifdef ATTACK_IGNORE_LIST_MEMBERS
/* The pool every list shares: the object (0x80570C70) and its guard (0x805EE040) are the weak
 * statics of this inline function, which an earlier unit is the first to emit. */
inline UnkAttackIgnorePool_800535A4 &AttackIgnorePoolObject() {
    static UnkAttackIgnorePool_800535A4 s_pool(0x200);
    return s_pool;
}

/* The pool, once AttackIgnorePool has built it. */
extern UnkAttackIgnorePool_800535A4 *lbl_805EE06C;

/* The pool (name inferred). It calls AttackIgnorePoolObject, so it stays out of line. */
inline UnkAttackIgnorePool_800535A4 *AttackIgnorePool() {
    if (lbl_805EE06C == 0) {
        lbl_805EE06C = &AttackIgnorePoolObject();
    }
    return lbl_805EE06C;
}
#endif

/* The attack collisions an attack collision ignores, at 0x44 of CharaColliAttack (class name
 * unknown): a list of weak_ptr values in the manner of Game/PooledList.h, which holds shared_ptr
 * values. */
struct UnkAttackIgnoreList_80053340 {
    typedef UnkAttackIgnoreNode_80053644 Node;

    class iterator {
      public:
        iterator() : m_node(0) {}
        iterator(Node *node) : m_node(node) {}

        boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> operator*() const {
            return m_node->Get();
        }
        iterator operator++(int) {
            iterator previous = *this;
            m_node = m_node ? m_node->m_next : 0;
            return previous;
        }
        bool operator!=(const Node *node) const {
            return m_node != node;
        }

        Node *m_node;
    };

#ifdef ATTACK_IGNORE_LIST_MEMBERS
    /* An empty list: its end node only. */
    UnkAttackIgnoreList_80053340() : m_pool(AttackIgnorePool()) {
        Node *node;
        NewNode(node);
        node->m_raw = 0;
        m_head = m_tail = node;
        m_size = 0;
    }
    ~UnkAttackIgnoreList_80053340() {
        Clear();
        Node *node = m_head;
        if (!node->m_allocated) {
            m_pool->Free(node);
        } else {
            delete node;
        }
    }

    /* Gives every node but the end node back. */
    void Clear() {
        Node *node = m_head;
        while (node != m_tail) {
            Node *next = node->m_next;
            if (!node->m_allocated) {
                m_pool->Free(node);
            } else {
                delete node;
            }
            node = next;
        }
        m_tail->m_prev = 0;
        m_head = m_tail;
        m_size = 0;
    }
#endif

    Node *Head() const {
        return m_head;
    }
    Node *End() const {
        return m_tail;
    }

    /* Takes a node from the pool, or from the heap when the pool has none left. */
    void NewNode(Node *&result) {
        Node *node;
        UnkAttackIgnorePool_800535A4 *pool = m_pool;
        node = pool->m_free;
        if (pool->m_used == pool->m_capacity - 1) {
            node = 0;
        } else {
            node->m_next->m_prev = 0;
            pool->m_used++;
            pool->m_free = node->m_next;
            node->m_next = 0;
        }
        if (!node) {
            node = new Node;
            node->m_allocated = true;
        }
        result = node;
    }

    void PushFront(boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> value) {
        Node *node;
        NewNode(node);
        node->m_prev = 0;
        node->m_value = value;
        if (value.lock()) {
            node->m_raw = value.lock().get();
        } else {
            node->m_raw = 0;
        }
        node->m_next = m_head;
        m_head->m_prev = node;
        m_head = node;
        m_size++;
    }

    /* 0x0 */ UnkAttackIgnorePool_800535A4 *m_pool;
    /* 0x4 */ u32 m_size;
    /* 0x8 */ Node *m_head;
    /* 0xC */ Node *m_tail; /* the empty node End() returns */
};
SIZE_ASSERT(UnkAttackIgnoreList_80053340, 0x10);

#include "Game/CharaColliAttack.h"
#include "boost/shared_ptr.hpp"

/* Makes attack ignore other (other units call it by its address). */
extern "C" inline void fn_800532F8(SonicteamUSA::System::CharaColli::CharaColliAttack *attack,
                                   boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> other) {
    reinterpret_cast<UnkAttackIgnoreList_80053340 *>(&attack->field_0x44)->PushFront(other);
}

/* Releases an attack collision: shared_ptr::reset (other units call it by its address). */
extern "C" inline void fn_80053B40(boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack> *attack) {
    boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliAttack>().swap(*attack);
}

#endif
