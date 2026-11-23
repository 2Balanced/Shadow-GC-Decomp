/* RenderWare GameCube driver, vertex-array conversion (first part), 0x80466B58..0x804683AC:
 * RwV3d arrays packed into the byte, short or float component streams of a display list's
 * vertex arrays. Names are inferred from behaviour. Compiled by CodeWarrior GC 1.3.2 with
 * -O4,p (see configure.py): every loop below is unrolled eight times by the compiler, and the
 * output pointers advance through an integer cast as in retail. Both functions match. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

extern "C" {
/* The literals of this file. RwCpp29 and RwCpp30 are one source file cut in two (RwCpp30
 * uses the same 64.0f and 16384.0f, and .sdata2 holds one copy), so until the units are joined
 * the constants are referenced by address: an anonymous literal could not be shared. */
extern const f32 lbl_805FB1F0; /* 1.0f */
extern const f32 lbl_805FB1F4; /* 0.0f */
extern const f32 lbl_805FB1F8; /* 64.0f */
extern const f32 lbl_805FB1FC; /* 16384.0f */

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
        f32 quantum = lbl_805FB1F0 / scale;
        shiftX = offset->x - (f32)fmod(offset->x, quantum);
        shiftY = offset->y - (f32)fmod(offset->y, quantum);
        shiftZ = offset->z - (f32)fmod(offset->z, quantum);
    } else {
        shiftX = lbl_805FB1F4;
        shiftY = shiftX;
        shiftZ = shiftX;
    }

    switch (type) {
    case 0: {
        if (stride == 0) {
            stride = 3;
        }
        u8 *out = (u8 *)dest;
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
        if (stride == 0) {
            stride = 3;
        }
        s8 *out = (s8 *)dest;
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
        if (stride == 0) {
            stride = 6;
        }
        u16 *out = (u16 *)dest;
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
        if (stride == 0) {
            stride = 6;
        }
        s16 *out = (s16 *)dest;
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
        if (stride == 0) {
            stride = 12;
        }
        RwGameCubeV3d *out = (RwGameCubeV3d *)dest;
        s32 j;
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
        if (stride == 0) {
            stride = 3;
        }
        s8 *out = (s8 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s8)(s32)(src->x * lbl_805FB1F8);
            out[1] = (s8)(s32)(src->y * lbl_805FB1F8);
            out[2] = (s8)(s32)(src->z * lbl_805FB1F8);
            out = (s8 *)((u32)out + stride);
            src++;
        }
        size = num * 3;
        break;
    }
    case 2:
        break;
    case 3: {
        if (stride == 0) {
            stride = 6;
        }
        s16 *out = (s16 *)dest;
        for (i = 0; i < num; i++) {
            out[0] = (s16)(s32)(src->x * lbl_805FB1FC);
            out[1] = (s16)(s32)(src->y * lbl_805FB1FC);
            out[2] = (s16)(s32)(src->z * lbl_805FB1FC);
            out = (s16 *)((u32)out + stride);
            src++;
        }
        size = num * 6;
        break;
    }
    case 4: {
        if (stride == 0) {
            stride = 12;
        }
        RwGameCubeV3d *out = (RwGameCubeV3d *)dest;
        s32 j;
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
}
