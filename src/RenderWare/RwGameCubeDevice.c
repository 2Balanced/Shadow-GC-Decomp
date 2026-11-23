/* RenderWare 3.7 GameCube driver: the device (0x804939DC..0x804953EF). The display queue and its VI
 * and GX breakpoint callbacks, the render mode set-up, the plugin attach, the device's system
 * function (open/close, start/stop, modes, the standard functions), the camera standard functions
 * (begin and end update, clear, show raster), the RwDevice record, the view matrix load and the
 * copy of the frame into a camera texture. The file owns the standard function table (.rodata
 * 0x8051C2E8, the initializer of the system function's local array), the video modes, messages,
 * projection and RwDevice (.data 0x8056E348..0x8056E4F7), the display queue, the application's
 * render mode and the view matrix (.bss 0x805E3178..0x805E320F), the queue settings (.sdata
 * 0x805EDF28..0x805EDF3F), the driver's state (.sbss 0x805F15E0..0x805F1657) and its constants
 * (.sdata2 0x805FB3B0..0x805FB3CF). Built as C with the driver's flags (-O4,p, no automatic
 * inlining, writable strings).
 *
 * Names are inferred from behaviour; definitions are in retail address order.
 * NonMatching: fn_80493C84 (3 rows: retail copies the format before testing the antialiasing
 * flag; every spelling that does so swaps the registers of the render mode copy above it) and
 * fn_80493E8C (8 rows: the standard function table's loop counters trade r5 and r7). The video
 * set-up of the start request and the standard function table are inline functions in retail
 * (written out, their locals take other registers). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/gx/GXPrivate.h>
#include "Game/rw/r7.h"

extern RwDlGlobals *lbl_805F15B4; /* RwEngineInstance */
extern RwInt32 lbl_805F1658;      /* offset of the driver's data in a raster (RwGameCubeRaster.c) */
extern volatile u16 lbl_805F1678; /* the last draw-sync token passed (RwGameCubeDrawSync.c) */
extern volatile u16 lbl_805EDF40;  /* the next draw-sync token (RwGameCubeDrawSync.c) */

void fn_8037E354(void *mode);           /* VIConfigure */
void fn_8037F094(RwInt32 black);        /* VISetBlack */
void fn_8037EEF0(void);                 /* VIFlush */
u32 fn_8037F1BC(void);                  /* VIGetNextField */
void fn_8037D6A8(void (*callback)(void)); /* VISetPreRetraceCallback */
void fn_8037D6EC(void (*callback)(void)); /* VISetPostRetraceCallback */
void fn_8037DE8C(void);                 /* VIWaitForRetrace */
void fn_8037F020(void *xfb);            /* VISetNextFrameBuffer */
RwBool fn_8049A78C(u16 token);          /* the draw-sync token has been passed */
void fn_804953F0(void);                 /* the raster plugin attach (RwGameCubeRaster.c) */
void fn_80499BF8(void);                 /* the texture plugin attach (RwGameCubeTexture.c) */
void fn_80497198(void);                 /* render state open (RwGameCubeRenderState.c) */
void fn_8049742C(void);                 /* render state close */
RwBool fn_80498E08(RwDlRaster *raster, RwDlRect *rect, void *color, RwInt32 clearMode);
void *RwFrameGetLTM(void *frame);
void fn_8047F0B0(void *dst, void *src);   /* RwMatrixInvert */
void fn_8047F72C(void *matrix, void *transform, RwInt32 combine); /* RwMatrixTransform */
void fn_8047EBB8(void *dst, void *a, void *b); /* RwMatrixMultiply */
RwInt32 fn_80491AB4(RwInt32 value);
void GXInvalidateTexRegion(const GXTexRegion *region);
void *fn_804950A0(void);

/* The standard functions this file's system function hands out (defined in this file and in the
 * conversion, raster, raster rendering and native texture files). */
