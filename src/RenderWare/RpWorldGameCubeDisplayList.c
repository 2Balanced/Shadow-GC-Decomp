/* RenderWare world plugin, GameCube: writing triangle lists and strips into display lists,
 * 0x8046D8C8..0x8046EE8C. fn_8046D8C8 counts the strips an index list is cut into at its
 * degenerate triangles, fn_8046DA24 writes their indices, fn_8046E3E0 their primitive headers,
 * and fn_8046E594 writes a whole display list. Names are inferred from behaviour.
 *
 * Source file: fn_8046E594's jump table (.data 0x8056DB50..0x8056DBA8, padded to 8 bytes after
 * 0x8056DBA4) and its own int-to-float double (.sdata2 0x805FB210): the file after it pools a
 * second copy (0x805FB218), so the two are separate files. The strip helpers have no data;
 * fn_8046DA24 and fn_8046E3E0 serve only fn_8046E594 and stand with it. Old units Game/RwCpp33
 * (end) and RwCpp34 (first function).
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw (cflags_rw_132). The GX command byte
 * is passed as a 32-bit value all the way down (retail truncates it only at the store).
 * fn_8046D8C8 differs in its first six instructions (retail stores the two zeroes through r9
 * before the counter's li r9,0; 15 orders probed, probes/RenderWare_RpWorldGameCubeDisplayList.py),
 * fn_8046E3E0 in its registers, fn_8046E594 (first draft, 90%) in registers and scheduling;
 * fn_8046DA24 (the unrolled strip index writer) is not written yet. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

void fn_8046DA24(u16 *stripIndices, u16 *attrIndices, u32 numIndices, s32 split, u32 vertexSize,
                u32 indexType, s32 swap, u8 *out);
void fn_8046E3E0(u16 *indices, u32 numIndices, s32 split, u32 vertexSize, u32 command, s32 swap,
                 u8 *out);

#define CLOSE_STRIP()            \
    do {                         \
        *numVertices += run + 2; \
        run = 0;                 \
        *numStrips += 1;         \
    } while (0)

/* Counts the strips and strip vertices a triangle strip's index list becomes once it is cut
 * at its degenerate triangles. With `swap` set a strip may only start on an even triangle, so
 * that its winding stays: an odd start takes one index more.
 * NonMatching in its first six instructions only: retail zeroes the two results through r9
 * and loads the counter's zero after them, GC 1.3.2 keeps the counter in r9 from the start
 * and takes r10 for the stores (ten source orders tried). */
void fn_8046D8C8(u16 *indices, u32 numIndices, s32 *numStrips, s32 *numVertices, s32 swap) {
    u32 run;
    u32 i;

    *numStrips = 0;
    *numVertices = 0;
    run = 0;
    for (i = 0; i < numIndices - 2; i++) {
        if (indices[0] != indices[1] && indices[0] != indices[2] && indices[1] != indices[2]) {
            if (run == 0 && (i & 1) && swap) {
                run = 2;
            } else {
                run++;
            }
        } else if (run != 0) {
            if (swap && i < numIndices - 3) {
                if (indices[1] != indices[2] && indices[1] != indices[3] &&
                    indices[2] != indices[3]) {
                    if (((i + 1) & 1) == 0) {
                        CLOSE_STRIP();
                    } else {
                        run++;
                    }
                } else {
                    CLOSE_STRIP();
                }
            } else {
                CLOSE_STRIP();
            }
        }
        indices++;
    }
    if (run != 0) {
        CLOSE_STRIP();
    }
}

#define WRITE_STRIP()                                              \
    do {                                                           \
        u8 *head = &out[numStrips * 3 + numVertices * vertexSize]; \
        head[0] = command;                                         \
        *(u16 *)(head + 1) = run + 2;                              \
    } while (0)
#define EMIT_STRIP()            \
    do {                        \
        WRITE_STRIP();          \
        numVertices += run + 2; \
        run = 0;                \
        numStrips++;            \
    } while (0)

