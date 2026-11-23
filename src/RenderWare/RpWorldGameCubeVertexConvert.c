/* RenderWare world plugin, GameCube: vertex attribute conversion, 0x80466B58..0x8046ACBC.
 * RwV3d positions and normals, packed normals (RpVertexNormal), RwRGBA colours and texture
 * coordinates written into the byte, short or float component streams of a display list's
 * vertex arrays. Names are inferred from behaviour.
 *
 * Source file: one .sdata2 pool, 0x805FB1F0..0x805FB210 (1.0f, 0.0f, the normal scales 64.0f and
 * 16384.0f, 1/128 and the int-to-float double), in the order the functions here use them; the
 * next file starts with fn_8046ACBC (RpWorldGameCubeListSize.c), whose jump table opens a new
 * .data group at 0x8056DAF8. Nothing in the binary fixes the start of this file: the
 * vertex-format helpers before it (0x8046693C..0x80466B58, range r3) neither share data with it
 * nor are called by it, so the range boundary is used. Old units Game/RwCpp29, RwCpp30 and the
 * start of RwCpp31.
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw (cflags_rw_132): every loop below is
 * unrolled eight times by the compiler, which GC 2.x schedules differently (measured: 2.7 misses
 * fn_80466B58 by 1342 rows). The output pointers advance through an integer cast as in retail.
 * Every function and section matches; the unit is Matching. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

double fmod(double, double);

/* Packs `num` positions into `dest`, one record every `stride` bytes (0 selects the packed
 * record size). `type` is the GX component type: 0/1 bytes, 2/3 shorts, 4 floats (copied).
 * `scale` quantises each component; `offset`, when present, is first snapped down to a whole
 * number of quanta (1 / scale) and subtracted. Returns the packed size of the data. */
