/* AX SDLIB, the game's GameCube AX sound library (see include/Game/rw/r1.h), 0x8043E2B4..0x8043FB58:
 * the library's core (the aux effects, the AX frame callback, the 5 ms tick, start-up and
 * shut-down, with the library's three globals in .sbss and its two start-up messages in .rodata)
 * and the API the game's AudioGCN calls (each entry checks that the library is up and that a
 * player or loader handle is valid). The file name is descriptive. Its .sdata2 pool
 * (0x805FAF40..0x805FAF68) holds the int-to-float constant 0x4330000080000000 that the next file's
 * pool (0x805FAF80) has again, so the sequencer after 0x8043FB58 is another file; the cut between
 * the two is placed where the API entries end (the functions after it are only called by them).
 * C with the game's flags: definitions in reverse address order.
 * Function and member names are inferred from behavior. fn_8043E6F4 matches with its bank loop in
 * an inline function (NewAramBanks: retail copies the zeroed counter), fn_8043E9C4 with the
 * choice of memory in an inline function that returns early (TakeMemory).
 * NonMatching: fn_8043F70C (22 rows: the loader, command, count, base and result registers rotate;
 * about 25 declaration orders and statement shapes tried, probes/Game_gcaxSnd_R4.py, _R5.py). */
#include <dolphin/types.h>
#include "size_assert.h"
#include "Game/rw/r1.h"

/* The library's globals. C lays out tentative definitions in reverse order. */
int lbl_805F1408; /* the memory was the caller's */
GcaxGlobals *lbl_805F1404;
u32 lbl_805F1400; /* the library's memory */

extern u32 __OSCurrHeap;

void *__va_arg(void *, int);
typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_arg(ap, t) (*((t *)__va_arg(ap, 1)))
#define va_end(ap) ((void)0)

void fn_80384978(int);
void OSEnableInterrupts(void);
s64 OSGetTime(void);
void *OSAllocFromHeap(u32, u32);
void OSFreeToHeap(u32, u32);
void fn_8043B5FC(GcaxListNode **);
int fn_8043BB18(void);
float fn_8043D318(int);
int fn_8043D49C(void);
void fn_8043D61C(void);
void fn_8043D7B4(GcaxPlayer *, float, float);
void fn_8043D870(GcaxPlayer *);
void fn_8043D8E0(GcaxPlayer *, float, float);
void fn_8043D99C(GcaxPlayer *);
void fn_8043DA0C(GcaxPlayer *);
void fn_8043DAB0(GcaxPlayer *);
void fn_8043DD14(GcaxPlayer *);
void fn_8043E004(GcaxPlayer *, u32);
void fn_8043E088(GcaxPlayer *);
GcaxPlayer *fn_8043E0E8(void);
void fn_8043E228(GcaxPlayer *, int);
void fn_8043E2B4(int, int);
void fn_8043E47C(int);
void fn_8043E494(void);
void fn_8043E624(void);
u32 fn_8043E6F4(u32, u32, u32, u32);
void fn_8043E950(void);
void fn_8043E9C4(u32, u32);
int fn_8043F13C(GcaxPlayer *);
int fn_8043F5AC(void);
int fn_8043F60C(GcaxPlayer *);
void fn_8043FB58(GcaxPlayer *);
void fn_8043FC98(GcaxPlayer *);
int fn_8043FD70(GcaxPlayer *);
int fn_8043FDF8(GcaxPlayer *, int, u32, u32);
void fn_8043FF5C(GcaxPlayer *);
void fn_8043AB6C(void *, int, u32, void *, void *);
void *fn_8043AD94(u32);
void fn_8043ACE8(void *);
void fn_8043AEBC(u32, int);
int fn_8043AF64(int, GcaxChunk *, GcaxCallback, void *, int);
int fn_8043B05C(int, void *, GcaxCallback, void *, int);
void fn_8043B1AC(int, u32);
void fn_8043B25C(void);
void fn_80381AE0(void *);
u32 fn_80381A78(u32);
void fn_80384244(void (*)(void), void *);
void fn_80384250(void (*)(void), void *);
void fn_803876D8(GcaxReverbHi *);
void fn_80387748(GcaxReverbHi *);
void fn_80387794(void);
void fn_803881EC(GcaxReverbStd *);
void fn_80388258(GcaxReverbStd *);
void fn_803882A4(void);
void fn_80388658(GcaxChorus *);
void fn_803888A0(GcaxChorus *);
void fn_80388A00(void);
void fn_80388D50(void);
void fn_803891E8(GcaxDelay *);
void fn_80389230(GcaxDelay *);
void fn_80389320(void *(*)(u32), void (*)(void *));
void fn_8038A3EC(void);

