#ifndef DOLPHIN_GX_GXPRIVATE_H
#define DOLPHIN_GX_GXPRIVATE_H

/* State shared by the GX units: the shadow copies of the graphics processor's registers that GX
 * keeps in main memory, the register-field helpers and the write-gather FIFO. Field offsets are
 * fixed by the binary; the names are ours. */

#include <dolphin/gx.h>
#include <dolphin/os.h>
#include <dolphin/base/PPCArch.h>

/* Everything written to this address goes through the write-gather pipe into the CPU FIFO. */
volatile union {
    u8 u8;
    s8 s8;
    u16 u16;
    s16 s16;
    u32 u32;
    s32 s32;
    f32 f32;
} GXWGFifo : 0xCC008000;

#define GX_WRITE_U8(v) (GXWGFifo.u8 = (u8)(v))
#define GX_WRITE_U16(v) (GXWGFifo.u16 = (u16)(v))
#define GX_WRITE_U32(v) (GXWGFifo.u32 = (u32)(v))
#define GX_WRITE_F32(v) (GXWGFifo.f32 = (f32)(v))

/* Command-processor, transform-unit and raster (BP) register loads. */
#define GX_WRITE_CP_REG(addr, v)                                                                   \
    do {                                                                                           \
        GX_WRITE_U8(0x08);                                                                         \
        GX_WRITE_U8(addr);                                                                         \
        GX_WRITE_U32(v);                                                                           \
    } while (0)

#define GX_WRITE_XF_REG(addr, v)                                                                   \
    do {                                                                                           \
        GX_WRITE_U8(0x10);                                                                         \
        GX_WRITE_U32(0x1000 + (addr));                                                             \
        GX_WRITE_U32(v);                                                                           \
    } while (0)

#define GX_WRITE_RAS_REG(v)                                                                        \
    do {                                                                                           \
        GX_WRITE_U8(0x61);                                                                         \
        GX_WRITE_U32(v);                                                                           \
    } while (0)

/* Replaces `size` bits of `reg` starting at bit `shift` (bit 0 = least significant). The SDK
 * inserts fields with the rlwimi intrinsic, which keeps constant values in registers. */
#define SET_REG_FIELD(reg, size, shift, val)                                                           ((reg) = __rlwimi((reg), (val), (shift), 32 - (shift) - (size), 31 - (shift)))

#define GET_REG_FIELD(reg, size, shift) (((reg) >> (shift)) & ((1 << (size)) - 1))

/* Dirty bits in GXData.dirtyState: state that GXBegin flushes before drawing. */
#define GX_DIRTY_SU_TEX 0x0001
#define GX_DIRTY_BP_MASK 0x0002
#define GX_DIRTY_GEN_MODE 0x0004
#define GX_DIRTY_VCD 0x0008
#define GX_DIRTY_VAT 0x0010

typedef struct GXData {
    u16 vNumNot;          /* 0x000 */
    u16 bpSentNot;        /* 0x002 */
    u16 vNum;             /* 0x004 */
    u16 vLim;             /* 0x006 */
    u32 cpEnable;         /* 0x008 */
    u32 cpStatus;         /* 0x00C */
    u32 cpClr;            /* 0x010 */
    u32 vcdLo;            /* 0x014 */
    u32 vcdHi;            /* 0x018 */
    u32 vatA[8];          /* 0x01C */
    u32 vatB[8];          /* 0x03C */
    u32 vatC[8];          /* 0x05C */
    u32 lpSize;           /* 0x07C */
    u32 matIdxA;          /* 0x080 */
    u32 matIdxB;          /* 0x084 */
    u32 indexBase[4];     /* 0x088 */
    u32 indexStride[4];   /* 0x098 */
    u32 ambColor[2];      /* 0x0A8 */
    u32 matColor[2];      /* 0x0B0 */
    u32 suTs0[8];         /* 0x0B8 */
    u32 suTs1[8];         /* 0x0D8 */
    u32 suScis0;          /* 0x0F8 */
    u32 suScis1;          /* 0x0FC */
    u32 tref[8];          /* 0x100 */
    u32 iref;             /* 0x120 */
    u32 bpMask;           /* 0x124 */
    u32 IndTexScale0;     /* 0x128 */
    u32 IndTexScale1;     /* 0x12C */
    u32 tevc[16];         /* 0x130 */
    u32 teva[16];         /* 0x170 */
    u32 tevKsel[8];       /* 0x1B0 */
    u32 cmode0;           /* 0x1D0 */
    u32 cmode1;           /* 0x1D4 */
    u32 zmode;            /* 0x1D8 */
    u32 peCtrl;           /* 0x1DC */
    u32 cpDispSrc;        /* 0x1E0 */
    u32 cpDispSize;       /* 0x1E4 */
    u32 cpDispStride;     /* 0x1E8 */
    u32 cpDisp;           /* 0x1EC */
    u32 cpTexSrc;         /* 0x1F0 */
    u32 cpTexSize;        /* 0x1F4 */
    u32 cpTexStride;      /* 0x1F8 */
    u32 cpTex;            /* 0x1FC */
    GXBool cpTexZ;        /* 0x200 */
    u32 genMode;          /* 0x204 */
    GXTexRegion TexRegions0[8];  /* 0x208 */
    GXTexRegion TexRegions1[8];  /* 0x288 */
    GXTexRegion TexRegions2[8];  /* 0x308 */
    GXTlutRegion TlutRegions[20]; /* 0x388 */
    GXTexRegionCallback texRegionCallback;   /* 0x4C8 */
    GXTlutRegionCallback tlutRegionCallback; /* 0x4CC */
    GXAttrType nrmType;   /* 0x4D0 */
    GXBool hasNrms;       /* 0x4D4 */
    GXBool hasBiNrms;     /* 0x4D5 */
    u32 projType;         /* 0x4D8 */
    f32 projMtx[6];       /* 0x4DC */
    f32 vpLeft;           /* 0x4F4 */
    f32 vpTop;            /* 0x4F8 */
    f32 vpWd;             /* 0x4FC */
    f32 vpHt;             /* 0x500 */
    f32 vpNearz;          /* 0x504 */
    f32 vpFarz;           /* 0x508 */
    f32 zOffset;          /* 0x50C */
    f32 zScale;           /* 0x510 */
    u32 tImage0[8];       /* 0x514 */
    u32 tMode0[8];        /* 0x534 */
    u32 texmapId[16];     /* 0x554 */
    u32 tcsManEnab;       /* 0x594 */
    u32 tevTcEnab;        /* 0x598 */
    GXPerf0 perf0;        /* 0x59C */
    GXPerf1 perf1;        /* 0x5A0 */
    u32 perfSel;          /* 0x5A4 */
    GXBool inDispList;    /* 0x5A8 */
    GXBool dlSaveContext; /* 0x5A9 */
    GXBool abtWaitPECopy; /* 0x5AA */
    u8 dirtyVAT;          /* 0x5AB */
    u32 dirtyState;       /* 0x5AC */
} GXData;

