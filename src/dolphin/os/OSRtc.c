/* Dolphin SDK OS: the real-time clock chip's battery-backed SRAM and the IPL ROM, both behind
 * EXI channel 0, device 1.
 *
 * The 64 bytes of SRAM are read once at start-up into Scb.sram. Callers lock the cached copy,
 * change it and unlock it with commit = TRUE; the changed tail is then written back (the
 * checksum first, if the first block changed). A write that cannot get the EXI bus right away
 * is finished from the EXI unlock callback. The sound, progressive-scan, language and wireless
 * controller ID accessors keep their placeholder names where game code calls them by those
 * names: fn_80376F58/fn_80376FD8 are OSGetSoundMode/OSSetSoundMode and fn_8037707C/fn_803770EC
 * OSGetProgressiveMode/OSSetProgressiveMode. fn_803B2DB4 is EXIImmEx. */
#include <dolphin/os.h>

#define RTC_CHAN 0
#define RTC_DEV 1
#define RTC_FREQ 3 /* 8 MHz */

#define RTC_CMD_READ 0x20000000
#define RTC_CMD_WRITE 0xA0000000
#define RTC_SRAM_ADDR 0x00000100
#define RTC_SRAM_SIZE 64

#define EXI_READ 0
#define EXI_WRITE 1

#define OS_SOUND_MODE_MONO 0
#define OS_SOUND_MODE_STEREO 1

#define SRAM_FLAG_SOUND 0x04
#define SRAM_FLAG_PROGRESSIVE 0x80

typedef void (*EXICallback)(s32 chan, OSContext *context);

typedef struct OSSram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct OSSramEx {
    u8 flashID[2][12];
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 _padding0;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u8 _padding1[2];
} OSSramEx;

typedef struct SramControlBlock {
    u8 sram[RTC_SRAM_SIZE];
    u32 offset; /* first byte not yet written back */
    BOOL enabled; /* interrupt state saved by the lock */
    BOOL locked;
    BOOL sync;    /* the hardware holds what the cache holds */
    void (*callback)(void);
} SramControlBlock;

BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback);
BOOL EXIUnlock(s32 chan);
BOOL EXISelect(s32 chan, u32 dev, u32 freq);
BOOL EXIDeselect(s32 chan);
BOOL EXIImm(s32 chan, void *buf, s32 len, u32 type, EXICallback callback);
BOOL fn_803B2DB4(s32 chan, void *buf, s32 len, u32 mode);
BOOL EXIDma(s32 chan, void *buf, s32 len, u32 type, EXICallback callback);
BOOL EXISync(s32 chan);

u16 OSGetGbsMode(void);
void OSSetGbsMode(u16 mode);

static SramControlBlock Scb __attribute__((aligned(32)));

static BOOL WriteSram(void *buffer, u32 offset, u32 size);

/* Retries the write-back once the EXI bus is free again. */
static void WriteSramCallback(s32 chan, OSContext *context) {
    Scb.sync = WriteSram(Scb.sram + Scb.offset, Scb.offset, RTC_SRAM_SIZE - Scb.offset);
    if (Scb.sync) {
        Scb.offset = RTC_SRAM_SIZE;
    }
}

static BOOL WriteSram(void *buffer, u32 offset, u32 size) {
    BOOL err;
    u32 cmd;

    if (!EXILock(RTC_CHAN, RTC_DEV, WriteSramCallback)) {
        return FALSE;
    }
    if (!EXISelect(RTC_CHAN, RTC_DEV, RTC_FREQ)) {
        EXIUnlock(RTC_CHAN);
        return FALSE;
    }

    offset <<= 6;
    cmd = RTC_CMD_WRITE | (RTC_SRAM_ADDR + offset);
    err = !EXIImm(RTC_CHAN, &cmd, 4, EXI_WRITE, NULL);
    err |= !EXISync(RTC_CHAN);
    err |= !fn_803B2DB4(RTC_CHAN, buffer, (s32)size, EXI_WRITE);
    err |= !EXIDeselect(RTC_CHAN);
    EXIUnlock(RTC_CHAN);
    return !err;
}

