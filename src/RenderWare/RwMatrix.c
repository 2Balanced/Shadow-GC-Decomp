/* RenderWare matrices (RwMatrix*), 0x8047E100..0x8047FA28: the default multiply, orthonormalising,
 * the error measures, the module's close and open (engine plugin 0x402), the tolerances,
 * optimising, RwMatrixMultiply, rotation, RwMatrixInvert, RwMatrixScale, RwMatrixTranslate,
 * RwMatrixTransform, RwMatrixDestroy and RwMatrixCreate.
 *
 * One source file: its .rodata is the default tolerances (0x8051C2C0), its .data the message
 * "Invalid combination type" (0x8056DF50), its .bss the matrix free list (0x805E2A58), its .sdata
 * the free list's parameters (0x805EDE98), its .sbss the module's RwModuleInfo (0x805F1560) and its
 * .sdata2 the pool 0.0f, 1.0f, pi/180 (0x805FB2A0..0x805FB2B0). The default multiply at
 * 0x8047E100 is the function the module open stores, not an rwSList one.
 * Not written: fn_8047EBB8 (RwMatrixMultiply) is hand-written paired-single assembly in the
 * original (psq_l/ps_madds blocks behind a bare frame) and stays unwritten; fn_8047E298
 * (orthonormalise) and fn_8047E990 (optimise) are ordinary C not written yet.
 * NonMatching: fn_8047ECD4 (RwMatrixRotateOneMinusCosineSine: float register allocation and the
 * spill of the axis), fn_8047F0B0 (RwMatrixInvert: the general case's schedule and frame) and
 * fn_8047E750, fn_8047E7A8 (float register numbering).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C.
 * Flags: GC/2.7 -O4,p, no stmw/lmw, automatic inlining, -lang=c, writable pooled strings
 * (cflags_rw_r5). Measured per file against the other RenderWare flag sets: GC 2.0-2.7 give the
 * same code, GC 1.3.2, -O4,s, stmw/lmw and -inline noauto match fewer functions, and the
 * read-only string sets (cflags_rw_r3/r6/c) would move strings out of .data/.sdata. */
#include "Game/rw/r5.h"

void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);   /* RwFreeListCreateAndPreallocateSpace */
RwBool fn_8047FCDC(void *freeList); /* RwFreeListDestroy */
RwMatrix *fn_8047E100(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b);
RwMatrix *fn_8047EBB8(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b); /* RwMatrixMultiply (assembly) */
f32 fn_80482394(f32 x); /* inverse square root */
double sin(double x);
double cos(double x);

/* The matrix free list: matrices per block, blocks to preallocate, and the list itself. */
s32 lbl_805EDE98 = 256;
s32 lbl_805EDE9C = 1;
R5FreeList lbl_805E2A58;

/* The matrix module's bookkeeping. */
R5ModuleInfo lbl_805F1560;

#define R5MATRIXGLOBAL ((R5MatrixGlobals *)((u8 *)lbl_805F15B4 + lbl_805F1560.globalsOffset))

/* The matrix module's own multiply (C), which the module open installs: dst = a x b, flags
 * untouched. */
RwMatrix *fn_8047E100(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b) {
    dst->right.x = a->right.x * b->right.x + a->right.y * b->up.x + a->right.z * b->at.x;
    dst->right.y = a->right.x * b->right.y + a->right.y * b->up.y + a->right.z * b->at.y;
    dst->right.z = a->right.x * b->right.z + a->right.y * b->up.z + a->right.z * b->at.z;
    dst->up.x = a->up.x * b->right.x + a->up.y * b->up.x + a->up.z * b->at.x;
    dst->up.y = a->up.x * b->right.y + a->up.y * b->up.y + a->up.z * b->at.y;
    dst->up.z = a->up.x * b->right.z + a->up.y * b->up.z + a->up.z * b->at.z;
    dst->at.x = a->at.x * b->right.x + a->at.y * b->up.x + a->at.z * b->at.x;
    dst->at.y = a->at.x * b->right.y + a->at.y * b->up.y + a->at.z * b->at.y;
    dst->at.z = a->at.x * b->right.z + a->at.y * b->up.z + a->at.z * b->at.z;
    dst->pos.x = b->pos.x + (a->pos.x * b->right.x + a->pos.y * b->up.x + a->pos.z * b->at.x);
    dst->pos.y = b->pos.y + (a->pos.x * b->right.y + a->pos.y * b->up.y + a->pos.z * b->at.y);
    dst->pos.z = b->pos.z + (a->pos.x * b->right.z + a->pos.y * b->up.z + a->pos.z * b->at.z);
    return dst;
}

