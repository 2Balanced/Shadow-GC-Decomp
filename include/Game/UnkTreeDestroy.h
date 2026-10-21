#ifndef GAME_UNKTREEDESTROY_H
#define GAME_UNKTREEDESTROY_H

#include "Game/UnkSingletons.h"

/* UnkTree<T>::destroy, kept apart from Game/UnkSingletons.h: a unit includes this header after its
 * own code. Retail emits destroy with the unit's last functions (after the boost::bind instances)
 * and expands its recursive call once into itself. GC/2.7 does one or the other: an inline
 * function defined before its first use is expanded into itself but emitted after its caller, one
 * defined after its first use is emitted last but not expanded. So the definition comes last and
 * the one level of expansion is written out (the expanded copy calls the allocator members that
 * the outer level inlines). */
template <class T> inline void UnkTree<T>::destroy(Node *n) {
    Node *child = static_cast<Node *>(n->left);
    if (child != 0) {
        if (child->left != 0) {
            destroy(static_cast<Node *>(child->left));
        }
        if (child->right != 0) {
            destroy(static_cast<Node *>(child->right));
        }
        value_allocator().destroy(&child->value);
        node_allocator().deallocate(child, 1);
    }
    child = static_cast<Node *>(n->right);
    if (child != 0) {
        if (child->left != 0) {
            destroy(static_cast<Node *>(child->left));
        }
        if (child->right != 0) {
            destroy(static_cast<Node *>(child->right));
        }
        value_allocator().destroy(&child->value);
        node_allocator().deallocate(child, 1);
    }
    /* The outer level's value destruction and deallocation, inlined. */
    value_allocator();
    node_allocator();
    ::operator delete(n);
}

#endif