RwBool fn_804946A4(void *out, void *inOut, RwInt32 num);
RwBool fn_80494C5C(void *out, void *inOut, RwInt32 num);
RwBool fn_804945E8(void *out, void *inOut, RwInt32 num);
RwBool fn_80494C6C(void *out, void *inOut, RwInt32 num);
RwBool fn_80491BAC(void *out, void *inOut, RwInt32 num);
RwBool fn_80491EC0(void *out, void *inOut, RwInt32 num);
RwBool fn_804935C4(void *out, void *inOut, RwInt32 num);
RwBool fn_804929F0(void *out, void *inOut, RwInt32 num);
RwBool fn_80497008(void *out, void *inOut, RwInt32 num);
RwBool fn_80496C58(void *out, void *inOut, RwInt32 num);
RwBool fn_80493888(void *out, void *inOut, RwInt32 num);
RwBool fn_8049714C(void *out, void *inOut, RwInt32 num);
RwBool fn_80495C54(void *out, void *inOut, RwInt32 num);
RwBool fn_804960B4(void *out, void *inOut, RwInt32 num);
RwBool fn_80496268(void *out, void *inOut, RwInt32 num);
RwBool fn_80496338(void *out, void *inOut, RwInt32 num);
RwBool fn_804994DC(void *out, void *inOut, RwInt32 num);
RwBool fn_804994B8(void *out, void *inOut, RwInt32 num);
RwBool fn_80498A4C(void *out, void *inOut, RwInt32 num);
RwBool fn_80498A9C(void *out, void *inOut, RwInt32 num);
RwBool fn_80498A74(void *out, void *inOut, RwInt32 num);
RwBool fn_80499520(void *out, void *inOut, RwInt32 num);
RwBool fn_80497168(void *out, void *inOut, RwInt32 num);
RwBool fn_8049952C(void *out, void *inOut, RwInt32 num);
RwBool fn_804995B4(void *out, void *inOut, RwInt32 num);
RwBool fn_8049988C(void *out, void *inOut, RwInt32 num);
RwBool fn_80495688(void *out, void *inOut, RwInt32 num);
RwBool fn_80497904(RwInt32 state, void *value);
RwBool fn_80497474(RwInt32 state, void *value);
RwInt32 fn_80490CBC(void *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwInt32 fn_80490AB8(void *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3);
RwInt32 fn_80490E30(RwUInt32 type, u8 *data, RwUInt32 count);
RwInt32 fn_8049140C(RwUInt32 type, u8 *records, RwInt32 numVertices, u16 *indices, RwUInt32 count);

typedef RwBool (*RwDlStandardFunc)(void *out, void *inOut, RwInt32 num);

/* RwStandard: an entry of a device's standard function table. */
typedef struct RwDlStandard {
    RwInt32 id;
    RwDlStandardFunc func;
} RwDlStandard;

/* RwDevice (public layout). */
typedef struct RwDlDevice {
    RwReal gammaCorrection;
    RwBool (*fpSystem)(RwInt32 request, void *out, void *inOut, RwInt32 in);
    RwReal zBufferNear;
    RwReal zBufferFar;
    RwBool (*fpRenderStateSet)(RwInt32 state, void *value);
    RwBool (*fpRenderStateGet)(RwInt32 state, void *value);
    void *fpIm2DRenderLine;
    void *fpIm2DRenderTriangle;
    void *fpIm2DRenderPrimitive;
    void *fpIm2DRenderIndexedPrimitive;
    void *fpIm3DRenderLine;
    void *fpIm3DRenderTriangle;
    void *fpIm3DRenderPrimitive;
    void *fpIm3DRenderIndexedPrimitive;
} RwDlDevice;

/* What RwEngineOpen hands the device: the render mode to use (NULL for the TV standard's), its
 * pixel format and the size of the GX FIFO. */
typedef struct RwDlOpenParams {
    RwDlRenderMode *mode;
    RwInt32 format;
    RwInt32 fifoSize;
} RwDlOpenParams;

/* .sbss: the driver's state. MWCC lays uninitialised globals out in the reverse of their
 * definition order, so they are defined from the last address down. */
void *lbl_805F1650[2];                 /* the camera being updated, and the memory functions */
RwDlRenderMode *lbl_805F164C;          /* render mode in use */
RwInt32 lbl_805F1648;                  /* the line where the two halves meet */
RwInt32 lbl_805F1644;                  /* the GX FIFO object GXInit returned */
RwInt32 lbl_805F1640;                  /* the FIFO, 32-byte aligned */
u16 lbl_805F1638[4];                   /* per queued frame: token the draw thread must pass */
OSThreadQueue lbl_805F1630;            /* the thread waiting for a frame */
RwInt32 lbl_805F162C;                  /* GX has been initialised */
RwInt32 lbl_805F1628;                  /* in two-halves mode: the first breakpoint has passed */
RwInt32 lbl_805F1624;                  /* a retrace has shown a new frame */
RwInt32 lbl_805F1620;                  /* the next frame is still on screen: retry at retrace */
volatile RwInt32 lbl_805F161C;         /* a frame is ready for the retrace handler */
RwInt32 lbl_805F1618;                  /* the FIFO breakpoint is enabled */
volatile RwInt32 lbl_805F1614;         /* queue index of the frame to show next */
volatile RwInt32 lbl_805F1610;         /* queue index of the last frame queued */
volatile RwInt32 lbl_805F160C;         /* queue write index */
volatile RwInt32 lbl_805F1608;         /* queue read index */
void *volatile lbl_805F1604;           /* the frame buffer being displayed */
void *volatile lbl_805F1600;           /* the frame buffer drawn to */
void *lbl_805F15FC;                    /* external frame buffer 1 */
void *lbl_805F15F8;                    /* external frame buffer 0 */
RwInt32 lbl_805F15F4;                  /* the FIFO and frame buffers as allocated */
RwInt32 lbl_805F15F0;                  /* nonzero: the frame is drawn in two halves */
RwInt32 lbl_805F15EC;                  /* video mode index; 0x2A for an application mode */
RwInt32 lbl_805F15E8;                  /* the pixel format GX is set to */
RwInt32 lbl_805F15E4;                  /* the pixel format asked for */
RwInt32 lbl_805F15E0;                  /* nonzero: the frame copy clears the frame buffer */

/* .sdata */
RwInt32 lbl_805EDF28 = 1;              /* nonzero while the top half is drawn */
RwInt32 lbl_805EDF2C = 0x40000;        /* the GX FIFO's size */
RwInt32 lbl_805EDF30 = 1;              /* the screen is still blanked */
RwInt32 lbl_805EDF34 = 3;              /* the display queue's length */
u8 lbl_805EDF38 = 1;                   /* retraces since the last swap */
u8 lbl_805EDF39 = 1;                   /* retraces a frame stays up at least */

/* .bss */
static RwDlQueuedFrame sDlQueue[3];    /* frames waiting to be shown */
static RwDlRenderMode sDlRenderMode;   /* the application's own render mode */
RwDlMatrix lbl_805E31CC;               /* the camera's inverse frame with the view offset */

/* .data: the video modes of the three TV standards and of the application's render mode. */
static RwDlVideoMode sDlVideoModes[4] = {
    {640, 480, 24, 1, 0, 0},
    {640, 528, 24, 1, 0, 0},
    {640, 480, 24, 1, 0, 0},
    {0, 0, 0, 1, 0, 0},
};

/* The display queue: frames are copied to one of the external frame buffers by a command in
 * the GX FIFO, and a FIFO breakpoint after each copy tells the interrupt side when the frame is
 * ready to be shown (the SDK's triple-buffering scheme). */
static inline void RwDlQueueAdvance(void) {
    RwInt32 next = (lbl_805F1608 + 1) % lbl_805EDF34;
    if (next == lbl_805F160C) {
        GXDisableBreakPt();
        lbl_805F1618 = 0;
    } else {
        GXEnableBreakPt(sDlQueue[next].breakPt);
    }
    lbl_805F1608 = next;
    OSWakeupThread(&lbl_805F1630);
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
    if (sDlQueue[lbl_805F1608].xfb != lbl_805F1604) {
        RwDlQueueAdvance();
    } else {
        lbl_805F1620 = 1;
    }
}

/* VI pre-retrace callback: shows the frame that is ready. */
void fn_80493ABC(void) {
    s16 token;
    lbl_805EDF38++;
    token = lbl_805F1638[lbl_805F1614];
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

/* The standard function of everything the device does not support. */
RwBool fn_80493C7C(void) {
    return 0;
}

/* Chooses the GX render mode: the application's (with its frame buffer format: 0 and 1 are
 * 24-bit, 2 is the 16-bit format antialiasing needs) or the default of the console's TV
 * standard. */
void fn_80493C84(RwDlRenderMode *mode, RwInt32 format) {
    if (mode != NULL) {
        RwInt32 used;
        sDlRenderMode = *mode;
        lbl_805F164C = &sDlRenderMode;
        lbl_805F15EC = 0x2A;
        /* Retail moves the format into r5 first and loads the 2 behind the test; a ternary (any
         * spelling, or an inline function) loads the 2 first (3 rows), while `used = format;
         * if (aa) used = 2;` swaps the address and copy registers of the mode copy above (39
         * rows). Probes: probes/Game_RwCpp56_R9.py. */
        used = (lbl_805F164C->gx.aa != 0) ? 2 : format;
        lbl_805F15E4 = used;
        lbl_805F15E8 = used;
        sDlVideoModes[3].width = lbl_805F164C->gx.fbWidth;
        sDlVideoModes[3].height = lbl_805F164C->gx.efbHeight;
        switch (format) {
        case 0:
        case 1:
            sDlVideoModes[3].depth = 24;
            break;
        case 2:
            sDlVideoModes[3].depth = 16;
            break;
        default:
            RW_ERROR_STRING(2, "Invalid pixel format");
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
            RW_ERROR_STRING(2, "Invalid TV format");
            break;
        }
    }
}

/* Attaches the driver's raster and texture plugins. */
RwBool fn_80493E64(void) {
    fn_804953F0();
    fn_80499BF8();
    return 1;
}

static inline void RwDlSetUpVideo(GXRenderModeObj *mode, RwInt32 format) {
    fn_8037E354(mode);
    fn_8037F094(1);
    fn_8037EEF0();
    if (mode->field_rendering != 0) {
        GXSetViewportJitter(0.0f, 0.0f, mode->fbWidth, mode->xfbHeight, 0.0f, 1.0f, fn_8037F1BC() ^ 1);
    } else {
        GXSetViewport(0.0f, 0.0f, mode->fbWidth, mode->xfbHeight, 0.0f, 1.0f);
    }
    GXSetScissor(0, 0, mode->fbWidth, mode->efbHeight);
    GXSetScissorBoxOffset(0, 0);
    GXSetDispCopySrc(0, 0, mode->fbWidth, mode->efbHeight);
    GXSetDispCopyDst(mode->fbWidth, mode->xfbHeight);
    GXSetCopyFilter(mode->aa, mode->sample_pattern, 1, mode->vfilter);
    if (mode->aa != 0 && mode->xfbHeight == mode->viHeight) {
        GXSetDispCopyYScale(1.0f);
        lbl_805F1648 = (RwUInt32)mode->xfbHeight >> 1;
        lbl_805F15F0 = 1;
        lbl_805EDF28 = 1;
    } else {
        GXSetDispCopyYScale((f32)mode->xfbHeight / (f32)mode->efbHeight);
        lbl_805F15F0 = 0;
    }
    GXSetFieldMode(mode->field_rendering, mode->xfbHeight < mode->viHeight);
    GXSetPixelFmt(format, 0);
}

static inline RwBool RwDlSetStandards(RwDlStandardFunc *fpStandards, RwInt32 numStdFuncs,
                                      RwDlStandard *standards, RwInt32 numStandards) {
    RwInt32 i;
    for (i = 0; i < numStdFuncs; i++) fpStandards[i] = (RwDlStandardFunc)fn_80493C7C;
    while (numStandards--) {
        if (standards->id < numStdFuncs && standards->id >= 0) {
            fpStandards[standards[numStandards].id] = standards[numStandards].func;
        }
    }
    return 1;
}

/* The device's system function (RwDevice.fpSystem). */
RwBool fn_80493E8C(RwInt32 request, void *out, void *inOut, RwInt32 numIn) {
    switch (request) {
    case 7: /* use mode: only mode 0 */
        return numIn < 1;
    case 5: /* number of modes */
        *(RwInt32 *)out = 1;
        return 1;
    case 6: /* mode information */
        if (numIn < 1) {
            switch (lbl_805F15EC) {
        case 0:
            *(RwDlVideoMode *)out = sDlVideoModes[0];
            break;
        case 1:
        case 5:
            *(RwDlVideoMode *)out = sDlVideoModes[1];
            break;
        case 2:
            *(RwDlVideoMode *)out = sDlVideoModes[2];
            break;
        case 0x2A:
            *(RwDlVideoMode *)out = sDlVideoModes[3];
            break;
            }
            return 1;
        }
        return 0;
    case 10: /* current mode */
        *(RwInt32 *)out = 0;
        return 1;
    case 8: /* focus */
        return 1;
    case 4: { /* register: hand out the RwDevice */
        *(RwDlDevice *)out = *(RwDlDevice *)fn_804950A0();
        lbl_805F1650[1] = inOut;
        return 1;
    }
    case 0: { /* open */
        RwDlOpenParams *params = *(RwDlOpenParams **)inOut;
        RwUInt32 xfbBytes;
        if (params != NULL) {
            fn_80493C84(params->mode, params->format);
            lbl_805EDF2C = params->fifoSize;
        } else {
            fn_80493C84(NULL, lbl_805F15E4);
        }
        xfbBytes = (u16)((lbl_805F164C->gx.fbWidth + 15) & ~15) * lbl_805F164C->gx.xfbHeight * 2;
        lbl_805F15F4 = (RwInt32)lbl_805F15B4->malloc(lbl_805EDF2C + xfbBytes * 2 + 0x1F, 0x40411);
        lbl_805F1640 = (lbl_805F15F4 + 0x1F) & ~0x1F;
        DCInvalidateRange((void *)lbl_805F1640, lbl_805EDF2C);
        lbl_805F1604 = lbl_805F15F8 = (void *)(lbl_805F1640 + lbl_805EDF2C);
        lbl_805F1600 = lbl_805F15FC = (u8 *)lbl_805F15F8 + xfbBytes;
        return 1;
    }
    case 1: /* close */
        GXFlush();
        lbl_805F15B4->free((void *)lbl_805F15F4);
        lbl_805F1604 = NULL;
        lbl_805F15F8 = NULL;
        lbl_805F15FC = NULL;
        lbl_805F1600 = NULL;
        lbl_805F1640 = 0;
        lbl_805F15F4 = 0;
        return 1;
    case 2: { /* start */
        GXFifoObj fifo;
        if (lbl_805F162C == 0) {
            lbl_805F1644 = (RwInt32)GXInit((void *)lbl_805F1640, lbl_805EDF2C);
            lbl_805F162C = 1;
        } else {
            GXInitFifoBase(&fifo, (void *)lbl_805F1640, lbl_805EDF2C);
            GXSetCPUFifo(&fifo);
            GXSetGPFifo(&fifo);
            GXInitFifoBase((GXFifoObj *)lbl_805F1644, (void *)lbl_805F1640, lbl_805EDF2C);
            GXSetCPUFifo((GXFifoObj *)lbl_805F1644);
            GXSetGPFifo((GXFifoObj *)lbl_805F1644);
        }
        while (GXReadDrawSync() != lbl_805F1678) {
            GXSetDrawSync(lbl_805F1678);
        }
        RwDlSetUpVideo(&lbl_805F164C->gx, lbl_805F15E4);
        lbl_805EDF30 = 1;
        fn_8037DE8C();
        if (lbl_805F164C->gx.viTVmode & 1) fn_8037DE8C();
        GXSetDispCopyGamma(0);
        lbl_805EDF38 = lbl_805EDF39;
        fn_8037D6A8(fn_80493ABC);
        fn_8037D6EC(fn_80493B98);
        GXSetBreakPtCallback(fn_804939DC);
        OSInitThreadQueue(&lbl_805F1630);
        return 1;
    }
    case 17: /* finalize start */
        fn_80497198();
        return 1;
    case 18: /* initiate stop */
        fn_8049742C();
        return 1;
    case 3: /* stop: wait until the queue has drained */
        do {
            while (lbl_805F161C == 1) {
            }
        } while (lbl_805F1610 != lbl_805F1614);
        return 1;
    case 9: /* initialise the pipelines */
        return 1;
    case 11: { /* the standard functions */
        RwDlStandard standards[27] = {
            {1, fn_804946A4},  {10, fn_80494C5C}, {21, fn_804945E8}, {20, fn_80494C6C},
            {2, fn_80491BAC},  {3, fn_80491EC0},  {7, fn_804935C4},  {6, fn_804929F0},
            {5, fn_80497008},  {4, fn_80496C58},  {9, fn_80493888},  {8, fn_8049714C},
            {15, fn_80495C54}, {16, fn_804960B4}, {23, fn_80496268}, {24, fn_80496338},
            {14, fn_804994DC}, {13, fn_804994B8}, {17, fn_80498A4C}, {18, fn_80498A9C},
            {19, fn_80498A74}, {11, fn_80499520}, {12, fn_80497168}, {25, fn_8049952C},
            {27, fn_804995B4}, {26, fn_8049988C}, {28, fn_80495688},
        };
        RwDlSetStandards((RwDlStandardFunc *)out, numIn, standards, sizeof(standards) / sizeof(RwDlStandard));
        return 1;
    }
    case 19: /* the largest texture */
        *(RwInt32 *)out = 0x400;
        return 1;
    case 22: /* the device's id */
        *(u16 *)out = 3;
        return 1;
    }
    return 0;
}

/* Driver standard function CameraClear: inside a frame it draws the clear quad over the camera's
 * raster (fn_80498E08); between frames it sets the copy clear colour. */
RwBool fn_804945E8(void *out, void *inOut, RwInt32 num) {
    RwDlCamera *camera = (RwDlCamera *)out;
    RwDlRect rect;
    if (lbl_805F15E0 == 0) {
        RwDlRaster *raster = camera->frameBuffer;
        if (lbl_805F15F0 == 0) {
            rect.x = raster->nOffsetX;
            rect.y = raster->nOffsetY;
            rect.w = raster->width;
            rect.h = raster->height;
        } else {
            rect.x = raster->nOffsetX;
            rect.y = raster->nOffsetY * 2;
            rect.w = raster->width;
            rect.h = raster->height * 2;
        }
        fn_80498E08(raster, &rect, inOut, num);
    } else if (num & 1) {
        GXSetCopyClear(*(GXColor *)inOut, 0xFFFFFF);
    }
    return 1;
}

/* The GX projection of the camera being updated: type, scales and offsets (GXSetProjectionv). */
static f32 sDlProjection[7] = {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

/* Driver standard function CameraBeginUpdate: the view matrix (the camera frame's inverse with the
 * view offset), the projection, the pixel format, viewport and scissor box of the camera's raster
 * (a texture target, or the frame buffer, whole or in two halves). */
RwBool fn_804946A4(void *out, void *inOut, RwInt32 num) {
    RwDlCamera *camera = (RwDlCamera *)inOut;
    RwDlMatrix viewOffset;
    RwDlRaster *raster;
    GXSetCurrentGXThread();
    lbl_805F1650[0] = camera;
    viewOffset.flags = 0;
    viewOffset.right.x = 1.0f;
    viewOffset.right.y = 0.0f;
    viewOffset.right.z = 0.0f;
    viewOffset.up.x = 0.0f;
    viewOffset.up.y = 1.0f;
    viewOffset.up.z = 0.0f;
    viewOffset.at.x = -camera->viewOffsetX;
    viewOffset.at.y = camera->viewOffsetY;
    viewOffset.at.z = 1.0f;
    viewOffset.pos.x = camera->viewOffsetX;
    viewOffset.pos.y = -camera->viewOffsetY;
    viewOffset.pos.z = 0.0f;
    lbl_805E31CC.flags = 0x20003;
    fn_8047F0B0(&lbl_805E31CC, RwFrameGetLTM(camera->frame));
    fn_8047F72C(&lbl_805E31CC, &viewOffset, 2);
    sDlProjection[1] = camera->recipViewWindowX;
    sDlProjection[3] = camera->recipViewWindowY;
    if (camera->projection == 2) {
        sDlProjection[0] = 1.0f;
        sDlProjection[5] = -1.0f / (camera->farPlane - camera->nearPlane);
        sDlProjection[6] = camera->farPlane * sDlProjection[5];
    } else {
        sDlProjection[0] = 0.0f;
        sDlProjection[5] = -camera->nearPlane / (camera->farPlane - camera->nearPlane);
        sDlProjection[6] = camera->farPlane * sDlProjection[5];
    }
    GXSetProjectionv(sDlProjection);
    raster = camera->frameBuffer;
    if (raster->cType & 5) {
        if (((RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658))->flags & 1) {
            if (lbl_805F15E8 != 1) {
                GXSetPixelFmt(1, 0);
                lbl_805F15E8 = 1;
            }
        } else if (lbl_805F15E8 != 0) {
            GXSetPixelFmt(0, 0);
            lbl_805F15E8 = 0;
        }
        GXSetViewport(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height, 0.0f, 1.0f);
        GXSetScissor(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height);
        GXSetScissorBoxOffset(0, 0);
    } else {
        if (lbl_805F15E8 != lbl_805F15E4) {
            GXSetPixelFmt(lbl_805F15E4, 0);
            lbl_805F15E8 = lbl_805F15E4;
        }
        if (lbl_805F15F0 == 0) {
            if (lbl_805F164C->gx.field_rendering != 0) {
                GXSetViewportJitter(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height, 0.0f, 1.0f,
                                    fn_8037F1BC() ^ 1);
            } else {
                GXSetViewport(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height, 0.0f, 1.0f);
            }
            GXSetScissor(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height);
        } else {
            if (lbl_805F164C->gx.field_rendering != 0) {
                GXSetViewportJitter(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height, 0.0f, 1.0f,
                                    fn_8037F1BC() ^ 1);
            } else {
                GXSetViewport(raster->nOffsetX, raster->nOffsetY * 2, raster->width, raster->height * 2, 0.0f, 1.0f);
            }
            if (lbl_805EDF28 != 0) {
                if ((raster->nOffsetY + raster->height) * 2 <= lbl_805F1648 + 2) {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2, lbl_805F164C->gx.fbWidth, raster->height * 2);
                } else if (raster->nOffsetY * 2 > lbl_805F1648 + 2) {
                    GXSetScissor(0, 0, lbl_805F164C->gx.fbWidth, lbl_805F1648 + 2);
                } else {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2, lbl_805F164C->gx.fbWidth, lbl_805F1648 + 2);
                }
                GXSetScissorBoxOffset(0, 0);
            } else {
                if (raster->nOffsetY * 2 >= lbl_805F1648 - 2) {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2, lbl_805F164C->gx.fbWidth, raster->height * 2);
                } else if ((raster->nOffsetY + raster->height) * 2 < lbl_805F1648 + 2) {
                    GXSetScissor(0, lbl_805F1648 - 2, lbl_805F164C->gx.fbWidth, lbl_805F1648 + 2);
                } else {
                    GXSetScissor(raster->nOffsetX, lbl_805F1648 - 2, lbl_805F164C->gx.fbWidth, raster->height * 2);
                }
                GXSetScissorBoxOffset(0, lbl_805F1648 - 2);
            }
        }
    }
    return 1;
}

