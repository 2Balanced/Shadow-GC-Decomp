/* RenderWare 2D toolkit (rt2d), the path module continued from RwCpp37 (fn_80474BEC on): curve
 * and close, flattening (curves halved into lines), dropping empty sub-paths, destroy, empty,
 * create, lock and unlock, and the module's open and close. The path module's source file ends
 * with this unit (0x80476ABC); its .sdata2 constants are 0x805FB268..0x805FB278.
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * Built as C: -O4,p, no stmw/lmw, automatic inlining; the recursion depths retail inlines are set
 * per function with #pragma inline_depth. */
#include "Game/rw/r5.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The path module's constants, 0x805FB268..0x805FB278 in .sdata2, are shared with the end of
 * RwCpp37, so neither unit can own them: where the code allows it they are read through these
 * names, elsewhere as literals (those functions then differ from retail in the constant's name). */
extern const f32 lbl_805FB268; /* 0.0f */
extern const f32 lbl_805FB26C; /* 1.0f */
extern const f32 lbl_805FB270; /* 0.5f */
extern s32 lbl_805EDE50;       /* paths per free list block (32) */
extern s32 lbl_805EDE54;       /* blocks to preallocate (1) */
extern u8 lbl_805E26A8[0x28];  /* the path free list's own storage */

void *memcpy(void *dst, const void *src, u32 n);
f32 fn_80482340(f32 x); /* square root */
f32 fn_80482394(f32 x); /* inverse square root */

/* Frees one sub-path and returns the one after it. This is fn_80474BEC of RwCpp37 again: the two
 * units are one source file, and retail inlines the function into the destroy function below
 * (and calls it where the inlining stops, so the copy here keeps the name; it is `inline` so that
 * its out-of-line copy is weak and RwCpp37's is the one linked). */
inline Rt2dPath *fn_80474BEC(Rt2dPath *path) {
    Rt2dPath *next = path->next;

    if (path->segments != 0) {
        if (path->numSegment == -1) {
            fn_8047DFC8(path->segments);
        } else {
            RWSRCGLOBAL(rwfree)(path->segments);
        }
    }
    path->segments = 0;
    RWSRCGLOBAL(memoryFree)(lbl_805E1950.pathFreeList, path);
    return next;
}
void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);         /* RwFreeListCreateAndPreallocateSpace */
RwBool fn_8047FCDC(void *freeList);  /* RwFreeListDestroy */

Rt2dPath *fn_80476510(Rt2dPath *path);
f32 fn_80474168(void); /* the squared flatness tolerance in the current coordinate system */

/* Adds a line to `pos` to a sub-path and gives the corner before it the bisecting normal. */
static void SubPathLineto(Rt2dPath *sub, const RwV2d *pos) {
    s32 count = fn_8047E03C(sub->segments);
    Rt2dPathNode *node = (Rt2dPathNode *)fn_8047DDBC(sub->segments, 0x30190);
    f32 length;
    f32 squared;
    f32 dx;
    f32 dy;
    f32 inverse;

    node->type = 1;
    node->pos.x = pos->x;
    node->pos.y = pos->y;
    dy = pos->y - node[-1].pos.y;
    dx = node[-1].pos.x - pos->x;
    squared = dy * dy + dx * dx;
    length = fn_80482340(squared);
    inverse = fn_80482394(squared);
    node->normal.x = dy * inverse;
    node->normal.y = dx * inverse;
    node->dist = node[-1].dist + length;
    if (count > 1) {
        f32 scale;
        f32 sumY;
        f32 sumX;
        f32 px = node[-1].normal.x;
        f32 py = node[-1].normal.y;

        sumY = py + node->normal.y;
        sumX = px + node->normal.x;
        scale = lbl_805FB26C / (px * sumX + py * sumY);
        node[-1].normal.x = sumX * scale;
        node[-1].normal.y = sumY * scale;
    } else {
        node[-1].normal = node->normal;
    }
}

/* Adds a cubic curve through (x1, y1) and (x2, y2) to (x3, y3): three curve nodes (the second
 * and third with locals of their own, declared in reverse order of use, which gives retail's
 * f24..f26). */
