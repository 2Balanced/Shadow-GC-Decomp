/* RenderWare core: image resampling (0x8048A178-0x8048ADE0).
 * The mean colour of one row span and of a block of rows of a 32-bit image, RwImageResample and
 * RwImageCreateResample (which converts a source of another depth first and inlines the
 * resample). The file owns its constant pool (.sdata2 0x805FB328-0x805FB358).
 * Boundary: the frame hierarchy sync after 0x8048ADE0 uses no constants; the 1.0f at 0x805FB358
 * is the texture module's (this pool already has one at 0x805FB330).
 * NonMatching: fn_8048A178 and fn_8048A790: scheduling, and GC/2.7 reassociates
 * srcY + (yStep - 1) (wave 40, probes/Game_RwCpp50.py; eight more declaration and grouping
 * spellings tried in wave 41); fn_8048A96C (RwImageCreateResample, rewritten from retail in wave
 * 41) inlines RwImageResample twice and differs where that does. C, cflags_rw_core
 * (configure.py). */
#include "Game/rw/r6.h"

RwImage *fn_80487D08(s32 width, s32 height, s32 depth);
RwBool fn_80487DB0(RwImage *image);
RwImage *fn_80487E18(RwImage *image);
RwImage *fn_80487F50(RwImage *image);
RwImage *fn_804893B8(RwImage *dst, const RwImage *src);


#define RGBAREALFROMPIXEL(o, p)                                                                    \
    do {                                                                                           \
        (o).red = (1.0f / 255.0f) * (f32)(p)[0];                                                      \
        (o).green = (1.0f / 255.0f) * (f32)(p)[1];                                                    \
        (o).blue = (1.0f / 255.0f) * (f32)(p)[2];                                                     \
        (o).alpha = (1.0f / 255.0f) * (f32)(p)[3];                                                    \
    } while (0)

#define RGBAREALSCALE(o, s)                                                                        \
    do {                                                                                           \
        (o).red *= (s);                                                                            \
        (o).green *= (s);                                                                          \
        (o).blue *= (s);                                                                           \
        (o).alpha *= (s);                                                                          \
    } while (0)

#define RGBAREALADD(o, a)                                                                          \
    do {                                                                                           \
        (o).red = (o).red + (a).red;                                                               \
        (o).green = (o).green + (a).green;                                                         \
        (o).blue = (o).blue + (a).blue;                                                            \
        (o).alpha = (o).alpha + (a).alpha;                                                         \
    } while (0)

/* The mean colour of one row of a 32-bit image between two 16.16 x positions, end pixels
 * weighted by how much of them the span covers. */
void fn_8048A178(const RwImage *image, s32 x0, s32 x1, s32 y, RwRGBAReal *out) {
    f32 width = (f32)(x1 - x0) * (1.0f / 65536.0f);
    s32 first = x0 >> 16;
    s32 last = x1 >> 16;
    const u8 *pixel = image->cpPixels + (y >> 16) * image->stride + first * 4;

    if (first == last) {
        RGBAREALFROMPIXEL(*out, pixel);
        RGBAREALSCALE(*out, width);
    } else {
        s32 pos = (first + 1) << 16;
        f32 weight = (f32)(pos - x0) * (1.0f / 65536.0f);
        RwRGBAReal color;
        s32 i;

        RGBAREALFROMPIXEL(*out, pixel);
        RGBAREALSCALE(*out, weight);
        pixel += 4;
        for (i = pos >> 16; i != last; i++) {
            RGBAREALFROMPIXEL(color, pixel);
            RGBAREALADD(*out, color);
            pixel += 4;
            pos += 0x10000;
        }
        weight = (f32)(x1 - pos) * (1.0f / 65536.0f);
        RGBAREALFROMPIXEL(color, pixel);
        RGBAREALSCALE(color, weight);
        RGBAREALADD(*out, color);
    }
    width = 1.0f / width;
    RGBAREALSCALE(*out, width);
}

