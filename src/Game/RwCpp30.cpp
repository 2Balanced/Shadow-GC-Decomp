/* RenderWare GameCube driver, vertex-array conversion (second part), 0x804683AC..0x804699F0:
 * packed normals (RpVertexNormal, three signed bytes in 1.7 fixed point) and RwV3d normals
 * converted to the signed byte (2.6), signed short (2.14) or float normal streams of a display
 * list. Names are inferred from behaviour. Compiled by CodeWarrior GC 1.3.2 with -O4,p.
 * All three functions match and the unit is Matching. RwCpp29 and RwCpp30 are one source file
 * cut in two: its .sdata2 pool is 0x805FB1F0..0x805FB210, of which this unit owns 1/128 and the
 * int-to-float double (0x805FB200..0x805FB210); the 64.0f and 16384.0f it shares with
 * fn_80467BC8 (RwCpp29.cpp) are referenced by address. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

extern "C" {
/* The literals of fn_80467BC8 (RwCpp29.cpp), which fn_80468ADC shares: see below. */
extern const f32 lbl_805FB1F8; /* 64.0f */
extern const f32 lbl_805FB1FC; /* 16384.0f */

/* Packed normals to a normal stream. `type` is the GX component type; only the signed types
 * (1 bytes, 3 shorts) and floats (4) are produced. Returns the packed size of the data. */
s32 fn_804683AC(void *dest, const RpVertexNormal *src, s32 type, s32 num, u32 stride) {
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
            RwV3d normal;
            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s8)(s32)(normal.x * lbl_805FB1F8);
            out[1] = (s8)(s32)(normal.y * lbl_805FB1F8);
            out[2] = (s8)(s32)(normal.z * lbl_805FB1F8);
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
            RwV3d normal;
            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s16)(s32)(normal.x * lbl_805FB1FC);
            out[1] = (s16)(s32)(normal.y * lbl_805FB1FC);
            out[2] = (s16)(s32)(normal.z * lbl_805FB1FC);
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

/* RwV3d normals to a normal stream: the twin of fn_80467BC8 (RwCpp29.cpp), instruction for
 * instruction, so probably the same static function of a second source file. */
s32 fn_80468ADC(void *dest, const RwV3d *src, s32 type, s32 num, u32 stride) {
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
        size = num * 36;
        break;
    }
    }
    return size;
}

/* Packed normals again: the twin of fn_804683AC. */
s32 fn_804692C0(void *dest, const RpVertexNormal *src, s32 type, s32 num, u32 stride) {
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
            RwV3d normal;
            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s8)(s32)(normal.x * lbl_805FB1F8);
            out[1] = (s8)(s32)(normal.y * lbl_805FB1F8);
            out[2] = (s8)(s32)(normal.z * lbl_805FB1F8);
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
            RwV3d normal;
            normal.x = (1.0f / 128.0f) * (f32)src->x;
            normal.y = (1.0f / 128.0f) * (f32)src->y;
            normal.z = (1.0f / 128.0f) * (f32)src->z;
            out[0] = (s16)(s32)(normal.x * lbl_805FB1FC);
            out[1] = (s16)(s32)(normal.y * lbl_805FB1FC);
            out[2] = (s16)(s32)(normal.z * lbl_805FB1FC);
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
}