extern GXData *const __GXData;
#define gx __GXData

/* Register blocks, mapped by GXInit. */
extern vu32 *__piReg;
extern vu16 *__cpReg;
extern vu16 *__peReg;
extern vu16 *__memReg;

/* Register access by index; the address is formed in an integer, as the SDK's macros do. */
#define GX_GET_MEM_REG(i) (*(vu16 *)((u32)__memReg + (i) * 2))

#define GX_GET_CP_REG_U32(lo, hi) (((u32)__cpReg[hi] << 16) | __cpReg[lo])

/* MEM-unit counter of pixel-engine requests, read as two halves until the high half is stable. */
#define MEM_PE_REQ_HI 39
#define MEM_PE_REQ_LO 40

/* Reads a 32-bit MEM-unit counter as two halves, retrying until the high half is stable. */
static inline u32 __GXReadMEMCounterU32(u32 regAddrL, u32 regAddrH) {
    u32 hi, hiOld, lo;

    hi = GX_GET_MEM_REG(regAddrH);
    do {
        hiOld = hi;
        hi = GX_GET_MEM_REG(regAddrH);
        lo = GX_GET_MEM_REG(regAddrL);
    } while (hi != hiOld);
    return (hi << 16) | lo;
}

/* Internal objects behind the public opaque types. */
typedef struct GXFifoObjPriv {
    u8 *base;     /* 0x00 */
    u8 *top;      /* 0x04 */
    u32 size;     /* 0x08 */
    u32 hiWatermark; /* 0x0C */
    u32 loWatermark; /* 0x10 */
    void *rdPtr;  /* 0x14 */
    void *wrPtr;  /* 0x18 */
    s32 count;    /* 0x1C */
    GXBool bind_cpu; /* 0x20 */
    GXBool bind_gp;  /* 0x21 */
} GXFifoObjPriv;

/* OS services GX uses that os.h does not declare. */

void OSInitThreadQueue(OSThreadQueue *queue);
OSThread *OSGetCurrentThread(void);
s32 OSResumeThread(OSThread *thread);
s32 OSSuspendThread(OSThread *thread);
void OSSleepThread(OSThreadQueue *queue);
void OSWakeupThread(OSThreadQueue *queue);
typedef s16 __GXOSInterrupt;
typedef void (*__GXOSInterruptHandler)(__GXOSInterrupt interrupt, OSContext *context);
__GXOSInterruptHandler __OSSetInterruptHandler(__GXOSInterrupt interrupt,
                                               __GXOSInterruptHandler handler);
u32 __OSUnmaskInterrupts(u32 mask);
void OSRegisterVersion(const char *id);
u32 VIGetTvFormat(void);

/* Bus clock in Hz, set by the boot code. */
#define __OSBusClock (*(u32 *)OSPhysicalToCached(0x00F8))

/* GXInit.c */
void __GXInitGX(void);
void __GXInitRevisionBits(void);

/* GXFifo.c */
void __GXFifoInit(void);
void __GXFifoReadEnable(void);
void __GXFifoReadDisable(void);
void __GXFifoLink(u8 link);
void __GXWriteFifoIntEnable(u8 hiWatermarkEn, u8 loWatermarkEn);
void __GXWriteFifoIntReset(u8 hiWatermarkClr, u8 loWatermarkClr);
void __GXCleanGPFifo(void);

/* GXAttr.c */
void __GXSetVCD(void);
void __GXSetVAT(void);
void __GXCalculateVLim(void);

/* GXMisc.c */
void __GXAbort(void);
void __GXPEInit(void);

/* GXGeometry.c */
void __GXSetDirtyState(void);
void __GXSendFlushPrim(void);
void __GXSetGenMode(void);

/* GXTexture.c */
void __GetImageTileCount(GXTexFmt fmt, u16 wd, u16 ht, u32 *rowTiles, u32 *colTiles, u32 *cmpTiles);
void __GXSetSUTexRegs(void);
void __GXSetTmemConfig(u32 config);

/* GXBump.c */
void __GXUpdateBPMask(void);
void __GXSetIndirectMask(u32 mask);
void __GXFlushTextureState(void);

/* GXTransform.c */
void __GXSetMatrixIndex(GXAttr matIdxAttr);

#endif
