/* Dolphin SDK OS: bringing the audio DSP up and down.
 *
 * __OSInitAudioSystem runs a short boot program on the DSP. The program is placed at the very start
 * of main RAM (whatever lived there is parked just below the arena top and put back at the end),
 * pulled in by two ARAM DMA transfers, and then expected to post its "ready" mail before the DSP is
 * halted and reset again. __OSStopAudioSystem quiesces audio DMA and resets the DSP ahead of a
 * system reset. */
#include <dolphin/os.h>

void *memcpy(void *dst, const void *src, unsigned long n);
void *OSGetArenaHi(void);

volatile u16 __DSPRegs[] : 0xCC005000;

/* Register views into the DSP interface block. */
#define DSP_CPU_MAIL_HI __DSPRegs[0]
#define DSP_MAIL_HI     __DSPRegs[2]
#define DSP_MAIL_LO     __DSPRegs[3]
#define DSP_CSR         __DSPRegs[5]
#define AR_SIZE_CFG     __DSPRegs[9]
#define AR_DMA_MMADDR   (*(u32 *)&__DSPRegs[16])
#define AR_DMA_ARADDR   (*(u32 *)&__DSPRegs[18])
#define AR_DMA_CNT      (*(u32 *)&__DSPRegs[20])
#define AUDIO_DMA_LEN   __DSPRegs[27]

/* DSP_CSR bits. */
#define CSR_RESET     0x0001
#define CSR_HALT      0x0004
#define CSR_ARINT     0x0020
#define CSR_DSPDMA    0x0200
#define CSR_INIT_BUSY 0x0400
#define CSR_BOOT_MODE 0x0800
#define CSR_START     0x08AC /* boot mode, interrupt acks, halted */
#define CSR_STOP      0x0804

#define AUDIO_DMA_ENABLE 0x8000
#define MAIL_PENDING     0x8000
#define DSP_READY_MAIL   0x80544348 /* the boot program's greeting */

/* Boot program, one 16-bit DSP instruction word per entry. */
static u16 DSPInitCode[64] = {
    0x029F, 0x0010, 0x029F, 0x0033, 0x029F, 0x0034, 0x029F, 0x0035,
    0x029F, 0x0036, 0x029F, 0x0037, 0x029F, 0x0038, 0x029F, 0x0039,
    0x1206, 0x1203, 0x1204, 0x1205, 0x0080, 0x8000, 0x0088, 0xFFFF,
    0x0084, 0x1000, 0x0064, 0x001D, 0x0218, 0x0000, 0x8100, 0x1C1E,
    0x0044, 0x1B1E, 0x0084, 0x0800, 0x0064, 0x0027, 0x191E, 0x0000,
    0x00DE, 0xFFFC, 0x02A0, 0x8000, 0x029C, 0x0028, 0x16FC, 0x0054,
    0x16FD, 0x4348, 0x0021, 0x02FF, 0x02FF, 0x02FF, 0x02FF, 0x02FF,
    0x02FF, 0x02FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

#define BOOT_AREA   ((void *)0x81000000)
#define SAVE_AREA() ((void *)((u32)OSGetArenaHi() - sizeof(DSPInitCode)))

/* Queue one ARAM DMA of the boot area into the DSP and wait for its completion interrupt. */
#define AR_DMA_BOOT_AREA(status)                                   \
    AR_DMA_MMADDR = 0x1000000;                                     \
    AR_DMA_ARADDR = 0;                                             \
    AR_DMA_CNT = 0x20;                                             \
    for (status = DSP_CSR; !(status & CSR_ARINT); status = DSP_CSR) {} \
    DSP_CSR = status

#define DSP_MAIL_FULL() (((DSP_MAIL_HI << 16) | DSP_MAIL_LO) & 0x80000000)

void __OSInitAudioSystem(void) {
    u8 failed;
    u16 status;
    u32 t0;

    memcpy(SAVE_AREA(), BOOT_AREA, sizeof(DSPInitCode));
    memcpy(BOOT_AREA, (void *)DSPInitCode, sizeof(DSPInitCode));
    DCFlushRange(BOOT_AREA, sizeof(DSPInitCode));

    AR_SIZE_CFG = 0x43;
    DSP_CSR = CSR_START;
    DSP_CSR |= CSR_RESET;
    while (DSP_CSR & CSR_RESET) {}
    DSP_CPU_MAIL_HI = 0;
    while (DSP_MAIL_FULL()) {}

    AR_DMA_BOOT_AREA(status);
    for (t0 = OSGetTick(); (s32)(OSGetTick() - t0) < 0x892;) {}
    AR_DMA_BOOT_AREA(status);

    DSP_CSR &= ~CSR_BOOT_MODE;
    while (DSP_CSR & CSR_INIT_BUSY) {}
    DSP_CSR &= ~CSR_HALT;
    failed = 0;

    for (status = DSP_MAIL_HI; !(status & MAIL_PENDING); status = DSP_MAIL_HI) {}

    /* Only debug builds check the greeting; the retail code keeps just the read of the low half. */
    (u32)((status << 16) | DSP_MAIL_LO) != DSP_READY_MAIL;

    DSP_CSR |= CSR_HALT;
    DSP_CSR = CSR_START;
    DSP_CSR |= CSR_RESET;
    while (DSP_CSR & CSR_RESET) {}

    memcpy(BOOT_AREA, SAVE_AREA(), sizeof(DSPInitCode));
}

void __OSStopAudioSystem(void) {
    u16 status;
    u32 t0;

    DSP_CSR = CSR_STOP;
    status = AUDIO_DMA_LEN;
    AUDIO_DMA_LEN = status & ~AUDIO_DMA_ENABLE;

    for (status = DSP_CSR; status & CSR_INIT_BUSY; status = DSP_CSR) {}
    for (status = DSP_CSR; status & CSR_DSPDMA; status = DSP_CSR) {}

    DSP_CSR = CSR_START;
    DSP_CPU_MAIL_HI = 0;
    while (DSP_MAIL_FULL()) {}

    for (t0 = OSGetTick(); (s32)(OSGetTick() - t0) < 0x2C;) {}

    status = DSP_CSR;
    DSP_CSR = status | CSR_RESET;
    for (status = DSP_CSR; status & CSR_RESET; status = DSP_CSR) {}
}
