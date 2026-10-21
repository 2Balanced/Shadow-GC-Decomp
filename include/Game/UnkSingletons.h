#ifndef GAME_UNKSINGLETONS_H
#define GAME_UNKSINGLETONS_H

#include <dolphin/types.h>
#include "size_assert.h"

/* Three game-wide objects, each reached through an accessor that creates it on first use in a
 * function-local static and caches its address in a second one. The accessors are defined in
 * Game/SavePoint.cpp (their code and statics lie in that unit) and keep their fn_ names (other
 * units call them by those names). Class names and meanings are unknown. */

/* A 4-byte object whose constructor and destructor are out of line (0x802E5B2C, 0x802E5A5C). */
class UnkObject_802E5B2C {
  public:
    UnkObject_802E5B2C();
    ~UnkObject_802E5B2C();

    /* The object's address once created (see fn_80117514; name inferred). */
    static UnkObject_802E5B2C *s_cached;

    /* 0x0 */ u32 field_0x0;
};
SIZE_ASSERT(UnkObject_802E5B2C, 0x4);

/* A 4-byte object cleared on construction. */
class UnkObject_801175AC {
  public:
    UnkObject_801175AC() : field_0x0(0) {}

    /* The object's address once created (see fn_80117578; name inferred). */
    static UnkObject_801175AC *s_cached;

    /* 0x0 */ u32 field_0x0;
};
SIZE_ASSERT(UnkObject_801175AC, 0x4);

/* A node of the trees below: its links, then its value. */
struct UnkTreeNodeBase {
    /* 0x0 */ UnkTreeNodeBase *left;
    /* 0x4 */ UnkTreeNodeBase *right;
    /* 0x8 */ UnkTreeNodeBase *parent;
};

template <class T> struct UnkTreeNode : public UnkTreeNodeBase {
    /* 0xC */ T value;
};

/* Do-nothing and free helpers of the allocators (names inferred). Calling an inline function keeps
 * the allocator members below from being inlined where retail calls them. */
inline void UnkTreeNothing() {}
inline void UnkTreeFree(void *p) {
    ::operator delete(p);
}

/* A binary tree of T in the shape of a standard library map: a word, then the header node whose
 * left link is the root. Only what the destructor needs is reconstructed (names inferred; the
 * insertion and search code lives in the unit at 0x801A04C0). */
template <class T> class UnkTree {
  public:
    typedef UnkTreeNode<T> Node;

    /* The allocators: values need no destruction; nodes are freed with operator delete. */
    struct ValueAllocator {
        void destroy(T *p) {
            UnkTreeNothing();
        }
    };
    struct NodeAllocator {
        void deallocate(Node *p, u32 n) {
            UnkTreeFree(p);
        }
    };

    ~UnkTree() {
        if (root() != 0) {
            destroy(static_cast<Node *>(root()));
        }
    }

    /* Frees the subtree under n. Defined in Game/UnkTreeDestroy.h, which a unit includes after its
     * code (see there). */
    void destroy(Node *n);

    UnkTreeNodeBase *&root() {
        return header()->left;
    }
    UnkTreeNodeBase *header() {
        return HeaderNode();
    }
    ValueAllocator &value_allocator() {
        return ValueAllocatorOf();
    }
    NodeAllocator &node_allocator() {
        return NodeAllocatorOf();
    }

  private:
    UnkTreeNodeBase *HeaderNode() {
        return &m_header;
    }
    ValueAllocator &ValueAllocatorOf() {
        return *reinterpret_cast<ValueAllocator *>(this);
    }
    NodeAllocator &NodeAllocatorOf() {
        return *reinterpret_cast<NodeAllocator *>(&m_header);
    }

  public:
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkTreeNodeBase m_header;
};

/* The values of the two trees of UnkObject_801A2AF4 (unknown). */
struct UnkTreeValueA_801A2AF4 {
    u32 field_0x0;
};
struct UnkTreeValueB_801A2AF4 {
    u32 field_0x0;
};

/* The maps of UnkObject_801A2AF4 (class names unknown). */
struct UnkMapA_801A2AF4 {
    UnkTree<UnkTreeValueA_801A2AF4> m_tree;
};
struct UnkMapB_801A2AF4 {
    UnkTree<UnkTreeValueB_801A2AF4> m_tree;
};

/* A 0x68-byte object holding two maps; its constructor is out of line (0x801A2AF4). */
class UnkObject_801A2AF4 {
  public:
    UnkObject_801A2AF4();

    /* The object's address once created (see fn_80119178; name inferred). */
    static UnkObject_801A2AF4 *s_cached;

    /* 0x00 */ UnkMapA_801A2AF4 m_mapA;
    /* 0x10 */ UnkMapB_801A2AF4 m_mapB;
    /* 0x20 */ u8 field_0x20[0x68 - 0x20];
};
SIZE_ASSERT(UnkObject_801A2AF4, 0x68);

extern "C" {
/* The accessors, defined in Game/SavePoint.cpp (behaviour from their bodies): each returns the
 * object, creating it on first use. */
UnkObject_802E5B2C *fn_80117514(void);
UnkObject_801175AC *fn_80117578(void);
UnkObject_801A2AF4 *fn_80119178(void);
}

#endif
