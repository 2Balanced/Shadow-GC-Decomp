/* Dolphin SDK OS: the fatal error screen.
 *
 * OSFatal stops everything (scheduler, audio, VI callbacks, EXI), gives the reset functions up
 * to a second to finish, and then continues on a fresh stack at the top of the arena (the
 * "fiber" switch), where Halt loads the IPL font, paints the whole frame buffer in the
 * background colour, prints the message in the foreground colour and halts the CPU.
 *
 * The VI, AI, EXI and GX calls are still unnamed in the binary: fn_8037D9DC is VIInit,
 * fn_8037F094 VISetBlack, fn_8037EEF0 VIFlush, fn_8037D6A8/fn_8037D6EC
 * VISetPreRetraceCallback/VISetPostRetraceCallback, fn_8037F110 VIGetRetraceCount,
 * fn_8037F020 VISetNextFrameBuffer, fn_8037E354 VIConfigure, fn_8037EB5C VIConfigurePan,
 * fn_803813E0/fn_8038140C AISetStreamVolLeft/Right, fn_803B31D4 EXISetExiCallback and
 * fn_80395454 GXAbortFrame. OSSwitchFiber is OSSwitchFiber, fn_80374690 OSLoadFont and
 * fn_8037490C OSGetFontTexel. ScreenReport is not matched yet: its size, frame and instruction
 * sequence are right (the pixel offset needs its own local, n), but the register allocation still
 * differs. */
#include <dolphin/os.h>

/* The colour conversion keeps separate multiplies and adds (no fused multiply-add). */
#pragma fp_contract off

void *memmove(void *dst, const void *src, unsigned long n);
unsigned long strlen(const char *s);

volatile u16 __VIRegs[] : 0xCC002000;
volatile u32 __EXIRegs[] : 0xCC006800;

u32 __OSBusClock : 0x800000F8;
#define OS_TIMER_CLOCK (__OSBusClock / 4)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))

#define VI_NTSC 0
#define VI_PAL 1
#define VI_MPAL 2
#define VI_EURGB60 5

#define VI_TVMODE_NTSC_INT 0
#define VI_TVMODE_NTSC_PROG 2
#define VI_TVMODE_PAL_INT 4
#define VI_TVMODE_EURGB60_INT 20

#define VI_XFBMODE_SF 0
#define VI_XFBMODE_DF 1

#define OS_INTERRUPTMASK_PI_VI 0x00000080
#define DECREMENTER_EXCEPTION 8

typedef struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;

typedef struct GXRenderModeObj {
    s32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    s32 xFBmode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
} GXRenderModeObj;

/* Only the field this file reads. */
typedef struct OSFontHeader {
    u16 fontType;
    u16 firstChar;
    u16 lastChar;
    u16 invalChar;
    u16 ascent;
    u16 descent;
    u16 width;
    u16 leading;
} OSFontHeader;

typedef struct OSBootInfo {
    u8 diskID[0x20];
    u32 magic;
    u32 version;
    u32 memorySize;
    u32 consoleType;
    void *arenaLo;
    void *arenaHi;
    void *FSTLocation;
    u32 FSTMaxLength;
} OSBootInfo;

typedef struct OSFatalParam {
    GXColor fg;
    GXColor bg;
    const char *msg;
} OSFatalParam;

typedef void (*VIRetraceCallback)(u32 retraceCount);
typedef void (*EXICallback)(s32 chan, OSContext *context);

void fn_8037D9DC(void);
void fn_8037F094(BOOL black);
void fn_8037EEF0(void);
VIRetraceCallback fn_8037D6A8(VIRetraceCallback callback);
VIRetraceCallback fn_8037D6EC(VIRetraceCallback callback);
u32 fn_8037F110(void);
void fn_8037F020(void *fb);
void fn_8037E354(const GXRenderModeObj *rm);
void fn_8037EB5C(u16 xOrg, u16 yOrg, u16 width, u16 height);
u32 VIGetTvFormat(void);
void fn_803813E0(u8 volume);
void fn_8038140C(u8 volume);
EXICallback fn_803B31D4(s32 chan, EXICallback callback);
BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback);
BOOL EXIUnlock(s32 chan);
BOOL EXIDeselect(s32 chan);
BOOL EXISync(s32 chan);
void fn_80395454(void);
void OSSwitchFiber(u32 pc, u32 newsp);
u32 fn_80374690(OSFontHeader *fontData, void *temp);
char *fn_8037490C(char *string, void *image, s32 pos, s32 stride, s32 *width);
void __OSStopAudioSystem(void);
u32 __OSUnmaskInterrupts(u32 mask);
BOOL __OSCallResetFunctions(BOOL final);
void OSDefaultExceptionHandler(__OSException exception, OSContext *context);
void *OSAllocFromArenaLo(u32 size, u32 align);
void OSSetArenaLo(void *addr);
void OSSetArenaHi(void *addr);
void *OSGetArenaLo(void);
void *OSGetArenaHi(void);

