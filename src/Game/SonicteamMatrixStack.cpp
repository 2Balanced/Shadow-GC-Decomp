/* A stack of matrices whose bottom entry is the identity (Game/MotionHeap.cpp creates one): its
 * constructor and destructor, push (copies the top matrix one up) and pop. Other units call them by
 * their fn_ names, so they keep them; the struct keeps Game/MotionHeap.cpp's name and the file name
 * and member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * The matrix default constructor is referenced from its owning unit, as retail's object does.
 * The constructor's exception table entry (the DELETEPOINTERCOND action that frees the array if a
 * constructor throws) has two bytes of alignment padding that mwcceppc leaves uninitialized;
 * configure.py's extab_padding sets them to retail's 0xAC02. Every function matches. */
#define UNK_MATRIX_DEFAULT_CTOR_EXTERN
#include "Game/MathTypes.h"
#include "Game/UnkMatrix_800B1304.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct UnkMotionHeapBlock_8040C2B8 {
    /* 0x0 */ UnkMatrix_800B1304 *m_matrices;
    /* 0x4 */ int m_capacity;
    /* 0x8 */ int m_top;
};
SIZE_ASSERT(UnkMotionHeapBlock_8040C2B8, 0xC);

extern "C" {
void fn_8040C204(UnkMotionHeapBlock_8040C2B8 *stack);
void fn_8040C244(UnkMotionHeapBlock_8040C2B8 *stack);
UnkMotionHeapBlock_8040C2B8 *fn_8040C25C(UnkMotionHeapBlock_8040C2B8 *stack, short free);
UnkMotionHeapBlock_8040C2B8 *fn_8040C2B8(UnkMotionHeapBlock_8040C2B8 *stack, int capacity);
}

UnkMotionHeapBlock_8040C2B8 *fn_8040C2B8(UnkMotionHeapBlock_8040C2B8 *stack, int capacity) {
    stack->m_capacity = capacity;
    stack->m_top = 0;
    stack->m_matrices = new UnkMatrix_800B1304[stack->m_capacity];
    fn_8040C148(stack->m_matrices, &lbl_805E0720);
    return stack;
}

UnkMotionHeapBlock_8040C2B8 *fn_8040C25C(UnkMotionHeapBlock_8040C2B8 *stack, short free) {
    if (stack != 0) {
        if (stack->m_matrices != 0) {
            delete[] stack->m_matrices;
        }
        if (free > 0) {
            delete stack;
        }
    }
    return stack;
}

void fn_8040C244(UnkMotionHeapBlock_8040C2B8 *stack) {
    if (stack->m_top > 0) {
        stack->m_top--;
    }
}

void fn_8040C204(UnkMotionHeapBlock_8040C2B8 *stack) {
    UnkMatrix_800B1304 *top = &stack->m_matrices[stack->m_top];
    UnkMatrix_800B1304 *next = &stack->m_matrices[++stack->m_top];
    fn_8040C148(next, top);
}