/* The determinant of the rotation part. */
f32 fn_8047E704(const RwMatrix *matrix) {
    RwV3d cross;

    cross.x = matrix->up.y * matrix->at.z - matrix->up.z * matrix->at.y;
    cross.y = matrix->up.z * matrix->at.x - matrix->up.x * matrix->at.z;
    cross.z = matrix->up.x * matrix->at.y - matrix->up.y * matrix->at.x;
    return cross.x * matrix->right.x + cross.y * matrix->right.y + cross.z * matrix->right.z;
}

/* How far the axes are from orthogonal: the sum of their squared dot products. */
f32 fn_8047E750(const RwMatrix *matrix) {
    f32 upAt = matrix->up.x * matrix->at.x + matrix->up.y * matrix->at.y + matrix->up.z * matrix->at.z;
    f32 atRight = matrix->at.x * matrix->right.x + matrix->at.y * matrix->right.y + matrix->at.z * matrix->right.z;
    f32 rightUp = matrix->right.x * matrix->up.x + matrix->right.y * matrix->up.y + matrix->right.z * matrix->up.z;

    return upAt * upAt + atRight * atRight + rightUp * rightUp;
}

/* How far the axes are from unit length. */
f32 fn_8047E7A8(const RwMatrix *matrix) {
    f32 rightError;
    RwV3d at;
    RwV3d right;
    RwV3d up;
    f32 atError;
    f32 upError;

    up.x = matrix->up.x * matrix->up.x;
    up.y = matrix->up.y * matrix->up.y;
    right.x = matrix->right.x * matrix->right.x;
    right.y = matrix->right.y * matrix->right.y;
    up.z = matrix->up.z * matrix->up.z;
    right.z = matrix->right.z * matrix->right.z;
    at.x = matrix->at.x * matrix->at.x;
    at.y = matrix->at.y * matrix->at.y;
    at.z = matrix->at.z * matrix->at.z;
    upError = up.x + up.y + up.z - 1.0f;
    rightError = right.x + right.y + right.z - 1.0f;
    atError = at.x + at.y + at.z - 1.0f;
    return rightError * rightError + upError * upError + atError * atError;
}

/* Closes the matrix module. */
void *fn_8047E828(void *instance) {
    if (R5MATRIXGLOBAL->freeList != 0) {
        fn_8047FCDC(R5MATRIXGLOBAL->freeList);
        R5MATRIXGLOBAL->freeList = 0;
    }
    lbl_805F1560.numInstances--;
    return instance;
}

RwBool fn_8047E960(const RwV3d *tolerance);

/* Opens the matrix module: the free list of matrices, the default optimizations and tolerances. */
void *fn_8047E88C(void *instance, s32 offset) {
    lbl_805F1560.globalsOffset = offset;
    R5MATRIXGLOBAL->freeList = fn_8047FC04(sizeof(RwMatrix), lbl_805EDE98, 4, lbl_805EDE9C, &lbl_805E2A58, 0x4000D);
    if (R5MATRIXGLOBAL->freeList == 0) {
        instance = 0;
    } else {
        RwV3d tolerance = {0.01f, 0.01f, 0.01f};

        R5MATRIXGLOBAL->optimizations = 0x20000;
        R5MATRIXGLOBAL->multiply = fn_8047E100;
        fn_8047E960(&tolerance);
        lbl_805F1560.numInstances++;
    }
    return instance;
}