static OSFatalParam FatalParam;
static OSContext FatalContext;

/* Fills a YUY2 frame buffer (two pixels per Y-Cb-Y-Cr word) with one colour. */
static void ScreenClear(void *xfb, u16 xfbW, u16 xfbH, GXColor yuv) {
    int i;
    int j;
    u8 *ptr;

    ptr = (u8 *)xfb;
    for (i = 0; i < xfbH; i++) {
        for (j = 0; j < xfbW; j += 2) {
            *ptr++ = yuv.r;
            *ptr++ = yuv.g;
            *ptr++ = yuv.r;
            *ptr++ = yuv.b;
        }
    }
}

/* Draws `string` with the IPL font into a YUY2 frame buffer, starting at (x, y). */
static void ScreenReport(void *xfb, u16 xfbW, u16 xfbH, GXColor yuv, s32 x, s32 y, s32 leading,
                         char *string) {
    /* The declaration order steers the register allocation (this order is the closest found). */
    u32 i;
    s32 xpos;
    u8 *ptr;
    u32 j;
    u32 k;
    u32 texel;
    s32 width;
    u32 n;
    u32 image[24 / 8 * 24];
    /* Never used: each still takes a stack slot below image with this compiler. */
    s32 unused0;
    s32 unused1;

next:
    if (xfbH - 24 < y) {
        return;
    }
    ptr = (u8 *)xfb + (y * xfbW + x) * 2;
    xpos = x;
    while (*string != '\0') {
        if (*string == '\n') {
            ++string;
            y += leading;
            goto next;
        }
        if (xfbW - 48 < xpos) {
            y += leading;
            goto next;
        }

        for (i = 0; i < 24; ++i) {
            j = 24 * (i / 8) + (i % 8);
            image[j + 0] = 0;
            image[j + 8] = 0;
            image[j + 16] = 0;
        }
        string = fn_8037490C(string, image, 0, 6, &width);

        for (i = 0; i < 24; ++i) {
            j = 24 * (i / 8) + (i % 8);
            for (k = 0; k < 24; ++k) {
                texel = (image[j + 8 * (k / 8)] >> (4 * (7 - (k % 8)))) & 0xF;
                if (texel != 0) {
                    n = (i * xfbW + k) * 2;
                    ptr[n] = (u8)(16 + (s32)(yuv.r * (texel * 239)) / 255 / 15);
                    if ((xpos + k) % 2) {
                        ptr[n - 1] = yuv.g;
                        ptr[n + 1] = yuv.b;
                    } else {
                        ptr[n - 1] = yuv.b;
                        ptr[n + 1] = yuv.g;
                    }
                }
            }
        }
        ptr += width * 2;
        xpos += width;
    }
}

static void ConfigureVideo(u16 xfbWidth, u16 xfbHeight) {
    GXRenderModeObj mode;

    mode.fbWidth = xfbWidth;
    mode.efbHeight = 480;
    mode.xfbHeight = xfbHeight;
    mode.viXOrigin = 40;
    mode.viWidth = 640;
    mode.viHeight = xfbHeight;

    switch (VIGetTvFormat()) {
    case VI_NTSC:
    case VI_MPAL:
        if (__VIRegs[54] & 1) {
            mode.viTVmode = VI_TVMODE_NTSC_PROG;
            mode.viYOrigin = 0;
            mode.xFBmode = VI_XFBMODE_SF;
        } else {
            mode.viTVmode = VI_TVMODE_NTSC_INT;
            mode.viYOrigin = 0;
            mode.xFBmode = VI_XFBMODE_DF;
        }
        break;
    case VI_EURGB60:
        mode.viTVmode = VI_TVMODE_EURGB60_INT;
        mode.viYOrigin = 0;
        mode.xFBmode = VI_XFBMODE_DF;
        break;
    case VI_PAL:
        mode.viTVmode = VI_TVMODE_PAL_INT;
        mode.viYOrigin = 47;
        mode.xFBmode = VI_XFBMODE_DF;
        break;
    }

    fn_8037E354(&mode);
    fn_8037EB5C(0, 0, 640, 480);
}