/* Driver standard function CameraEndUpdate. */
RwBool fn_80494C5C(void *out, void *inOut, RwInt32 num) {
    lbl_805F1650[0] = NULL;
    return 1;
}

/* The display queue is full: the frame after the last one queued is still being shown. */
static inline RwBool RwDlQueueFull(void) {
    return lbl_805F160C - lbl_805F1608 == -1 || lbl_805EDF34 - 1 == lbl_805F160C - lbl_805F1608;
}

/* Puts the frame just drawn into the display queue: the FIFO position of its copy command (a
 * breakpoint there tells the interrupt side the copy is done) and, for a whole frame, the
 * draw-sync token GX passes after it. */
#define RW_DL_QUEUE_FRAME()                                  \
    do {                                                     \
        GXFlush();                                           \
        GXGetFifoPtrs(GXGetCPUFifo(), &readPtr, &writePtr);  \
        level = OSDisableInterrupts();                       \
        sDlQueue[lbl_805F160C].breakPt = writePtr;           \
        sDlQueue[lbl_805F160C].xfb = lbl_805F1600;           \
        lbl_805F1638[lbl_805F1610] = lbl_805EDF40;           \
        lbl_805F160C = (lbl_805F160C + 1) % lbl_805EDF34;    \
        lbl_805F1610 = (lbl_805F1610 + 1) % lbl_805EDF34;    \
        OSRestoreInterrupts(level);                          \
        if (lbl_805F1618 == 0) {                             \
            lbl_805F1618 = 1;                                \
            GXEnableBreakPt(writePtr);                       \
        }                                                    \
    } while (0)

