/* The motion library's matrix helpers, which work on the top of its current matrix stack (the
 * stack of Game/SonicteamMatrixStack.cpp; Game/MotionHeap.cpp installs it with fn_804116F8) when
 * they are not given a matrix: multiplying onto it (optionally pushed and loaded first), rotating
 * a vector, the sum of two vectors, reading the translation, setting the rotation part, the scale
 * of each axis and normalizing the axes. Other units call them by their fn_ names, so they keep
 * them; the file name is invented and everything here is named from what the code does.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/MathTypes.h"
#include "Game/UnkMatrix_800B1304.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The matrix stack (as in Game/SonicteamMatrixStack.cpp). */
struct UnkMotionHeapBlock_8040C2B8 {
    /* The top matrix. */
    Mtx34f *Top() {
        return &m_matrices[m_top];
    }

    /* 0x0 */ UnkMatrix_800B1304 *m_matrices;
    /* 0x4 */ int m_capacity;
    /* 0x8 */ int m_top;
};
SIZE_ASSERT(UnkMotionHeapBlock_8040C2B8, 0xC);

/* The smallest float step (FLT_EPSILON; declared as MSL declares it, so it is not small data). */
extern "C" float lbl_805EDCF0[];

extern "C" {
Mtx34f *fn_80411700(UnkMotionHeapBlock_8040C2B8 *stack);
float fn_8000FEE4(float x); /* square root */
void fn_8040BAC4(Mtx34f *m, const Mtx34f *other);
void fn_8040C204(UnkMotionHeapBlock_8040C2B8 *stack);
void fn_8040C244(UnkMotionHeapBlock_8040C2B8 *stack);

void fn_804116F8(UnkMotionHeapBlock_8040C2B8 *stack);
void fn_80411634(Mtx34f *out, const Mtx34f *load, const Mtx34f *m);
void fn_8041157C(Vec3f *out, const Mtx34f *m, const Vec3f *v);
void fn_80411548(Vec3f *out, const Vec3f *a, const Vec3f *b);
void fn_804114D0(Vec3f *out, const Mtx34f *m);
void fn_804113E8(Mtx34f *out, const Mtx34f *rotation);
void fn_80411328(float *scale, const Mtx34f *m);
void fn_8041116C(Mtx34f *out, const Mtx34f *m);
}

/* The current matrix stack. */
UnkMotionHeapBlock_8040C2B8 *lbl_805F13A8;

/* The matrix copy through fn_8040C148, inline (as in Game/SonicteamMatrix.cpp). */
inline void CopyMatrix(Mtx34f *m, const Mtx34f *from) {
    fn_8040C148(m, from);
}

/* The top matrix of a stack. */
Mtx34f *fn_80411700(UnkMotionHeapBlock_8040C2B8 *stack) {
    return stack->Top();
}

/* Installs the current matrix stack. */
void fn_804116F8(UnkMotionHeapBlock_8040C2B8 *stack) {
    lbl_805F13A8 = stack;
}

/* Multiplies m onto the top matrix, first loading load into it when given; with out, the stack is
 * pushed first, the result goes to out and the stack is popped again. */
void fn_80411634(Mtx34f *out, const Mtx34f *load, const Mtx34f *m) {
    if (out != 0) {
        fn_8040C204(lbl_805F13A8);
        if (load != 0) {
            CopyMatrix(fn_80411700(lbl_805F13A8), load);
        }
        fn_8040BAC4(lbl_805F13A8->Top(), m);
        CopyMatrix(out, fn_80411700(lbl_805F13A8));
        fn_8040C244(lbl_805F13A8);
    } else {
        if (load != 0) {
            CopyMatrix(fn_80411700(lbl_805F13A8), load);
        }
        fn_8040BAC4(lbl_805F13A8->Top(), m);
    }
}

/* v rotated by m's rotation part (the top matrix when m is 0). */
void fn_8041157C(Vec3f *out, const Mtx34f *m, const Vec3f *v) {
    Mtx34f top;
    if (m == 0) {
        CopyMatrix(&top, fn_80411700(lbl_805F13A8));
        m = &top;
    }
    Vec3f result;
    result.x = v->x * m->values[0][0] + v->y * m->values[0][1] + v->z * m->values[0][2];
    result.y = v->x * m->values[1][0] + v->y * m->values[1][1] + v->z * m->values[1][2];
    result.z = v->x * m->values[2][0] + v->y * m->values[2][1] + v->z * m->values[2][2];
    fn_800091D0(out, &result);
}

