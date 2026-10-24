#ifndef GAME_UNKTREEMAP_H
#define GAME_UNKTREEMAP_H

#include "Game/UnkSingletons.h"

/* The larger members of UnkTree (Game/UnkSingletons.h), which a unit that defines UNK_TREE_MAP
 * includes after its own code: the game emits these instances after the code that uses them, as
 * GC/2.7 does for templates defined after their first use. They are <map_impl>'s members of
 * std::__tree for this tree, with find_or_insert for the map's operator[]. */

template <class T>
UnkTree<T>::UnkTree(const KeyCompare &comp, const ValueAllocator &a)
    : m_size(a, 0), m_header(NodeAllocator()), m_begin(comp) {
    begin_node() = header();
}

template <class T> void UnkTree<T>::clear() {
    if (root() != 0) {
        destroy(static_cast<Node *>(root()));
        size_ref() = 0;
        root() = 0;
        begin_node() = header();
    }
}

template <class T> template <class Key> T &UnkTree<T>::find_or_insert(const Key &k) {
    Node *last = 0;
    UnkTreeNodeBase *y = header();
    UnkTreeNodeBase *x = root();
    bool left = true;
    bool leftmost = true;
    while (x != 0) {
        y = x;
        if (comp()(k, static_cast<Node *>(x)->value)) {
            x = x->left;
            left = true;
        } else {
            last = static_cast<Node *>(x);
            x = x->right;
            left = false;
            leftmost = false;
        }
    }
    if (last == 0 || comp()(last->value, k)) {
        return insert_node(y, left, leftmost, T(k, typename T::MappedType()))->value;
    }
    return last->value;
}

template <class T> template <class Key> typename UnkTree<T>::Iterator UnkTree<T>::find(const Key &k) {
    UnkTreeNodeBase *x = root();
    UnkTreeNodeBase *y = header();
    while (x != 0) {
        if (!comp()(static_cast<Node *>(x)->value, k)) {
            y = x;
            x = x->left;
        } else {
            x = x->right;
        }
    }
    if (y == header() || comp()(k, static_cast<Node *>(y)->value)) {
        return end();
    }
    return Iterator(y);
}

template <class T>
typename UnkTree<T>::Node *UnkTree<T>::insert_node(UnkTreeNodeBase *parent, bool left, bool leftmost,
                                                   const T &v) {
    if (size() > max_size() - 1) {
        std::__msl_error("tree::insert length error\n");
    }
    Node *n = node_allocator().allocate(1);
    value_allocator().construct(&n->value, v);
    n->left = n->right = 0;
    n->SetParent(parent);
    if (left) {
        parent->left = n;
    } else {
        parent->right = n;
    }
    ++size_ref();
    fn_8000CD64(n, root());
    if (leftmost) {
        begin_node() = n;
    }
    return n;
}

#endif
