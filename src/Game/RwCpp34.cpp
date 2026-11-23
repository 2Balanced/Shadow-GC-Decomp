/* RenderWare GameCube driver, display-list writer and vertex descriptor, 0x8046E594..0x8046FF34.
 * Names are inferred from behaviour. Compiled by CodeWarrior GC 1.3.2 with -O4,p.
 * Matching: fn_8046EE8C, fn_8046EEA0, fn_8046FC1C, fn_8046FC48, fn_8046FC50. fn_8046E594,
 * fn_8046F208 and fn_8046F6CC (the list writer and the attribute-array writers) are not written
 * yet; their jump tables are in this unit's .data (0x8056DB50..0x8056DC7C). */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

extern "C" {
/* The size of a display-list header for `desc`: 0x14 bytes and an 8-byte record for each
 * attribute array after the first. */
u32 fn_8046EE8C(RwGameCubeVtxDesc *desc) {
    return (desc->numArrays - 1) * 8 + 0x14;
}

#define INDEXED(type) ((type) == GX_INDEX8 || (type) == GX_INDEX16)
#define ROUND32(n) (((n) + 31) & ~31)

/* The size of the attribute arrays a display list for `desc` carries with it: each indexed
 * attribute's array, rounded up to 32 bytes. Normals with binormals and tangents are three
 * times the size. */
u32 fn_8046EEA0(RwGameCubeVtxDesc *desc, RwGameCubeArrays *arrays) {
    u32 attr;
    u32 size = 0;

    for (attr = GX_VA_POS; attr < GX_VA_TEX7 + 1; attr++) {
        switch (attr) {
        case GX_VA_POS:
            if (INDEXED(RWGAMECUBE_POS_TYPE(desc))) {
                size += ROUND32(arrays->num[attr] * fn_8046693C(desc));
            }
            break;
        case GX_VA_NRM:
            if (INDEXED(RWGAMECUBE_NRM_TYPE(desc))) {
                if (((desc->vatA >> 9) & 1) == 1) {
                    size += ROUND32(arrays->num[GX_VA_NRM] * fn_804669A0(desc) * 3);
                } else {
                    size += ROUND32(arrays->num[GX_VA_NRM] * fn_804669A0(desc));
                }
            }
            break;
        case GX_VA_CLR0:
        case GX_VA_CLR1:
            if (INDEXED(RWGAMECUBE_CLR_TYPE(desc, attr - GX_VA_CLR0))) {
                size += ROUND32(arrays->num[attr] * fn_804669EC(desc, attr - GX_VA_CLR0));
            }
            break;
        case GX_VA_TEX0:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 0))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        case GX_VA_TEX1:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 1))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        case GX_VA_TEX2:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 2))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        case GX_VA_TEX3:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 3))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        case GX_VA_TEX4:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 4))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        case GX_VA_TEX5:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 5))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        case GX_VA_TEX6:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 6))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        case GX_VA_TEX7:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 7))) {
                size += ROUND32(arrays->num[attr] * fn_80466A54(desc, attr - GX_VA_TEX0));
            }
            break;
        }
    }
    return size;
}

/* Resets a vertex descriptor to GX's defaults (byte dequantisation on, normal index 3 off). */
void fn_8046FC1C(RwGameCubeVtxDesc *desc) {
    desc->unk0 = 0;
    desc->vatA = 0x40000000;
    desc->vatB = 0x80000000;
    desc->vatC = 0;
    desc->vcdLo = 0;
    desc->vcdHi = 0;
    desc->unk18 = 0;
}

void fn_8046FC48(RwGameCubeVtxDesc *desc, u32 value) {
    desc->unk0 = value;
}

#define SET_FIELD(reg, mask, value) \
    do {                            \
        (reg) &= ~(mask);           \
        (reg) |= (value) & (mask);  \
    } while (0)

/* GXSetVtxAttrFmt on the descriptor: the component count, component type and fixed-point
 * shift of one attribute. */
void fn_8046FC50(RwGameCubeVtxDesc *desc, GXAttr attr, s32 count, s32 type, u8 frac) {
    switch (attr) {
    case GX_VA_POS:
        SET_FIELD(desc->vatA, 0x000001FF, count | (type << 1) | (frac << 4));
        break;
    case GX_VA_NRM:
        SET_FIELD(desc->vatA, 0x00001E00, (count << 9) | (type << 10));
        desc->vatA &= ~0x80000000;
        break;
    case GX_VA_NBT:
        SET_FIELD(desc->vatA, 0x00001E00, (count << 9) | (type << 10));
        if (count == 2) {
            desc->vatA |= 0x80000000;
        } else {
            desc->vatA &= ~0x80000000;
        }
        break;
    case GX_VA_CLR0:
        SET_FIELD(desc->vatA, 0x0001E000, (count << 13) | (type << 14));
        break;
    case GX_VA_CLR1:
        SET_FIELD(desc->vatA, 0x001E0000, (count << 17) | (type << 18));
        break;
    case GX_VA_TEX0:
        SET_FIELD(desc->vatA, 0x3FE00000, (count << 21) | (type << 22) | (frac << 25));
        break;
    case GX_VA_TEX1:
        SET_FIELD(desc->vatB, 0x000001FF, count | (type << 1) | (frac << 4));
        break;
    case GX_VA_TEX2:
        SET_FIELD(desc->vatB, 0x0003FE00, (count << 9) | (type << 10) | (frac << 13));
        break;
    case GX_VA_TEX3:
        SET_FIELD(desc->vatB, 0x0FFC0000, (count << 18) | (type << 19) | (frac << 22));
        break;
    case GX_VA_TEX4:
        SET_FIELD(desc->vatB, 0xF0000000, (count << 27) | (type << 28));
        SET_FIELD(desc->vatC, 0x0000001F, frac);
        break;
    case GX_VA_TEX5:
        SET_FIELD(desc->vatC, 0x00003FE0, (count << 5) | (type << 6) | (frac << 9));
        break;
    case GX_VA_TEX6:
        SET_FIELD(desc->vatC, 0x007FC000, (count << 14) | (type << 15) | (frac << 18));
        break;
    case GX_VA_TEX7:
        SET_FIELD(desc->vatC, 0xFF800000, (count << 23) | (type << 24) | (frac << 27));
        break;
    }
}
}
