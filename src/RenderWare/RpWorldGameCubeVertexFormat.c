/* RenderWare world plugin, GameCube: the display-list header, its attribute arrays and the
 * vertex descriptor, 0x8046EE8C..0x80470474. fn_8046EE8C and fn_8046EEA0 size the header and
 * the arrays an instance carries, fn_8046F208 lays the arrays out and fn_8046F6CC fills them,
 * and fn_8046FC1C .. fn_8047046C set the descriptor's vertex attribute formats
 * (GXSetVtxAttrFmt) and vertex descriptor (GXSetVtxDesc) on the driver's copy. Names are
 * inferred from behaviour.
 *
 * Source file: .data 0x8056DBA8..0x8056DCE8 (the jump tables of fn_8046EEA0, fn_8046F208,
 * fn_8046F6CC, fn_8046FC50 and fn_8046FF34, padded to 8 bytes after 0x8056DCE4) and its own
 * int-to-float double (.sdata2 0x805FB218). The 2D toolkit's brush file starts at 0x80470474.
 * Old units Game/RwCpp34 (from fn_8046EE8C) and RwCpp35 (first two functions).
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw (cflags_rw_132).
 * Matching: fn_8046EE8C, fn_8046EEA0, fn_8046F208 (its counters declared in retail's register
 * order, initialised), fn_8046FC1C, fn_8046FC48, fn_8046FC50, fn_8047046C. fn_8046FF34 differs
 * in two scratch registers of its texture-matrix and colour cases (attribute and type
 * parameter types, mask spellings and statement forms probed: no change), fn_8046F6CC (92%) in
 * the scheduling of its texture cases. The texture shifts of TEX1..TEX3 are masked after the
 * shift, as retail computes them. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

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

/* One attribute array of an instance: where it is, which attribute it holds, the size of an
 * element and the index type. */
#define SET_RECORD(n, array, attr, stride, type) \
    do {                                         \
        header->arrays[n].data = (array);        \
        header->arrays[n].attr = (attr);         \
        header->arrays[n].stride = (stride);     \
        header->arrays[n].type = (type);         \
    } while (0)

/* Lays out the attribute arrays of an instance in `data` and records them in its header: every
 * indexed attribute gets an array, rounded up to 32 bytes. Normals with binormals and tangents
 * hold three normals per element. */
