/* Dolphin SDK AX: the studio parameter block.
 *
 * When a voice is cut off while still sounding, whatever it last contributed to each bus would
 * drop to zero at once and click. __AXDepopVoice collects those leftovers per bus, and every frame
 * __AXPrintStudio hands the DSP a ramp that fades each bus's leftover out over a few frames, at
 * most 20 steps of 160 samples' worth per frame. */
#include <dolphin/os.h>
#include <dolphin/ax.h>

#define AX_DPOP_STEPS 160
#define AX_DPOP_MAX_DELTA 20

#define CLAMP_DELTA(d)                                                                                 if ((d) > AX_DPOP_MAX_DELTA) {                                                                         (d) = AX_DPOP_MAX_DELTA;                                                                       }                                                                                                  if ((d) < -AX_DPOP_MAX_DELTA) {                                                                        (d) = -AX_DPOP_MAX_DELTA;                                                                      }

/* Handed to the DSP by address, so it starts on a cache line: retail leaves 24 bytes free in
 * front of it. */
static AXSPB __AXStudio __attribute__((aligned(32)));

static s32 __AXDpopL;
static s32 __AXDpopR;
static s32 __AXDpopS;
static s32 __AXDpopAL;
static s32 __AXDpopAR;
static s32 __AXDpopAS;
static s32 __AXDpopBL;
static s32 __AXDpopBR;
static s32 __AXDpopBS;

AXSPB *__AXGetStudio(void) { return &__AXStudio; }

void __AXPrintStudio(void) {
    s32 delta;

    if ((delta = __AXDpopL / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopLHi = __AXDpopL;
        __AXDpopL -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopLDelta = (s16)-delta;
    } else {
        __AXDpopL = 0;
        *(s32 *)&__AXStudio.dpopLHi = 0;
        __AXStudio.dpopLDelta = 0;
    }
    if ((delta = __AXDpopR / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopRHi = __AXDpopR;
        __AXDpopR -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopRDelta = (s16)-delta;
    } else {
        __AXDpopR = 0;
        *(s32 *)&__AXStudio.dpopRHi = 0;
        __AXStudio.dpopRDelta = 0;
    }
    if ((delta = __AXDpopS / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopSHi = __AXDpopS;
        __AXDpopS -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopSDelta = (s16)-delta;
    } else {
        __AXDpopS = 0;
        *(s32 *)&__AXStudio.dpopSHi = 0;
        __AXStudio.dpopSDelta = 0;
    }
    if ((delta = __AXDpopAL / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopALHi = __AXDpopAL;
        __AXDpopAL -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopALDelta = (s16)-delta;
    } else {
        __AXDpopAL = 0;
        *(s32 *)&__AXStudio.dpopALHi = 0;
        __AXStudio.dpopALDelta = 0;
    }
    if ((delta = __AXDpopAR / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopARHi = __AXDpopAR;
        __AXDpopAR -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopARDelta = (s16)-delta;
    } else {
        __AXDpopAR = 0;
        *(s32 *)&__AXStudio.dpopARHi = 0;
        __AXStudio.dpopARDelta = 0;
    }
    if ((delta = __AXDpopAS / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopASHi = __AXDpopAS;
        __AXDpopAS -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopASDelta = (s16)-delta;
    } else {
        __AXDpopAS = 0;
        *(s32 *)&__AXStudio.dpopASHi = 0;
        __AXStudio.dpopASDelta = 0;
    }
    if ((delta = __AXDpopBL / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopBLHi = __AXDpopBL;
        __AXDpopBL -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopBLDelta = (s16)-delta;
    } else {
        __AXDpopBL = 0;
        *(s32 *)&__AXStudio.dpopBLHi = 0;
        __AXStudio.dpopBLDelta = 0;
    }
    if ((delta = __AXDpopBR / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopBRHi = __AXDpopBR;
        __AXDpopBR -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopBRDelta = (s16)-delta;
    } else {
        __AXDpopBR = 0;
        *(s32 *)&__AXStudio.dpopBRHi = 0;
        __AXStudio.dpopBRDelta = 0;
    }
    if ((delta = __AXDpopBS / AX_DPOP_STEPS) != 0) {
        CLAMP_DELTA(delta);
        *(s32 *)&__AXStudio.dpopBSHi = __AXDpopBS;
        __AXDpopBS -= delta * AX_DPOP_STEPS;
        __AXStudio.dpopBSDelta = (s16)-delta;
    } else {
        __AXDpopBS = 0;
        *(s32 *)&__AXStudio.dpopBSHi = 0;
        __AXStudio.dpopBSDelta = 0;
    }
    DCFlushRange(&__AXStudio, sizeof(__AXStudio));
}

void __AXSPBInit(void) {
    __AXDpopBS = 0;
    __AXDpopBR = 0;
    __AXDpopBL = 0;
    __AXDpopAS = 0;
    __AXDpopAR = 0;
    __AXDpopAL = 0;
    __AXDpopS = 0;
    __AXDpopR = 0;
    __AXDpopL = 0;
}

void __AXSPBQuit(void) {}

void __AXDepopVoice(AXPB *pb) {
    __AXDpopL += (s16)pb->dpop.aL;
    __AXDpopAL += (s16)pb->dpop.aAuxAL;
    __AXDpopBL += (s16)pb->dpop.aAuxBL;
    __AXDpopR += (s16)pb->dpop.aR;
    __AXDpopAR += (s16)pb->dpop.aAuxAR;
    __AXDpopBR += (s16)pb->dpop.aAuxBR;
    __AXDpopS += (s16)pb->dpop.aS;
    __AXDpopAS += (s16)pb->dpop.aAuxAS;
    __AXDpopBS += (s16)pb->dpop.aAuxBS;
}