#define RW_DL_QUEUE_HALF()                                   \
    do {                                                     \
        GXFlush();                                           \
        GXGetFifoPtrs(GXGetCPUFifo(), &readPtr, &writePtr);  \
        level = OSDisableInterrupts();                       \
        sDlQueue[lbl_805F160C].breakPt = writePtr;           \
        sDlQueue[lbl_805F160C].xfb = lbl_805F1600;           \
        lbl_805F160C = (lbl_805F160C + 1) % lbl_805EDF34;    \
        OSRestoreInterrupts(level);                          \
        if (lbl_805F1618 == 0) {                             \
            lbl_805F1618 = 1;                                \
            GXEnableBreakPt(writePtr);                       \
        }                                                    \
    } while (0)

/* Sends the current draw-sync token and moves to the next one. */
static inline void RwDlNextToken(void) {
    GXSetDrawSync(lbl_805EDF40);
    lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
}

/* Driver standard function RasterShowRaster: queues the finished frame for display (in two-halves
 * mode the top half is copied first and the bottom half completes the frame). */
RwBool fn_80494C6C(void *out, void *inOut, RwInt32 flags) {
    void *readPtr;
    void *writePtr;
    volatile RwBool full;
    BOOL level = OSDisableInterrupts();
    full = RwDlQueueFull() != 0;
    if (full == 1) OSSleepThread(&lbl_805F1630);
    OSRestoreInterrupts(level);
    if (lbl_805F15F0 == 0) {
        RW_DL_QUEUE_FRAME();
        GXCopyDisp(lbl_805F1600, lbl_805F15E0);
        RwDlNextToken();
        GXFlush();
        lbl_805F1600 = (lbl_805F1600 == lbl_805F15F8) ? lbl_805F15FC : lbl_805F15F8;
    } else if (lbl_805EDF28 != 0) {
        RW_DL_QUEUE_HALF();
        GXCopyDisp(lbl_805F1600, lbl_805F15E0);
        RwDlNextToken();
        GXFlush();
        lbl_805EDF28 = 0;
    } else {
        RW_DL_QUEUE_FRAME();
        GXSetCopyClamp(2);
        GXSetDispCopySrc(0, 2, lbl_805F164C->gx.fbWidth, lbl_805F164C->gx.efbHeight - 2);
        GXCopyDisp((u8 *)lbl_805F1600 + (u16)((lbl_805F164C->gx.fbWidth + 15) & ~15) * (lbl_805F164C->gx.efbHeight - 2) * 2,
                   lbl_805F15E0);
        RwDlNextToken();
        GXSetCopyClamp(3);
        GXSetDispCopySrc(0, 0, lbl_805F164C->gx.fbWidth, lbl_805F164C->gx.efbHeight);
        GXFlush();
        lbl_805F1600 = (lbl_805F1600 == lbl_805F15F8) ? lbl_805F15FC : lbl_805F15F8;
        lbl_805EDF28 = 1;
    }
    return 1;
}

