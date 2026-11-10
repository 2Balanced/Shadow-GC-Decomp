/* RenderWare vertex-array element conversion, float triples packed into byte,
 * short or float streams, 0x80466B58..0x804683AC.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern f32 lbl_805FB1F0; /* 1.0f */
extern f32 lbl_805FB1F4; /* 0.0f */
extern f32 lbl_805FB1F8; /* 64.0f */
extern f32 lbl_805FB1FC; /* 16384.0f */

double fmod(double, double);

/* Packs `vertices` three-component float records from `source` into `dest`, one
 * record every `stride` bytes: a zero stride selects the packed record size (3
 * bytes, 6 shorts-worth, or 12 floats-worth). `type` selects the component
 * storage -- 0 and 1 store bytes, 2 and 3 store shorts, 4 copies the floats
 * unchanged; any other value does nothing. `scale` quantizes each component and
 * `offset`, when present, phase-shifts it after being wrapped to one destination
 * quantum (1 / scale), which makes the wrapped shift exact after scaling.
 * Returns the destination bytes produced, i.e. vertices times the record size. */
s32 fn_80466B58(u8 *dest, const f32 *source, s32 type, s32 vertices, u32 stride,
                const f32 *offset, f32 scale) {
    f32 shift[3];
    if (offset != NULL) {
        f32 period = lbl_805FB1F0 / scale;
        shift[0] = offset[0] - (f32)fmod((double)offset[0], (double)period);
        shift[1] = offset[1] - (f32)fmod((double)offset[1], (double)period);
        shift[2] = offset[2] - (f32)fmod((double)offset[2], (double)period);
    } else {
        shift[0] = shift[1] = shift[2] = lbl_805FB1F4;
    }

    s32 bytes = 0;
    switch (type) {
    case 0:
    case 1:
        if (stride == 0) stride = 3;
        for (s32 i = 0; i < vertices; i++) {
            u8 *out = dest + (u32)i * stride;
            const f32 *in = source + i * 3;
            out[0] = (u8)(s32)(scale * (in[0] - shift[0]));
            out[1] = (u8)(s32)(scale * (in[1] - shift[1]));
            out[2] = (u8)(s32)(scale * (in[2] - shift[2]));
        }
        bytes = vertices * 3;
        break;
    case 2:
    case 3:
        if (stride == 0) stride = 6;
        for (s32 i = 0; i < vertices; i++) {
            u16 *out = (u16 *)(dest + (u32)i * stride);
            const f32 *in = source + i * 3;
            out[0] = (u16)(s32)(scale * (in[0] - shift[0]));
            out[1] = (u16)(s32)(scale * (in[1] - shift[1]));
            out[2] = (u16)(s32)(scale * (in[2] - shift[2]));
        }
        bytes = vertices * 6;
        break;
    case 4:
        if (stride == 0) stride = 0xC;
        for (s32 i = 0; i < vertices; i++) {
            u32 *out = (u32 *)(dest + (u32)i * stride);
            const u32 *in = (const u32 *)source + i * 3;
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
        }
        bytes = vertices * 0xC;
        break;
    }
    return bytes;
}

/* The same packing with the quantizer fixed per storage width instead of taken
 * from the caller: `type` 1 scales by 64 into bytes, 3 scales by 16384 (2^14)
 * into shorts, and 4 copies floats; 0, 2 and any larger value do nothing. No
 * offset is applied. */
s32 fn_80467BC8(u8 *dest, const f32 *source, s32 type, s32 vertices, u32 stride) {
    switch (type) {
    case 1:
        if (stride == 0) stride = 3;
        for (s32 i = 0; i < vertices; i++) {
            u8 *out = dest + (u32)i * stride;
            const f32 *in = source + i * 3;
            out[0] = (u8)(s32)(lbl_805FB1F8 * in[0]);
            out[1] = (u8)(s32)(lbl_805FB1F8 * in[1]);
            out[2] = (u8)(s32)(lbl_805FB1F8 * in[2]);
        }
        return vertices * 3;
    case 3:
        if (stride == 0) stride = 6;
        for (s32 i = 0; i < vertices; i++) {
            u16 *out = (u16 *)(dest + (u32)i * stride);
            const f32 *in = source + i * 3;
            out[0] = (u16)(s32)(lbl_805FB1FC * in[0]);
            out[1] = (u16)(s32)(lbl_805FB1FC * in[1]);
            out[2] = (u16)(s32)(lbl_805FB1FC * in[2]);
        }
        return vertices * 6;
    case 4:
        if (stride == 0) stride = 0xC;
        for (s32 i = 0; i < vertices; i++) {
            u32 *out = (u32 *)(dest + (u32)i * stride);
            const u32 *in = (const u32 *)source + i * 3;
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
        }
        return vertices * 0xC;
    }
    return 0;
}
}