int fn_8043FB24(void *heap, int arg1, int arg2) {
    fn_8043E6F4((u32)heap, 0x30, 0xE1000, 0x800000);
    return 0;
}

/* Selects stereo (0) or mono (1) output. */
int fn_8043FAB0(int mode) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    switch (mode) {
    case 0:
        fn_8043E47C(1);
        break;
    case 1:
        fn_8043E47C(0);
        break;
    default:
        return 0x3000001;
    }
    return 0;
}

/* Takes a free loader handle. */
int fn_8043FA20(GcaxLoader **out) {
    u8 *object;
    u32 i;
    if (lbl_805F1404 == NULL) return 0x0D000001;
    object = (u8 *)lbl_805F1404 + 0x13F6C;
    for (i = 0; i < 0x40; i++) {
        if (*object == 0) goto found;
        object += 0x14;
    }
    return 0x07000101;
found:
    memset(object, 0, 0x14);
    *object = 1;
    *out = (GcaxLoader *)object;
    return 0;
}

/* Sets up a loader for a 'gcax' 'MLT ' file. */
int fn_8043F980(GcaxLoader *loader, u32 *file, u32 arg2, GcaxCallback callback, void *data) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckLoader(loader);
    if (result != 0) {
        return result;
    }
    loader->m_file = file;
    loader->m_request = -1;
    loader->field_0x8 = arg2;
    loader->m_callback = callback;
    loader->m_callbackData = data;
    return 0;
}

/* Waits for the loader's ARAM request and frees the handle. */
int fn_8043F8C4(GcaxLoader *loader) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckLoader(loader);
    if (result != 0) {
        return result;
    }
    if (loader->m_used != 0 && loader->m_request >= 0) {
        u8 busy = *(u8 *)&lbl_805F1404->m_requestBusy[loader->m_request];
        while (busy != 0) {
        }
    }
    loader->m_used = 0;
    return 0;
}

/* Loads the sequence and program banks a 'gcax' 'MLT ' file lists. */
int fn_8043F70C(GcaxLoader *loader) {
    int error;
    u32 *file;
    u32 *command;
    u32 *base;
    int result;
    int count;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    error = GcaxCheckLoader(loader);
    if (error != 0) {
        return error;
    }
    file = loader->m_file;
    command = file + 8;
    base = command;
    if (file[0] != 0x67636178 || file[1] != 0x4D4C5420 || file[4] != 0x67636178 ||
        file[5] != 0x4D4C544D) {
        return 0x9000001;
    }
    result = 0;
    count = file[7] >> 4;
    while (count-- > 0) {
        int status;
        int bank;
        if (command[1] == 0xFFFFFFFF) {
            continue;
        }
        status = -1;
        bank = (command[1] >> 24) & 0xF;
        switch ((command[0] >> 24) & 0xF) {
        case 1:
            status = fn_8043B05C(bank, (u8 *)base + command[2], loader->m_callback,
                                 loader->m_callbackData, lbl_805F1404->m_loaderWait);
            break;
        case 4:
            status = fn_8043AF64(bank, (GcaxChunk *)((u8 *)base + command[2]), loader->m_callback,
                                 loader->m_callbackData, lbl_805F1404->m_loaderWait);
            break;
        }
        if (status < 0) {
            result = 0x9000101;
            break;
        }
        command += 4;
    }
    return result;
}

/* Takes a free player. */
int fn_8043F6AC(GcaxPlayer **out) {
    GcaxPlayer *player;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    player = fn_8043E0E8();
    if (player == NULL) {
        return 0x7000101;
    }
    *out = player;
    return 0;
}

/* Frees a player. */
int fn_8043F60C(GcaxPlayer *player) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043E088(player);
    return 0;
}

/* Frees every player (the loop runs over 64 slots although there are eight players). */
int fn_8043F5AC(void) {
    int i;
    GcaxPlayer *player;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    player = lbl_805F1404->m_players;
    for (i = 0; i < 64; i++) {
        fn_8043F60C(player);
        player++;
    }
    return 0;
}

/* Packs a MIDI message: the status byte and up to two data bytes from the arguments. */
int fn_8043F41C(GcaxPlayer *player, u8 *out, u32 status, ...) {
    int result;
    va_list args;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    va_start(args, status);
    out[0] = status;
    out[1] = va_arg(args, u32);
    switch (status & 0xF0) {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xD0:
    case 0xE0:
        out[2] = va_arg(args, u32);
        break;
    default:
        out[2] = 0;
        break;
    }
    out[3] = 0;
    va_end(args);
    return 0;
}

