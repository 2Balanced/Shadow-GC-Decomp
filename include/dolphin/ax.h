#ifndef DOLPHIN_AX_H
#define DOLPHIN_AX_H

/* The parts of the Dolphin SDK AX (DSP audio mixer) library the decompiled AX units share. */

#include <dolphin/os.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AXPBMIX {
    u16 vL, vDeltaL;
    u16 vR, vDeltaR;
    u16 vAuxAL, vDeltaAuxAL;
    u16 vAuxAR, vDeltaAuxAR;
    u16 vAuxBL, vDeltaAuxBL;
    u16 vAuxBR, vDeltaAuxBR;
    u16 vAuxBS, vDeltaAuxBS;
    u16 vS, vDeltaS;
    u16 vAuxAS, vDeltaAuxAS;
} AXPBMIX;

typedef struct AXPBITD {
    u16 flag;
    u16 bufferHi, bufferLo;
    u16 shiftL, shiftR;
    u16 targetShiftL, targetShiftR;
} AXPBITD;

typedef struct AXPBUPDATE {
    u16 updNum[5];
    u16 dataHi, dataLo;
} AXPBUPDATE;

/* Per-voice depop accumulators: what the voice still contributed to each bus when it stopped. */
typedef struct AXPBDPOP {
    u16 aL;     /* 0x0 */
    u16 aAuxAL; /* 0x2 */
    u16 aAuxBL; /* 0x4 */
    u16 aR;     /* 0x6 */
    u16 aAuxAR; /* 0x8 */
    u16 aAuxBR; /* 0xA */
    u16 aS;     /* 0xC */
    u16 aAuxAS; /* 0xE */
    u16 aAuxBS; /* 0x10 */
} AXPBDPOP;

typedef struct AXPBVE {
    u16 currentVolume;
    s16 currentDelta;
} AXPBVE;

typedef struct AXPBFIR {
    u16 numCoefs;
    u16 coefsHi, coefsLo;
} AXPBFIR;

typedef struct AXPBADDR {
    u16 loopFlag;
    u16 format;
    u16 loopAddressHi, loopAddressLo;
    u16 endAddressHi, endAddressLo;
    u16 currentAddressHi, currentAddressLo;
} AXPBADDR;

typedef struct AXPBADPCM {
    u16 a[8][2];
    u16 gain;
    u16 pred_scale;
    u16 yn1;
    u16 yn2;
} AXPBADPCM;

typedef struct AXPBSRC {
    u16 ratioHi, ratioLo;
    u16 currentAddressFrac;
    u16 last_samples[4];
} AXPBSRC;

typedef struct AXPBADPCMLOOP {
    u16 loop_pred_scale;
    u16 loop_yn1;
    u16 loop_yn2;
} AXPBADPCMLOOP;

typedef struct AXPBLPF {
    u16 on;
    u16 yn1;
    u16 a0;
    u16 b0;
} AXPBLPF;

/* The parameter block the DSP reads for one voice (0xF4 bytes in main memory). */
typedef struct AXPB {
    u16 nextHi, nextLo;           /* 0x00 */
    u16 currHi, currLo;           /* 0x04 */
    u16 srcSelect;                /* 0x08 */
    u16 coefSelect;               /* 0x0A */
    u16 mixerCtrl;                /* 0x0C */
    u16 state;                    /* 0x0E: 1 while the voice runs */
    u16 type;                     /* 0x10 */
    AXPBMIX mix;                  /* 0x12 */
    AXPBITD itd;                  /* 0x36 */
    AXPBUPDATE update;            /* 0x44 */
    AXPBDPOP dpop;                /* 0x52 */
    AXPBVE ve;                    /* 0x64 */
    AXPBFIR fir;                  /* 0x68 */
    AXPBADDR addr;                /* 0x6E */
    AXPBADPCM adpcm;              /* 0x7E */
    AXPBSRC src;                  /* 0xA6 */
    AXPBADPCMLOOP adpcmLoop;      /* 0xB4 */
    AXPBLPF lpf;                  /* 0xBA */
    u16 pad[25];                  /* 0xC2 */
} AXPB;

/* The studio block: the depop ramps for the nine output buses (main, aux A and aux B; left,
 * right and surround). Each ramp is a 32-bit start value split over two halfwords and a delta. */
typedef struct AXSPB {
    u16 dpopLHi, dpopLLo;
    s16 dpopLDelta;
    u16 dpopRHi, dpopRLo;
    s16 dpopRDelta;
    u16 dpopSHi, dpopSLo;
    s16 dpopSDelta;
    u16 dpopALHi, dpopALLo;
    s16 dpopALDelta;
    u16 dpopARHi, dpopARLo;
    s16 dpopARDelta;
    u16 dpopASHi, dpopASLo;
    s16 dpopASDelta;
    u16 dpopBLHi, dpopBLLo;
    s16 dpopBLDelta;
    u16 dpopBRHi, dpopBRLo;
    s16 dpopBRDelta;
    u16 dpopBSHi, dpopBSLo;
    s16 dpopBSDelta;
} AXSPB;

typedef struct AXVPB AXVPB;
typedef void (*AXVoiceCallback)(void *voice);

/* The CPU-side voice record wrapping a parameter block. */
struct AXVPB {
    AXVPB *next;           /* 0x00: priority stack links */
    AXVPB *prev;           /* 0x04 */
    AXVPB *next1;          /* 0x08: callback stack link */
    u32 priority;          /* 0x0C: 0 while free */
    AXVoiceCallback callback; /* 0x10 */
    u32 userContext;       /* 0x14 */
    u32 index;             /* 0x18 */
    u32 sync;              /* 0x1C: dirty flags for the DSP copy */
    u32 depop;             /* 0x20 */
    u32 updateMS;          /* 0x24 */
    u32 updateCounter;     /* 0x28: words queued in updateData */
    u32 updateTotal;       /* 0x2C */
    u16 *updateWrite;      /* 0x30 */
    u16 updateData[128];   /* 0x34 */
    void *itdBuffer;       /* 0x134 */
    AXPB pb;               /* 0x138: the CPU-side copy of the voice's parameter block */
};

/* Timings of one audio frame, recorded by AXOut into the caller's profile ring. */
typedef struct AXPROFILE {
    OSTime axFrameStart;
    OSTime auxProcessingStart;
    OSTime auxProcessingEnd;
    OSTime userCallbackStart;
    OSTime userCallbackEnd;
    OSTime axFrameEnd;
    u32 axNumVoices;
} AXPROFILE;

#ifdef __cplusplus
}
#endif

#endif