/* Writes the primitive headers (GX command byte and 16-bit vertex count) of the strips
 * fn_8046D8C8 counts into the display list at `out`, leaving `vertexSize` bytes for each
 * vertex after them. Without `split` the whole list is one primitive.
 * NonMatching by registers only: retail keeps the run and the vertex count in r11 and r10 and
 * the loaded indices in r30 and r31, GC 1.3.2 gives this source the opposite. */
void fn_8046E3E0(u16 *indices, u32 numIndices, s32 split, u32 vertexSize, u32 command, s32 swap,
                 u8 *out) {
    u32 numStrips = 0;
    u32 numVertices = 0;
    u32 run = 0;
    u32 i;

    if (split) {
        for (i = 0; i < numIndices - 2; i++) {
            if (indices[0] != indices[1] && indices[0] != indices[2] &&
                indices[1] != indices[2]) {
                if (run == 0 && (i & 1) && swap) {
                    run = 2;
                } else {
                    run++;
                }
            } else if (run != 0) {
                if (swap && i < numIndices - 3) {
                    if (indices[1] != indices[2] && indices[1] != indices[3] &&
                        indices[2] != indices[3]) {
                        if (((i + 1) & 1) == 0) {
                            EMIT_STRIP();
                        } else {
                            run++;
                        }
                    } else {
                        EMIT_STRIP();
                    }
                } else {
                    EMIT_STRIP();
                }
            }
            indices++;
        }
        if (run != 0) {
            WRITE_STRIP();
        }
    } else {
        out[0] = command;
        *(u16 *)(out + 1) = numIndices;
    }
}

/* Writes the indices of an indexed attribute after the vertices' earlier attributes and
 * steps over them: one byte each for GX_INDEX8, two for GX_INDEX16. */
#define INDEXED_ATTR(type)                                                                     \
    do {                                                                                       \
        fn_8046DA24(indices[GX_VA_POS], indices[attr], numIndices, split, vertexSize, (type),  \
                    swap, dst);                                                                \
        if ((type) == GX_INDEX8) {                                                             \
            vtxOffset += 1;                                                                    \
        } else if ((type) == GX_INDEX16) {                                                     \
            vtxOffset += 2;                                                                    \
        }                                                                                      \
    } while (0)

/* Writes a display list for `numIndices` vertices into `list`: the primitive headers (one list,
 * or one strip per run with `split`), then every attribute of every vertex in descriptor
 * order, as indices or as data converted from the arrays `indices` holds for direct
 * attributes. `offset` is subtracted from direct positions. */
