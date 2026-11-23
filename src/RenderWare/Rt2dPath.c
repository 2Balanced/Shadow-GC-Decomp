/* RenderWare 2D toolkit (rt2d): paths (Rt2dPath*), 0x80474BEC..0x80476ABC: building sub-paths
 * (moveto, lineto, curveto, close), flattening curves into lines, destroy, empty, create, lock
 * and unlock, and the module's close and open.
 *
 * One source file: its constants are one .sdata2 pool (0.0f, 1.0f, 0.5f, 0x805FB268..0x805FB278;
 * the next file repeats 1.0f), the module open uses its own .sdata (the free list's block size
 * and preallocation, 0x805EDE50..0x805EDE58) and .bss (the free list itself, 0x805E26A8, padded to
 * 0x805E26D0), and retail inlines the sub-path free (fn_80474BEC) into the destroy function and
 * the lineto (fn_80474EA0) into the close function. The stream reader and the stroke renderer that
 * follow at 0x80476ABC belong to the next file (Rt2dStroke.c): the stroke's constant starts a new
 * pool, and the path's free list is followed by the link's padding before the brush's.
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C, built with cflags_rw_r5 (-O4,p, no stmw/lmw, automatic inlining in definition order); the
 * recursion depths retail inlines are set per function with #pragma inline_depth. */
#include "Game/rw/r5.h"

void *memcpy(void *dst, const void *src, u32 n);
f32 fn_80482340(f32 x); /* square root */
f32 fn_80482394(f32 x); /* inverse square root */
void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);         /* RwFreeListCreateAndPreallocateSpace */
RwBool fn_8047FCDC(void *freeList);  /* RwFreeListDestroy */
f32 fn_80474168(void); /* the squared flatness tolerance in the current coordinate system */

Rt2dPath *fn_80476490(void);           /* Rt2dPathCreate */
Rt2dPath *fn_80476510(Rt2dPath *path); /* Rt2dPathLock */

/* The path free list: paths per block, blocks to preallocate, and the list itself. */
s32 lbl_805EDE50 = 32;
s32 lbl_805EDE54 = 1;
R5FreeList lbl_805E26A8;

/* Frees one sub-path and returns the one after it. */
Rt2dPath *fn_80474BEC(Rt2dPath *path) {
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

/* Starts a sub-path at (x, y): in the current sub-path if it is empty, else in a new one.
 * Retail inlines the recursion three times; GC/2.7 does that at depth 3, and one level more at
 * its default depth for this (probably shorter) source. */
#pragma inline_depth(3)
Rt2dPath *fn_80474C7C(Rt2dPath *path, f32 x, f32 y) {
    if (fn_8047E03C(path->curr->segments) == 0) {
        Rt2dPathNode *node = (Rt2dPathNode *)fn_8047DDBC(path->curr->segments, 0x30190);

        node->type = 0;
        node->pos.x = x;
        node->pos.y = y;
        node->normal.x = 0.0f;
        node->normal.y = 0.0f;
        node->dist = 0.0f;
    } else {
        path->curr->next = fn_80476490();
        fn_80476510(path->curr->next);
        path->curr = path->curr->next;
        fn_80474C7C(path, x, y);
    }
    return path;
}

#pragma inline_depth(8)

/* Adds a line to (x, y) and gives the corner before it the bisecting normal. */
Rt2dPath *fn_80474EA0(Rt2dPath *path, f32 x, f32 y) {
    s32 count = fn_8047E03C(path->curr->segments);
    Rt2dPathNode *node = (Rt2dPathNode *)fn_8047DDBC(path->curr->segments, 0x30190);
    f32 length;
    f32 dy;
    f32 dx;
    f32 squared;
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
        f32 sumX;
        f32 sumY;
        f32 scale;

        sumY = node[-1].normal.y + node->normal.y;
        sumX = node[-1].normal.x + node->normal.x;
        scale = 1.0f / (node[-1].normal.x * sumX + node[-1].normal.y * sumY);
        node[-1].normal.x = sumX * scale;
        node[-1].normal.y = sumY * scale;
    } else {
        node[-1].normal = node->normal;
    }
    return path;
}

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
        scale = 1.0f / (px * sumX + py * sumY);
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
        scale = 1.0f / (node[-1].normal.x * sumX + node[-1].normal.y * sumY);
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
    if (0.0f != dy || 0.0f != dx) {
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
        scale = 1.0f / (first->normal.x * sumX + first->normal.y * sumY);
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

            left[1].x = 0.5f * (x0 + x1);
            left[1].y = 0.5f * (y0 + y1);
            mid.x = 0.5f * (x1 + p[2].x);
            mid.y = 0.5f * (y1 + p[2].y);
            right[2].x = 0.5f * (p[3].x + p[2].x);
            right[2].y = 0.5f * (p[3].y + p[2].y);
            left[2].x = 0.5f * (left[1].x + mid.x);
            left[2].y = 0.5f * (left[1].y + mid.y);
            right[1].x = 0.5f * (right[2].x + mid.x);
            right[1].y = 0.5f * (right[2].y + mid.y);
            left[3].x = 0.5f * (left[2].x + right[1].x);
            left[3].y = 0.5f * (left[2].y + right[1].y);
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
 * (depth 7; see the note at fn_80474C7C). */
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
        path->inset = 0.0f;
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
        fn_8047FC04(sizeof(Rt2dPath), lbl_805EDE50, 4, lbl_805EDE54, &lbl_805E26A8, 0x401A1);
    lbl_805E1950.scratchPath = fn_80476490();
    fn_80476510(lbl_805E1950.scratchPath);
    return 1;
}
