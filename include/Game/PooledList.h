#ifndef GAME_POOLEDLIST_H
#define GAME_POOLEDLIST_H

/* A doubly linked list of shared_ptr values whose nodes come from a node pool shared by every list
 * (the class names are unknown; this is a reconstruction from the instances the boss support units
 * emit: PooledList<CharaColliBase> in Game/BossCharaColli.cpp, PooledList<BossCtrlUnit> in
 * Game/BossCtrl.cpp and PooledList<Target> in Game/BossTarget.cpp).
 *
 * A list always holds one more node than it has values: the empty node m_tail, which End()
 * returns. Nodes come from the shared pool (fn_80028B30, 0x200 nodes) while it has free nodes and
 * from the heap after that; m_allocated tells them apart when a node is given back. The pool hands
 * out its free nodes from a linked free list and resets each node it takes back
 * (fn_80028A1C).
 *
 * Every member is defined in the class, and each unit emits the ones it calls out of line as weak
 * copies after their first user (local copies for the member template Release). Under GC/2.7's
 * -inline level=1 the functions that call other inline functions (Clear, PushBack, Erase,
 * Remove, Release, NewNode, the constructor and destructor) are never inlined, nor are those
 * that return a class by value (Get, operator*, operator++(int)). Head() and End() are leaves but
 * are called out of line where they are arguments of the iterator's inline functions, as is the
 * iterator's constructor in it = iterator(list.Head()). Giving a node back is written out in each
 * function rather than through a helper: a helper would be emitted out of line. Release calls
 * the non-template Push, which keeps it out of line as in retail. Names are inferred. */

#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

/* The shared node pool (fn_80028B30 returns it; its constructor 0x8004E920 links all nodes into
 * the free list). */
struct PooledListPool;
extern "C" {
PooledListPool *fn_80028B30(void);
/* Clears the value of a pool node (a shared_ptr reset of the pool's own node type). */
void fn_80028A1C(void *node);
}

template <class T>
struct PooledListNode;

/* A node as the pool sees it: the same layout as PooledListNode, whatever the value type. */
struct PooledListPoolNode {
    /* 0x00 */ u8 m_value[0x8];
    /* 0x08 */ void *m_raw;
    /* 0x0C */ PooledListPoolNode *m_prev;
    /* 0x10 */ PooledListPoolNode *m_next;
    /* 0x14 */ bool m_allocated;
};

struct PooledListPool {
    /* Links node into the free list. */
    void Push(PooledListPoolNode *node) {
        m_free->m_prev = node;
        node->m_next = m_free;
        node->m_prev = 0;
        m_free = node;
        fn_80028A1C(m_free);
        m_used--;
    }

    /* Gives a node back to the pool. */
    template <class T>
    void Release(PooledListNode<T> *node) {
        node->m_value.reset();
        Push(reinterpret_cast<PooledListPoolNode *>(node));
    }

    /* 0x0 */ PooledListPoolNode *m_nodes;  /* the nodes' array */
    /* 0x4 */ PooledListPoolNode *m_free;   /* the first free node */
    /* 0x8 */ s32 m_capacity; /* the number of nodes */
    /* 0xC */ s32 m_used;     /* the number of nodes handed out */
};

template <class T>
struct PooledListNode {
    PooledListNode() : m_raw(0), m_prev(0), m_next(0), m_allocated(false) {}

    boost::shared_ptr<T> Get() const {
        return m_value;
    }

    /* 0x00 */ boost::shared_ptr<T> m_value;
    /* 0x08 */ T *m_raw; /* the value's pointer, set when the value is added */
    /* 0x0C */ PooledListNode *m_prev;
    /* 0x10 */ PooledListNode *m_next;
    /* 0x14 */ bool m_allocated; /* from the heap rather than the pool */
};

template <class T>
class PooledList {
  public:
    typedef PooledListNode<T> Node;

    class iterator {
      public:
        iterator() : m_node(0) {}
        iterator(Node *node) : m_node(node) {}

        boost::shared_ptr<T> operator*() const {
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

    PooledList() {
        m_pool = fn_80028B30();
        Node *node;
        NewNode(node);
        node->m_value.reset();
        node->m_raw = 0;
        m_head = m_tail = node;
        m_size = 0;
    }

    ~PooledList() {
        Clear();
        if (!m_head->m_allocated) {
            m_pool->Release(m_head);
        } else {
            delete m_head;
        }
    }

    s32 Size() const {
        return m_size;
    }
    Node *Head() const {
        return m_head;
    }
    Node *End() const {
        return m_tail;
    }

    void PushBack(boost::shared_ptr<T> value) {
        Node *node;
        NewNode(node);
        node->m_prev = m_tail->m_prev;
        node->m_next = m_tail;
        node->m_value = value;
        node->m_raw = value.get();
        if (m_tail->m_prev) {
            m_tail->m_prev->m_next = node;
        }
        m_tail->m_prev = node;
        if (m_size == 0) {
            m_head = node;
        }
        m_size++;
    }

    void Erase(iterator it) {
        Remove(it.m_node);
    }

    void Remove(Node *node) {
        if (node) {
            if (node->m_prev) {
                node->m_prev->m_next = node->m_next;
            } else {
                m_head = node->m_next;
            }
            if (node->m_next) {
                node->m_next->m_prev = node->m_prev;
            } else {
                m_tail = node->m_prev;
            }
            if (!node->m_allocated) {
                m_pool->Release(node);
            } else {
                delete node;
            }
            m_size--;
        }
    }

    void Clear() {
        Node *node = m_head;
        while (node != m_tail) {
            Node *next = node->m_next;
            if (!node->m_allocated) {
                m_pool->Release(node);
            } else {
                delete node;
            }
            node = next;
        }
        m_tail->m_prev = 0;
        m_head = m_tail;
        m_size = 0;
    }

    /* Takes a node from the pool, or from the heap when the pool has none left. */
    void NewNode(Node *&result) {
        Node *node;
        PooledListPool *pool = m_pool;
        node = reinterpret_cast<Node *>(pool->m_free);
        if (pool->m_used == pool->m_capacity - 1) {
            node = 0;
        } else {
            node->m_next->m_prev = 0;
            pool->m_used++;
            pool->m_free = reinterpret_cast<PooledListPoolNode *>(node->m_next);
            node->m_next = 0;
        }
        if (!node) {
            node = new Node;
            node->m_allocated = true;
        }
        result = node;
    }

    /* 0x0 */ PooledListPool *m_pool;
    /* 0x4 */ u32 m_size;
    /* 0x8 */ Node *m_head;
    /* 0xC */ Node *m_tail;
};

#endif