/* Queues a packed MIDI message for the player. */
int fn_8043F35C(GcaxPlayer *player, u8 *message, u8 flags) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043E004(player, (message[0] << 24) | (message[1] << 16) | (message[2] << 8) | flags);
    return 0;
}

/* Runs one tick by hand. */
int fn_8043F2F0(void) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    GcaxLock();
    fn_8043E494();
    GcaxUnlock();
    return 0;
}

int fn_8043F284(void) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    GcaxLock();
    fn_8043E494();
    GcaxUnlock();
    return 0;
}

int fn_8043F240(void) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    fn_8043E2B4(0, 2);
    return 0;
}

int fn_8043F1FC(void) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    fn_8043E2B4(0, 0);
    return 0;
}

/* Resets the controllers of the player's 16 channels. */
int fn_8043F13C(GcaxPlayer *player) {
    int result;
    int i;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    for (i = 0; i < 16; i++) {
        fn_8043E228(player, i);
    }
    return 0;
}

int fn_8043F0D4(void) {
    int i;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    for (i = 0; i < 8; i++) {
        fn_8043F13C(&lbl_805F1404->m_players[i]);
    }
    return 0;
}

/* Starts sequence `id` of sequence bank `bank` on the player. */
int fn_8043EFF0(GcaxPlayer *player, s8 bank, s8 id, s8 flags) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    if (bank < 0 || bank > 0x1F) {
        return 0x3000001;
    }
    if (id < 0 || id > 0x7F) {
        return 0x3000001;
    }
    fn_8043FDF8(player, bank, id, flags);
    return 0;
}

int fn_8043EF50(GcaxPlayer *player) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043FD70(player);
    return 0;
}

u32 fn_8043EF48(void) {
    return 0;
}

int fn_8043EE74(void *handle, s16 value, int duration) {
    GcaxPlayer *player = (GcaxPlayer *)handle;
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043D7B4(player, fn_8043D318(value), (float)duration);
    return 0;
}

int fn_8043ED7C(GcaxPlayer *player, s8 value, int duration) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    if (value < -0x7F || value > 0x7F) {
        return 0x3000001;
    }
    fn_8043D8E0(player, (float)value / 127.0f, (float)duration);
    return 0;
}

int fn_8043ECDC(GcaxPlayer *player) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043FC98(player);
    return 0;
}

int fn_8043EC3C(GcaxPlayer *player) {
    int result;
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043FB58(player);
    return 0;
}

u32 fn_8043EBFC(void) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    fn_8043F5AC();
    fn_8043E624();
    return 0;
}

static inline u32 TakeMemory(u32 memory, u32 size) {
    if (memory != 0) {
        lbl_805F1408 = 1;
        return memory;
    }
    return (u32)OSAllocFromHeap(__OSCurrHeap, size);
}

/* Takes the library's memory (allocated from the current heap when `memory` is NULL) and sets
 * the effects' default parameters. */
void fn_8043E9C4(u32 memory, u32 size) {
    u32 interrupt = OSDisableInterrupts();
    u32 heap;
    memory = TakeMemory(memory, size);
    if (memory == 0) {
        OSReport("AX SDLIB: Panic: cannot allocate memory (%d)\n", size);
        while (1) {
        }
    }
    OSReport("AX SDLIB: allocated (%d)\n", size);
    lbl_805F1400 = memory;
    lbl_805F1404 = (GcaxGlobals *)((memory + 0x1F) & 0xFFFFFFE0);
    lbl_805F1404->m_initialized = 0;
    lbl_805F1404->field_0x13A38 = 0;
    OSRestoreInterrupts(interrupt);
    memset(lbl_805F1404, 0, sizeof(GcaxGlobals));
    heap = ((u32)lbl_805F1404 + sizeof(GcaxGlobals) + 0x1F) & 0xFFFFFFE0;
    fn_8043AEBC(heap, size - (heap - memory));
    lbl_805F1404->m_reverbStd.m_tempDisableFX = 0;
    lbl_805F1404->m_reverbStd.m_time = 3.0f;
    lbl_805F1404->m_reverbStd.m_preDelay = 0.1f;
    lbl_805F1404->m_reverbStd.m_damping = 0.5f;
    lbl_805F1404->m_reverbStd.m_coloration = 0.5f;
    lbl_805F1404->m_reverbStd.m_mix = 0.5f;
    lbl_805F1404->m_reverbHi.m_tempDisableFX = 0;
    lbl_805F1404->m_reverbHi.m_time = 3.0f;
    lbl_805F1404->m_reverbHi.m_preDelay = 0.1f;
    lbl_805F1404->m_reverbHi.m_damping = 0.5f;
    lbl_805F1404->m_reverbHi.m_coloration = 0.5f;
    lbl_805F1404->m_reverbHi.m_crosstalk = 0.3f;
    lbl_805F1404->m_reverbHi.m_mix = 0.5f;
    lbl_805F1404->m_chorus.m_baseDelay = 0xF;
    lbl_805F1404->m_chorus.m_variation = 0;
    lbl_805F1404->m_chorus.m_period = 500;
    lbl_805F1404->m_delay.m_delay[0] = 500;
    lbl_805F1404->m_delay.m_delay[1] = 500;
    lbl_805F1404->m_delay.m_delay[2] = 500;
    lbl_805F1404->m_delay.m_feedback[0] = 0x32;
    lbl_805F1404->m_delay.m_feedback[1] = 0x32;
    lbl_805F1404->m_delay.m_feedback[2] = 0x32;
    lbl_805F1404->m_delay.m_output[0] = 100;
    lbl_805F1404->m_delay.m_output[1] = 100;
    lbl_805F1404->m_delay.m_output[2] = 100;
}