/* The device's RwDevice record. */
static RwDlDevice sDlDevice = {
    1.0f,
    fn_80493E8C,
    0.0f,
    0.99999994f,
    fn_80497904,
    fn_80497474,
    (void *)fn_80490CBC,
    (void *)fn_80490AB8,
    (void *)fn_80490E30,
    (void *)fn_8049140C,
    NULL,
    NULL,
    NULL,
    NULL,
};

/* _rwDeviceGetHandle */
void *fn_804950A0(void) {
    return &sDlDevice;
}

/* Loads the view matrix into GX: the camera frame's matrix times the driver's axis flip
 * (lbl_805E31CC), inverted into GX's conventions (x and z negated), or the flip alone. */
void fn_804950AC(void *frameMatrix, RwBool normals) {
    RwDlMatrix view;
    f32 mtx[3][4];
    RwDlMatrix *m;
    if (frameMatrix != NULL) {
        view.flags = 0x20003;
        fn_8047EBB8(&view, frameMatrix, &lbl_805E31CC);
        m = &view;
    } else {
        m = &lbl_805E31CC;
    }
    mtx[0][0] = -m->right.x;
    mtx[0][1] = -m->up.x;
    mtx[0][2] = -m->at.x;
    mtx[0][3] = -m->pos.x;
    mtx[1][0] = m->right.y;
    mtx[1][1] = m->up.y;
    mtx[1][2] = m->at.y;
    mtx[1][3] = m->pos.y;
    mtx[2][0] = -m->right.z;
    mtx[2][1] = -m->up.z;
    mtx[2][2] = -m->at.z;
    mtx[2][3] = -m->pos.z;
    GXLoadPosMtxImm(mtx, 0);
    if (normals) GXLoadNrmMtxImm(mtx, 0);
    GXSetCurrentMtx(0);
}

