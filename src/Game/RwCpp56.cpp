/* RenderWare GameCube driver, 0x804929F0..0x80493E8B: the end of the image/raster conversion
 * file (RwRasterSetFromImage, RwImageSetFromRaster and RwImageFindRasterFormat on the driver
 * side; its .data runs 0x8056E0C8..0x8056E347) and, from 0x804939DC, the start of the device file
 * (display queue callbacks, the render mode set-up; its .data starts at 0x8056E348). Names are
 * inferred from behaviour; definitions are in retail address order.
 *
 * Status: every function matches except fn_80493C84 (3 rows, the order of a select). The code
 * needs RenderWare's own int types (RwInt32 is int, not long: Game/rw/r7.h). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "Game/rw/r7.h"

extern "C" {
extern char lbl_8056E0C8[]; /* "rwRASTERFORMATLUM8 not yet supported", the first of this file's
                              * literals: the rest of the file is in Game/RwCpp55 */
extern RwInt32 lbl_805F1658;                /* offset of the driver's data in a raster */
extern f32 lbl_805FB3A8;

void *fn_80489F70(void *, RwUInt32, RwInt32);     /* RwRasterLock(raster, level, mode) */
void *fn_80489C94(void *, RwInt32);          /* RwRasterLockPalette(raster, mode) */
void fn_80489B88(void *);                /* RwRasterUnlockPalette */
void fn_80489B40(void *);                /* RwRasterUnlock */
void fn_8049231C(void *, void *);
void fn_804920C8(void *, void *);
void fn_80492610(void *, void *);
RwDlImage *fn_80487D08(RwInt32, RwInt32, RwInt32);   /* RwImageCreate(width, height, depth) */
void fn_80487E18(void);                  /* RwImageAllocatePixels */
int fn_8048FED8(void *);
void fn_8048DEB0(void *, f32, void *);
void fn_8048F5D0(RwDlRGBA *, RwInt32, void *);
void fn_8048FAB0(RwUInt32, RwUInt32, RwUInt32, RwUInt32, void *, void *);
void fn_804903F8(void *);
void fn_80487DB0(void *);                /* RwImageDestroy */
RwUInt32 fn_80491AF4(const RwDlRGBA *);
RwUInt32 fn_80491AD4(const RwDlRGBA *);
RwUInt32 fn_80491B14(const RwDlRGBA *);
RwUInt32 fn_80491B64(const RwDlRGBA *);
RwUInt32 fn_80491B88(const RwDlRGBA *);
int fn_8049A78C(u16);
int VIGetTvFormat(void);
void fn_804953F0(void);
void fn_80499BF8(void);

/* The device file's .data (0x8056E348): the video modes of the three TV standards and of the
 * application's render mode, then its two messages, all reached from one base register (pooled
 * data). Defined here, ahead of the conversion file's code, so that this unit's pool starts with
 * them as retail's does; the messages are arrays for the same reason (as literals they would come
 * after fn_804935C4's jump table). */
#pragma pool_data on
static RwDlVideoMode sVideoModes[4] = {
    {640, 480, 24, 1, 0, 0},
    {640, 528, 24, 1, 0, 0},
    {640, 480, 24, 1, 0, 0},
    {0, 0, 0, 1, 0, 0},
};
static char sInvalidPixelFormat[] = "Invalid pixel format";
static char sInvalidTvFormat[] = "Invalid TV format";
#pragma pool_data off

typedef RwUInt32 (*RwDlPacker)(const RwDlRGBA *rgba);


/* Picks the function that packs an RwRGBA into a pixel of the raster's colour format
 * (rwRASTERFORMAT1555 .. rwRASTERFORMAT555, the format's 0x0F00 field). An inline function in
 * retail too: its two error records sit below the caller's on the stack. */