void fn_80411548(Vec3f *out, const Vec3f *a, const Vec3f *b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

/* m's translation (the top matrix's when m is 0). */
void fn_804114D0(Vec3f *out, const Mtx34f *m) {
    if (m != 0) {
        out->x = m->values[0][3];
        out->y = m->values[1][3];
        out->z = m->values[2][3];
    } else {
        Mtx34f top;
        CopyMatrix(&top, fn_80411700(lbl_805F13A8));
        out->x = top.values[0][3];
        out->y = top.values[1][3];
        out->z = top.values[2][3];
    }
}

/* Copies the rotation part of a matrix. */
inline void CopyRotation(Mtx34f *out, const Mtx34f *rotation) {
    out->values[0][0] = rotation->values[0][0];
    out->values[1][0] = rotation->values[1][0];
    out->values[2][0] = rotation->values[2][0];
    out->values[0][1] = rotation->values[0][1];
    out->values[1][1] = rotation->values[1][1];
    out->values[2][1] = rotation->values[2][1];
    out->values[0][2] = rotation->values[0][2];
    out->values[1][2] = rotation->values[1][2];
    out->values[2][2] = rotation->values[2][2];
}

/* Sets out's rotation part (the top matrix's when out is 0). */
void fn_804113E8(Mtx34f *out, const Mtx34f *rotation) {
    if (out != 0) {
        CopyRotation(out, rotation);
    } else {
        Mtx34f top;
        CopyMatrix(&top, fn_80411700(lbl_805F13A8));
        CopyRotation(&top, rotation);
        CopyMatrix(fn_80411700(lbl_805F13A8), &top);
    }
}

/* The scale of each axis of m (the top matrix when m is 0). */
void fn_80411328(float *scale, const Mtx34f *m) {
    Mtx34f top;
    if (m == 0) {
        CopyMatrix(&top, fn_80411700(lbl_805F13A8));
        m = &top;
    }
    scale[0] = fn_8000FEE4(m->values[0][0] * m->values[0][0] + m->values[1][0] * m->values[1][0] + m->values[2][0] * m->values[2][0]);
    scale[1] = fn_8000FEE4(m->values[0][1] * m->values[0][1] + m->values[1][1] * m->values[1][1] + m->values[2][1] * m->values[2][1]);
    scale[2] = fn_8000FEE4(m->values[0][2] * m->values[0][2] + m->values[1][2] * m->values[1][2] + m->values[2][2] * m->values[2][2]);
}

/* m with its axes made unit length into out (the top matrix stands in for either when it is 0). */
void fn_8041116C(Mtx34f *out, const Mtx34f *m) {
    Mtx34f source;
    Mtx34f result;
    if (m == 0) {
        CopyMatrix(&source, fn_80411700(lbl_805F13A8));
        m = &source;
    }
    Mtx34f *target;
    if (out == 0) {
        CopyMatrix(&result, fn_80411700(lbl_805F13A8));
        target = &result;
    } else {
        target = out;
    }
    if (target != m) {
        CopyMatrix(target, m);
    }
    {
        float length = fn_8000FEE4(target->values[0][0] * target->values[0][0] + target->values[1][0] * target->values[1][0] + target->values[2][0] * target->values[2][0]);
        if (length > lbl_805EDCF0[0]) {
            float scale = 1.0f / length;
            target->values[0][0] *= scale;
            target->values[1][0] *= scale;
            target->values[2][0] *= scale;
        }
    }
    {
        float length = fn_8000FEE4(target->values[0][1] * target->values[0][1] + target->values[1][1] * target->values[1][1] + target->values[2][1] * target->values[2][1]);
        if (length > lbl_805EDCF0[0]) {
            float scale = 1.0f / length;
            target->values[0][1] *= scale;
            target->values[1][1] *= scale;
            target->values[2][1] *= scale;
        }
    }
    {
        float length = fn_8000FEE4(target->values[0][2] * target->values[0][2] + target->values[1][2] * target->values[1][2] + target->values[2][2] * target->values[2][2]);
        if (length > lbl_805EDCF0[0]) {
            float scale = 1.0f / length;
            target->values[0][2] *= scale;
            target->values[1][2] *= scale;
            target->values[2][2] *= scale;
        }
    }
    if (out == 0) {
        CopyMatrix(fn_80411700(lbl_805F13A8), target);
    }
}
