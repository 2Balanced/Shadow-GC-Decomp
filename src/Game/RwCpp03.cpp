/* AX SDLIB, the game's GameCube AX sound library (see include/Game/rw/r1.h), 0x8043D4D8..0x8043EFF0:
 * voices, the sequence players' fades and MIDI messages, the 5 ms tick, start-up and shut-down.
 * Function and member names are inferred from behavior. Definitions are in reverse address order
 * because deferred inlining emits the last one first. 26 of 32 functions match; left:
 * - fn_8043ED7C, fn_8043EE74: instruction-identical; their int-to-float constant is lbl_805FAF60
 *   in .sdata2, which the split does not give this unit and no source can name (the other float
 *   constants are named through externs).
 * - fn_8043E0E8, fn_8043E6F4: the documented later-compiler floor (retail copies the zeroed loop
 *   counter, `mr r23,r21` / `mr r31,r29`, where GC/2.7 emits a second `li`).
 * - fn_8043DAE4 (5 rows): the masked bend value and the channel pointer trade r4/r5 (~20 spellings).
 * - fn_8043E9C4 (11 rows): retail does not thread the `memory != 0` branch past the null check and
 *   keeps the memory argument in r30 (ten spellings tried). */
#include <string.h>
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern const float lbl_805FAF28; // 0.0f
extern const float lbl_805FAF2C; // -1.0f
extern const float lbl_805FAF30; // 1.0f
extern const float lbl_805FAF34; // 5.0f
extern const float lbl_805FAE74; // 1.0f
extern const float lbl_805FAF38; // 2290649344.0f
extern const float lbl_805FAF40;
extern const float lbl_805FAF44;
extern const float lbl_805FAF48;
extern const float lbl_805FAF4C;
extern const float lbl_805FAF50;
extern const float lbl_805FAF54;
extern const float lbl_805FAF58; // 127.0f
extern u32 __OSCurrHeap;
extern u32 lbl_805F1400;
extern int lbl_805F1408;

extern "C" {
void fn_80384978(int);
void OSReport(const char *, ...);
void OSEnableInterrupts(void);
s64 OSGetTime(void);
void *OSAllocFromHeap(u32, u32);
void OSFreeToHeap(u32, u32);
GcaxListNode *fn_8043B3DC(GcaxListNode **, GcaxListNode *);
void fn_8043B5FC(GcaxListNode **);
void fn_8043B840(GcaxListNode **);
void fn_8043B868(GcaxListNode **, GcaxListNode *);
GcaxListNode *fn_8043B8F0(GcaxListNode **, GcaxListNode *);
GcaxListNode *fn_8043B910(GcaxListNode **, void *);
GcaxGroup *fn_8043B954(GcaxPlayer *, int, int, int, int);
int fn_8043BB18(void);
void fn_8043BE6C(GcaxVoice *);
GcaxListNode *fn_8043C10C(GcaxListNode **, GcaxListNode *);
void fn_8043C170(GcaxVoice *, int, float);
void fn_8043C2A4(GcaxVoice *, GcaxPlayer *);
float fn_8043D318(int);
int fn_8043D49C(void);
void fn_8043D61C(void);
void fn_8043D6FC(GcaxAxVoice *);
void fn_8043D7B4(GcaxPlayer *, float, float);
void fn_8043D870(GcaxPlayer *);
void fn_8043D8E0(GcaxPlayer *, float, float);
void fn_8043D99C(GcaxPlayer *);
void fn_8043DA0C(GcaxPlayer *);
void fn_8043DAB0(GcaxPlayer *);
void fn_8043DAE4(GcaxPlayer *, u8, u32);
void fn_8043DB08(GcaxPlayer *, u8, u8, u32);
void fn_8043DD14(GcaxPlayer *);
void fn_8043E494(void);
void fn_8043E624(void);
void fn_8043E950(void);
void fn_8043E9C4(u32, u32);
void fn_8043F5AC(void);
void fn_8043FB58(GcaxPlayer *);
void fn_8043FC98(GcaxPlayer *);
void fn_8043FD70(GcaxPlayer *);
void fn_8043FF5C(GcaxPlayer *);
void fn_8043AB6C(void *, int, u32, void *, void *);
void fn_8043AD94(u32);
void fn_8043ACE8(void *);
void fn_8043AEBC(u32, int);
void fn_8043B1AC(int, u32);
void fn_8043B25C(void);
GcaxAxVoice *fn_80383C48(u32, void (*)(GcaxAxVoice *), void *);
void fn_80386348(GcaxAxVoice *, u32);
void fn_8038A338(GcaxAxVoice *);
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
void fn_80389320(void *, void *);
void fn_8038A3EC(void);
extern const char lbl_8051BC08[]; // "AX SDLIB: AXAcquireVoice() failed in drop callback" + newline
extern const char lbl_8051BC40[]; // "AX SDLIB: aramRest=%d" + newline
extern const char lbl_8051BC58[]; // "AX SDLIB: Panic: cannot allocate memory (%d)" + newline
extern const char lbl_8051BC88[]; // "AX SDLIB: allocated (%d)" + newline

/* Resets a channel's controllers (MIDI controller 121). */
static inline void ResetChannel(GcaxPlayer *player, int channel) {
    u32 interrupt = OSDisableInterrupts();
    player->m_channels[channel].field_0x0 = 0;
    player->m_channels[channel].m_volume = 100;
    player->m_channels[channel].m_pan = 0x40;
    player->m_channels[channel].m_bendRaw = 0x2000;
    player->m_channels[channel].m_expression = 0x7F;
    player->m_channels[channel].m_depth = 0;
    player->m_channels[channel].field_0x28 = 0;
    player->m_channels[channel].field_0x24 = -1;
    player->m_channels[channel].field_0x38 = 0x88888888;
    player->m_channels[channel].field_0x3C = lbl_805FAF38;
    player->m_channels[channel].field_0x40 = lbl_805FAF38;
    OSRestoreInterrupts(interrupt);
}

int fn_8043EF50(GcaxPlayer *player) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043D7B4(player, fn_8043D318(value), (float)duration);
    return 0;
}

