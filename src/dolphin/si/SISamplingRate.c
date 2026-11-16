/* Dolphin SDK SI: how often the hardware polls the controllers, as the number of polls per
 * frame (x: lines between polls, y: polls per field), from a table for the TV format. */
#include <dolphin/si.h>

typedef struct XY {
    u16 line;
    u8 count;
} XY;

#define VI_NTSC 0
#define VI_PAL 1
#define VI_MPAL 2
#define VI_EURGB60 5

volatile u16 __VIRegs[] : 0xCC002000;

u32 VIGetTvFormat(void);

static u32 SamplingRate;

static XY XYNTSC[12] = {
    {263 - 17, 2}, {14, 19}, {30, 9}, {44, 6}, {52, 5}, {65, 4},
    {87, 3}, {87, 3}, {87, 3}, {131, 2}, {131, 2}, {131, 2},
};

static XY XYPAL[12] = {
    {313 - 17, 2}, {15, 21}, {29, 11}, {45, 7}, {52, 6}, {63, 5},
    {78, 4}, {104, 3}, {104, 3}, {104, 3}, {104, 3}, {156, 2},
};

void SISetSamplingRate(u32 msec) {
    XY *xy;
    BOOL enabled;

    if (msec > 11) {
        msec = 11;
    }

    enabled = OSDisableInterrupts();
    SamplingRate = msec;

    switch (VIGetTvFormat()) {
    case VI_NTSC:
    case VI_MPAL:
    case VI_EURGB60:
        xy = XYNTSC;
        break;
    case VI_PAL:
        xy = XYPAL;
        break;
    default:
        OSReport("SISetSamplingRate: unknown TV format. Use default.");
        msec = 0;
        xy = XYNTSC;
        break;
    }

    /* Twice as many lines between polls in a progressive-scan mode. */
    SISetXY(((__VIRegs[54] & 1) ? 2 : 1) * xy[msec].line, xy[msec].count);
    OSRestoreInterrupts(enabled);
}

void SIRefreshSamplingRate(void) {
    SISetSamplingRate(SamplingRate);
}