/* Starts the voices and the four aux effects. */
void fn_8043E950(void) {
    fn_8043D61C();
    fn_80389320(fn_8043AD94, fn_8043ACE8);
    fn_803881EC(&lbl_805F1404->m_reverbStd);
    fn_803876D8(&lbl_805F1404->m_reverbHi);
    fn_80388658(&lbl_805F1404->m_chorus);
    fn_803891E8(&lbl_805F1404->m_delay);
}

static inline void NewAramBanks(void) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_805F1404->m_config[i] != 0) {
            fn_8043B1AC(i, (lbl_805F1404->m_config[i] + 0x1F) & 0xFFFFFFE0);
        }
    }
}

/* Initialises the library: memory, ARAM (a silent 0x500-byte block first), the default bank sizes. */
u32 fn_8043E6F4(u32 memory, u32 voices, u32 size, u32 aramSize) {
    void *zero;
    fn_8043E9C4(memory, size);
    lbl_805F1404->field_0x4 = sizeof(GcaxGlobals);
    lbl_805F1404->m_masterDecibels = 0.0f;
    lbl_805F1404->m_arqOwner = 0xF;
    lbl_805F1404->m_voiceLimit = voices;
    lbl_805F1404->m_aramSize = aramSize;
    lbl_805F1404->m_aramBase = fn_80381A78(lbl_805F1404->m_aramSize);
    lbl_805F1404->m_aramNext = (lbl_805F1404->m_aramBase + 0x1F) & 0xFFFFFFE0;
    lbl_805F1404->m_aramFree =
        lbl_805F1404->m_aramSize - (lbl_805F1404->m_aramNext - lbl_805F1404->m_aramBase);
    zero = (void *)((u32)lbl_805F1404->field_0x13A48 + 0x23 & 0xFFFFFFE0);
    memset(zero, 0, 0x500);
    fn_8043AB6C(zero, lbl_805F1404->m_aramNext, 0x500, 0, 0);
    lbl_805F1404->m_aramZero = lbl_805F1404->m_aramNext;
    lbl_805F1404->m_aramNext += 0x500;
    lbl_805F1404->m_aramFree -= 0x500;
    lbl_805F1404->field_0x13A44 = 1.0f;
    lbl_805F1404->m_config[0] = 0x7D000;
    lbl_805F1404->m_config[1] = 460000;
    lbl_805F1404->m_config[2] = 360000;
    lbl_805F1404->m_config[3] = 400000;
    lbl_805F1404->m_config[4] = 360000;
    lbl_805F1404->m_config[5] = 360000;
    lbl_805F1404->m_config[6] = 140000;
    lbl_805F1404->m_config[7] = 100000;
    lbl_805F1404->m_config[8] = 0x29820;
    lbl_805F1404->m_config[9] = 200000;
    lbl_805F1404->m_config[10] = 240000;
    lbl_805F1404->m_config[11] = 24000;
    lbl_805F1404->m_config[12] = 400000;
    lbl_805F1404->m_config[13] = 40000;
    fn_8043B25C();
    NewAramBanks();
    OSReport("AX SDLIB: aramRest=%d\n", lbl_805F1404->m_aramFree);
    lbl_805F1404->m_pauseVoices = 1;
    fn_8043E950();
    lbl_805F1404->m_initialized = 1;
    return 0;
}

