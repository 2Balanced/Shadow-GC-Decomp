#ifndef GAME_UNKSINGLETONS_H
#define GAME_UNKSINGLETONS_H

#include <dolphin/types.h>
#include "size_assert.h"

#ifdef UNK_TREE_MAP
/* The library parts that the search and insertion code of UnkTree below uses. */
#include <functional>
#include <memory>
#endif

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
#ifdef UNK_TREE_MAP
    /* Links the node under p; the parent link keeps the node's colour in its low bit. */
    void SetParent(UnkTreeNodeBase *p) {
        parent = reinterpret_cast<UnkTreeNodeBase *>(reinterpret_cast<u32>(p) |
                                                     (reinterpret_cast<u32>(parent) & 1));
    }

#endif
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

#ifdef UNK_TREE_MAP
/* Returns x (an inline call, like UnkTreeNothing, that keeps its caller out of line). */
template <class T> inline T UnkTreeId(T x) {
    return x;
}

class Enemy;

extern "C" {
/* Rebalances the tree under root after x was linked in (the library code <map> also calls). */
void fn_8000CD64(UnkTreeNodeBase *x, UnkTreeNodeBase *root);
}
#endif

/* A binary tree of T in the shape of a standard library map: a word, then the header node whose
 * left link is the root. It is the tree behind the library's map (<map> reconstructs the same
 * code as std::__tree); these names are the ones the instances in Game/SavePoint.cpp carry. Every
 * unit sees what the destructor needs; the search and insertion code, and the layout it uses, only
 * a unit that defines UNK_TREE_MAP (Game/EnemyEntry.cpp, which holds that code; the larger members
 * are in Game/UnkTreeMap.h). Names are inferred. */
