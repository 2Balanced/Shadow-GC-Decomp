/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * NonMatching: the point transforms fn_80482458 and fn_804824A4 below remain
 * scalar C++; retail uses paired-single arithmetic (psq_l / ps_madds0 /
 * ps_madds1 / psq_st) that GC/2.7 does not generate from these C++ forms, and
 * fn_804824A4 keeps a plain loop instead of the retail count-controlled bdnz with
 * one prefetched point. fn_804823F0 routes its float result through one stack
 * slot that retail reloads repeatedly. No inline assembly is used. */
#include <dolphin/types.h>

extern "C" {
void fn_80482F60(void) {
}

u32 fn_804829D4(void) {
    return 1;
}

char *fn_80482DF0(char *text, char value) {
    char *found = NULL;
    char current;
    do {
        current = *text;
        if (current == value) {
            found = text;
        }
        text++;
    } while (current != 0);
    return found;
}

char *fn_80482DBC(char *text, char value) {
    char *found = NULL;
    char current;
    do {
        current = *text;
        if (current == value) {
            found = text;
            break;
        }
        text++;
    } while (current != 0);
    return found;
}

char *fn_80482D3C(char *text) {
    if (text == NULL) {
        return text;
    }
    char *cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'a' && current <= 'z') {
            *cursor = current - 0x20;
        }
        cursor++;
    }
    return text;
}

char *fn_80482D7C(char *text) {
    if (text == NULL) {
        return text;
    }
    char *cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'A' && current <= 'Z') {
            *cursor = current + 0x20;
        }
        cursor++;
    }
    return text;
}

extern u32 lbl_805FB2D0;
extern u8 *lbl_805F15B4;

u32 fn_80482F64(u32 argument) {
    if (argument == 0) {
        argument = (u32)&lbl_805FB2D0;
    }
    u32 result = ((u32 (*)(u32))*(u32 *)(lbl_805F15B4 + 0xF4))(argument);
    return (result + 4) & ~3;
}

extern u32 lbl_805F1598;

union RwCpp46FloatBits {
    float value;
    u32 bits;
};

struct RwCpp46V3d {
    float x;
    float y;
    float z;
};

struct RwCpp46Matrix {
    /* 0x00 */ RwCpp46V3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwCpp46V3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwCpp46V3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwCpp46V3d pos;
    /* 0x3C */ u32 pad3;
};

/* Bit-level reworking of the squared length of the two leading components of
 * vector. The squared length is held in one stack slot and re-read as integer
 * bits: the table pointer kept at engine block + lbl_805F1598 is indexed with
 * bits 18..29 of ((bits + 2048) rotated left 22), and the entry that is read
 * there is offset by ((bits + 2048) >> 1) & 0x3FC before being returned as a
 * float. A zero bit pattern skips the table lookup entirely. The 0x40000 table
 * stride and the 0x3FC addend follow the retail rotate/mask fields directly. */
float fn_804823F0(const float *vector) {
    RwCpp46FloatBits squared;

    squared.value = vector[0] * vector[0] + vector[1] * vector[1];
    if (squared.bits != 0) {
        u32 adjusted = squared.bits + 2048;
        u8 *table = *(u8 **)(lbl_805F15B4 + lbl_805F1598);
        u32 index = ((adjusted << 22) | (adjusted >> 10)) & 0x3FFC0000;
        squared.bits = ((adjusted >> 1) & 0x3FC) + *(u32 *)(table + index);
    }
    return squared.value;
}

/* Transform one point by a RenderWare matrix: point = source * matrix, with the
 * basis vectors held as the matrix rows right/up/at and the translation as pos.
 * Retail accumulates (x, y) as one paired-single lane and z in the companion
 * lane, starting from the translation: pos + source.x * right, then
 * + source.y * up, then + source.z * at.
 * NonMatching: the GC/2.7 compiler does not turn these C++ forms into the retail
 * psq_l / ps_madds0 / ps_madds1 / psq_st pair sequence, so the transform stays
 * scalar float arithmetic. No inline assembly is used. */
void fn_80482458(RwCpp46V3d *point, const RwCpp46V3d *source,
                 const RwCpp46Matrix *matrix) {
    float x = matrix->pos.x + source->x * matrix->right.x;
    float y = matrix->pos.y + source->x * matrix->right.y;
    float z = matrix->pos.z + source->x * matrix->right.z;

    x = x + source->y * matrix->up.x;
    y = y + source->y * matrix->up.y;
    z = z + source->y * matrix->up.z;

    x = x + source->z * matrix->at.x;
    y = y + source->z * matrix->at.y;
    z = z + source->z * matrix->at.z;

    point->x = x;
    point->y = y;
    point->z = z;
}

/* Counted form of fn_80482458: transform count points from sources into points.
 * Retail keeps the matrix in eight paired-single registers across the whole call,
 * preloads the first point before the loop and advances the source and
 * destination cursors by one point with the paired-single auto-increment
 * accesses (the destination cursor starts four bytes below the array). The loop
 * is a count-controlled bdnz, so it always transforms at least one point; a count
 * of zero wraps. Same paired-single codegen floor as fn_80482458. */
void fn_804824A4(RwCpp46V3d *points, const RwCpp46V3d *sources, s32 count,
                 const RwCpp46Matrix *matrix) {
    s32 remaining = count;

    do {
        float x = matrix->pos.x + sources->x * matrix->right.x;
        float y = matrix->pos.y + sources->x * matrix->right.y;
        float z = matrix->pos.z + sources->x * matrix->right.z;

        x = x + sources->y * matrix->up.x;
        y = y + sources->y * matrix->up.y;
        z = z + sources->y * matrix->up.z;

        x = x + sources->z * matrix->at.x;
        y = y + sources->z * matrix->at.y;
        z = z + sources->z * matrix->at.z;

        points->x = x;
        points->y = y;
        points->z = z;
        points++;
        sources++;
    } while (--remaining != 0);
}
}