static inline RwDlPacker RwDlSelectPacker(RwInt32 format) {
    RwDlPacker packer = NULL;
    switch (format & 0x0F00) {
    case 0x400:
        RW_ERROR_STRING(2, lbl_8056E0C8);
        break;
    case 0xA00:
        packer = fn_80491AD4;
        break;
    case 0x200:
        packer = fn_80491AF4;
        break;
    case 0x100:
    case 0x300:
        packer = fn_80491B14;
        break;
    case 0x600:
        packer = fn_80491B64;
        break;
    case 0x500:
        packer = fn_80491B88;
        break;
    default:
        RW_ERROR(0x8000000D);
        break;
    }
    return packer;
}

/* Driver side of RwImageSetFromRaster: locks the raster (and its palette) for reading unless the
 * caller already has, and converts by the image's depth. */
RwUInt32 fn_804929F0(void *out, void *inOut, RwInt32 num) {
    RwDlImage *image = (RwDlImage *)out;
    RwDlRaster *raster = (RwDlRaster *)inOut;
    RwInt32 lockedPixels = 0;
    RwInt32 lockedPalette = 0;
    if ((raster->privateFlags & 2) == 0) {
        fn_80489F70(raster, 0, 2);
        lockedPixels = 1;
    }
    if (((raster->cFormat << 8) & 0x6000) != 0 && (raster->privateFlags & 8) == 0) {
        fn_80489C94(raster, 2);
        lockedPalette = 1;
    }
    switch (image->depth) {
    case 4:
        fn_804920C8(image, raster);
        break;
    case 8:
        fn_8049231C(image, raster);
        break;
    case 32:
        fn_80492610(image, raster);
        break;
    default:
        RW_ERROR(0x80000008);
        break;
    }
    if (lockedPalette == 1) fn_80489B88(raster);
    if (lockedPixels == 1) fn_80489B40(raster);
    return 1;
}

/* Makes a copy of an image with the given palettised depth (colour reduction through the
 * palette quantiser of the previous units). */
RwDlImage *fn_80492B24(RwDlImage *image, RwInt32 depth) {
    RwDlImage *result = fn_80487D08(image->width, image->height, depth);
    if (result == NULL) return NULL;
    u8 buffer[0x4000];
    fn_80487E18();
    if (fn_8048FED8(buffer) == 0) return NULL;
    fn_8048DEB0(buffer, lbl_805FB3A8, image);
    fn_8048F5D0(result->palette, 1 << depth, buffer);
    fn_8048FAB0((RwUInt32)result->cpPixels, result->stride, result->depth, 0, buffer, image);
    fn_804903F8(buffer);
    return result;
}

/* Image to 4-bit palettised raster: packs two indices per byte and converts the 16 palette
 * entries; an 8-bit or 32-bit image goes through a 4-bit copy first. */