Rt2dPath *fn_80474FF8(Rt2dPath *path, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3) {
    s32 count = fn_8047E03C(path->curr->segments);
    Rt2dPathNode *node = (Rt2dPathNode *)fn_8047DEA0(path->curr->segments, 3, 0x30190);
    f32 length;
    f32 dy;
    f32 dx;
    f32 squared;
    f32 inverse;

    node[0].type = 2;
    node[0].pos.x = x1;
    node[0].pos.y = y1;
    dy = y1 - node[-1].pos.y;
    dx = node[-1].pos.x - x1;
    squared = dy * dy + dx * dx;
    length = fn_80482340(squared);
    inverse = fn_80482394(squared);
    node[0].normal.x = dy * inverse;
    node[0].normal.y = dx * inverse;
    node[0].dist = node[-1].dist + length;
    if (count > 1) {
        f32 sumX;
        f32 sumY;
        f32 scale;

        sumY = node[-1].normal.y + node[0].normal.y;
        sumX = node[-1].normal.x + node[0].normal.x;
        scale = lbl_805FB26C / (node[-1].normal.x * sumX + node[-1].normal.y * sumY);
        node[-1].normal.x = sumX * scale;
        node[-1].normal.y = sumY * scale;
    } else {
        node[-1].normal = node[0].normal;
    }

    {
        f32 inverse;
        f32 squared;
        f32 dy;
        f32 dx;
        f32 length;

        node[1].type = 2;
        node[1].pos.x = x2;
        node[1].pos.y = y2;
        dy = y2 - node[0].pos.y;
        dx = node[0].pos.x - x2;
        squared = dy * dy + dx * dx;
        length = fn_80482340(squared);
        inverse = fn_80482394(squared);
        node[1].normal.x = dy * inverse;
        node[1].normal.y = dx * inverse;
        node[1].dist = node[0].dist + length;
    }
    {
        f32 inverse;
        f32 squared;
        f32 dy;
        f32 dx;
        f32 length;

        node[2].type = 2;
        node[2].pos.x = x3;
        node[2].pos.y = y3;
        dy = y3 - node[1].pos.y;
        dx = node[1].pos.x - x3;
        squared = dy * dy + dx * dx;
        length = fn_80482340(squared);
        inverse = fn_80482394(squared);
        node[2].normal.x = dy * inverse;
        node[2].normal.y = dx * inverse;
        node[2].dist = node[1].dist + length;
    }

    path->curr->flat = 0;
    return path;
}

/* Adds a line to (x, y). This is fn_80474EA0 of RwCpp37 again (see fn_80474BEC above): retail
 * inlines it into the close function. */
inline Rt2dPath *fn_80474EA0(Rt2dPath *path, f32 x, f32 y) {
    s32 count = fn_8047E03C(path->curr->segments);
    Rt2dPathNode *node = (Rt2dPathNode *)fn_8047DDBC(path->curr->segments, 0x30190);
    f32 length;
    f32 squared;
    f32 dx;
    f32 dy;
    f32 inverse;

    node->type = 1;
    node->pos.x = x;
    node->pos.y = y;
    dy = y - node[-1].pos.y;
    dx = node[-1].pos.x - x;
    squared = dy * dy + dx * dx;
    length = fn_80482340(squared);
    inverse = fn_80482394(squared);
    node->normal.x = dy * inverse;
    node->normal.y = dx * inverse;
    node->dist = node[-1].dist + length;
    if (count > 1) {
        f32 scale;
        f32 sumY;
        f32 sumX;
        f32 px = node[-1].normal.x;
        f32 py = node[-1].normal.y;

        sumY = py + node->normal.y;
        sumX = px + node->normal.x;
        scale = lbl_805FB26C / (px * sumX + py * sumY);
        node[-1].normal.x = sumX * scale;
        node[-1].normal.y = sumY * scale;
    } else {
        node[-1].normal = node->normal;
    }
    return path;
}

/* Closes the current sub-path: a line back to its start if it does not end there, and one normal
 * for the shared corner.
 * NonMatching (8 rows): float register numbering of the two differences and of the start's y
 * (retail f30, coalesced with the inlined line's dy; here f28). 120 declaration orders of the
 * inlined line, 6 of this function and four spellings of the corner test were tried. */
#pragma auto_inline off
Rt2dPath *fn_80475260(Rt2dPath *path) {
    Rt2dPath *sub = path->curr;
    s32 count = fn_8047E03C(sub->segments);
    Rt2dPathNode *first = (Rt2dPathNode *)fn_8047DFAC(sub->segments);
    Rt2dPathNode *last = (Rt2dPathNode *)fn_8047E0CC(sub->segments, count - 1);
    RwV2d start;
    f32 dx;
    f32 dy;

    start = first->pos;
    dy = start.y - last->pos.y;
    dx = last->pos.x - start.x;
    if (lbl_805FB268 != dy || lbl_805FB268 != dx) {
        fn_80474EA0(path, start.x, start.y);
        first = (Rt2dPathNode *)fn_8047DFAC(sub->segments);
        last = (Rt2dPathNode *)fn_8047E0CC(sub->segments, count);
    }
    {
        f32 sumX;
        f32 sumY;
        f32 scale;

        sumY = first->normal.y + last->normal.y;
        sumX = first->normal.x + last->normal.x;
        scale = lbl_805FB26C / (first->normal.x * sumX + first->normal.y * sumY);
        last->normal.x = sumX * scale;
        last->normal.y = sumY * scale;
    }
    first->normal = last->normal;
    sub->closed = 1;
    return path;
}
#pragma auto_inline reset