/* Shuts the library down and gives its memory back. */
void fn_8043E624(void) {
    u8 local[8];
    if (lbl_805F1404 != NULL) {
        u32 interrupt = OSDisableInterrupts();
        lbl_805F1404->m_initialized = 0;
        lbl_805F1404->m_lock++;
        OSRestoreInterrupts(interrupt);
    }
    fn_80388258(&lbl_805F1404->m_reverbStd);
    fn_80387748(&lbl_805F1404->m_reverbHi);
    fn_803888A0(&lbl_805F1404->m_chorus);
    fn_80389230(&lbl_805F1404->m_delay);
    if (lbl_805F1404 != NULL) {
        lbl_805F1404 = NULL;
        if (lbl_805F1408 != 0) {
            lbl_805F1408 = 0;
        } else {
            OSFreeToHeap(__OSCurrHeap, lbl_805F1400);
        }
        lbl_805F1400 = 0;
        fn_80381AE0(local);
    }
}

/* The 5 ms tick: steps every player's sequence, fades and voices unless the library is locked. */
void fn_8043E494(void) {
    int i;
    if (lbl_805F1404 == NULL || lbl_805F1404->m_initialized == 0) {
        return;
    }
    GcaxLock();
    if (lbl_805F1404->m_lock != 1) {
        lbl_805F1404->m_missedTicks++;
        GcaxUnlock();
        return;
    }
    lbl_805F1404->m_lastTickTime = lbl_805F1404->m_tickTime;
    lbl_805F1404->m_tickTime = OSGetTime();
    lbl_805F1404->m_activeGroups = fn_8043BB18();
    lbl_805F1404->m_activeVoices = fn_8043D49C();
    for (i = 0; i < 8; i++) {
        if ((lbl_805F1404->m_players[i].m_flags & 0x80000000) != 0) {
            fn_8043FF5C(&lbl_805F1404->m_players[i]);
            fn_8043DD14(&lbl_805F1404->m_players[i]);
        }
        fn_8043D99C(&lbl_805F1404->m_players[i]);
        fn_8043D870(&lbl_805F1404->m_players[i]);
        fn_8043DAB0(&lbl_805F1404->m_players[i]);
        fn_8043DA0C(&lbl_805F1404->m_players[i]);
        fn_8043B5FC(&lbl_805F1404->m_players[i].m_groups);
    }
    lbl_805F1404->m_activeGroups = fn_8043BB18();
    lbl_805F1404->m_activeVoices = fn_8043D49C();
    GcaxUnlock();
}

void fn_8043E47C(int mono) {
    lbl_805F1404->m_mono = mono != 0;
}

void fn_8043E430(int surround) {
    lbl_805F1404->m_surround = surround != 0;
    if (surround != 0) {
        fn_80384978(2);
    } else {
        fn_80384978(0);
    }
}

/* The AX frame callback: runs the tick (with interrupts enabled when called from AX). */
void fn_8043E394(void) {
    if (lbl_805F1404 != NULL && lbl_805F1404->field_0x13A38 != 0) {
        lbl_805F1404->field_0x13A39++;
        OSEnableInterrupts();
        if (lbl_805F1404 != NULL && lbl_805F1404->m_initialized != 0) {
            fn_8043E494();
            lbl_805F1404->m_ticks++;
        }
        OSDisableInterrupts();
        fn_8038A3EC();
    } else {
        if (lbl_805F1404 != NULL && lbl_805F1404->m_initialized != 0) {
            fn_8043E494();
        }
        fn_8038A3EC();
    }
}

/* Registers one of the four effects (`effect` 1..4, 0 for none) as the aux A (`bus` 0) or aux B
 * callback. */
void fn_8043E2B4(int bus, int effect) {
    void (*callback)(void);
    void *data;
    switch (effect) {
    default:
        return;
    case 0:
        callback = NULL;
        data = NULL;
        break;
    case 1:
        callback = fn_803882A4;
        data = &lbl_805F1404->m_reverbStd;
        break;
    case 2:
        callback = fn_80387794;
        data = &lbl_805F1404->m_reverbHi;
        break;
    case 3:
        callback = fn_80388A00;
        data = &lbl_805F1404->m_chorus;
        break;
    case 4:
        callback = fn_80388D50;
        data = &lbl_805F1404->m_delay;
        break;
    }
    switch (bus) {
    case 0:
        fn_80384244(callback, data);
        break;
    case 1:
        fn_80384250(callback, data);
        break;
    }
}
