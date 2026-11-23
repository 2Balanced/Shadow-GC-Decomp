/* RenderWare multi-texture effects, GameCube attach and vertex streams, 0x804A6C74..0x804A869C:
 * fn_804A6C74 attaches everything the GameCube effects need (the multi-texture plugin, the
 * platform's material extension 0x129 with 12 bytes of extra data, the effect stream callbacks and
 * the engine plugin 0x129; RpMatFXPluginAttach calls it), then the instancing of the extra vertex
 * stream (normal, binormal and tangent per vertex) that bump-mapping texture generation needs:
 * fn_804A8478 tests for it and fn_804A8120 builds it (RpMatFXGameCube.c and RpSkin's GameCube
 * code call both), fn_804A6F8C walks a mesh's strip triangles, fn_804A6CDC / fn_804A6E34 find a
 * triangle's vertices (8- and 16-bit indices).
 * fn_804A8558 computes one triangle's binormal and tangent and was hand-written paired-single
 * assembly in the original (no prologue; psq_l/psq_st through GQR5..7, cr1/cr2): it stays
 * unwritten, so this file cannot become Matching.
 * Data: .sdata2 0x805FB498..0x805FB4C0 (fn_804A6F8C's three five-byte index tables, FLT_MAX and the
 * int to float constant; fn_804A8120 shares FLT_MAX).
 * Built with cflags_rw_r5.  Functions keep their fn_ symbols.
 * Status: fn_804A6C74, fn_804A8120 and fn_804A8478 match; fn_804A6CDC / fn_804A6E34 have the two
 * strip vertices' registers swapped; fn_804A6F8C (0x1194 bytes) is not written yet. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

s32 fn_804A8E90(void);
s32 fn_804A8EFC(s32 platform, u32 pluginId, u32 extraSize);
s32 fn_804A5F08(void);
s32 fn_804A6C30(void);

void DCFlushRange(void *addr, u32 size);
void fn_80394F2C(void);
void fn_804A6F8C(RwGameCubeInstance *instance, RwGameCubeDisplayList *list,
                 RwGameCubeVertexFormat *format);

/* Attaches plugin 0x129 and the callbacks it needs; RpMatFXPluginAttach calls it. */
s32 fn_804A6C74(void) {
    if (!fn_804A8E90()) return FALSE;
    if (!fn_804A8EFC(6, 0x129, 12)) return FALSE;
    if (fn_804A5F08() == FALSE) return FALSE;
    return fn_804A6C30();
}

/* The vertex data of one triangle of an indexed strip: where its positions, its normal (and
 * binormal and tangent) and its texture coordinates are. */
typedef struct GcnStripTriangle {
    u8 *pos[3];   /* 0x00 */
    u8 *nbt;      /* 0x0C: the first vertex's */
    u8 *uv[3];    /* 0x10 */
    u8 posSize;   /* 0x1C: bytes per position component */
    u8 nbtSize;   /* 0x1D: bytes per normal component */
    u8 uvSize;    /* 0x1E: bytes per texture coordinate component */
} GcnStripTriangle;

/* Strip triangle `i` of 8-bit indices `stride` bytes apart; degenerate triangles that only
 * restart the strip are stepped over, and odd triangles have their winding flipped. */
void fn_804A6CDC(GcnStripTriangle *tri, const u8 *indices, u32 stride, u8 *pos, u8 *nbt, u8 *uv,
                 s32 i) {
#define INDEX(k) (indices[stride * (k)])
    u32 v0 = INDEX(i);
    u8 v2;
    u8 v1;

    if (i < 2) {
        if (i == 0) {
            v1 = INDEX(1);
            v2 = INDEX(2);
        } else {
            v2 = INDEX(0);
            v1 = INDEX(2);
        }
    } else {
        v1 = INDEX(i - 2);
        v2 = INDEX(i - 1);
        if (v2 == v1) {
            u8 next = INDEX(i + 1);
            if (v0 == next) {
                v1 = INDEX(i + 2);
                i++;
                if (v0 == v1) {
                    v1 = INDEX(i + 2);
                    i++;
                }
                v2 = INDEX(i + 2);
            } else {
                v1 = v2;
                v2 = next;
            }
        }
        if (i & 1) {
            u8 swap = v1;
            v1 = v2;
            v2 = swap;
        }
    }
#undef INDEX
    tri->pos[0] = pos + v0 * (tri->posSize * 3);
    tri->pos[1] = pos + v1 * (tri->posSize * 3);
    tri->pos[2] = pos + v2 * (tri->posSize * 3);
    tri->nbt = nbt + v0 * (tri->nbtSize * 9);
    tri->uv[0] = uv + v0 * (tri->uvSize * 2);
    tri->uv[1] = uv + v1 * (tri->uvSize * 2);
    tri->uv[2] = uv + v2 * (tri->uvSize * 2);
}

