/* AX SDLIB, the game's GameCube AX sound library (see include/Game/rw/r1.h), 0x8043FB58..0x80440410:
 * the sequencer: pausing, resuming, stopping and starting a player's sequence (a standard MIDI file
 * track in a 'gcax' 'MSBM' chunk) and stepping it every tick. The file name is descriptive. Its
 * .sdata2 pool (0x805FAF68..0x805FAF88) holds the int-to-float constant 0x4330000080000000 that
 * Game/gcaxSnd.c's pool has too. RenderWare starts after it.
 * C with the game's flags: definitions in reverse address order.
 * Function and member names are inferred from behavior. NonMatching: fn_8043FDF8 (5 rows: retail
 * stores the format through the sequence pointer and gives the file pointer the player's r30;
 * probes/Game_gcaxSndSequence*.py) and fn_8043FF5C (draft). */
#include <dolphin/types.h>
#include "size_assert.h"
#include "Game/rw/r1.h"

void fn_8043E004(GcaxPlayer *, u32);
void fn_8043B6D0(GcaxListNode **);
void fn_8043B794(GcaxListNode **);
void fn_8043B840(GcaxListNode **);

#define G8 ((u8 *)lbl_805F1404) /* byte view for the code not yet typed */

/* Steps the player's sequence by the time since the last tick and queues its due events. */
void fn_8043FF5C(GcaxPlayer *player) {
    u32 *object = (u32 *)player;
    u8 *cursor;
    s32 elapsed;
    u32 clock;
    u32 divisor;
    u32 frames;
    f32 value;
    u32 *global;
    if ((object[0] & 0x80000000) == 0) return;
    if ((object[1] & 2) == 0) return;
    if ((object[1] & 1) != 0) return;
    cursor = (u8 *)object[0x23];
    if (cursor == NULL) return;

    elapsed = ((s32 *)G8)[7] - (s32)object[0x1D];
    ((s32 *)G8)[10] = elapsed;
    if (((s32 *)G8)[10] < 0) ((s32 *)G8)[10] = -((s32 *)G8)[10];

restart:
    *(f32 *)(object + 0x1F) = 60.0f / *(f32 *)(object + 0x18);
    clock = *(volatile u32 *)0x800000F8;
    divisor = (clock >> 2) / 125000;
    frames = ((u32 *)G8)[10] * 8 / divisor;
    *(f32 *)(object + 0x17) = *(f32 *)(object + 0x16);
    value = (f32)(s32)frames;
    value *= *(f32 *)(G8 + 0x13A44);
    value *= (f32)object[0x13];
    value *= *(f32 *)(object + 0x18);
    value /= 60000000.0f;
    *(f32 *)(object + 0x20) = value;
    *(f32 *)(object + 0x16) += *(f32 *)(object + 0x20);

    while (1) {
        u8 *scan = cursor;
        int length = 0;
        int delta = 0;
        u8 status;
        for (; length < 4; length++) {
            if ((scan[length] & 0x80) == 0) break;
        }
        if (length == 4) {
            delta = -1;
        } else {
            int i;
            length++;
            for (i = 0; i < length; i++) {
                delta = (delta << 7) + (*scan++ & 0x7F);
            }
            cursor = scan;
        }
        if (delta < 0) {
            cursor = NULL;
            break;
        }
        if (*(f32 *)(object + 0x14) + (f32)delta > *(f32 *)(object + 0x16)) break;
        *(f32 *)(object + 0x15) = *(f32 *)(object + 0x14);
        *(f32 *)(object + 0x14) += (f32)delta;

        if ((*cursor & 0x80) != 0) {
            status = *cursor++;
        } else {
            status = *(u8 *)(object + 0x1E);
        }
        if (status == 0xFF) {
            *(u8 *)(object + 0x1E) = 0xFF;
            if (*(s8 *)cursor == 0x51) {
                u32 tempo = ((u32)cursor[2] << 16) | ((u32)cursor[3] << 8) | cursor[4];
                cursor += 5;
                *(f32 *)(object + 0x18) = 60000000.0f / (f32)tempo;
            } else if (*(s8 *)cursor == 0x2F) {
                cursor = NULL;
                break;
            }
        } else {
            switch (status & 0xF0) {
            case 0x80:
            case 0x90:
            case 0xE0: {
                u32 message = ((u32)status << 24) | ((u32)cursor[0] << 16) |
                              ((u32)cursor[1] << 8) | (object[0x21] & 0xFF);
                fn_8043E004(player, message);
                *(u8 *)(object + 0x1E) = status;
                cursor += 2;
                break;
            }
            case 0xB0:
                if (cursor[0] == 0x1F) {
                    if (object[0x24] == 0) {
                        object[0x24] = (u32)(cursor + 2);
                        object[0x27] = cursor[1];
                        cursor += 2;
                        *(f32 *)(object + 0x25) = *(f32 *)(object + 0x14);
                        *(f32 *)(object + 0x26) = *(f32 *)(object + 0x17);
                        *(f32 *)(object + 0x19) = *(f32 *)(object + 0x18);
                    } else if (*(s32 *)(object + 0x27) == 0) {
                        cursor = (u8 *)object[0x24];
                        *(f32 *)(object + 0x16) = *(f32 *)(object + 0x26);
                        *(f32 *)(object + 0x14) = *(f32 *)(object + 0x25);
                        *(f32 *)(object + 0x18) = *(f32 *)(object + 0x19);
                        goto restart;
                    } else {
                        if (*(s32 *)(object + 0x27) > 0) {
                            --*(s32 *)(object + 0x27);
                            if (*(s32 *)(object + 0x27) > 0) {
                                cursor = (u8 *)object[0x24];
                                *(f32 *)(object + 0x16) = *(f32 *)(object + 0x26);
                                *(f32 *)(object + 0x14) = *(f32 *)(object + 0x25);
                                *(f32 *)(object + 0x18) = *(f32 *)(object + 0x19);
                                goto restart;
                            }
                        }
                        cursor += 2;
                        *(u8 *)(object + 0x1E) = 0;
                    }
                } else {
                    u32 message = ((u32)status << 24) | ((u32)cursor[0] << 16) |
                                  ((u32)cursor[1] << 8) | (object[0x21] & 0xFF);
                    fn_8043E004(player, message);
                    *(u8 *)(object + 0x1E) = status;
                    cursor += 2;
                }
                break;
            case 0xA0:
            case 0xC0:
            case 0xD0: {
                u32 message = ((u32)status << 24) | ((u32)cursor[0] << 16);
                fn_8043E004(player, message);
                *(u8 *)(object + 0x1E) = status;
                cursor++;
                break;
            }
            }
        }
        if (*(f32 *)(object + 0x14) < *(f32 *)(object + 0x16)) continue;
        break;
    }

    object[0x23] = (u32)cursor;
    global = (u32 *)G8;
    object[0x1C] = global[6];
    object[0x1D] = global[7];
    if (cursor == NULL) {
        object[1] &= ~3;
        if (object[0x28] != 0) {
            fn_8043B840((GcaxListNode **)(object + 0x28));
            object[0x28] = 0;
        }
        memset(object + 0x12, 0, 0x58);
    }
}