static BOOL ReadSram(void *buffer) {
    BOOL err;
    u32 cmd;

    DCInvalidateRange(buffer, RTC_SRAM_SIZE);
    if (!EXILock(RTC_CHAN, RTC_DEV, NULL)) {
        return FALSE;
    }
    if (!EXISelect(RTC_CHAN, RTC_DEV, RTC_FREQ)) {
        EXIUnlock(RTC_CHAN);
        return FALSE;
    }

    cmd = RTC_CMD_READ | RTC_SRAM_ADDR;
    err = !EXIImm(RTC_CHAN, &cmd, 4, EXI_WRITE, NULL);
    err |= !EXISync(RTC_CHAN);
    err |= !EXIDma(RTC_CHAN, buffer, RTC_SRAM_SIZE, EXI_READ, NULL);
    err |= !EXISync(RTC_CHAN);
    err |= !EXIDeselect(RTC_CHAN);
    EXIUnlock(RTC_CHAN);
    return !err;
}

void __OSInitSram(void) {
    Scb.locked = Scb.enabled = FALSE;
    Scb.sync = ReadSram(Scb.sram);
    Scb.offset = RTC_SRAM_SIZE;
    OSSetGbsMode(OSGetGbsMode());
}

static void *LockSram(u32 offset) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (Scb.locked != FALSE) {
        OSRestoreInterrupts(enabled);
        return NULL;
    }
    Scb.enabled = enabled;
    Scb.locked = TRUE;
    return Scb.sram + offset;
}

OSSram *__OSLockSram(void) {
    return (OSSram *)LockSram(0);
}

OSSramEx *__OSLockSramEx(void) {
    return (OSSramEx *)LockSram(sizeof(OSSram));
}

/* Commits a change: refreshes the checksums when the first block changed and writes back
 * everything from the lowest changed offset on. */
static BOOL UnlockSram(BOOL commit, u32 offset) {
    u16 *p;

    if (commit) {
        if (offset == 0) {
            OSSram *sram = (OSSram *)Scb.sram;

            if ((sram->flags & 3) > 2u) {
                sram->flags &= ~3;
            }
            sram->checkSum = sram->checkSumInv = 0;
            for (p = (u16 *)&sram->counterBias; p < (u16 *)(Scb.sram + sizeof(OSSram)); p++) {
                sram->checkSum += *p;
                sram->checkSumInv += ~*p;
            }
        }

        if (offset < Scb.offset) {
            Scb.offset = offset;
        }

        if (Scb.offset <= 20) {
            OSSramEx *sram = (OSSramEx *)(Scb.sram + sizeof(OSSram));

            if ((sram->gbs & 0x7C00u) == 0x5000u || (sram->gbs & 0xC0u) == 0xC0u) {
                sram->gbs = 0;
            }
        }

        Scb.sync = WriteSram(Scb.sram + Scb.offset, Scb.offset, RTC_SRAM_SIZE - Scb.offset);
        if (Scb.sync) {
            Scb.offset = RTC_SRAM_SIZE;
        }
    }
    Scb.locked = FALSE;
    OSRestoreInterrupts(Scb.enabled);
    return Scb.sync;
}

BOOL __OSUnlockSram(BOOL commit) {
    return UnlockSram(commit, 0);
}

BOOL __OSUnlockSramEx(BOOL commit) {
    return UnlockSram(commit, sizeof(OSSram));
}

BOOL __OSSyncSram(void) {
    return Scb.sync;
}

