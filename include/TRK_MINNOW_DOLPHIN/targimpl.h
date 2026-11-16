#ifndef TRK_MINNOW_DOLPHIN_TARGIMPL_H
#define TRK_MINNOW_DOLPHIN_TARGIMPL_H

/* The nub's picture of the target processor (a PowerPC 750 class CPU). */

#include <TRK_MINNOW_DOLPHIN/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The registers of the stopped target, in the four blocks the debugger protocol reads and
 * writes. */
typedef struct TRKCPUState {
    struct {
        u32 gpr[32];        /* 0x000 */
        u32 pc;             /* 0x080 */
        u32 lr;             /* 0x084 */
        u32 cr;             /* 0x088 */
        u32 ctr;            /* 0x08C */
        u32 xer;            /* 0x090 */
    } general;              /* 0x000, 0x25 registers */
    struct {
        u64 fpr[32];        /* 0x098 */
        u64 fpscr;          /* 0x198 */
        u64 fpecr;          /* 0x1A0 */
    } fp;                   /* 0x098, 0x22 registers */
    u32 extended1[0x61];    /* 0x1A8, the supervisor registers (TRK_EXT1_*) and the exception */
    u32 pad32C;
    u64 extended2[0x20];    /* 0x330, the paired-single halves */
} TRKCPUState;              /* 0x430 */

/* Indices into TRKCPUState.extended1 that the nub itself uses. */
#define TRK_EXT1_MSR 20         /* 0x1F8 */
#define TRK_EXT1_TBL 16         /* 0x1E8 */
#define TRK_EXT1_TBU 17         /* 0x1EC */
#define TRK_EXT1_DBAT3U 36      /* 0x238 */
#define TRK_EXT1_HID2 52        /* 0x278 */
#define TRK_EXT1_EXCEPTION 84   /* 0x2F8, the exception number in the low half */

/* What the nub saves of itself while the target runs (TRKSwapAndGo), and its run state. */
typedef struct TRKState {
    u32 gpr[32];            /* 0x00 */
    u32 lr;                 /* 0x80 */
    u32 ctr;                /* 0x84 */
    u32 xer;                /* 0x88 */
    u32 msr;                /* 0x8C */
    u32 dar;                /* 0x90 */
    u32 dsisr;              /* 0x94 */
    BOOL stopped;           /* 0x98 */
    BOOL inputActivated;    /* 0x9C */
    void *inputPendingPtr;  /* 0xA0 */
} TRKState;                 /* 0xA4 */

extern TRKCPUState gTRKCPUState;
extern TRKState gTRKState;

/* Which registers TRKRestoreExtended1Block must write back: the time base and HID2 are only
 * restored when the debugger changed them. */
typedef struct TRKRestoreFlags {
    u8 timeBase;    /* 0x0 */
    u8 hid2;        /* 0x1 */
    u8 unused[7];
} TRKRestoreFlags;  /* 0x9 */

extern TRKRestoreFlags gTRKRestoreFlags;

u32 __TRK_get_MSR(void);
void __TRK_set_MSR(u32 msr);
void TRKSaveExtended1Block(void);
void TRKRestoreExtended1Block(void);
void TRKSwapAndGo(void);
void TRKInterruptHandler(void);

/* The interrupt vector code the nub installs over the low-memory vectors (assembly, in .init). */
extern u8 gTRKInterruptVectorTable[];

void __TRK_write_aram(void *data, u32 address, TRKSize *length);
void __TRK_read_aram(void *data, u32 address, TRKSize *length);

#ifdef __cplusplus
}
#endif

#endif