s32 fn_80466B58(void *dest, const RwV3d *src, s32 type, s32 num, u32 stride, const RwV3d *offset,
                f32 scale) {
    s32 size = 0;
    f32 shiftX;
    f32 shiftY;
    f32 shiftZ;
    s32 i;

    if (offset != NULL) {
        f32 quantum = 1.0f / scale;
        shiftX = offset->x - (f32)fmod(offset->x, quantum);
        shiftY = offset->y - (f32)fmod(offset->y, quantum);
        shiftZ = offset->z - (f32)fmod(offset->z, quantum);
    } else {
        shiftX = 0.0f;
        shiftY = shiftX;
        shiftZ = shiftX;
    }

    switch (type) {
    case 0: {
        u8 *out;

        if (stride == 0) {
            stride = 3;
        }
        out = (u8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (u8)(s32)(scale * (src->x - shiftX));
            out[1] = (u8)(s32)(scale * (src->y - shiftY));
            out[2] = (u8)(s32)(scale * (src->z - shiftZ));
            out = (u8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 1: {
        s8 *out;

        if (stride == 0) {
            stride = 3;
        }
        out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s8)(s32)(scale * (src->x - shiftX));
            out[1] = (s8)(s32)(scale * (src->y - shiftY));
            out[2] = (s8)(s32)(scale * (src->z - shiftZ));
            out = (s8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 2: {
        u16 *out;

        if (stride == 0) {
            stride = 6;
        }
        out = (u16 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (u16)(s32)(scale * (src->x - shiftX));
            out[1] = (u16)(s32)(scale * (src->y - shiftY));
            out[2] = (u16)(s32)(scale * (src->z - shiftZ));
            out = (u16 *)((u32)out + stride);
            src++;
        }
        size = num * 6;
        break;
    }
    case 3: {
        s16 *out;

        if (stride == 0) {
            stride = 6;
        }
        out = (s16 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s16)(s32)(scale * (src->x - shiftX));
            out[1] = (s16)(s32)(scale * (src->y - shiftY));
            out[2] = (s16)(s32)(scale * (src->z - shiftZ));
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 6;
        break;
    }
    case 4: {
        RwGameCubeV3d *out;
        s32 j;

        if (stride == 0) {
            stride = 12;
        }
        out = (RwGameCubeV3d *)dest;
        for (j = 0; j < num; j++) {
            *out = *(const RwGameCubeV3d *)src;
            out = (RwGameCubeV3d *)((u32)out + stride);
            src++;
        }
        size = num * 12;
        break;
    }
    }
    return size;
}

/* The fixed-point scales of the signed normal types: bytes hold 2.6, shorts 2.14. They are
 * objects of their own, not literals: GC 1.3.2 folds a scalar constant and then multiplies
 * with the constant as the first operand, where retail has the component first (and the
 * two values sit between the position code's and the packed normals' literals in .sdata2). */
static const f32 normalByteScale[1] = { 64.0f };
static const f32 normalShortScale[1] = { 16384.0f };

/* The same for normals, with the quantiser fixed by the component type: signed bytes hold
 * 2.6 fixed point (x64), signed shorts 2.14 (x16384), floats are copied. The unsigned types
 * 0 and 2 store nothing. */
s32 fn_80467BC8(void *dest, const RwV3d *src, s32 type, s32 num, u32 stride) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0:
        break;
    case 1: {
        s8 *out;

        if (stride == 0) {
            stride = 3;
        }
        out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s8)(s32)(src->x * normalByteScale[0]);
            out[1] = (s8)(s32)(src->y * normalByteScale[0]);
            out[2] = (s8)(s32)(src->z * normalByteScale[0]);
            out = (s8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 2:
        break;
    case 3: {
        s16 *out;

        if (stride == 0) {
            stride = 6;
        }
        out = (s16 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s16)(s32)(src->x * normalShortScale[0]);
            out[1] = (s16)(s32)(src->y * normalShortScale[0]);
            out[2] = (s16)(s32)(src->z * normalShortScale[0]);
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 6;
        break;
    }
    case 4: {
        RwGameCubeV3d *out;
        s32 j;

        if (stride == 0) {
            stride = 12;
        }
        out = (RwGameCubeV3d *)dest;
        for (j = 0; j < num; j++) {
            *out = *(const RwGameCubeV3d *)src;
            out = (RwGameCubeV3d *)((u32)out + stride);
            src++;
        }
        size = num * 12;
        break;
    }
    }
    return size;
}

/* Packed normals to a normal stream. `type` is the GX component type; only the signed types
 * (1 bytes, 3 shorts) and floats (4) are produced. Returns the packed size of the data. */
s32 fn_804683AC(void *dest, const RpVertexNormal *src, s32 type, s32 num, u32 stride) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0:
        break;
    case 1: {
        s8 *out;

        if (stride == 0) {
            stride = 3;
        }
        out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            RwV3d normal;

            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s8)(s32)(normal.x * normalByteScale[0]);
            out[1] = (s8)(s32)(normal.y * normalByteScale[0]);
            out[2] = (s8)(s32)(normal.z * normalByteScale[0]);
            out = (s8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 2:
        break;
    case 3: {
        s16 *out;

        if (stride == 0) {
            stride = 6;
        }
        out = (s16 *)dest;
        for (i = 0; i < num; i++) {
            RwV3d normal;

            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s16)(s32)(normal.x * normalShortScale[0]);
            out[1] = (s16)(s32)(normal.y * normalShortScale[0]);
            out[2] = (s16)(s32)(normal.z * normalShortScale[0]);
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 6;
        break;
    }
    case 4: {
        RwGameCubeV3d *out;
        s32 j;

        if (stride == 0) {
            stride = 12;
        }
        out = (RwGameCubeV3d *)dest;
        for (j = 0; j < num; j++) {
            RwGameCubeV3d normal;

            normal.v[0] = (1.0f / 128.0f) * (f32)src->x;
            normal.v[1] = (1.0f / 128.0f) * (f32)src->y;
            normal.v[2] = (1.0f / 128.0f) * (f32)src->z;
            src++;
            *out = normal;
            out = (RwGameCubeV3d *)((u32)out + stride);
        }
        size = num * 12;
        break;
    }
    }
    return size;
}

/* RwV3d normals to a normal stream: fn_80467BC8 again, instruction for instruction, except
 * that the float case reports three normals per vertex (normal, binormal and tangent). */
s32 fn_80468ADC(void *dest, const RwV3d *src, s32 type, s32 num, u32 stride) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0:
        break;
    case 1: {
        s8 *out;

        if (stride == 0) {
            stride = 3;
        }
        out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s8)(s32)(src->x * normalByteScale[0]);
            out[1] = (s8)(s32)(src->y * normalByteScale[0]);
            out[2] = (s8)(s32)(src->z * normalByteScale[0]);
            out = (s8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 2:
        break;
    case 3: {
        s16 *out;

        if (stride == 0) {
            stride = 6;
        }
        out = (s16 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s16)(s32)(src->x * normalShortScale[0]);
            out[1] = (s16)(s32)(src->y * normalShortScale[0]);
            out[2] = (s16)(s32)(src->z * normalShortScale[0]);
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 6;
        break;
    }
    case 4: {
        RwGameCubeV3d *out;
        s32 j;

        if (stride == 0) {
            stride = 12;
        }
        out = (RwGameCubeV3d *)dest;
        for (j = 0; j < num; j++) {
            *out = *(const RwGameCubeV3d *)src;
            out = (RwGameCubeV3d *)((u32)out + stride);
            src++;
        }
        size = num * 36;
        break;
    }
    }
    return size;
}

/* Packed normals again: the twin of fn_804683AC, with fn_80468ADC's float size. */
s32 fn_804692C0(void *dest, const RpVertexNormal *src, s32 type, s32 num, u32 stride) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0:
        break;
    case 1: {
        s8 *out;

        if (stride == 0) {
            stride = 3;
        }
        out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            RwV3d normal;

            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s8)(s32)(normal.x * normalByteScale[0]);
            out[1] = (s8)(s32)(normal.y * normalByteScale[0]);
            out[2] = (s8)(s32)(normal.z * normalByteScale[0]);
            out = (s8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 2:
        break;
    case 3: {
        s16 *out;

        if (stride == 0) {
            stride = 6;
        }
        out = (s16 *)dest;
        for (i = 0; i < num; i++) {
            RwV3d normal;

            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s16)(s32)(normal.x * normalShortScale[0]);
            out[1] = (s16)(s32)(normal.y * normalShortScale[0]);
            out[2] = (s16)(s32)(normal.z * normalShortScale[0]);
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 6;
        break;
    }
    case 4: {
        RwGameCubeV3d *out;
        s32 j;

        if (stride == 0) {
            stride = 12;
        }
        out = (RwGameCubeV3d *)dest;
        for (j = 0; j < num; j++) {
            RwGameCubeV3d normal;

            normal.v[0] = (1.0f / 128.0f) * (f32)src->x;
            normal.v[1] = (1.0f / 128.0f) * (f32)src->y;
            normal.v[2] = (1.0f / 128.0f) * (f32)src->z;
            src++;
            *out = normal;
            out = (RwGameCubeV3d *)((u32)out + stride);
        }
        size = num * 36;
        break;
    }
    }
    return size;
}