/* Reads `length` bytes of the IPL ROM (fonts and boot code) from `offset`. */
BOOL __OSReadROM(void *buffer, s32 length, s32 offset) {
    BOOL err;
    u32 cmd;

    DCInvalidateRange(buffer, (u32)length);
    if (!EXILock(RTC_CHAN, RTC_DEV, NULL)) {
        return FALSE;
    }
    if (!EXISelect(RTC_CHAN, RTC_DEV, RTC_FREQ)) {
        EXIUnlock(RTC_CHAN);
        return FALSE;
    }

    cmd = (u32)(offset << 6);
    err = !EXIImm(RTC_CHAN, &cmd, 4, EXI_WRITE, NULL);
    err |= !EXISync(RTC_CHAN);
    err |= !EXIDma(RTC_CHAN, buffer, length, EXI_READ, NULL);
    err |= !EXISync(RTC_CHAN);
    err |= !EXIDeselect(RTC_CHAN);
    EXIUnlock(RTC_CHAN);
    return !err;
}

u32 fn_80376F58(void) {
    OSSram *sram;
    u32 mode;

    sram = __OSLockSram();
    mode = (sram->flags & SRAM_FLAG_SOUND) ? OS_SOUND_MODE_STEREO : OS_SOUND_MODE_MONO;
    __OSUnlockSram(FALSE);
    return mode;
}

void fn_80376FD8(u32 mode) {
    OSSram *sram;
    u32 unused[2]; /* stack the original reserves but never touches */

    mode <<= 2;
    mode &= SRAM_FLAG_SOUND;

    sram = __OSLockSram();
    if (mode == (sram->flags & SRAM_FLAG_SOUND)) {
        __OSUnlockSram(FALSE);
        return;
    }
    sram->flags &= ~SRAM_FLAG_SOUND;
    sram->flags |= mode;
    __OSUnlockSram(TRUE);
}

u32 fn_8037707C(void) {
    OSSram *sram;
    u32 on;

    sram = __OSLockSram();
    on = (sram->flags & SRAM_FLAG_PROGRESSIVE) >> 7;
    __OSUnlockSram(FALSE);
    return on;
}

void fn_803770EC(u32 on) {
    OSSram *sram;
    u32 unused[2]; /* stack the original reserves but never touches */

    on <<= 7;
    on &= SRAM_FLAG_PROGRESSIVE;

    sram = __OSLockSram();
    if (on == (sram->flags & SRAM_FLAG_PROGRESSIVE)) {
        __OSUnlockSram(FALSE);
        return;
    }
    sram->flags &= ~SRAM_FLAG_PROGRESSIVE;
    sram->flags |= on;
    __OSUnlockSram(TRUE);
}

u8 OSGetLanguage(void) {
    OSSram *sram;
    u8 language;

    sram = __OSLockSram();
    language = sram->language;
    __OSUnlockSram(FALSE);
    return language;
}

u16 OSGetWirelessID(s32 chan) {
    OSSramEx *sram;
    u16 id;

    sram = __OSLockSramEx();
    id = sram->wirelessPadID[chan];
    __OSUnlockSramEx(FALSE);
    return id;
}

void OSSetWirelessID(s32 chan, u16 id) {
    OSSramEx *sram;

    sram = __OSLockSramEx();
    if (sram->wirelessPadID[chan] != id) {
        sram->wirelessPadID[chan] = id;
        __OSUnlockSramEx(TRUE);
        return;
    }
    __OSUnlockSramEx(FALSE);
}

u16 OSGetGbsMode(void) {
    OSSramEx *sram;
    u16 gbs;

    sram = __OSLockSramEx();
    gbs = sram->gbs;
    __OSUnlockSramEx(FALSE);
    return gbs;
}

/* Invalid GameBoy Player settings are stored as 0. */
void OSSetGbsMode(u16 mode) {
    OSSramEx *sram;
    u32 unused[2]; /* stack the original reserves but never touches */

    if ((mode & 0x7C00u) == 0x5000u || (mode & 0xC0u) == 0xC0u) {
        mode = 0;
    }

    sram = __OSLockSramEx();
    if (mode == sram->gbs) {
        __OSUnlockSramEx(FALSE);
        return;
    }
    sram->gbs = mode;
    __OSUnlockSramEx(TRUE);
}