#define CLAMP(x, lo, hi) ((x) > (hi) ? (hi) : ((x) < (lo) ? (lo) : (x)))

/* ITU-R BT.601 conversion with rounding and range clamping. */
static GXColor RGB2YUV(GXColor rgb) {
    f32 Y;
    f32 Cb;
    f32 Cr;
    GXColor yuv;

    Y = 0.257f * rgb.r + 0.504f * rgb.g + 0.098f * rgb.b + 16.0f + 0.5f;
    Cb = -0.148f * rgb.r - 0.291f * rgb.g + 0.439f * rgb.b + 128.0f + 0.5f;
    Cr = 0.439f * rgb.r - 0.368f * rgb.g - 0.071f * rgb.b + 128.0f + 0.5f;

    yuv.r = (u8)CLAMP(Y, 16.0f, 235.0f);
    yuv.g = (u8)CLAMP(Cb, 16.0f, 240.0f);
    yuv.b = (u8)CLAMP(Cr, 16.0f, 240.0f);
    yuv.a = 0;
    return yuv;
}

static void Halt(void);

void OSFatal(GXColor fg, GXColor bg, const char *msg) {
    OSBootInfo *bootInfo = (OSBootInfo *)OSPhysicalToCached(0);
    u32 count;
    OSTime t;

    OSDisableInterrupts();
    OSDisableScheduler();
    OSClearContext(&FatalContext);
    OSSetCurrentContext(&FatalContext);
    __OSStopAudioSystem();
    fn_803813E0(0);
    fn_8038140C(0);

    fn_8037D9DC();
    __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_VI);
    fn_8037F094(TRUE);
    fn_8037EEF0();
    fn_8037D6A8(NULL);
    fn_8037D6EC(NULL);
    OSEnableInterrupts();
    count = fn_8037F110();
    while ((s32)fn_8037F110() - (s32)count < 1) {
    }

    /* Give the reset functions up to a second. */
    t = OSGetTime();
    while (!__OSCallResetFunctions(FALSE) && OSGetTime() - t < OSMillisecondsToTicks(1000)) {
    }
    OSDisableInterrupts();
    __OSCallResetFunctions(TRUE);

    fn_803B31D4(0, NULL);
    fn_803B31D4(2, NULL);
    while (!EXILock(0, 1, NULL)) {
        EXISync(0);
        EXIDeselect(0);
        EXIUnlock(0);
    }
    EXIUnlock(0);
    while ((__EXIRegs[3] & 1) == 1) {
    }

    __OSSetExceptionHandler(DECREMENTER_EXCEPTION, OSDefaultExceptionHandler);
    fn_80395454();
    OSSetArenaLo((void *)0x81400000);
    OSSetArenaHi(bootInfo->FSTLocation);

    FatalParam.fg = fg;
    FatalParam.bg = bg;
    FatalParam.msg = msg;
    OSSwitchFiber((u32)Halt, (u32)OSGetArenaHi());
}

static void Halt(void) {
    u32 count;
    OSFontHeader *fontData;
    u8 *xfb;
    u32 len;
    OSFatalParam *fp;

    OSEnableInterrupts();
    fp = &FatalParam;
    len = strlen(fp->msg) + 1;
    fp->msg = memmove(OSAllocFromArenaLo(len, 32), fp->msg, len);

    fontData = (OSFontHeader *)OSAllocFromArenaLo(0xA1004, 32);
    fn_80374690(fontData, OSGetArenaLo());

    xfb = (u8 *)OSAllocFromArenaLo(640 * 480 * 2, 32);
    ScreenClear(xfb, 640, 480, RGB2YUV(fp->bg));
    fn_8037F020(xfb);
    ConfigureVideo(640, 480);
    fn_8037EEF0();
    count = fn_8037F110();
    while ((s32)fn_8037F110() - (s32)count < 2) {
    }

    ScreenReport(xfb, 640, 480, RGB2YUV(fp->fg), 48, 100, fontData->leading, (char *)fp->msg);
    DCFlushRange(xfb, 640 * 480 * 2);
    fn_8037F094(FALSE);
    fn_8037EEF0();
    count = fn_8037F110();
    while ((s32)fn_8037F110() - (s32)count < 1) {
    }

    OSDisableInterrupts();
    OSReport("%s\n", fp->msg);
    PPCHalt();
}