template <class T> class UnkTree {
  public:
    typedef UnkTreeNode<T> Node;
#ifdef UNK_TREE_MAP
    typedef typename T::KeyType KeyType;
    typedef std::less<KeyType> KeyCompare;

    /* Orders a key against a value (the values are ordered by their keys). */
    class Compare {
      public:
        Compare(const KeyCompare &c) : m_comp(c) {
            UnkTreeNothing();
        }

        bool operator()(const KeyType &x, const T &y) const {
            return m_comp(x, y.first);
        }
        bool operator()(const T &x, const KeyType &y) const {
            return m_comp(x.first, y);
        }

      protected:
        /* 0x0 */ KeyCompare m_comp;
    };

    /* A position in the tree. */
    class Iterator {
      public:
        Iterator(UnkTreeNodeBase *node) : m_node(node) {}
        Iterator(const Iterator &x) : m_node(x.m_node) {}

        T &operator*() const {
            return static_cast<Node *>(m_node)->value;
        }
        bool operator!=(const Iterator &x) const {
            return m_node != x.m_node;
        }

        /* 0x0 */ UnkTreeNodeBase *m_node;
    };
#endif

    /* The allocators: values need no destruction; nodes are freed with operator delete. */
    struct ValueAllocator {
#ifdef UNK_TREE_MAP
        ValueAllocator() {}

        void construct(T *p, const T &value) {
            new ((void *)p) T(value);
        }
#endif
        void destroy(T *p) {
            UnkTreeNothing();
        }
    };
    struct NodeAllocator {
#ifdef UNK_TREE_MAP
        NodeAllocator() {}

        /* An allocation failure stops the game. */
        Node *allocate(u32 n, const void *hint = 0) {
            Node *p = static_cast<Node *>(::operator new(n * sizeof(Node)));
            if (p == 0) {
                std::__msl_error("Memory allocation failure");
            }
            return p;
        }
#endif
        void deallocate(Node *p, u32 n) {
            UnkTreeFree(p);
        }
    };

#ifdef UNK_TREE_MAP
    UnkTree(const KeyCompare &comp, const ValueAllocator &a);
#endif
    ~UnkTree() {
        if (root() != 0) {
            destroy(static_cast<Node *>(root()));
        }
    }

    /* Frees the subtree under n. Defined in Game/UnkTreeDestroy.h, which a unit includes after its
     * code (see there). */
    void destroy(Node *n);

#ifdef UNK_TREE_MAP
    Iterator end() {
        return Iterator(header());
    }
    u32 size() const {
        return size_ref();
    }
    u32 max_size() const {
        return std::__max_size_t();
    }
    /* The value with key k, inserted first (with a default mapped value) if there is none. */
    template <class Key> T &find_or_insert(const Key &k);
    template <class Key> Iterator find(const Key &k);
    void clear();
#endif

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
#ifdef UNK_TREE_MAP
    u32 &size_ref() {
        return m_size.second();
    }
    const u32 &size_ref() const {
        return m_size.second();
    }
    UnkTreeNodeBase *&begin_node() {
        return m_begin.second();
    }
    Compare &comp() {
        return m_begin.first();
    }
    Node *insert_node(UnkTreeNodeBase *parent, bool left, bool leftmost, const T &v);

    UnkTreeNodeBase *HeaderNode() {
        return reinterpret_cast<UnkTreeNodeBase *>(&m_header);
    }
#else
    UnkTreeNodeBase *HeaderNode() {
        return &m_header;
    }
#endif
    ValueAllocator &ValueAllocatorOf() {
        return *reinterpret_cast<ValueAllocator *>(this);
    }
    NodeAllocator &NodeAllocatorOf() {
        return *reinterpret_cast<NodeAllocator *>(&m_header);
    }

  public:
#ifdef UNK_TREE_MAP
    /* The tree's three parts, each also holding an allocator or the comparison (as <map>'s tree
     * has them): the size, the end node (its left link is the root) and the leftmost node. */
    struct SizeBase : public ValueAllocator {
        SizeBase(const ValueAllocator &a, u32 n) : m_value(UnkTreeId(n)) {}

        /* 0x0 */ u32 m_value;
    };
    struct SizePair : public SizeBase {
        SizePair(const ValueAllocator &a, u32 n) : SizeBase(a, n) {}

        u32 &second() {
            return this->m_value;
        }
        const u32 &second() const {
            return this->m_value;
        }
    };
    struct EndNode {
        EndNode() : m_left(UnkTreeId<UnkTreeNodeBase *>(0)) {}

        /* 0x0 */ UnkTreeNodeBase *m_left;
    };
    struct HeaderPair : public NodeAllocator {
        HeaderPair(const NodeAllocator &a) {}

        /* 0x0 */ EndNode m_end;
    };
    struct BeginPair : public Compare {
        BeginPair(const KeyCompare &c) : Compare(c) {
            UnkTreeNothing();
        }

        Compare &first() {
            return *this;
        }
        UnkTreeNodeBase *&second() {
            return m_leftmost;
        }

        /* 0x4 */ UnkTreeNodeBase *m_leftmost;
    };

    /* 0x0 */ SizePair m_size;
    /* 0x4 */ HeaderPair m_header;
    /* 0x8 */ BeginPair m_begin;
#else
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkTreeNodeBase m_header;
#endif
};

/* The values of the two trees of UnkObject_801A2AF4: a key and what it maps to (see
 * Game/EnemyEntry.cpp; names inferred). */
struct UnkTreeValueA_801A2AF4 {
#ifdef UNK_TREE_MAP
    typedef s32 KeyType;
    typedef Enemy *MappedType;

    UnkTreeValueA_801A2AF4(const KeyType &key, const MappedType &value) : first(key), second(value) {}

    /* 0x0 */ const KeyType first; /* a handle (fn_801A29B0) */
    /* 0x4 */ MappedType second;   /* the enemy it was given to, 0 once given back */
#else
    u32 field_0x0;
#endif
};
struct UnkTreeValueB_801A2AF4 {
#ifdef UNK_TREE_MAP
    typedef s16 KeyType;
    typedef s16 MappedType;

    UnkTreeValueB_801A2AF4(const KeyType &key, const MappedType &value) : first(key), second(value) {}