/* The mean colour of a block of rows between two 16.16 y positions. */
void fn_8048A50C(const RwImage *image, s32 x0, s32 x1, s32 y0, s32 y1, RwRGBAReal *out) {
    f32 height = (f32)(y1 - y0) * (1.0f / 65536.0f);
    s32 first = y0 >> 16;
    s32 last = y1 >> 16;

    if (first == last) {
        fn_8048A178(image, x0, x1, y0, out);
        RGBAREALSCALE(*out, height);
    } else {
        s32 pos = (first + 1) << 16;
        RwRGBAReal row;
        f32 weight;

        fn_8048A178(image, x0, x1, y0, out);
        weight = (f32)(pos - y0) * (1.0f / 65536.0f);
        RGBAREALSCALE(*out, weight);
        for (first = pos >> 16; first != last; first++) {
            fn_8048A178(image, x0, x1, pos, &row);
            pos += 0x10000;
            out->red = row.red + out->red;
            out->green = row.green + out->green;
            out->blue = row.blue + out->blue;
            out->alpha = row.alpha + out->alpha;
        }
        fn_8048A178(image, x0, x1, pos, &row);
        weight = (f32)(y1 - pos) * (1.0f / 65536.0f);
        RGBAREALSCALE(row, weight);
        RGBAREALADD(*out, row);
    }
    height = 1.0f / height;
    RGBAREALSCALE(*out, height);
}

/* RwImageResample: each destination pixel is the mean of the source area it covers. */
RwImage *fn_8048A790(RwImage *destImage, const RwImage *sourceImage) {
    s32 destWidth = destImage->width;
    s32 destHeight = destImage->height;
    s32 srcY = 0;
    s32 xStep;
    s32 yStep;
    s32 y;
    RwRGBAReal color;

    destImage->flags |= sourceImage->flags & 2;
    xStep = 65536.0f * ((f32)sourceImage->width / (f32)destWidth);
    yStep = 65536.0f * ((f32)sourceImage->height / (f32)destHeight);
    for (y = 0; y < destHeight; y++) {
        s32 srcYEnd = srcY + (yStep - 1);
        u8 *pixel = destImage->cpPixels + destImage->stride * y;
        s32 srcX = 0;
        s32 x;

        for (x = 0; x < destWidth; x++) {
            fn_8048A50C(sourceImage, srcX, srcX + (xStep - 1), srcY, srcYEnd, &color);
            srcX += xStep;
            pixel[0] = 255.0f * color.red + 0.5f;
            pixel[1] = 255.0f * color.green + 0.5f;
            pixel[2] = 255.0f * color.blue + 0.5f;
            pixel[3] = 255.0f * color.alpha + 0.5f;
            pixel += 4;
        }
        srcY += yStep;
    }
    return destImage;
}

/* RwImageCreateResample: the source is copied to 32 bits first when it has another depth. */
RwImage *fn_8048A96C(const RwImage *srcImage, s32 width, s32 height) {
    RwImage *dstImage = fn_80487D08(width, height, 32);

    if (!dstImage) {
        return NULL;
    }
    if (!fn_80487E18(dstImage)) {
        fn_80487DB0(dstImage);
        return NULL;
    }
    if (srcImage->depth != 32) {
        RwImage *tmpImage = fn_80487D08(srcImage->width, srcImage->height, 32);

        if (!tmpImage) {
            fn_80487F50(dstImage);
            fn_80487DB0(dstImage);
            return NULL;
        }
        if (!fn_80487E18(tmpImage)) {
            fn_80487DB0(tmpImage);
            fn_80487F50(dstImage);
            fn_80487DB0(dstImage);
            return NULL;
        }
        fn_804893B8(tmpImage, srcImage);
        if (!fn_8048A790(dstImage, tmpImage)) {
            fn_80487F50(tmpImage);
            fn_80487DB0(tmpImage);
            fn_80487F50(dstImage);
            fn_80487DB0(dstImage);
            return NULL;
        }
        fn_80487F50(tmpImage);
        fn_80487DB0(tmpImage);
    } else if (!fn_8048A790(dstImage, srcImage)) {
        fn_80487F50(dstImage);
        fn_80487DB0(dstImage);
        return NULL;
    }
    return dstImage;
}
