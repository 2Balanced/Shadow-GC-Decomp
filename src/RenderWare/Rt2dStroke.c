/* RenderWare 2D toolkit (rt2d), 0x80476ABC..0x8047726C: the path stream reader
 * (Rt2dPathStreamRead), the stroke renderer (Rt2dPathStroke: a triangle strip along the flattened
 * path, coloured and textured by the brush) and the brush module's close and open (the free list
 * of brushes). The brush's other functions are not in the binary (dead-stripped).
 *
 * One source file: its .sdata2 is one constant (1.0f at 0x805FB278, a repeat of the path file's
 * 1.0f), its .sdata the brush free list's parameters (0x805EDE58..0x805EDE60) and its .bss the free
 * list itself (0x805E26D0, padded to 0x805E26F8; the path file's free list before it is padded
 * too). Whether the stream reader (no data of its own) belongs here or at the end of the path file
 * the binary does not say; it is kept with the stroke, as the previous cut had it.
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C, built with cflags_rw_r5 (-O4,p, no stmw/lmw, automatic inlining in definition order).
 * Named Rt2dStroke.c at merge: Rt2dBrush.c holds the brush API (Rt2dBrushCreate .. SetTexture) at
 * 0x80470474, so the free list opened and closed here may be the strokes' rather than the brushes'. */
#include "Game/rw/r5.h"

RwMatrix *fn_804749CC(void);                     /* the current 2D-to-world matrix */
Rt2dPath *fn_8047548C(void);                     /* empties the scratch path */
Rt2dPath *fn_804758C8(Rt2dPath *dst, const Rt2dPath *src); /* flattens a path */
RwBool fn_80475E2C(Rt2dPath *path);              /* Rt2dPathDestroy */
void fn_804760C8(Rt2dPath *path);                /* Rt2dPathEmpty */
Rt2dPath *fn_80476490(void);                     /* Rt2dPathCreate */
void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);                     /* RwFreeListCreateAndPreallocateSpace */
RwBool fn_8047FCDC(void *freeList);              /* RwFreeListDestroy */

/* The brush free list: brushes per block, blocks to preallocate, and the list itself. */
s32 lbl_805EDE58 = 32;
s32 lbl_805EDE5C = 1;
R5FreeList lbl_805E26D0;

Rt2dPath *fn_80476C4C(RwStream *stream);

/* Reads the nodes of a path whose header has been read, and the sub-paths chained to it. Retail
 * does not inline it into its caller below, where GC/2.7 would (as with Rt2dPathClose). */
#pragma auto_inline off
Rt2dPath *fn_80476ABC(Rt2dPath *path, Rt2dPathStreamHeader *header, RwStream *stream) {
    Rt2dPathNode *nodes;
    s32 i;

    fn_804760C8(path);
    path->curr = path;
    path->numSegment = header->numSegment;
    path->closed = header->flag & 1;
    path->flat = (header->flag >> 1) & 1;
    path->inset = header->inset;
    nodes = (Rt2dPathNode *)RWSRCGLOBAL(rwmalloc)(path->numSegment * sizeof(Rt2dPathNode), 0x301A1);
    if (nodes == 0) {
        return 0;
    }
    if (path->numSegment != 0) {
        if (fn_80480D4C(stream, nodes, path->numSegment * sizeof(Rt2dPathNode)) !=
            path->numSegment * sizeof(Rt2dPathNode)) {
            return 0;
        }
        fn_8047D158(nodes, path->numSegment * sizeof(Rt2dPathNode));
    }
    for (i = 0; i < header->numSegment; i++) {
    }
    path->segments = (RwSList *)nodes;
    if (header->flag & 4) {
        if (fn_8047CF50(stream, 0x1A1, 0, 0)) {
            path->next = fn_80476C4C(stream);
        } else {
            return 0;
        }
    }
    {
        Rt2dPath *sub;

        for (sub = path->next; sub != 0; sub = sub->next) {
            sub->curr = path;
        }
    }
    return path;
}
#pragma auto_inline reset