/* The same for 16-bit indices. */
void fn_804A6E34(GcnStripTriangle *tri, const u8 *indices, u32 stride, u8 *pos, u8 *nbt, u8 *uv,
                 s32 i) {
#define INDEX(k) (*(const u16 *)(indices + stride * (k)))
    u32 v0 = INDEX(i);
    u16 v2;
    u16 v1;

    if (i < 2) {
        if (i == 0) {
            v1 = INDEX(1);
            v2 = INDEX(2);
        } else {
            v2 = INDEX(0);
            v1 = INDEX(2);
        }
    } else {
        v1 = INDEX(i - 2);
        v2 = INDEX(i - 1);
        if (v2 == v1) {
            u16 next = INDEX(i + 1);
            if (v0 == next) {
                v1 = INDEX(i + 2);
                i++;
                if (v0 == v1) {
                    v1 = INDEX(i + 2);
                    i++;
                }
                v2 = INDEX(i + 2);
            } else {
                v1 = v2;
                v2 = next;
            }
        }
        if (i & 1) {
            u16 swap = v1;
            v1 = v2;
            v2 = swap;
        }
    }
#undef INDEX
    tri->pos[0] = pos + v0 * (tri->posSize * 3);
    tri->pos[1] = pos + v1 * (tri->posSize * 3);
    tri->pos[2] = pos + v2 * (tri->posSize * 3);
    tri->nbt = nbt + v0 * (tri->nbtSize * 9);
    tri->uv[0] = uv + v0 * (tri->uvSize * 2);
    tri->uv[1] = uv + v1 * (tri->uvSize * 2);
    tri->uv[2] = uv + v2 * (tri->uvSize * 2);
}

/* fn_804A6F8C (0x1194 bytes) is not decompiled yet. */

/* Prepares the extra vertex stream of an instance (stream 1: normal, binormal and tangent per
 * vertex): the binormal's first component of every vertex is set to its largest value, then
 * fn_804A6F8C fills the stream in for each mesh whose material generates texture coordinates from
 * the geometry. */
void fn_804A8120(RwGameCubeRenderNode *node, RwGameCubeVertexFormat *format, s32 numVertices) {
    RwGameCubeInstance *instance = (RwGameCubeInstance *)(node->resEntry + 0x18);
    RwGameCubeDisplayList *list;
    RpMeshHeader *header = node->meshHeader;
    RpMesh *mesh;
    s32 i;

    list = (RwGameCubeDisplayList *)&instance->streams[1] + (instance->numStreams - 1);

    if (format != NULL) {
        if (format->normalType == 1) {
            u8 *nbt = (u8 *)instance->streams[1].data;
            nbt += 3;
            for (i = 0; i < numVertices; i++) {
                *nbt = 0xFF;
                nbt += 9;
            }
        } else if (format->normalType == 3) {
            u16 *nbt = (u16 *)instance->streams[1].data;
            nbt += 3;
            for (i = 0; i < numVertices; i++) {
                *nbt = 0xFFFF;
                nbt += 9;
            }
        } else {
            f32 *nbt = (f32 *)instance->streams[1].data;
            nbt += 3;
            for (i = 0; i < numVertices; i++) {
                *nbt = 3.4028235e38f;
                nbt += 9;
            }
        }
    } else {
        f32 *nbt = (f32 *)instance->streams[1].data;
        nbt += 3;
        for (i = 0; i < numVertices; i++) {
            *nbt = 3.4028235e38f;
            nbt += 9;
        }
    }
    mesh = (RpMesh *)(header + 1);
    for (i = 0; i < header->numMeshes; list++, mesh++, i++) {
        u8 *data = (u8 *)fn_804A90C8(mesh->material, 6);
        if (data != NULL) {
            u8 *matInstance = *(u8 **)(data + 0x30);
            if (matInstance != NULL && *(s32 *)matInstance == 6) {
                GcnMatInstanceBody *body = (GcnMatInstanceBody *)fn_804A60D0(matInstance);
                s32 j;
                for (j = 0; j < body->numTexGens; j++) {
                    GcnMatTexGen *texGen = &body->texGens[j];
                    if (texGen->src == 2 || texGen->src == 3 ||
                        (texGen->type >= 2 && texGen->type <= 9)) {
                        fn_804A6F8C(instance, list, format);
                        break;
                    }
                }
            }
        }
    }
    DCFlushRange(instance->streams[1].data, numVertices * ((u8 *)&instance->streams[1].info)[1]);
    fn_80394F2C();
}

/* Whether a mesh of the node has a material whose instance data generates texture coordinates
 * from the geometry, which needs the extra vertex streams. */
s32 fn_804A8478(RwGameCubeRenderNode *node) {
    RpMeshHeader *header = node->meshHeader;
    RpMesh *mesh = (RpMesh *)(header + 1);
    s32 i;

    for (i = 0; i < header->numMeshes; i++, mesh++) {
        u8 *data = (u8 *)fn_804A90C8(mesh->material, 6);
        if (data != NULL) {
            u8 *instance = *(u8 **)(data + 0x30);
            if (instance != NULL && *(s32 *)instance == 6) {
                GcnMatInstanceBody *body = (GcnMatInstanceBody *)fn_804A60D0(instance);
                s32 j;
                for (j = 0; j < body->numTexGens; j++) {
                    GcnMatTexGen *texGen = &body->texGens[j];
                    if (texGen->src == 2 || texGen->src == 3 ||
                        (texGen->type >= 2 && texGen->type <= 9)) {
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

/* fn_804A8558: hand-written paired-single assembly in the original, not written here. */