void fn_80492BE8(RwDlRaster *raster, RwDlImage *image) {
    RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4: {
        RwInt32 y;
        for (y = 0; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            u8 *dst = raster->cpPixels + raster->stride * y;
            RwInt32 x;
            for (x = 0; x < raster->width; x += 2) {
                *dst++ = ((src[0] & 0xF) << 4) | (src[1] & 0xF);
                src += 2;
            }
        }
        if (!ext->sharedPalette) {
            u16 *entry = (u16 *)raster->palette;
            RwInt32 i;
            for (i = 0; i < 16; i++) {
                entry[i] = packer(&image->palette[i]);
            }
        }
        break;
    }
    case 8:
    case 32: {
        RwDlImage *reduced = fn_80492B24(image, raster->depth);
        if (reduced != NULL) {
            fn_80492BE8(raster, reduced);
            fn_80487DB0(reduced);
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Image to 8-bit palettised raster: copies the indices and converts the palette; a 32-bit image
 * goes through an 8-bit copy first. */
void fn_80492E40(RwDlRaster *raster, RwDlImage *image) {
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4:
    case 8: {
        RwInt32 y = 0;
        RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
        for (; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            u8 *dst = raster->cpPixels + raster->stride * y;
            RwInt32 x;
            for (x = 0; x < raster->width; x++) {
                *dst = *src;
                src++;
                dst++;
            }
        }
        if (!ext->sharedPalette) {
            u16 *entry = (u16 *)raster->palette;
            RwInt32 i;
            for (i = 0; i < (1 << image->depth); i++) {
                entry[i] = packer(&image->palette[i]);
            }
        }
        break;
    }
    case 32: {
        RwDlImage *reduced = fn_80492B24(image, raster->depth);
        if (reduced != NULL) {
            fn_80492E40(raster, reduced);
            fn_80487DB0(reduced);
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Image to 16-bit raster: palettised images through a converted palette, 32-bit images pixel by
 * pixel. */
void fn_8049309C(RwDlRaster *raster, RwDlImage *image) {
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4:
    case 8: {
        u16 palette[256];
        RwInt32 i;
        RwInt32 y;
        for (i = 0; i < (1 << image->depth); i++) {
            palette[i] = packer(&image->palette[i]);
        }
        for (y = 0; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            u16 *dst = (u16 *)(raster->cpPixels + raster->stride * y);
            RwInt32 x;
            for (x = 0; x < raster->width; x++) {
                *dst = palette[*src];
                src++;
                dst++;
            }
        }
        break;
    }
    case 32: {
        RwInt32 y;
        RwInt32 x;
        u8 *src;
        u16 *dst;
        for (y = 0; y < raster->height; y++) {
            src = image->cpPixels + image->stride * y;
            dst = (u16 *)(raster->cpPixels + raster->stride * y);
            for (x = 0; x < raster->width; x++) {
                *dst = packer((RwDlRGBA *)src);
                dst++;
                src += 4;
            }
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Image to 32-bit raster; as fn_8049309C with 32-bit pixels. */
void fn_80493330(RwDlRaster *raster, RwDlImage *image) {
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4:
    case 8: {
        RwUInt32 palette[256];
        RwInt32 i;
        RwInt32 y;
        for (i = 0; i < (1 << image->depth); i++) {
            palette[i] = packer(&image->palette[i]);
        }
        for (y = 0; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            RwUInt32 *dst = (RwUInt32 *)(raster->cpPixels + raster->stride * y);
            RwInt32 x;
            for (x = 0; x < raster->width; x++) {
                *dst = palette[*src];
                src++;
                dst++;
            }
        }
        break;
    }
    case 32: {
        RwInt32 y;
        RwInt32 x;
        u8 *src;
        RwUInt32 *dst;
        for (y = 0; y < raster->height; y++) {
            src = image->cpPixels + image->stride * y;
            dst = (RwUInt32 *)(raster->cpPixels + raster->stride * y);
            for (x = 0; x < raster->width; x++) {
                *dst = packer((RwDlRGBA *)src);
                dst++;
                src += 4;
            }
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Driver side of RwRasterSetFromImage: locks the raster (and its palette) for writing unless the
 * caller already has, and converts by the raster's depth. */
RwUInt32 fn_804935C4(void *out, void *inOut, RwInt32 num) {
    RwDlRaster *raster = (RwDlRaster *)out;
    RwInt32 pixelsLocked = 0;
    RwInt32 paletteLocked = 0;
    RwInt32 format = RW_RASTER_FORMAT(raster);
    RwDlImage *image = (RwDlImage *)inOut;
    if ((raster->privateFlags & 4) != 0) pixelsLocked = 1;
    if (!pixelsLocked) {
        if (fn_80489F70(raster, 0, 5) == 0) return 0;
    }
    format &= 0x6000;
    if (format != 0) {
        if ((raster->privateFlags & 0x10) != 0) paletteLocked = 1;
        if (!paletteLocked) {
            if (fn_80489C94(raster, 5) == 0) return 0;
        }
    }
    switch (raster->depth) {
    case 4:
        fn_80492BE8(raster, image);
        break;
    case 8:
        fn_80492E40(raster, image);
        break;
    case 16:
        fn_8049309C(raster, image);
        break;
    case 24:
        RW_ERROR(0x8000000C);
        break;
    case 32:
        fn_80493330(raster, image);
        break;
    default:
        RW_ERROR(0x8000000C);
        break;
    }
    if (format != 0) {
        if (!paletteLocked) fn_80489B88(raster);
    }
    if (!pixelsLocked) fn_80489B40(raster);
    return 1;
}

/* Adds the palette bit of a 4-bit or 8-bit image to a raster format. Retail ORs at run time,
 * which only an inline function whose parameter is modified gives. */
static inline RwInt32 RwDlPalettised(RwInt32 format, RwInt32 depth) {
    if (depth == 4) {
        format |= 0x4000;
    } else {
        format |= 0x2000;
    }
    return format;
}

/* Picks the raster format that suits an image: 0x100 (1555) when its alpha is only on or off,
 * 0x200 (565) when it is opaque, 0x300 (4444) when it has partial alpha, with the palette bits
 * (0x2000 for 8-bit, 0x4000 for 4-bit) of a palettised image. */

RwInt32 fn_8049375C(RwDlImage *image) {
    RwInt32 depth = image->depth;
    RwInt32 hasAlpha = 0;
    RwInt32 format;
    if (depth == 4 || depth == 8) {
        RwInt32 width = image->width;
        RwInt32 height = image->height;
        RwInt32 y = 0;
        u8 *row = image->cpPixels;
        RwDlRGBA *palette = image->palette;
        for (; y < height; y++) {
            u8 *pixel = row;
            RwInt32 x;
            for (x = 0; x < width; x++) {
                u8 alpha = palette[*pixel].alpha;
                if (alpha != 0xFF) {
                    hasAlpha = 1;
                    if (alpha > 0xF) {
                        return RwDlPalettised(0x300, depth);
                    }
                }
                pixel++;
            }
            row += image->stride;
        }
    } else {
        RwInt32 width = image->width;
        RwInt32 height = image->height;
        RwInt32 y = 0;
        u8 *row = image->cpPixels;
        for (; y < height; y++) {
            u8 *pixel = row;
            RwInt32 x;
            for (x = 0; x < width; x++) {
                u8 alpha = pixel[3];
                if (alpha != 0xFF) {
                    hasAlpha = 1;
                    if (alpha > 0xF) {
                        return 0x300;
                    }
                }
                pixel += 4;
            }
            row += image->stride;
        }
    }
    format = 0x200;
    if (hasAlpha) format = 0x100;
    if (depth == 4) {
        format |= 0x4000;
    } else if (depth == 8) {
        format |= 0x2000;
    }
    return format;
}

#pragma use_lmw_stmw off
/* Driver side of RwImageFindRasterFormat: fills in the type, size and format of a raster that
 * would hold the image. Textures are at most 1024 texels a side and, with mip maps (0x8000),
 * rounded down to powers of two; camera and Z rasters keep the image's size. */
RwUInt32 fn_80493888(void *out, void *inOut, RwInt32 flags) {
    RwDlRaster *raster = (RwDlRaster *)out;
    RwDlImage *image = (RwDlImage *)inOut;
    RwInt32 type = flags & 7;
    raster->cType = type;
    raster->depth = 0;
    switch (type) {
    case 0:
    case 4:
    case 5:
    {

        RwInt32 w, h;
        w = image->width;
        raster->width = (w > 1024) ? 1024 : w;
        h = image->height;
        raster->height = (h > 1024) ? 1024 : h;
        if ((flags & 0x8000) != 0) {
            RwInt32 size;
            RwInt32 logWidth = -1;
            RwInt32 logHeight;
            for (size = raster->width; size != 0; size >>= 1) logWidth++;
            raster->width = 1 << logWidth;
            logHeight = -1;
            for (size = raster->height; size != 0; size >>= 1) logHeight++;
            raster->height = 1 << logHeight;
        }
        {
            RwInt32 f;
            f = fn_8049375C(image);
            f |= flags & 0x9000;
            raster->cFormat = f >> 8;
        }
        return 1;
    }
    case 1:
    case 2:
        raster->cFormat = 0;
        raster->width = image->width;
        raster->height = image->height;
        return 1;
    default:
        RW_ERROR(0x8000000D);
        return 0;
    }
}
#pragma use_lmw_stmw on

/* The display queue: frames are copied to one of the external frame buffers by a command in
 * the GX FIFO, and a FIFO breakpoint after each copy tells the interrupt side when the frame is
 * ready to be shown (the SDK's triple-buffering scheme). */
extern RwDlQueuedFrame lbl_805E3178[3]; /* frames waiting to be shown */
extern void *volatile lbl_805F1604;     /* frame buffer being displayed */
extern volatile RwInt32 lbl_805F1608;       /* queue read index */
extern RwInt32 lbl_805F160C;                /* queue write index */
extern RwInt32 lbl_805EDF34;                /* queue length */
extern RwInt32 lbl_805F1618;
extern RwInt32 lbl_805F1620;                /* the next frame is still on screen: retry at retrace */
extern RwInt32 lbl_805F1624;                /* a retrace has shown a new frame */
extern RwInt32 lbl_805F15F0;                /* nonzero: every frame takes two breakpoints */
extern volatile RwInt32 lbl_805F161C;                /* a frame is ready for the retrace handler */
extern RwInt32 lbl_805F1628;
extern u8 lbl_805EDF38;                 /* retraces since the last swap */
extern u8 lbl_805EDF39;                 /* retraces a frame stays up at least */
extern u16 lbl_805F1638[4];             /* per queued frame: token the draw thread must pass */
extern volatile RwInt32 lbl_805F1614;       /* queue index of the frame to show next */
extern void *lbl_805F15F8;
extern void *lbl_805F15FC;
extern RwInt32 lbl_805EDF30;                /* screen still blanked */
extern u8 lbl_805F1630[8];              /* OSThreadQueue of the thread waiting for a frame */

void fn_80394228(void);         /* GXDisableBreakPt */
void fn_80394174(void *);       /* GXEnableBreakPt */
void fn_80378568(void *);       /* OSWakeupThread */
void fn_8037F020(void *);       /* VISetNextFrameBuffer */
void fn_8037F094(RwInt32);          /* VISetBlack */
void fn_8037EEF0(void);         /* VIFlush */

static inline RwInt32 RwDlQueueNext(RwInt32 index) {
    return (index + 1) % lbl_805EDF34;
}

static inline void RwDlQueueAdvance(void) {
    RwInt32 next = (lbl_805F1608 + 1) % lbl_805EDF34;
    if (next == lbl_805F160C) {
        fn_80394228();
        lbl_805F1618 = 0;
    } else {
        fn_80394174(lbl_805E3178[next].breakPt);
    }
    lbl_805F1608 = next;
    fn_80378568(lbl_805F1630);
    if (lbl_805F15F0 == 0) {
        lbl_805F161C = 1;
    } else if (lbl_805F1628 == 1) {
        lbl_805F161C = 1;
        lbl_805F1628 = 0;
    } else {
        lbl_805F1628 = 1;
    }
}

/* GX breakpoint callback: the copy of the frame at the queue's read index has been executed. */
void fn_804939DC(void) {
    if (lbl_805E3178[lbl_805F1608].xfb != lbl_805F1604) {
        RwDlQueueAdvance();
    } else {
        lbl_805F1620 = 1;
    }
}

/* VI pre-retrace callback: shows the frame that is ready. */
void fn_80493ABC(void) {
    lbl_805EDF38++;
    s16 token = lbl_805F1638[lbl_805F1614];
    if (lbl_805F161C == 1 && fn_8049A78C(token) != 0 && lbl_805EDF38 >= lbl_805EDF39) {
        lbl_805EDF38 = 0;
        lbl_805F1604 = (lbl_805F1604 == lbl_805F15F8) ? lbl_805F15FC : lbl_805F15F8;
        fn_8037F020(lbl_805F1604);
        if (lbl_805EDF30 != 0) {
            fn_8037F094(0);
            lbl_805EDF30 = 0;
        }
        fn_8037EEF0();
        lbl_805F1614 = (lbl_805F1614 + 1) % lbl_805EDF34;
        lbl_805F161C = 0;
        lbl_805F1624 = 1;
    }
}

/* VI post-retrace callback: releases the breakpoint held back by fn_804939DC. */
void fn_80493B98(void) {
    if (lbl_805F1620 != 0 && lbl_805F1624 != 0) {
        RwDlQueueAdvance();
        lbl_805F1620 = 0;
    }
    lbl_805F1624 = 0;
}

RwUInt32 fn_80493C7C(void) {
    return 0;
}

extern RwDlRenderMode lbl_805E3190;       /* the application's own render mode */
extern RwDlRenderMode *lbl_805F164C;      /* render mode in use */
extern RwInt32 lbl_805F15EC;                  /* video mode index; 0x2A for an application mode */
extern RwInt32 lbl_805F15E4;
extern RwInt32 lbl_805F15E8;



#pragma pool_data on
/* Chooses the GX render mode: the application's (with its frame buffer format: 0 and 1 are
 * 24-bit, 2 is the 16-bit format antialiasing needs) or the default of the console's TV
 * standard. */
static inline RwInt32 RwDlFrameBufferFormat(RwInt32 format) {
    if (lbl_805F164C->gx.aa != 0) return 2;
    return format;
}

static inline RwInt32 RwDlFrameBufferFormat2(RwInt32 format) {
    if (lbl_805F164C->gx.aa != 0) {
        format = 2;
    }
    return format;
}

static inline RwInt32 RwDlFrameBufferFormat3(RwInt32 format) {
    RwInt32 used = format;
    if (lbl_805F164C->gx.aa != 0) used = 2;
    return used;
}

void fn_80493C84(RwDlRenderMode *mode, RwInt32 format) {
    if (mode != NULL) {
        RwInt32 used;
        lbl_805E3190 = *mode;
        lbl_805F164C = &lbl_805E3190;
        lbl_805F15EC = 0x2A;
        /* Retail moves the format into r5 first and loads the 2 behind the test; a ternary (any
         * spelling, or an inline function) loads the 2 first (3 rows), while `used = format;
         * if (aa) used = 2;` swaps the address and copy registers of the mode copy above (39
         * rows). Probes: probes/Game_RwCpp56_R9.py. */
        used = (lbl_805F164C->gx.aa != 0) ? 2 : format;
        lbl_805F15E4 = used;
        lbl_805F15E8 = used;
        sVideoModes[3].width = lbl_805F164C->gx.fbWidth;
        sVideoModes[3].height = lbl_805F164C->gx.efbHeight;
        switch (format) {
        case 0:
        case 1:
            sVideoModes[3].depth = 24;
            break;
        case 2:
            sVideoModes[3].depth = 16;
            break;
        default:
            RW_ERROR_STRING(2, sInvalidPixelFormat);
            break;
        }
    } else {
        switch (VIGetTvFormat()) {
        case 0:
            lbl_805F164C = (RwDlRenderMode *)&GXNtsc480IntDf;
            lbl_805F15EC = 0;
            break;
        case 1:
        case 5:
            lbl_805F164C = (RwDlRenderMode *)&GXPal528IntDf;
            lbl_805F15EC = 1;
            break;
        case 2:
            lbl_805F164C = (RwDlRenderMode *)&GXMpal480IntDf;
            lbl_805F15EC = 2;
            break;
        default:
            RW_ERROR_STRING(2, sInvalidTvFormat);
            break;
        }
    }
}

#pragma pool_data off

RwUInt32 fn_80493E64(void) {
    fn_804953F0();
    fn_80499BF8();
    return 1;
}
}