/* Empties the toolkit's scratch path. */
Rt2dPath *fn_8047548C(void) {
    Rt2dPath *path = lbl_805E1950.scratchPath;

    fn_80475E2C(path->next);
    path->next = 0;
    fn_8047DDB0(path->segments);
    path->closed = 0;
    path->flat = 1;
    path->curr = path;
    return lbl_805E1950.scratchPath;
}

/* Flattens the cubic curve p[0..3] into lines in `sub`: halves it until it lies within the
 * flatness tolerance of its chord, or `depth` halvings have been made.
 * NonMatching (same instructions and stack layout; ~50 float registers differ): retail numbers the
 * chord test's values x0 f8, y0 f9, x1 f11, y1 f0 and the halving sums from f6 down, and gives the
 * second inlined line dx f30, dy f29. Declaration orders (2000 sampled) and three statement orders
 * of the halving were tried. */
void fn_804754F4(Rt2dPath *sub, const RwV2d *p, s32 depth) {
    RwBool stop = --depth < 0;

    if (stop) {
        SubPathLineto(sub, &p[3]);
    } else {
        f32 x1 = p[1].x;
        f32 bx;
        f32 by;
        f32 blen;
        f32 ax;
        f32 ay;
        f32 dot;
        f32 y0 = p[0].y;
        RwBool flat;
        f32 tolerance = lbl_805E1950.flatDepth;
        f32 y1 = p[1].y;
        f32 x0 = p[0].x;
        bx = p[3].x - x0;
        by = p[3].y - y0;
        blen = bx * bx + by * by;
        ax = x1 - x0;
        ay = y1 - y0;
        dot = ax * bx + ay * by;

        flat = blen * ((ax * ax + ay * ay) - tolerance) < dot * dot;
        if (flat) {
            f32 cx = p[2].x - x0;
            f32 cy = p[2].y - y0;

            dot = cx * bx + cy * by;
            flat = blen * ((cx * cx + cy * cy) - tolerance) < dot * dot;
        }
        if (flat) {
            SubPathLineto(sub, &p[3]);
        } else {
            RwV2d left[4];
            RwV2d right[4];
            RwV2d mid;

            left[1].x = lbl_805FB270 * (x0 + x1);
            left[1].y = lbl_805FB270 * (y0 + y1);
            mid.x = lbl_805FB270 * (x1 + p[2].x);
            mid.y = lbl_805FB270 * (y1 + p[2].y);
            right[2].x = lbl_805FB270 * (p[3].x + p[2].x);
            right[2].y = lbl_805FB270 * (p[3].y + p[2].y);
            left[2].x = lbl_805FB270 * (left[1].x + mid.x);
            left[2].y = lbl_805FB270 * (left[1].y + mid.y);
            right[1].x = lbl_805FB270 * (right[2].x + mid.x);
            right[1].y = lbl_805FB270 * (right[2].y + mid.y);
            left[3].x = lbl_805FB270 * (left[2].x + right[1].x);
            left[3].y = lbl_805FB270 * (left[2].y + right[1].y);
            left[0] = p[0];
            right[0] = left[3];
            right[3] = p[3];
            fn_804754F4(sub, left, depth);
            fn_804754F4(sub, right, depth);
        }
    }
}

/* Flattens `src` into `dst`: lines and moves are copied, curves become lines. */
Rt2dPath *fn_804758C8(Rt2dPath *dst, const Rt2dPath *src) {
    s32 count;
    Rt2dPathNode *node;
    s32 i;

    lbl_805E1950.flatDepth = fn_80474168();
    if (src->numSegment == -1) {
        node = (Rt2dPathNode *)fn_8047DFAC(src->segments);
    } else {
        node = (Rt2dPathNode *)src->segments;
    }
    count = src->numSegment;
    if (count == -1) {
        count = fn_8047E03C(src->segments);
    }
    for (i = 0; i < count; i++, node++) {
        if (node->type == 2) {
            RwV2d curve[4];

            curve[0] = node[-1].pos;
            curve[1] = node[0].pos;
            curve[2] = node[1].pos;
            curve[3] = node[2].pos;
            fn_804754F4(dst, curve, lbl_805E1950.field_0x330);
            i += 2;
            node += 2;
        } else if (node->type == 1) {
            SubPathLineto(dst, &node->pos);
        } else {
            *(Rt2dPathNode *)fn_8047DDBC(dst->segments, 0x301A1) = *node;
        }
    }
    if (src->closed) {
        fn_80475260(dst);
    }
    return dst;
}