int fn_8043ED7C(GcaxPlayer *player, s8 value, int duration) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    if (value < -0x7F || value > 0x7F) {
        return 0x3000001;
    }
    fn_8043D8E0(player, (float)value / lbl_805FAF58, (float)duration);
    return 0;
}

int fn_8043ECDC(GcaxPlayer *player) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    fn_8043FC98(player);
    return 0;
}

int fn_8043EC3C(GcaxPlayer *player) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
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

/* Takes the library's memory (allocated from the current heap when `memory` is NULL) and sets
 * the effects' default parameters. */
void fn_8043E9C4(u32 memory, u32 size) {
    u32 interrupt = OSDisableInterrupts();
    if (memory != 0) {
        lbl_805F1408 = 1;
    } else {
        memory = (u32)OSAllocFromHeap(__OSCurrHeap, size);
    }
    if (memory == 0) {
        OSReport(lbl_8051BC58, size);
        while (1) {
        }
    }
    OSReport(lbl_8051BC88, size);
    lbl_805F1400 = memory;
    lbl_805F1404 = (GcaxGlobals *)((memory + 0x1F) & 0xFFFFFFE0);
    lbl_805F1404->m_initialized = 0;
    lbl_805F1404->field_0x13A38 = 0;
    OSRestoreInterrupts(interrupt);
    memset(lbl_805F1404, 0, sizeof(GcaxGlobals));
    u32 heap = ((u32)lbl_805F1404 + sizeof(GcaxGlobals) + 0x1F) & 0xFFFFFFE0;
    fn_8043AEBC(heap, size - (heap - memory));
    lbl_805F1404->m_reverbStd.m_tempDisableFX = 0;
    lbl_805F1404->m_reverbStd.m_time = lbl_805FAF48;
    lbl_805F1404->m_reverbStd.m_preDelay = lbl_805FAF4C;
    lbl_805F1404->m_reverbStd.m_damping = lbl_805FAF50;
    lbl_805F1404->m_reverbStd.m_coloration = lbl_805FAF50;
    lbl_805F1404->m_reverbStd.m_mix = lbl_805FAF50;
    lbl_805F1404->m_reverbHi.m_tempDisableFX = 0;
    lbl_805F1404->m_reverbHi.m_time = lbl_805FAF48;
    lbl_805F1404->m_reverbHi.m_preDelay = lbl_805FAF4C;
    lbl_805F1404->m_reverbHi.m_damping = lbl_805FAF50;
    lbl_805F1404->m_reverbHi.m_coloration = lbl_805FAF50;
    lbl_805F1404->m_reverbHi.m_crosstalk = lbl_805FAF54;
    lbl_805F1404->m_reverbHi.m_mix = lbl_805FAF50;
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

/* Initialises the library: memory, ARAM (a silent 0x500-byte block first), the default bank sizes. */
u32 fn_8043E6F4(u32 memory, u32 voices, u32 size, u32 aramSize) {
    fn_8043E9C4(memory, size);
    lbl_805F1404->field_0x4 = sizeof(GcaxGlobals);
    lbl_805F1404->m_masterDecibels = lbl_805FAF40;
    lbl_805F1404->m_arqOwner = 0xF;
    lbl_805F1404->m_voiceLimit = voices;
    lbl_805F1404->m_aramSize = aramSize;
    lbl_805F1404->m_aramBase = fn_80381A78(lbl_805F1404->m_aramSize);
    lbl_805F1404->m_aramNext = (lbl_805F1404->m_aramBase + 0x1F) & 0xFFFFFFE0;
    lbl_805F1404->m_aramFree =
        lbl_805F1404->m_aramSize - (lbl_805F1404->m_aramNext - lbl_805F1404->m_aramBase);
    void *zero = (void *)((u32)lbl_805F1404->field_0x13A48 + 0x23 & 0xFFFFFFE0);
    memset(zero, 0, 0x500);
    fn_8043AB6C(zero, lbl_805F1404->m_aramNext, 0x500, 0, 0);
    lbl_805F1404->m_aramZero = lbl_805F1404->m_aramNext;
    lbl_805F1404->m_aramNext += 0x500;
    lbl_805F1404->m_aramFree -= 0x500;
    lbl_805F1404->field_0x13A44 = lbl_805FAF44;
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
    for (int i = 0; i < 16; i++) {
        if (lbl_805F1404->m_config[i] != 0) {
            fn_8043B1AC(i, (lbl_805F1404->m_config[i] + 0x1F) & 0xFFFFFFE0);
        }
    }
    OSReport(lbl_8051BC40, lbl_805F1404->m_aramFree);
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
    for (int i = 0; i < 8; i++) {
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

/* Resets a channel's controllers. */
void fn_8043E228(GcaxPlayer *player, int channel) {
    ResetChannel(player, channel);
}

/* Takes a free player and sets it up. */
GcaxPlayer *fn_8043E0E8(void) {
    int j;
    u32 interrupt = OSDisableInterrupts();
    int i;
    for (i = 0; i < 8; i++) {
        if ((lbl_805F1404->m_players[i].m_flags & 0x80000000) == 0) {
            goto found;
        }
    }
    OSRestoreInterrupts(interrupt);
    return NULL;
found:
    GcaxPlayer *player = &lbl_805F1404->m_players[i];
    memset(player, 0, sizeof(GcaxPlayer));
    player->m_flags |= 0x80000000;
    player->m_volume = lbl_805FAF28;
    player->field_0xC = lbl_805FAF28;
    player->m_speed = lbl_805FAF30;
    player->field_0x28 = lbl_805FAF28;
    player->field_0x10 = lbl_805FAF30;
    player->m_volumeFade.m_remaining = lbl_805FAF2C;
    player->m_speedFade.m_remaining = lbl_805FAF2C;
    for (j = 0; j < 16; j++) {
        player->m_channels[j].field_0x4 = j;
        ResetChannel(player, j);
    }
    OSRestoreInterrupts(interrupt);
    return player;
}

/* Frees a player. */
void fn_8043E088(GcaxPlayer *player) {
    u32 interrupt = OSDisableInterrupts();
    if (player->m_groups != NULL) {
        fn_8043B840(&player->m_groups);
        player->m_groups = NULL;
    }
    player->m_flags &= 0x7FFFFFFF;
    OSRestoreInterrupts(interrupt);
}

/* Queues a MIDI-style message for the player (1024-entry ring). */
void fn_8043E004(GcaxPlayer *player, u32 message) {
    u32 interrupt = OSDisableInterrupts();
    if (player->m_queueCount < 0x400) {
        player->m_queue[player->m_queueWrite] = message;
        player->m_queueCount++;
        player->m_queueWrite++;
        player->m_queueWrite %= 0x400;
    }
    OSRestoreInterrupts(interrupt);
}

/* Note off: releases the voices of the groups playing the key, unless the channel's sustain pedal
 * is down. */
static inline void NoteOff(GcaxPlayer *player, u32 channel, u32 key) {
    if ((player->m_channels[channel].field_0x0 & 1) != 0) {
        return;
    }
    GcaxListNode *node = NULL;
    while ((node = fn_8043B8F0(&player->m_groups, node)) != NULL) {
        GcaxGroup *group = (GcaxGroup *)node->m_item;
        if (group != NULL && group->m_channel == channel && group->m_key == key) {
            if ((player->m_flags & 0x40000000) != 0) {
                fn_8043B868(&player->m_groups, node);
            } else {
                GcaxListNode *voiceNode = NULL;
                while ((voiceNode = fn_8043B3DC(&group->m_voices, voiceNode)) != NULL) {
                    GcaxVoice *voice = (GcaxVoice *)voiceNode->m_item;
                    if (voice != NULL && voice->m_state < 4) {
                        voice->m_state = 4;
                    }
                }
            }
        }
    }
}

/* Handles the player's queued messages. */
void fn_8043DD14(GcaxPlayer *player) {
    GcaxListNode *node;
    u32 data1;
    u32 channel;
    int priority;
    u32 data2;
    u32 message;
    while (player->m_queueCount != 0) {
        u32 interrupt = OSDisableInterrupts();
        player->m_queueCount--;
        message = player->m_queue[player->m_queueRead];
        player->m_queueRead++;
        player->m_queueRead %= 0x400;
        OSRestoreInterrupts(interrupt);
        priority = (s8)(message & 0xFF);
        channel = (message >> 24) & 0xF;
        data1 = (message >> 16) & 0xFF;
        data2 = (message >> 8) & 0xFF;
        switch ((message >> 24) & 0xF0) {
        case 0x80:
            NoteOff(player, channel, data1);
            break;
        case 0x90:
            if (data2 != 0) {
                node = fn_8043B910(&player->m_groups, NULL);
                if (node != NULL) {
                    GcaxGroup *group = fn_8043B954(player, channel, data1, data2, priority);
                    if (group == NULL) {
                        fn_8043B868(&player->m_groups, node);
                    } else {
                        group->m_owner = player;
                        group->m_key = data1;
                        group->m_channel = channel;
                        node->m_item = group;
                    }
                }
            } else {
                NoteOff(player, channel, data1);
            }
            break;
        case 0xA0:
            break;
        case 0xB0:
            fn_8043DB08(player, channel, data1, data2);
            break;
        case 0xC0:
            player->m_channels[channel].m_program = data1 & 0x7F;
            break;
        case 0xD0:
            break;
        case 0xE0:
            fn_8043DAE4(player, channel, (u16)(((data2 & 0x7F) << 7) + (data1 & 0x7F)));
            break;
        }
    }
}

/* MIDI control change. */
void fn_8043DB08(GcaxPlayer *player, u8 channel, u8 controller, u32 value) {
    switch (controller) {
    case 0x00:
    case 0x20:
        player->m_channels[channel].m_bank = value & 0x7F;
        break;
    case 0x07:
        player->m_channels[channel].m_volume = value & 0x7F;
        break;
    case 0x0A:
        player->m_channels[channel].m_pan = value & 0x7F;
        player->m_channels[channel].field_0x24 = -1;
        break;
    case 0x0B:
        player->m_channels[channel].m_expression = value & 0x7F;
        break;
    case 0x3F:
        player->m_channels[channel].m_depth = value & 0x7F;
        break;
    case 0x3E: {
        int span;
        int depth;
        u8 byte = value;
        if ((value & 0x7F) <= 0x40) {
            span = (byte << 1) & 0xFE;
            if (span > 0x7F) {
                span = 0x7F;
            }
            depth = 0;
        } else {
            depth = 0x7F;
            span = (0x7F - (byte & 0x7F)) << 1;
        }
        player->m_channels[channel].field_0x24 = span;
        player->m_channels[channel].m_depth = depth;
        break;
    }
    case 0x40:
        if ((value & 0x7F) >= 0x40) {
            player->m_channels[channel].field_0x0 |= 1;
        } else {
            player->m_channels[channel].field_0x0 &= ~1;
        }
        break;
    case 0x79:
        ResetChannel(player, channel);
        break;
    }
}

/* Sets a channel's 14-bit pitch bend. */
void fn_8043DAE4(GcaxPlayer *player, u8 channel, u32 value) {
    value &= 0x3FFF;
    player->m_channels[channel].m_bendRaw = value;
    player->m_channels[channel].m_bend = player->m_channels[channel].m_bendRaw - 0x2000;
}

void fn_8043DAB0(GcaxPlayer *player) {
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    for (int i = 0; i < 16; i++) {
        player->m_channels[i].m_bend = player->m_channels[i].m_bendRaw - 0x2000;
    }
}

/* Steps the envelope, mix and pitch of every voice of the player's groups. */
void fn_8043DA0C(GcaxPlayer *player) {
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    GcaxListNode *groupNode = NULL;
    while ((groupNode = fn_8043B8F0(&player->m_groups, groupNode)) != NULL) {
        GcaxListNode *voiceNode = NULL;
        u8 channel = ((GcaxGroup *)groupNode->m_item)->m_channel;
        while ((voiceNode = fn_8043C10C(&((GcaxGroup *)groupNode->m_item)->m_voices, voiceNode)) != NULL) {
            fn_8043BE6C((GcaxVoice *)voiceNode->m_item);
            fn_8043C2A4((GcaxVoice *)voiceNode->m_item, player);
            fn_8043C170((GcaxVoice *)voiceNode->m_item, player->m_channels[channel].m_bend,
                        player->m_speed);
        }
    }
}

/* Steps the player's volume fade by one 5 ms tick. */
void fn_8043D99C(GcaxPlayer *player) {
    if (player->m_volumeFade.m_remaining >= lbl_805FAF28) {
        player->m_volumeFade.m_remaining -= lbl_805FAF34;
        if (player->m_volumeFade.m_remaining <= lbl_805FAF28) {
            player->m_volume = player->m_volumeFade.m_target;
            player->m_volumeFade.m_duration = lbl_805FAF28;
            player->m_volumeFade.m_remaining = lbl_805FAF2C;
            return;
        }
        player->m_volume = (player->m_volumeFade.m_target - player->m_volumeFade.m_start) *
                               (lbl_805FAF30 - player->m_volumeFade.m_remaining / player->m_volumeFade.m_duration) +
                           player->m_volumeFade.m_start;
    }
}

/* Starts a volume fade to `target` over `duration` milliseconds. */
void fn_8043D8E0(GcaxPlayer *player, float target, float duration) {
    u32 interrupt = OSDisableInterrupts();
    player->m_volumeFade.m_start = player->m_volume;
    player->m_volumeFade.m_target = target;
    player->m_volumeFade.m_duration = duration;
    player->m_volumeFade.m_remaining = duration;
    if (player->m_volumeFade.m_remaining <= lbl_805FAF28) {
        player->m_volume = player->m_volumeFade.m_target;
        player->m_volumeFade.m_duration = lbl_805FAF28;
        player->m_volumeFade.m_remaining = lbl_805FAF2C;
    } else {
        player->m_volume = (player->m_volumeFade.m_target - player->m_volumeFade.m_start) *
                               (lbl_805FAF30 - player->m_volumeFade.m_remaining / player->m_volumeFade.m_duration) +
                           player->m_volumeFade.m_start;
    }
    OSRestoreInterrupts(interrupt);
}

/* The same pair for the player's speed (0x24). */
void fn_8043D870(GcaxPlayer *player) {
    if (player->m_speedFade.m_remaining >= lbl_805FAF28) {
        player->m_speedFade.m_remaining -= lbl_805FAF34;
        if (player->m_speedFade.m_remaining <= lbl_805FAF28) {
            player->m_speed = player->m_speedFade.m_target;
            player->m_speedFade.m_duration = lbl_805FAF28;
            player->m_speedFade.m_remaining = lbl_805FAF2C;
            return;
        }
        player->m_speed = (player->m_speedFade.m_target - player->m_speedFade.m_start) *
                              (lbl_805FAF30 - player->m_speedFade.m_remaining / player->m_speedFade.m_duration) +
                          player->m_speedFade.m_start;
    }
}

void fn_8043D7B4(GcaxPlayer *player, float target, float duration) {
    u32 interrupt = OSDisableInterrupts();
    player->m_speedFade.m_start = player->m_speed;
    player->m_speedFade.m_target = target;
    player->m_speedFade.m_duration = duration;
    player->m_speedFade.m_remaining = duration;
    if (player->m_speedFade.m_remaining <= lbl_805FAF28) {
        player->m_speed = player->m_speedFade.m_target;
        player->m_speedFade.m_duration = lbl_805FAF28;
        player->m_speedFade.m_remaining = lbl_805FAF2C;
    } else {
        player->m_speed = (player->m_speedFade.m_target - player->m_speedFade.m_start) *
                              (lbl_805FAF30 - player->m_speedFade.m_remaining / player->m_speedFade.m_duration) +
                          player->m_speedFade.m_start;
    }
    OSRestoreInterrupts(interrupt);
}

/* AX drop callback: AX took the voice back; marks ours and asks for another one. */
void fn_8043D6FC(GcaxAxVoice *axVoice) {
    GcaxVoice *voice = (GcaxVoice *)axVoice->m_userContext;
    fn_80386348(axVoice, 0);
    fn_8038A338(axVoice);
    voice->m_flags |= 0x40000000;
    lbl_805F1404->m_axVoiceCount--;
    voice->m_axVoice = fn_80383C48(0x1B, fn_8043D6FC, voice);
    if (voice->m_axVoice != NULL) {
        voice->m_flags &= 0xBFFFFFFF;
        fn_80386348(voice->m_axVoice, 0);
        lbl_805F1404->m_axVoiceCount++;
    } else {
        OSReport(lbl_8051BC08);
    }
}

/* Clears the 64 voices and acquires an AX voice for each one up to the configured limit. */
void fn_8043D61C(void) {
    fn_80384978(0);
    memset(lbl_805F1404->m_voices, 0, sizeof(lbl_805F1404->m_voices));
    int i;
    for (int j = 0; j < 64; j++) {
        lbl_805F1404->m_voices[j].m_index = j;
    }
    int count = lbl_805F1404->m_voiceLimit;
    if (count > 64) {
        count = 64;
    }
    for (i = 0; i < count; i++) {
        lbl_805F1404->m_voices[i].m_axVoice =
            fn_80383C48(0x1B, fn_8043D6FC, &lbl_805F1404->m_voices[i]);
        if (lbl_805F1404->m_voices[i].m_axVoice == NULL) {
            break;
        }
        lbl_805F1404->m_axVoiceCount++;
    }
}

static inline GcaxVoice *FindFreeVoice() {
    for (int i = 0; i < 64; i++) {
        lbl_805F1404->m_nextVoice++;
        lbl_805F1404->m_nextVoice %= 64;
        GcaxVoice *voice = &lbl_805F1404->m_voices[lbl_805F1404->m_nextVoice];
        if ((voice->m_flags & 0x80000000) != 0 && (voice->m_flags & 0x40000000) != 0) {
            GcaxAxVoice *axVoice = fn_80383C48(0x1B, fn_8043D6FC, voice);
            if (axVoice != NULL) {
                lbl_805F1404->m_axVoiceCount++;
                voice->m_axVoice = axVoice;
                voice->m_flags &= 0xBFFFFFFF;
                return voice;
            }
        }
        if ((voice->m_flags & 0x80000000) == 0 && voice->m_axVoice != NULL) {
            return voice;
        }
    }
    return NULL;
}

/* Takes the next free voice (round robin) for a group. */
GcaxVoice *fn_8043D4D8(GcaxGroup *group, u32 arg2, u32 arg3) {
    GcaxVoice *voice = FindFreeVoice();
    if (voice != NULL) {
        voice->field_0x10 = lbl_805FAE74;
        voice->m_group = group;
        voice->field_0x8 = arg2;
        voice->field_0xC = arg3;
        voice->m_flags |= 0x80000000;
        voice->m_flags &= 0xFFFFFFFE;
        voice->m_flags &= 0xFFFFFFFD;
        voice->field_0x104 = 0;
    }
    return voice;
}
}