/* Copies the embedded frame buffer into a camera texture raster: the raster's rectangle (twice
 * its size, box-filtered down, when the frame is drawn at double size) into the parent texture's
 * image at the raster's tile offset, then invalidates the texture cache. */
void fn_804951B8(RwDlRaster *raster, RwBool doubled) {
    RwDlRaster *parent = raster->parent;
    RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)parent + lbl_805F1658);
    RwInt32 offset;
    GXSetCopyFilter(0, NULL, 0, NULL);
    if (doubled) {
        GXSetTexCopySrc(raster->nOffsetX * 2, raster->nOffsetY * 2, raster->width * 2,
                        raster->height * 2);
    } else {
        GXSetTexCopySrc(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height);
    }
    GXSetTexCopyDst(parent->width, parent->height, ext->texFormat, doubled);
    switch (parent->depth) {
    case 4:
        offset = (raster->nOffsetX * 8 + ((parent->width + 7) & ~7) * raster->nOffsetY) >> 1;
        break;
    case 8:
        offset = raster->nOffsetX * 4 + ((parent->width + 7) & ~7) * raster->nOffsetY;
        break;
    case 16:
        offset = (raster->nOffsetX * 4 + ((parent->width + 3) & ~3) * raster->nOffsetY) << 1;
        break;
    case 32:
        offset = (raster->nOffsetX * 4 + ((parent->width + 3) & ~3) * raster->nOffsetY) << 2;
        break;
    case 24:
    default:
        RW_ERROR(0x8000000C);
        return;
    }
    GXCopyTex(ext->pixels + offset, lbl_805F15E0);
    GXPixModeSync();
    GXSetCopyFilter(lbl_805F164C->gx.aa, lbl_805F164C->gx.sample_pattern, 1, lbl_805F164C->gx.vfilter);
    if (ext->region != NULL) {
        GXInvalidateTexRegion(ext->region);
    } else {
        GXInvalidateTexAll();
    }
}

/* Waits until no frame is waiting for the retrace and the last queued one is on screen, then
 * returns the frame buffer shown and the one drawn to. */
void fn_804953C0(void **displayed, void **drawn) {
    while (lbl_805F161C == 1 || lbl_805F1610 != lbl_805F1614) {
    }
    *displayed = (void *)lbl_805F1604;
    *drawn = (void *)lbl_805F1600;
}