/* Drops the sub-paths that hold nothing but their starting point. Inlined five times. */
#pragma inline_depth(5)
Rt2dPath *fn_80475B4C(Rt2dPath *path) {
    if (path != 0) {
        path->next = fn_80475B4C(path->next);
        if (fn_8047E03C(path->segments) == 1) {
            path = fn_80474BEC(path);
        }
    }
    return path;
}

#pragma inline_depth(8)

/* Destroys a path and the sub-paths chained to it. Retail inlines the recursion seven times
 * (depth 7; see the note at fn_80474C7C in RwCpp37). */
#pragma inline_depth(7)
RwBool fn_80475E2C(Rt2dPath *path) {
    RwBool ok = path != 0;

    if (ok) {
        fn_80475E2C(fn_80474BEC(path));
    }
    return ok;
}

#pragma inline_depth(8)

/* Empties a path: frees its nodes and destroys the sub-paths chained to it. */
void fn_804760C8(Rt2dPath *path) {
    if (path->numSegment == -1) {
        fn_8047DFC8(path->segments);
    } else if (path->segments != 0) {
        RWSRCGLOBAL(rwfree)(path->segments);
    }
    path->segments = 0;
    path->numSegment = 0;
    if (path->next != 0) {
        fn_804760C8(path->next);
        fn_80475E2C(path->next);
        path->next = 0;
    }
}

/* A new, empty, locked path. */
Rt2dPath *fn_80476490(void) {
    Rt2dPath *path = (Rt2dPath *)RWSRCGLOBAL(memoryAlloc)(lbl_805E1950.pathFreeList, 0x301A1);

    if (path != 0) {
        path->segments = 0;
        path->numSegment = 0;
        path->closed = 0;
        path->flat = 1;
        path->next = 0;
        path->curr = path;
        path->inset = lbl_805FB268;
        fn_80476510(path);
    }
    return path;
}

/* Locks a path for editing: every sub-path's node array becomes a growable list. Inlined three
 * times.
 * NonMatching (53 rows): the outer level matches; in the inlined levels retail keeps the next
 * sub-path in r28 and its list in r29 where this source reuses r27 for the sub-path. Six
 * declaration orders and an early-return form were tried. */
#pragma inline_depth(3)
Rt2dPath *fn_80476510(Rt2dPath *path) {
    Rt2dPath *result = path;

    if (path != 0 && path->numSegment != -1) {
        RwSList *list = fn_8047DC90(sizeof(Rt2dPathNode), 0x301A1);
        Rt2dPathNode *nodes;
        s32 count;

        if (list != 0) {
            if (path->numSegment == -1) {
                nodes = (Rt2dPathNode *)fn_8047DFAC(path->segments);
            } else {
                nodes = (Rt2dPathNode *)path->segments;
            }
            count = path->numSegment;
            if (count == -1) {
                count = fn_8047E03C(path->segments);
            }
            if (count != 0) {
                void *copy = fn_8047DEA0(list, count, 0x301A1);

                if (copy != 0) {
                    memcpy(copy, nodes, count * sizeof(Rt2dPathNode));
                }
                RWSRCGLOBAL(rwfree)(nodes);
            }
            path->numSegment = -1;
            path->segments = list;
            fn_80476510(path->next);
        } else {
            result = 0;
        }
    }
    return result;
}

#pragma inline_depth(8)

/* Unlocks a path: every sub-path's list becomes a plain array again. Inlined seven times. */
#pragma inline_depth(7)
Rt2dPath *fn_80476830(Rt2dPath *path) {
    if (path != 0 && path->numSegment == -1) {
        s32 count = fn_8047E03C(path->segments);

        path->segments = (RwSList *)fn_8047E044(path->segments);
        path->numSegment = count;
        fn_80476830(path->next);
    }
    return path;
}

#pragma inline_depth(8)

/* Closes the path module. */
void fn_80476A0C(void) {
    fn_80475E2C(lbl_805E1950.scratchPath);
    fn_8047FCDC(lbl_805E1950.pathFreeList);
    lbl_805E1950.pathFreeList = 0;
}

/* Opens the path module: the free list of paths and the scratch path. */
RwBool fn_80476A58(void) {
    lbl_805E1950.pathFreeList =
        fn_8047FC04(sizeof(Rt2dPath), lbl_805EDE50, 4, lbl_805EDE54, lbl_805E26A8, 0x401A1);
    lbl_805E1950.scratchPath = fn_80476490();
    fn_80476510(lbl_805E1950.scratchPath);
    return 1;
}

#ifdef __cplusplus
}
#endif