/* RwMatrixSetTolerance. */
RwBool fn_8047E960(const RwV3d *tolerance) {
    R5MATRIXGLOBAL->tolerance = *tolerance;
    return 1;
}

void fn_8047EBA0(u32 *object) {
    object[3] &= 0xFFFDFFFC;
}

void fn_8047E298(void);

void fn_8047ECB4(void) {
    fn_8047E298();
}

#define R5MATRIXFREELIST (*(void **)((u8 *)lbl_805F15B4 + lbl_805F1560.globalsOffset))

/* Builds the rotation about the unit `axis` by the angle whose sine and 1 - cosine are given, and
 * replaces the matrix by it or combines it before (1) or after (2) the matrix. */
RwMatrix *fn_8047ECD4(RwMatrix *matrix, const RwV3d *axis, f32 oneMinusCos, f32 sinA, s32 combineOp) {

    RwMatrix rotation;
    RwMatrix result;
    RwV3d leading;
    RwV3d crossed;
    RwV3d sine;
    f32 x = axis->x;
    f32 y = axis->y;
    f32 z = axis->z;

    leading.x = x * x;
    leading.y = y * y;
    leading.z = z * z;
    leading.x = 1.0f - leading.x;
    leading.y = 1.0f - leading.y;
    leading.z = 1.0f - leading.z;
    crossed.x = y * z;
    crossed.y = z * x;
    crossed.z = x * y;
    leading.x *= oneMinusCos;
    leading.y *= oneMinusCos;
    leading.z *= oneMinusCos;
    crossed.x *= oneMinusCos;
    crossed.y *= oneMinusCos;
    crossed.z *= oneMinusCos;
    sine.x = x * sinA;
    sine.y = y * sinA;
    sine.z = z * sinA;
    rotation.pos.x = 0.0f;
    rotation.pos.y = 0.0f;
    rotation.pos.z = 0.0f;
    rotation.flags = 3;
    rotation.right.x = 1.0f - leading.x;
    rotation.up.y = 1.0f - leading.y;
    rotation.at.z = 1.0f - leading.z;
    rotation.up.z = crossed.x + sine.x;
    rotation.right.z = crossed.y - sine.y;
    rotation.at.x = crossed.y + sine.y;
    rotation.at.y = crossed.x - sine.x;
    rotation.right.y = crossed.z + sine.z;
    rotation.up.x = crossed.z - sine.z;
    switch (combineOp) {
    case 0:
        *matrix = rotation;
        break;
    case 1:
        fn_8047EBB8(&result, &rotation, matrix);
        *matrix = result;
        break;
    case 2:
        fn_8047EBB8(&result, matrix, &rotation);
        *matrix = result;
        break;
    default:
        R5ERROR(1, (0x80000003, "Invalid combination type"));
        matrix = 0;
        break;
    }
    return matrix;
}


/* RwMatrixRotate: the rotation about `axis` by `angle` degrees. */
RwMatrix *fn_8047EFC8(RwMatrix *matrix, const RwV3d *axis, f32 angle, s32 combineOp) {

    f32 sinA;
    f32 radians = (3.1415927f / 180.0f) * angle;
    f32 recip = fn_80482394(axis->x * axis->x + axis->y * axis->y + axis->z * axis->z);
    RwV3d unit;
    f32 cosA;

    unit.x = axis->x * recip;
    unit.y = axis->y * recip;
    unit.z = axis->z * recip;
    sinA = (f32)sin(radians);
    cosA = (f32)cos(radians);
    fn_8047ECD4(matrix, &unit, 1.0f - cosA, sinA, combineOp);
    return matrix;
}


/* RwMatrixInvert: a copy for an identity matrix, the transpose for an orthonormal one, else the
 * adjoint over the determinant. */
