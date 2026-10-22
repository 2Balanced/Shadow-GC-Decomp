#ifndef DOLPHIN_BASE_PPCARCH_H
#define DOLPHIN_BASE_PPCARCH_H

/* Accessors for the Gekko's special-purpose registers (Dolphin SDK base library), and the SPR
 * numbers the inline assembler uses to name them. */

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HID0 1008
#define HID1 1009
#define HID2 920
#define L2CR 1017
#define WPAR 921
#define DMA_U 922
#define DMA_L 923
#define MMCR0 952
#define PMC1 953
#define PMC2 954
#define SIA 955
#define MMCR1 956
#define PMC3 957
#define PMC4 958
#define PVR 287

#define HID0_SPD 0x00000200 /* speculative cache access disable */

/* An FPSCR value as mffs/mtfsf move it: in the low word of a double. */
union FpscrUnion {
    f64 f;
    struct {
        u32 fpscr_pad;
        u32 fpscr;
    } u;
};

u32 PPCMfmsr(void);
void PPCMtmsr(u32 newMSR);
u32 PPCMfhid0(void);
void PPCMthid0(u32 newHID0);
u32 PPCMfl2cr(void);
void PPCMtl2cr(u32 newL2cr);
void PPCMtdec(u32 newDec);
void PPCSync(void);
void PPCHalt(void);
void PPCMtmmcr0(u32 newMmcr0);
void PPCMtmmcr1(u32 newMmcr1);
void PPCMtpmc1(u32 newPmc1);
void PPCMtpmc2(u32 newPmc2);
void PPCMtpmc3(u32 newPmc3);
void PPCMtpmc4(u32 newPmc4);
u32 PPCMffpscr(void);
void PPCMtfpscr(u32 newFPSCR);
u32 PPCMfhid2(void);
void PPCMthid2(u32 newhid2);
void PPCMtwpar(u32 newwpar);
void PPCDisableSpeculation(void);
void PPCSetFpNonIEEEMode(void);

#ifdef __cplusplus
}
#endif

#endif
