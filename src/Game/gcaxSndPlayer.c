/* AX SDLIB, the game's GameCube AX sound library (see include/Game/rw/r1.h), 0x8043D7B4..0x8043E2B4:
 * the sequence players: volume and speed fades, the MIDI messages a player queues and handles,
 * and taking and freeing players. The file name is descriptive. Its .sdata2 pool
 * (0x805FAF28..0x805FAF40) repeats 0.0f, 1.0f and 5.0f of Game/gcaxSndVoice.c's and ends padded to
 * 8 bytes; the next pool (0x805FAF40) repeats 0.0f and 1.0f again.
 * C with the game's flags: definitions in reverse address order.
 * Function and member names are inferred from behavior. NonMatching:
 * - fn_8043E0E8 (5 rows): retail copies the zeroed channel counter into the register that holds
 *   0 (`mr r23,r21`), the sign of the loop being in an inline function; with the loop in one
 *   (and the reset written as a macro, since an inline calling an inline is not inlined at level
 *   1) the copy appears but the counter and the channel pointer trade r21/r22 (about 20 shapes,
 *   probes/Game_gcaxSndPlayer.py).
 * - fn_8043DAE4 (5 rows): retail masks the bend in place in r5 and puts the channel in r4; every
 *   spelling tried (parameter types, a channel pointer, a local) keeps the reverse. */
#include <dolphin/types.h>
#include "size_assert.h"
#include "Game/rw/r1.h"

GcaxListNode *fn_8043B3DC(GcaxListNode **, GcaxListNode *);
void fn_8043B840(GcaxListNode **);
void fn_8043B868(GcaxListNode **, GcaxListNode *);
GcaxListNode *fn_8043B8F0(GcaxListNode **, GcaxListNode *);
GcaxListNode *fn_8043B910(GcaxListNode **, void *);
GcaxGroup *fn_8043B954(GcaxPlayer *, int, int, int, int);
void fn_8043BE6C(GcaxVoice *);
GcaxListNode *fn_8043C10C(GcaxListNode **, GcaxListNode *);
void fn_8043C170(GcaxVoice *, int, float);
void fn_8043C2A4(GcaxVoice *, GcaxPlayer *);
void fn_8043DAE4(GcaxPlayer *, u8, u32);
void fn_8043DB08(GcaxPlayer *, u8, u8, u32);

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
    player->m_channels[channel].field_0x3C = 2290649344.0f;
    player->m_channels[channel].field_0x40 = 2290649344.0f;
    OSRestoreInterrupts(interrupt);
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
    GcaxPlayer *player;
    for (i = 0; i < 8; i++) {
        if ((lbl_805F1404->m_players[i].m_flags & 0x80000000) == 0) {
            goto found;
        }
    }
    OSRestoreInterrupts(interrupt);
    return NULL;
found:
    player = &lbl_805F1404->m_players[i];
    memset(player, 0, sizeof(GcaxPlayer));
    player->m_flags |= 0x80000000;
    player->m_volume = 0.0f;
    player->field_0xC = 0.0f;
    player->m_speed = 1.0f;
    player->field_0x28 = 0.0f;
    player->field_0x10 = 1.0f;
    player->m_volumeFade.m_remaining = -1.0f;
    player->m_speedFade.m_remaining = -1.0f;
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
    GcaxListNode *node;
    if ((player->m_channels[channel].field_0x0 & 1) != 0) {
        return;
    }
    node = NULL;
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
    int i;
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    for (i = 0; i < 16; i++) {
        player->m_channels[i].m_bend = player->m_channels[i].m_bendRaw - 0x2000;
    }
}

/* Steps the envelope, mix and pitch of every voice of the player's groups. */
void fn_8043DA0C(GcaxPlayer *player) {
    GcaxListNode *groupNode;
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    groupNode = NULL;
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
    if (player->m_volumeFade.m_remaining >= 0.0f) {
        player->m_volumeFade.m_remaining -= 5.0f;
        if (player->m_volumeFade.m_remaining <= 0.0f) {
            player->m_volume = player->m_volumeFade.m_target;
            player->m_volumeFade.m_duration = 0.0f;
            player->m_volumeFade.m_remaining = -1.0f;
            return;
        }
        player->m_volume = (player->m_volumeFade.m_target - player->m_volumeFade.m_start) *
                               (1.0f - player->m_volumeFade.m_remaining / player->m_volumeFade.m_duration) +
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
    if (player->m_volumeFade.m_remaining <= 0.0f) {
        player->m_volume = player->m_volumeFade.m_target;
        player->m_volumeFade.m_duration = 0.0f;
        player->m_volumeFade.m_remaining = -1.0f;
    } else {
        player->m_volume = (player->m_volumeFade.m_target - player->m_volumeFade.m_start) *
                               (1.0f - player->m_volumeFade.m_remaining / player->m_volumeFade.m_duration) +
                           player->m_volumeFade.m_start;
    }
    OSRestoreInterrupts(interrupt);
}

/* The same pair for the player's speed (0x24). */
void fn_8043D870(GcaxPlayer *player) {
    if (player->m_speedFade.m_remaining >= 0.0f) {
        player->m_speedFade.m_remaining -= 5.0f;
        if (player->m_speedFade.m_remaining <= 0.0f) {
            player->m_speed = player->m_speedFade.m_target;
            player->m_speedFade.m_duration = 0.0f;
            player->m_speedFade.m_remaining = -1.0f;
            return;
        }
        player->m_speed = (player->m_speedFade.m_target - player->m_speedFade.m_start) *
                              (1.0f - player->m_speedFade.m_remaining / player->m_speedFade.m_duration) +
                          player->m_speedFade.m_start;
    }
}

void fn_8043D7B4(GcaxPlayer *player, float target, float duration) {
    u32 interrupt = OSDisableInterrupts();
    player->m_speedFade.m_start = player->m_speed;
    player->m_speedFade.m_target = target;
    player->m_speedFade.m_duration = duration;
    player->m_speedFade.m_remaining = duration;
    if (player->m_speedFade.m_remaining <= 0.0f) {
        player->m_speed = player->m_speedFade.m_target;
        player->m_speedFade.m_duration = 0.0f;
        player->m_speedFade.m_remaining = -1.0f;
    } else {
        player->m_speed = (player->m_speedFade.m_target - player->m_speedFade.m_start) *
                              (1.0f - player->m_speedFade.m_remaining / player->m_speedFade.m_duration) +
                          player->m_speedFade.m_start;
    }
    OSRestoreInterrupts(interrupt);
}