/* Starts sequence `id` of a sequence bank on the player. */
int fn_8043FDF8(GcaxPlayer *player, int bank, u32 id, u32 flags) {
    u32 *table;
    u32 *entry;
    GcaxChunk *header;
    u32 interrupt;
    GcaxSequence *sequence;
    u8 *file;
    if ((player->m_flags & 0x80000000) == 0) {
        return -1;
    }
    table = (u32 *)lbl_805F1404->m_sequenceBanks[bank].m_data;
    if (table == NULL) {
        return -1;
    }
    for (entry = table; *entry != 0; entry++) {
        if ((*entry >> 24) == id) {
            goto found;
        }
    }
    return -1;
found:
    header = (GcaxChunk *)((u8 *)table + (*entry & 0xFFFFFF));
    if (header->m_magic != 0x67636178 || header->m_type != 0x4D53424D) {
        return -1;
    }
    interrupt = OSDisableInterrupts();
    player->m_state = 0;
    player->m_state |= 2;
    if (player->m_groups != NULL) {
        fn_8043B840(&player->m_groups);
        player->m_groups = NULL;
    }
    sequence = &player->m_sequence;
    file = (u8 *)table + (*entry & 0xFFFFFF);
    sequence->m_format = *(u16 *)(file + 0x10);
    memset(sequence, 0, sizeof(GcaxSequence));
    sequence->m_division = *(u16 *)(file + 0x1E);
    sequence->m_lastTime = sequence->m_startTime = lbl_805F1404->m_tickTime;
    sequence->m_tempo = 120.0f;
    sequence->m_messageFlags = flags;
    sequence->m_start = file + 0x28;
    sequence->m_cursor = file + 0x28;
    OSRestoreInterrupts(interrupt);
    return 0;
}

/* Stops the player's sequence. */
int fn_8043FD70(GcaxPlayer *player) {
    u32 interrupt;
    if ((player->m_flags & 0x80000000) == 0) {
        return -1;
    }
    interrupt = OSDisableInterrupts();
    player->m_state &= ~2;
    if (player->m_groups != NULL) {
        fn_8043B840(&player->m_groups);
        player->m_groups = NULL;
    }
    memset(&player->m_sequence, 0, sizeof(GcaxSequence));
    OSRestoreInterrupts(interrupt);
    return 0;
}

/* Pauses the player: its voices are paused or freed (m_pauseVoices). */
void fn_8043FC98(GcaxPlayer *player) {
    u32 interrupt;
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    interrupt = OSDisableInterrupts();
    GcaxLock();
    if ((player->m_state & 1) == 0) {
        player->m_pauseTime = lbl_805F1404->m_tickTime;
        player->m_state |= 1;
        if (player->m_groups != NULL) {
            if (lbl_805F1404->m_pauseVoices == 1) {
                fn_8043B6D0(&player->m_groups);
            } else {
                fn_8043B840(&player->m_groups);
                player->m_groups = NULL;
            }
        }
    }
    GcaxUnlock();
    OSRestoreInterrupts(interrupt);
}

/* Resumes a paused player. */
void fn_8043FB58(GcaxPlayer *player) {
    u32 interrupt;
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    interrupt = OSDisableInterrupts();
    GcaxLock();
    if ((player->m_state & 2) == 0) {
        player->m_state &= ~1;
        GcaxUnlock();
        OSRestoreInterrupts(interrupt);
        return;
    }
    if ((player->m_state & 1) != 0) {
        player->m_sequence.m_startTime += lbl_805F1404->m_tickTime - player->m_pauseTime;
        player->m_sequence.m_lastTime += lbl_805F1404->m_tickTime - player->m_pauseTime;
        player->m_state &= ~1;
        if (lbl_805F1404->m_pauseVoices == 1) {
            fn_8043B794(&player->m_groups);
        }
    }
    GcaxUnlock();
    OSRestoreInterrupts(interrupt);
}