/* RwRGBA colours to a colour stream. `type` is the GX colour format: 0 RGB565, 1 RGB8,
 * 2 RGBX8, 3 RGBA4, 4 RGBA6, 5 RGBA8. Returns the packed size of the data. */
s32 fn_804699F0(void *dest, const RwRGBA *src, s32 type, s32 num, u32 stride) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0: {
        u16 *out;

        if (stride == 0) {
            stride = 2;
        }
        out = (u16 *)dest;
        for (i = 0; i < num; i++) {
            u8 r = src->red, g = src->green, bl = src->blue;
            *out = ((r << 8) & 0xF800) | ((g << 3) & 0x7E0) | ((bl >> 3) & 0x1F);
            out = (u16 *)((u32)out + stride);
            src++;
        }
        size = num * 2;
        break;
    }
    case 1: {
        u8 *out;
        s32 j;

        if (stride == 0) {
            stride = 3;
        }
        out = (u8 *)dest;
        for (j = 0; j < num; j++) {
            out[0] = src->red;
            out[1] = src->green;
            out[2] = src->blue;
            out = (u8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 3: {
        u16 *out;

        if (stride == 0) {
            stride = 2;
        }
        out = (u16 *)dest;
        for (i = 0; i < num; i++) {
            u8 r = src->red, g = src->green, bl = src->blue;
            *out = ((r << 8) & 0xF000) | ((g << 4) & 0xF00) | (bl & 0xF0) | ((src->alpha >> 4) & 0xF);
            out = (u16 *)((u32)out + stride);
            src++;
        }
        size = num * 2;
        break;
    }
    case 4: {
        u8 *out;

        if (stride == 0) {
            stride = 3;
        }
        out = (u8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (src->red & 0xFC) | ((src->green >> 6) & 0x03);
            out[1] = ((src->green << 2) & 0xF0) | ((src->blue >> 4) & 0x0F);
            out[2] = ((src->blue << 4) & 0xC0) | ((src->alpha >> 2) & 0x3F);
            out += stride;
            src++;
        }
        size = num * 3;
        break;
    }
    case 2:
    case 5: {
        u32 *out;
        s32 j;

        if (stride == 0) {
            stride = 4;
        }
        out = (u32 *)dest;
        for (j = 0; j < num; j++) {
            *out = *(const u32 *)src;
            out = (u32 *)((u32)out + stride);
            src++;
        }
        size = num * 4;
        break;
    }
    }
    return size;
}

/* Texture coordinates to a coordinate stream, scaled by 2^frac for the fixed-point types
 * (0 u8, 1 s8, 2 u16, 3 s16); 4 copies the floats. */
s32 fn_8046A270(void *dest, const RwTexCoords *src, s32 type, s32 num, u32 stride, f32 scale) {
    s32 size = 0;
    s32 i;

    switch (type) {
    case 0: {
        u8 *out;

        if (stride == 0) {
            stride = 2;
        }
        out = (u8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (u8)(s32)(src->u * scale);
            out[1] = (u8)(s32)(src->v * scale);
            out += stride;
            src++;
        }
        size = num * 2;
        break;
    }
    case 1: {
        s8 *out;

        if (stride == 0) {
            stride = 2;
        }
        out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s8)(s32)(src->u * scale);
            out[1] = (s8)(s32)(src->v * scale);
            out += stride;
            src++;
        }
        size = num * 2;
        break;
    }
    case 2: {
        u16 *out;
        s32 j;

        if (stride == 0) {
            stride = 4;
        }
        out = (u16 *)dest;
        for (j = 0; j < num; j++) {
            out[0] = (u16)(s32)(src->u * scale);
            out[1] = (u16)(s32)(src->v * scale);
            out = (u16 *)((u32)out + stride);
            src++;
        }
        size = num * 4;
        break;
    }
    case 3: {
        s16 *out;
        s32 j;

        if (stride == 0) {
            stride = 4;
        }
        out = (s16 *)dest;
        for (j = 0; j < num; j++) {
            out[0] = (s16)(s32)(src->u * scale);
            out[1] = (s16)(s32)(src->v * scale);
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 4;
        break;
    }
    case 4: {
        RwGameCubeV2d *out;
        s32 j;

        if (stride == 0) {
            stride = 8;
        }
        out = (RwGameCubeV2d *)dest;
        for (j = 0; j < num; j++) {
            *out = *(const RwGameCubeV2d *)src;
            out = (RwGameCubeV2d *)((u32)out + stride);
            src++;
        }
        size = num * 8;
        break;
    }
    }
    return size;
}