void fn_8046F208(RwGameCubeVtxDesc *desc, RwGameCubeInstHeader *header, RwGameCubeArrays *arrays,
                 u8 *data) {
    u32 n = 0;
    u32 offset = 0;
    u32 attr;

    for (attr = GX_VA_POS; attr < GX_VA_TEX7 + 1; attr++) {
        u8 *array = data + offset;
        u32 type;
        u32 stride;

        switch (attr) {
        case GX_VA_POS:
            type = RWGAMECUBE_POS_TYPE(desc);
            if (INDEXED(type)) {
                stride = fn_8046693C(desc);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_NRM:
            type = RWGAMECUBE_NRM_TYPE(desc);
            if (INDEXED(type)) {
                if (((desc->vatA >> 9) & 1) == 1) {
                    stride = fn_804669A0(desc) * 3;
                    SET_RECORD(n, array, attr, stride, type);
                    offset += ROUND32(stride * arrays->num[GX_VA_NRM]);
                } else {
                    stride = fn_804669A0(desc);
                    SET_RECORD(n, array, attr, stride, type);
                    offset += ROUND32(stride * arrays->num[GX_VA_NRM]);
                }
                n++;
            }
            break;
        case GX_VA_CLR0:
        case GX_VA_CLR1:
            type = RWGAMECUBE_CLR_TYPE(desc, attr - GX_VA_CLR0);
            if (INDEXED(type)) {
                stride = fn_804669EC(desc, attr - GX_VA_CLR0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX0:
            type = RWGAMECUBE_TEX_TYPE(desc, 0);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX1:
            type = RWGAMECUBE_TEX_TYPE(desc, 1);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX2:
            type = RWGAMECUBE_TEX_TYPE(desc, 2);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX3:
            type = RWGAMECUBE_TEX_TYPE(desc, 3);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX4:
            type = RWGAMECUBE_TEX_TYPE(desc, 4);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX5:
            type = RWGAMECUBE_TEX_TYPE(desc, 5);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX6:
            type = RWGAMECUBE_TEX_TYPE(desc, 6);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        case GX_VA_TEX7:
            type = RWGAMECUBE_TEX_TYPE(desc, 7);
            if (INDEXED(type)) {
                stride = fn_80466A54(desc, attr - GX_VA_TEX0);
                SET_RECORD(n, array, attr, stride, type);
                n++;
                offset += ROUND32(stride * arrays->num[attr]);
            }
            break;
        }
    }
    header->numArrays = n;
}

/* Fills the attribute arrays of an instance (fn_8046F208 laid them out) from the geometry's
 * arrays: positions and texture coordinates quantised with each attribute's fixed-point shift,
 * normals from RwV3d or, with `packedNormals`, from the packed normals of a world sector.
 * `offset` is subtracted from the positions. The texture shifts of TEX1 to TEX3 are masked
 * after the shift, as retail does. */
void fn_8046F6CC(RwGameCubeVtxDesc *desc, RwGameCubeInstHeader *header, RwGameCubeArrays *arrays,
                 s32 packedNormals, const RwV3d *offset) {
    u32 attr;
    u32 n = 0;

    for (attr = GX_VA_POS; attr < GX_VA_TEX7 + 1; attr++) {
        switch (attr) {
        case GX_VA_POS:
            if (INDEXED(RWGAMECUBE_POS_TYPE(desc))) {
                if ((desc->vatA & 1) == 1) {
                    fn_80466B58(header->arrays[n].data, arrays->data[attr], (desc->vatA >> 1) & 7,
                                arrays->num[attr], header->arrays[n].stride, offset,
                                (f32)(1 << ((desc->vatA >> 4) & 0x1F)));
                    n++;
                }
            }
            break;
        case GX_VA_NRM:
            if (INDEXED(RWGAMECUBE_NRM_TYPE(desc))) {
                u32 type = (desc->vatA >> 10) & 7;

                if (((desc->vatA >> 9) & 1) == 1) {
                    if (packedNormals == 0) {
                        fn_80468ADC(header->arrays[n].data, arrays->data[attr], type,
                                    arrays->num[attr], header->arrays[n].stride);
                    } else {
                        fn_804692C0(header->arrays[n].data, arrays->data[attr], type,
                                    arrays->num[attr], header->arrays[n].stride);
                    }
                } else {
                    if (packedNormals == 0) {
                        fn_80467BC8(header->arrays[n].data, arrays->data[attr], type,
                                    arrays->num[attr], header->arrays[n].stride);
                    } else {
                        fn_804683AC(header->arrays[n].data, arrays->data[attr], type,
                                    arrays->num[attr], header->arrays[n].stride);
                    }
                }
                n++;
            }
            break;
        case GX_VA_CLR0:
        case GX_VA_CLR1:
            if (INDEXED(RWGAMECUBE_CLR_TYPE(desc, attr - GX_VA_CLR0))) {
                fn_804699F0(header->arrays[n].data, arrays->data[attr],
                            (desc->vatA >> ((attr - GX_VA_CLR0) * 4 + 14)) & 7, arrays->num[attr],
                            header->arrays[n].stride);
                n++;
            }
            break;
        case GX_VA_TEX0:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 0))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatA >> 22) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)(1 << ((desc->vatA >> 25) & 0x1F)));
                n++;
            }
            break;
        case GX_VA_TEX1:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 1))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatB >> 1) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)((1 << (desc->vatB >> 4)) & 0x1F));
                n++;
            }
            break;
        case GX_VA_TEX2:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 2))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatB >> 10) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)((1 << (desc->vatB >> 13)) & 0x1F));
                n++;
            }
            break;
        case GX_VA_TEX3:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 3))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatB >> 19) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)((1 << (desc->vatB >> 22)) & 0x1F));
                n++;
            }
            break;
        case GX_VA_TEX4:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 4))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatB >> 28) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)(1 << (desc->vatC & 0x1F)));
                n++;
            }
            break;
        case GX_VA_TEX5:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 5))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatC >> 6) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)(1 << ((desc->vatC >> 9) & 0x1F)));
                n++;
            }
            break;
        case GX_VA_TEX6:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 6))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatC >> 15) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)(1 << ((desc->vatC >> 18) & 0x1F)));
                n++;
            }
            break;
        case GX_VA_TEX7:
            if (INDEXED(RWGAMECUBE_TEX_TYPE(desc, 7))) {
                fn_8046A270(header->arrays[n].data, arrays->data[attr], (desc->vatC >> 24) & 7,
                            arrays->num[attr], header->arrays[n].stride,
                            (f32)(1 << ((desc->vatC >> 27) & 0x1F)));
                n++;
            }
            break;
        }
    }
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
void fn_8046FC50(RwGameCubeVtxDesc *desc, s32 attr, s32 count, s32 type, u8 frac) {
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

/* Counts the texture coordinates the descriptor enables into bits 4..7 of `unk18`. */
#define COUNT_TEX()                             \
    do {                                        \
        u32 i = 8;                              \
        u32 num = 0;                            \
        while (i-- != 0) {                      \
            if (desc->vcdHi & (3 << (i * 2))) { \
                num++;                          \
            }                                   \
        }                                       \
        SET_FIELD(desc->unk18, 0xF0, num << 4); \
    } while (0)

/* GXSetVtxDesc on the descriptor: how one attribute reaches the display list (GX_NONE,
 * GX_DIRECT, GX_INDEX8 or GX_INDEX16). `unk18` keeps the counts GX derives from it: colours
 * in bits 0..1, the normal kind in bits 2..3, texture coordinates in bits 4..7. */
void fn_8046FF34(RwGameCubeVtxDesc *desc, s32 attr, GXAttrType type) {
    switch (attr) {
    case GX_VA_PNMTXIDX:
        SET_FIELD(desc->vcdLo, 1, type);
        break;
    case GX_VA_TEX0MTXIDX:
    case GX_VA_TEX0MTXIDX + 1:
    case GX_VA_TEX0MTXIDX + 2:
    case GX_VA_TEX0MTXIDX + 3:
    case GX_VA_TEX0MTXIDX + 4:
    case GX_VA_TEX0MTXIDX + 5:
    case GX_VA_TEX0MTXIDX + 6:
    case GX_VA_TEX0MTXIDX + 7:
        SET_FIELD(desc->vcdLo, 2 << (attr - GX_VA_TEX0MTXIDX), type << attr);
        break;
    case GX_VA_POS:
        SET_FIELD(desc->vcdLo, 0x600, type << 9);
        break;
    case GX_VA_NRM: {
        u32 kind = 0;
        SET_FIELD(desc->vcdLo, 0x1800, type << 11);
        if (type != GX_NONE) {
            kind = 1;
        }
        SET_FIELD(desc->unk18, 0xC, kind << 2);
        break;
    }
    case GX_VA_NBT: {
        u32 kind = 0;
        SET_FIELD(desc->vcdLo, 0x1800, type << 11);
        if (type != GX_NONE) {
            kind = 2;
        }
        SET_FIELD(desc->unk18, 0xC, kind << 2);
        break;
    }
    case GX_VA_CLR0:
    case GX_VA_CLR1: {
        u32 num = 0;
        SET_FIELD(desc->vcdLo, 0x6000 << (attr - GX_VA_CLR0), type << (attr + 2));
        if (desc->vcdLo & 0x6000) {
            num = 1;
        }
        if (desc->vcdLo & 0x18000) {
            num++;
        }
        SET_FIELD(desc->unk18, 3, num);
        break;
    }
    case GX_VA_TEX0:
        SET_FIELD(desc->vcdHi, 3 << 0, type << 0);
        COUNT_TEX();
        break;
    case GX_VA_TEX1:
        SET_FIELD(desc->vcdHi, 3 << 2, type << 2);
        COUNT_TEX();
        break;
    case GX_VA_TEX2:
        SET_FIELD(desc->vcdHi, 3 << 4, type << 4);
        COUNT_TEX();
        break;
    case GX_VA_TEX3:
        SET_FIELD(desc->vcdHi, 3 << 6, type << 6);
        COUNT_TEX();
        break;
    case GX_VA_TEX4:
        SET_FIELD(desc->vcdHi, 3 << 8, type << 8);
        COUNT_TEX();
        break;
    case GX_VA_TEX5:
        SET_FIELD(desc->vcdHi, 3 << 10, type << 10);
        COUNT_TEX();
        break;
    case GX_VA_TEX6:
        SET_FIELD(desc->vcdHi, 3 << 12, type << 12);
        COUNT_TEX();
        break;
    case GX_VA_TEX7:
        SET_FIELD(desc->vcdHi, 3 << 14, type << 14);
        COUNT_TEX();
        break;
    }
}

void fn_8047046C(RwGameCubeVtxDesc *desc, u8 numArrays) {
    desc->numArrays = numArrays;
}