    /* 0x0 */ const KeyType first; /* a switch number */
    /* 0x2 */ MappedType second;   /* the enemies placed with it that are still about */
#else
    u32 field_0x0;
#endif
};

/* The maps of UnkObject_801A2AF4 (class names unknown). */
struct UnkMapA_801A2AF4 {
#ifdef UNK_TREE_MAP
    typedef UnkTree<UnkTreeValueA_801A2AF4> Tree;

    UnkMapA_801A2AF4() : m_tree(Tree::KeyCompare(), Tree::ValueAllocator()) {}

    Enemy *&operator[](const s32 &handle) {
        return m_tree.find_or_insert(handle).second;
    }
    void clear() {
        m_tree.clear();
    }

#endif
    UnkTree<UnkTreeValueA_801A2AF4> m_tree;
};
struct UnkMapB_801A2AF4 {
#ifdef UNK_TREE_MAP
    typedef UnkTree<UnkTreeValueB_801A2AF4> Tree;

    UnkMapB_801A2AF4() : m_tree(Tree::KeyCompare(), Tree::ValueAllocator()) {}

    s16 &operator[](const s16 &group) {
        return m_tree.find_or_insert(group).second;
    }
    Tree::Iterator find(const s16 &group) {
        return m_tree.find(group);
    }
    Tree::Iterator end() {
        return m_tree.end();
    }
    void clear() {
        m_tree.clear();
    }

#endif
    UnkTree<UnkTreeValueB_801A2AF4> m_tree;
};

/* A 0x68-byte object holding two maps; its constructor is out of line (0x801A2AF4). It counts the
 * stage's enemies (Game/EnemyEntry.cpp, where its functions are): those placed and destroyed of
 * each of three kinds (by type: 100 to 119, 120 to 139, 140 to 149), and the enemies still about
 * for each switch number, which turns the switch on once they are all gone. */
class UnkObject_801A2AF4 {
  public:
    UnkObject_801A2AF4();

    /* The object's address once created (see fn_80119178; name inferred). */
    static UnkObject_801A2AF4 *s_cached;

    /* 0x00 */ UnkMapA_801A2AF4 m_mapA; /* the enemies by handle */
    /* 0x10 */ UnkMapB_801A2AF4 m_mapB; /* the enemies still about by switch number */
#ifdef UNK_TREE_MAP
    /* 0x20 */ s32 m_handles;                   /* the handles given out */
    /* 0x24 */ s32 m_placed;                    /* the enemies placed */
    /* 0x28 */ s32 m_placedOfKind[3];           /* the same by kind */
    /* 0x34 */ s32 m_destroyed[3];              /* the enemies destroyed by kind (fn_801A2140) */
    /* 0x40 */ s32 m_destroyedByPlayer[3];      /* those of them the player destroyed */
    /* 0x4C */ s32 m_savedDestroyed[3];         /* m_destroyed when the stage was last saved */
    /* 0x58 */ s32 m_savedDestroyedByPlayer[3]; /* m_destroyedByPlayer then */
    /* 0x64 */ u8 field_0x64[0x68 - 0x64];
#else
    /* 0x20 */ u8 field_0x20[0x68 - 0x20];
#endif
};
SIZE_ASSERT(UnkObject_801A2AF4, 0x68);

extern "C" {
/* The accessors, defined in Game/SavePoint.cpp (behaviour from their bodies): each returns the
 * object, creating it on first use. */
UnkObject_802E5B2C *fn_80117514(void);
UnkObject_801175AC *fn_80117578(void);
UnkObject_801A2AF4 *fn_80119178(void);
#ifdef UNK_TREE_MAP
/* Gives an enemy a handle and takes it back (Game/EnemyEntry.cpp; Game/Enemy.h declares them with
 * the counter as void * where UNK_TREE_MAP is not defined, and a unit that defines it includes this
 * header first). */
u32 fn_801A29B0(UnkObject_801A2AF4 *counter, Enemy *enemy);
void fn_801A2968(UnkObject_801A2AF4 *counter, u32 handle);
#endif
}

#endif