/* Rt2dPathStreamRead: a new path read from a stream. */
Rt2dPath *fn_80476C4C(RwStream *stream) {
    Rt2dPath *result;
    Rt2dPath *path;
    Rt2dPathStreamHeader header;

    path = fn_80476490();
    result = path;
    if (fn_80480D4C(stream, &header, sizeof(header)) != sizeof(header)) {
        result = 0;
    } else {
        fn_8047D254(&header, sizeof(header));
        switch (header.type) {
        case 1:
            result = fn_80476ABC(path, &header, stream);
            break;
        }
    }
    if (result == 0) {
        fn_80475E2C(path);
    }
    return result;
}

/* Strokes a path with a brush: a triangle strip along both sides of the flattened path, shaded
 * and textured along its length when the brush says so; then the sub-paths chained to it.
 * NonMatching (same instructions; ~85 rows of float and scratch register numbering): retail has
 * the half width in f29 and the inset in f30, the shading factor in f7, and loads the flat
 * colour's four bytes before storing them. 3500 sampled declaration orders and four spellings of
 * the colour copies were tried. */
Rt2dPath *fn_80476CF8(Rt2dPath *path, Rt2dBrush *brush) {
    if (path != 0) {
        f32 outer;
        f32 inner;
        RwIm3DVertex *vertex;
        s32 count;
        f32 inset;
        f32 scale;
        Rt2dPathNode *node;
        s32 i;
        const Rt2dPath *flat;
        s32 numVertex;
        f32 z;
        RwMatrix *ltm;
        f32 halfwidth;

        ltm = fn_804749CC();
        halfwidth = brush->halfwidth;
        z = lbl_805E1950.layerDepth;
        inset = path->inset;
        flat = path;
        if (!path->flat) {
            flat = fn_8047548C();
            fn_804758C8((Rt2dPath *)flat, path);
        }
        if (flat->numSegment == -1) {
            node = (Rt2dPathNode *)fn_8047DFAC(flat->segments);
        } else {
            node = (Rt2dPathNode *)flat->segments;
        }
        count = flat->numSegment;
        if (count == -1) {
            count = fn_8047E03C(flat->segments);
        }
        if (brush->texture != 0) {
            RwTexture *texture = brush->texture;
            u32 address;
            u32 filter;

            filter = texture->filterAddressing & 0xFF;
            address = 0;
            if (((texture->filterAddressing >> 8) & 0xF) == ((texture->filterAddressing >> 12) & 0xF)) {
                address = (texture->filterAddressing >> 12) & 0xF;
            }
            RWSRCGLOBAL(renderStateSet)(1, texture->raster);
            RWSRCGLOBAL(renderStateSet)(9, (void *)filter);
            RWSRCGLOBAL(renderStateSet)(2, (void *)address);
        } else {
            RWSRCGLOBAL(renderStateSet)(1, 0);
        }
        outer = halfwidth + inset;
        inner = halfwidth - inset;
        scale = 1.0f / node[count - 1].dist;
        vertex = lbl_805E1950.vertexBuffer;
        numVertex = 0;
        for (i = 0; i < count; i++, node++) {
            if (numVertex >= 62) {
                if (fn_8049A82C(lbl_805E1950.vertexBuffer, numVertex, ltm,
                                lbl_805E1950.field_0x31C | (brush->texture != 0))) {
                    fn_8049A95C(4, lbl_805E1950.index, numVertex);
                    fn_8049A90C();
                }
                vertex = lbl_805E1950.vertexBuffer;
                numVertex = 0;
                i--;
                node--;
            }
            {
                RwV2d offset;
                RwV2d pos;

                offset.x = node->normal.x * outer;
                offset.y = node->normal.y * outer;
                pos.x = node->pos.x - offset.x;
                pos.y = node->pos.y - offset.y;
                vertex[0].objVertex.x = pos.x;
                vertex[0].objVertex.y = pos.y;
                vertex[0].objVertex.z = z;
            }
            if (brush->flag & 1) {
                f32 t = node->dist * scale;
                RwRGBAReal delta;
                RwRGBAReal col;

                delta.red = brush->dtop.col.red * t;
                delta.green = brush->dtop.col.green * t;
                delta.blue = brush->dtop.col.blue * t;
                delta.alpha = brush->dtop.col.alpha * t;
                col.red = brush->top.col.red + delta.red;
                col.green = brush->top.col.green + delta.green;
                col.blue = brush->top.col.blue + delta.blue;
                col.alpha = brush->top.col.alpha + delta.alpha;
                vertex[0].color.red = col.red;
                vertex[0].color.green = col.green;
                vertex[0].color.blue = col.blue;
                vertex[0].color.alpha = col.alpha;
            } else {
                vertex[0].color.red = brush->colorCache.red;
                vertex[0].color.green = brush->colorCache.green;
                vertex[0].color.blue = brush->colorCache.blue;
                vertex[0].color.alpha = brush->colorCache.alpha;
            }
            if (brush->flag & 2) {
                f32 t = node->dist * scale;
                RwV2d delta;
                RwV2d uv;

                delta.x = brush->dtop.uv.x * t;
                delta.y = brush->dtop.uv.y * t;
                uv.x = brush->top.uv.x + delta.x;
                uv.y = brush->top.uv.y + delta.y;
                vertex[0].u = uv.x;
                vertex[0].v = uv.y;
            }
            {
                RwV2d offset;
                RwV2d pos;

                offset.x = node->normal.x * inner;
                offset.y = node->normal.y * inner;
                pos.x = node->pos.x + offset.x;
                pos.y = node->pos.y + offset.y;
                vertex[1].objVertex.x = pos.x;
                vertex[1].objVertex.y = pos.y;
                vertex[1].objVertex.z = z;
            }
            if (brush->flag & 1) {
                f32 t = node->dist * scale;
                RwRGBAReal delta;
                RwRGBAReal col;

                delta.red = brush->dbottom.col.red * t;
                delta.green = brush->dbottom.col.green * t;
                delta.blue = brush->dbottom.col.blue * t;
                delta.alpha = brush->dbottom.col.alpha * t;
                col.red = brush->bottom.col.red + delta.red;
                col.green = brush->bottom.col.green + delta.green;
                col.blue = brush->bottom.col.blue + delta.blue;
                col.alpha = brush->bottom.col.alpha + delta.alpha;
                vertex[1].color.red = col.red;
                vertex[1].color.green = col.green;
                vertex[1].color.blue = col.blue;
                vertex[1].color.alpha = col.alpha;
            } else {
                vertex[1].color.red = brush->colorCache.red;
                vertex[1].color.green = brush->colorCache.green;
                vertex[1].color.blue = brush->colorCache.blue;
                vertex[1].color.alpha = brush->colorCache.alpha;
            }
            if (brush->flag & 2) {
                f32 t = node->dist * scale;
                RwV2d delta;
                RwV2d uv;

                delta.x = brush->dbottom.uv.x * t;
                delta.y = brush->dbottom.uv.y * t;
                uv.x = brush->bottom.uv.x + delta.x;
                uv.y = brush->bottom.uv.y + delta.y;
                vertex[1].u = uv.x;
                vertex[1].v = uv.y;
            }
            vertex += 2;
            numVertex += 2;
        }
        if (numVertex > 0) {
            if (fn_8049A82C(lbl_805E1950.vertexBuffer, numVertex, ltm,
                            lbl_805E1950.field_0x31C | (brush->texture != 0))) {
                fn_8049A95C(4, lbl_805E1950.index, numVertex);
                fn_8049A90C();
            }
        }
        fn_80476CF8(path->next, brush);
    }
    return path;
}

/* Rt2dPathStroke. */
Rt2dPath *fn_804771C0(Rt2dPath *path, Rt2dBrush *brush) {
    return fn_80476CF8(path, brush);
}

/* Closes the brush module. */
void fn_804771E0(void) {
    fn_8047FCDC(lbl_805E1950.brushFreeList);
    lbl_805E1950.brushFreeList = 0;
}

/* Opens the brush module: the free list of brushes. */
RwBool fn_8047721C(void) {
    lbl_805E1950.brushFreeList =
        fn_8047FC04(0x100, lbl_805EDE58, 16, lbl_805EDE5C, &lbl_805E26D0, 0x401A3);
    return 1;
}