RwMatrix *fn_8047F0B0(RwMatrix *dst, const RwMatrix *src) {
    if (src->flags & R5MATRIXGLOBAL->optimizations & 0x20000) {
        *dst = *src;
    } else if ((src->flags & 3) == 3) {
        dst->right.x = src->right.x;
        dst->right.y = src->up.x;
        dst->right.z = src->at.x;
        dst->up.x = src->right.y;
        dst->up.y = src->up.y;
        dst->up.z = src->at.y;
        dst->at.x = src->right.z;
        dst->at.y = src->up.z;
        dst->at.z = src->at.z;
        dst->pos.x = -(src->pos.x * src->right.x + src->pos.y * src->right.y + src->pos.z * src->right.z);
        dst->pos.y = -(src->pos.x * src->up.x + src->pos.y * src->up.y + src->pos.z * src->up.z);
        dst->pos.z = -(src->pos.x * src->at.x + src->pos.y * src->at.y + src->pos.z * src->at.z);
        dst->flags = 3;
    } else {
        f32 det;
        f32 recip;

        dst->right.x = src->up.y * src->at.z - src->up.z * src->at.y;
        dst->right.y = src->right.z * src->at.y - src->right.y * src->at.z;
        dst->right.z = src->right.y * src->up.z - src->right.z * src->up.y;
        det = dst->right.x * src->right.x + dst->right.y * src->up.x + dst->right.z * src->at.x;
        if (*(s32 *)&det != 0) {
            recip = 1.0f / det;
        } else {
            recip = 1.0f;
        }
        dst->right.x *= recip;
        dst->right.y *= recip;
        dst->right.z *= recip;
        dst->up.x = recip * (src->up.z * src->at.x - src->up.x * src->at.z);
        dst->up.y = recip * (src->right.x * src->at.z - src->right.z * src->at.x);
        dst->up.z = recip * (src->right.z * src->up.x - src->right.x * src->up.z);
        dst->at.x = recip * (src->up.x * src->at.y - src->up.y * src->at.x);
        dst->at.y = recip * (src->right.y * src->at.x - src->right.x * src->at.y);
        dst->at.z = recip * (src->right.x * src->up.y - src->right.y * src->up.x);
        dst->pos.x = -(src->pos.x * dst->right.x + src->pos.y * dst->up.x + src->pos.z * dst->at.x);
        dst->pos.y = -(src->pos.x * dst->right.y + src->pos.y * dst->up.y + src->pos.z * dst->at.y);
        dst->pos.z = -(src->pos.x * dst->right.z + src->pos.y * dst->up.z + src->pos.z * dst->at.z);
        dst->flags = 0;
    }
    return dst;
}

#define SETIDENTITY(m)                do {                                  (m)->at.z = 1.0f;                 (m)->up.y = 1.0f;                 (m)->right.x = 1.0f;              (m)->up.x = 0.0f;                 (m)->right.z = 0.0f;              (m)->right.y = 0.0f;              (m)->at.y = 0.0f;                 (m)->at.x = 0.0f;                 (m)->up.z = 0.0f;                 (m)->pos.z = 0.0f;                (m)->pos.y = 0.0f;                (m)->pos.x = 0.0f;                (m)->flags |= 0x20003;        } while (0)

