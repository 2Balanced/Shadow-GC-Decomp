/* RenderWare world plugin, GameCube: display-list sizes, 0x8046ACBC..0x8046B110. The size of a
 * vertex for a vertex descriptor, of a display list, and the record of a list's buffer. Names
 * are inferred from behaviour.
 *
 * Source file: fn_8046ACBC's jump table is the whole of its .data, 0x8056DAF8..0x8056DB50 (padded
 * to 8 bytes after 0x8056DB4C). It calls the vertex-format helpers at 0x8046693C..0x80466B58 out
 * of line, which GC 1.3.2's automatic inlining would not do within one file (measured: it
 * inlines them, switches and all), and the instancing code from fn_8046B110 on calls the small
 * fn_8046B094 and fn_8046B0E0 out of line (measured: inlined within one file): the code before
 * and after it are other files. Old unit Game/RwCpp31 (second part).
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw (cflags_rw_132). Every function and
 * section matches; the unit is Matching. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

/* The size in bytes of one vertex of a display list drawn with `desc`: a byte for each matrix
 * index and for each attribute indexed by bytes, two for 16-bit indices, the attribute's own
 * size (0x8046693C..0x80466B58) when it is direct. Normals count three times with NBT indices. */
u32 fn_8046ACBC(RwGameCubeVtxDesc *desc) {
    s32 attr;
    u32 size = 0;

    for (attr = 0; attr < 21; attr++) {
        switch (attr) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            if ((1 << attr) & desc->vcdLo) {
                size += 1;
            }
            break;
        case 9:
            if (RWGAMECUBE_POS_TYPE(desc) == 1) {
                size += fn_8046693C(desc);
            } else if (RWGAMECUBE_POS_TYPE(desc) == 2) {
                size += 1;
            } else if (RWGAMECUBE_POS_TYPE(desc) == 3) {
                size += 2;
            }
            break;
        case 10: {
            /* One variable holds the flag and then the mode: only this keeps retail's registers. */
            u32 mode = (desc->vatA >> 9) & 1;
            u32 nbt = mode;
            mode = RWGAMECUBE_NRM_TYPE(desc);
            if (nbt == 1) {
                u32 count = 3;
                if ((desc->vatA >> 31) == 0) {
                    count = 1;
                }
                if (mode == 1) {
                    size += count * fn_804669A0(desc);
                } else if (mode == 2) {
                    size += count;
                } else if (mode == 3) {
                    size += count * 2;
                }
            } else {
                if (mode == 1) {
                    size += fn_804669A0(desc);
                } else if (mode == 2) {
                    size += 1;
                } else if (mode == 3) {
                    size += 2;
                }
            }
            break;
        }
        case 11:
        case 12:
            if (RWGAMECUBE_CLR_TYPE(desc, attr - 11) == 1) {
                size += fn_804669EC(desc, attr - 11);
            } else if (RWGAMECUBE_CLR_TYPE(desc, attr - 11) == 2) {
                size += 1;
            } else if (RWGAMECUBE_CLR_TYPE(desc, attr - 11) == 3) {
                size += 2;
            }
            break;
        case 13:
            if (RWGAMECUBE_TEX_TYPE(desc, 0) == 1) {
                size += fn_80466A54(desc, 0);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 0) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 0) == 3) {
                size += 2;
            }
            break;
        case 14:
            if (RWGAMECUBE_TEX_TYPE(desc, 1) == 1) {
                size += fn_80466A54(desc, 1);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 1) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 1) == 3) {
                size += 2;
            }
            break;
        case 15:
            if (RWGAMECUBE_TEX_TYPE(desc, 2) == 1) {
                size += fn_80466A54(desc, 2);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 2) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 2) == 3) {
                size += 2;
            }
            break;
        case 16:
            if (RWGAMECUBE_TEX_TYPE(desc, 3) == 1) {
                size += fn_80466A54(desc, 3);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 3) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 3) == 3) {
                size += 2;
            }
            break;
        case 17:
            if (RWGAMECUBE_TEX_TYPE(desc, 4) == 1) {
                size += fn_80466A54(desc, 4);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 4) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 4) == 3) {
                size += 2;
            }
            break;
        case 18:
            if (RWGAMECUBE_TEX_TYPE(desc, 5) == 1) {
                size += fn_80466A54(desc, 5);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 5) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 5) == 3) {
                size += 2;
            }
            break;
        case 19:
            if (RWGAMECUBE_TEX_TYPE(desc, 6) == 1) {
                size += fn_80466A54(desc, 6);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 6) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 6) == 3) {
                size += 2;
            }
            break;
        case 20:
            if (RWGAMECUBE_TEX_TYPE(desc, 7) == 1) {
                size += fn_80466A54(desc, 7);
            } else if (RWGAMECUBE_TEX_TYPE(desc, 7) == 2) {
                size += 1;
            } else if (RWGAMECUBE_TEX_TYPE(desc, 7) == 3) {
                size += 2;
            }
            break;
        }
    }
    return size;
}

/* The size of a display list of `numLists` primitives (a command byte and a 16-bit count each)
 * holding `numIndices` vertices, rounded up to GX's 32 bytes. */
u32 fn_8046B094(RwGameCubeVtxDesc *desc, u32 numLists, u32 numIndices) {
    u32 size;
    u32 dataSize = fn_8046ACBC(desc) * numIndices;

    size = numLists * 3;
    size += dataSize;
    size = (size + 31) & ~31;
    return size;
}

/* Records a display-list buffer and its size in `block`, and clears the buffer. */
void fn_8046B0E0(void **block, u32 unused, u32 size, void *mem) {
    block[0] = mem;
    block[1] = (void *)size;
    memset(mem, 0, size);
}