void fn_8046E594(RwGameCubeVtxDesc *desc, RwGameCubeDisplayList *list, u16 **indices, u32 numIndices,
                 s32 split, u32 vertexSize, s32 swap, u32 command, const RwV3d *offset) {
    u32 attr;
    u32 vtxOffset;
    u32 type;

    fn_8046E3E0(indices[GX_VA_POS], numIndices, split, vertexSize, command, swap, list->data);
    vtxOffset = 3;
    for (attr = GX_VA_PNMTXIDX; attr < GX_VA_TEX7 + 1; attr++) {
        u8 *dst = (u8 *)list->data + vtxOffset;

        switch (attr) {
        case GX_VA_PNMTXIDX:
        case GX_VA_TEX0MTXIDX:
        case GX_VA_TEX0MTXIDX + 1:
        case GX_VA_TEX0MTXIDX + 2:
        case GX_VA_TEX0MTXIDX + 3:
        case GX_VA_TEX0MTXIDX + 4:
        case GX_VA_TEX0MTXIDX + 5:
        case GX_VA_TEX0MTXIDX + 6:
        case GX_VA_TEX0MTXIDX + 7:
            if ((1 << attr) & desc->vcdLo) {
                fn_8046DA24(indices[GX_VA_POS], indices[attr], numIndices, split, vertexSize,
                            GX_INDEX8, swap, dst);
                vtxOffset += 1;
            }
            break;
        case GX_VA_POS:
            type = RWGAMECUBE_POS_TYPE(desc);
            if (type == GX_DIRECT) {
                if ((desc->vatA & 1) == 1) {
                    fn_80466B58(dst, (const RwV3d *)indices[attr], (desc->vatA >> 1) & 7, numIndices,
                                vertexSize, offset, (f32)(1 << ((desc->vatA >> 4) & 0x1F)));
                    vtxOffset += fn_8046693C(desc);
                }
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_NRM:
            type = RWGAMECUBE_NRM_TYPE(desc);
            if (((desc->vatA >> 9) & 1) == 1) {
                if (type == GX_DIRECT) {
                    vtxOffset += fn_804669A0(desc) * 3;
                } else if (type != GX_NONE) {
                    u32 count;

                    if ((desc->vatA >> 31) == 0) {
                        fn_8046DA24(indices[GX_VA_POS], indices[attr], numIndices, split, vertexSize,
                                    type, swap, dst);
                        count = 1;
                    } else {
                        count = 3;
                    }
                    if (type == GX_INDEX8) {
                        vtxOffset += count;
                    } else if (type == GX_INDEX16) {
                        vtxOffset += count * 2;
                    }
                }
            } else {
                if (type == GX_DIRECT) {
                    fn_80467BC8(dst, (const RwV3d *)indices[attr], (desc->vatA >> 10) & 7, numIndices,
                                vertexSize);
                    vtxOffset += fn_804669A0(desc);
                } else if (type != GX_NONE) {
                    INDEXED_ATTR(type);
                }
            }
            break;
        case GX_VA_CLR0:
        case GX_VA_CLR1:
            type = RWGAMECUBE_CLR_TYPE(desc, attr - GX_VA_CLR0);
            if (type == GX_DIRECT) {
                fn_804699F0(dst, (const RwRGBA *)indices[attr],
                            (desc->vatA >> ((attr - GX_VA_CLR0) * 4 + 14)) & 7, numIndices, vertexSize);
                vtxOffset += fn_804669EC(desc, attr - GX_VA_CLR0);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX0:
            type = RWGAMECUBE_TEX_TYPE(desc, 0);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatA >> 22) & 7, numIndices, vertexSize,
                            (f32)(1 << ((desc->vatA >> 25) & 0x1F)));
                vtxOffset += fn_80466A54(desc, 0);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX1:
            type = RWGAMECUBE_TEX_TYPE(desc, 1);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatB >> 1) & 7, numIndices, vertexSize,
                            (f32)((1 << (desc->vatB >> 4)) & 0x1F));
                vtxOffset += fn_80466A54(desc, 1);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX2:
            type = RWGAMECUBE_TEX_TYPE(desc, 2);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatB >> 10) & 7, numIndices, vertexSize,
                            (f32)((1 << (desc->vatB >> 13)) & 0x1F));
                vtxOffset += fn_80466A54(desc, 2);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX3:
            type = RWGAMECUBE_TEX_TYPE(desc, 3);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatB >> 19) & 7, numIndices, vertexSize,
                            (f32)((1 << (desc->vatB >> 22)) & 0x1F));
                vtxOffset += fn_80466A54(desc, 3);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX4:
            type = RWGAMECUBE_TEX_TYPE(desc, 4);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatB >> 28) & 7, numIndices, vertexSize,
                            (f32)(1 << (desc->vatC & 0x1F)));
                vtxOffset += fn_80466A54(desc, 4);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX5:
            type = RWGAMECUBE_TEX_TYPE(desc, 5);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatC >> 6) & 7, numIndices, vertexSize,
                            (f32)(1 << ((desc->vatC >> 9) & 0x1F)));
                vtxOffset += fn_80466A54(desc, 5);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX6:
            type = RWGAMECUBE_TEX_TYPE(desc, 6);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatC >> 15) & 7, numIndices, vertexSize,
                            (f32)(1 << ((desc->vatC >> 18) & 0x1F)));
                vtxOffset += fn_80466A54(desc, 6);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        case GX_VA_TEX7:
            type = RWGAMECUBE_TEX_TYPE(desc, 7);
            if (type == GX_DIRECT) {
                fn_8046A270(dst, (const RwTexCoords *)indices[attr], (desc->vatC >> 24) & 7, numIndices, vertexSize,
                            (f32)(1 << ((desc->vatC >> 27) & 0x1F)));
                vtxOffset += fn_80466A54(desc, 7);
            } else if (type != GX_NONE) {
                INDEXED_ATTR(type);
            }
            break;
        }
    }
}