/* RwMatrixScale: replaces the matrix by a scale, or scales its axes before (1) or after (2) it. */
RwMatrix *fn_8047F398(RwMatrix *matrix, const RwV3d *scale, s32 combineOp) {
    switch (combineOp) {
    case 0:
        SETIDENTITY(matrix);
        matrix->right.x = scale->x;
        matrix->up.y = scale->y;
        matrix->at.z = scale->z;
        break;
    case 1:
        matrix->right.x *= scale->x;
        matrix->right.y *= scale->x;
        matrix->right.z *= scale->x;
        matrix->up.x *= scale->y;
        matrix->up.y *= scale->y;
        matrix->up.z *= scale->y;
        matrix->at.x *= scale->z;
        matrix->at.y *= scale->z;
        matrix->at.z *= scale->z;
        break;
    case 2:
        matrix->right.x *= scale->x;
        matrix->right.y *= scale->y;
        matrix->right.z *= scale->z;
        matrix->up.x *= scale->x;
        matrix->up.y *= scale->y;
        matrix->up.z *= scale->z;
        matrix->at.x *= scale->x;
        matrix->at.y *= scale->y;
        matrix->at.z *= scale->z;
        matrix->pos.x *= scale->x;
        matrix->pos.y *= scale->y;
        matrix->pos.z *= scale->z;
        break;
    default:
        R5ERROR(1, (0x80000003, "Invalid combination type"));
        matrix = 0;
        break;
    }
    matrix->flags &= ~0x20003;
    return matrix;
}

/* RwMatrixTranslate: replaces the matrix by a translation, or translates it in its own frame (1) or
 * the parent's (2). */
RwMatrix *fn_8047F59C(RwMatrix *matrix, const RwV3d *translation, s32 combineOp) {
    switch (combineOp) {
    case 0:
        SETIDENTITY(matrix);
        matrix->pos.x = translation->x;
        matrix->pos.y = translation->y;
        matrix->pos.z = translation->z;
        break;
    case 1:
        matrix->pos.x += translation->x * matrix->right.x + translation->y * matrix->up.x +
                         translation->z * matrix->at.x;
        matrix->pos.y += translation->x * matrix->right.y + translation->y * matrix->up.y +
                         translation->z * matrix->at.y;
        matrix->pos.z += translation->x * matrix->right.z + translation->y * matrix->up.z +
                         translation->z * matrix->at.z;
        break;
    case 2:
        matrix->pos.x += translation->x;
        matrix->pos.y += translation->y;
        matrix->pos.z += translation->z;
        break;
    default:
        R5ERROR(1, (0x80000003, "Invalid combination type"));
        matrix = 0;
        break;
    }
    matrix->flags &= ~0x20000;
    return matrix;
}

/* RwMatrixTransform: replaces the matrix by `transform`, or multiplies it before (1) or after (2). */
RwMatrix *fn_8047F72C(RwMatrix *matrix, const RwMatrix *transform, s32 combineOp) {
    switch (combineOp) {
    case 0:
        *matrix = *transform;
        break;
    case 1: {
        RwMatrix result;

        fn_8047EBB8(&result, transform, matrix);
        *matrix = result;
        break;
    }
    case 2: {
        RwMatrix result;

        fn_8047EBB8(&result, matrix, transform);
        *matrix = result;
        break;
    }
    default:
        R5ERROR(1, (0x80000003, "Invalid combination type"));
        matrix = 0;
        break;
    }
    return matrix;
}

/* RwMatrixDestroy. */
RwBool fn_8047F958(RwMatrix *matrix) {
    RWSRCGLOBAL(memoryFree)(R5MATRIXFREELIST, matrix);
    return 1;
}

/* RwMatrixCreate: a new identity matrix. */
RwMatrix *fn_8047F994(void) {
    RwMatrix *matrix = (RwMatrix *)RWSRCGLOBAL(memoryAlloc)(R5MATRIXFREELIST, 0x3000D);

    if (matrix != 0) {
        matrix->flags = 3;
        matrix->at.z = 1.0f;
        matrix->up.y = 1.0f;
        matrix->right.x = 1.0f;
        matrix->up.x = 0.0f;
        matrix->right.z = 0.0f;
        matrix->right.y = 0.0f;
        matrix->at.y = 0.0f;
        matrix->at.x = 0.0f;
        matrix->up.z = 0.0f;
        matrix->pos.z = 0.0f;
        matrix->pos.y = 0.0f;
        matrix->pos.x = 0.0f;
        matrix->flags |= 0x20003;
    }
    return matrix;
}
