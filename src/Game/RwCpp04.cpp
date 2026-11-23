/* 0x8043EFF0..0x80440920: the end of the AX SDLIB sound library (see include/Game/rw/r1.h; its
 * player and loader API, game flags), then from 0x80440410 RenderWare (the collision tree's stream
 * functions, built like the rest of RenderWare with -O4,p and no stmw/lmw: the pragmas below).
 * Function and member names are inferred from behavior. Definitions are in reverse address order
 * because deferred inlining emits the last one first. 22 of 27 functions match; left:
 * fn_8043F70C (22 rows: the loader, count, base and result registers rotate; ~10 spellings),
 * fn_8043FDF8 (5 rows: the file pointer takes r29 where retail reuses the player's r30),
 * fn_8043FF5C (the sequencer; draft), fn_80440468 and fn_80440670 (drafts). */
#include <string.h>
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern u8 *lbl_805F15B4;
extern const float lbl_805FAF68;
#define G8 (*(u8 **)&lbl_805F1404) // byte view for the code not yet typed

extern "C" {
void *__va_arg(void *, int);
void fn_8043E004(GcaxPlayer *, u32);
void fn_8043E088(GcaxPlayer *);
GcaxPlayer *fn_8043E0E8(void);
void fn_8043E228(GcaxPlayer *, int);
void fn_8043E2B4(int, int);
void fn_8043E47C(int);
void fn_8043E494(void);
u32 fn_8043E6F4(u32, u32, u32, u32);
int fn_8043AF64(int, GcaxChunk *, GcaxCallback, void *, int);
int fn_8043B05C(int, void *, GcaxCallback, void *, int);
void fn_8043B6D0(GcaxListNode **);
void fn_8043B794(GcaxListNode **);
void fn_8043B840(GcaxListNode **);
int fn_8043F13C(GcaxPlayer *);
int fn_8043F60C(GcaxPlayer *);
int fn_8043FDF8(GcaxPlayer *, int, u32, u32);
void *fn_8047CEF0(void *, u32, u32, u32, u32);
int fn_8047CF50(void *, u32, u32, void *);
void fn_8047D158(void *, u32);
void fn_8047D1C8(void *, u32);
void fn_8047D254(void *, u32);
void fn_8047D2C4(void *, u32);
u32 fn_8047D560(void *, void *, u32);
u32 fn_8047D684(void *, void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);

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

#pragma optimize_for_size off
#pragma use_lmw_stmw off

void *fn_80440670(void *context) {
    u32 local[18];
    if (fn_8047CF50(context, 1, 0, local) == 0) return NULL;
    if (fn_80480D4C(context, local + 9, 0x24) != 0x24) return NULL;
    fn_8047D254(local + 9, 0x24);
    u32 flags = local[9];
    u32 nameCount = local[17] & 0xFFFF;
    u32 extraCount = local[16] & 0xFFFF;
    u32 size = (nameCount != 0) ? nameCount * 0x10 + 0x37 : 0x28;
    if ((flags & 1) != 0) size += extraCount * 2 + 1;
    u32 *header = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x3002C);
    if (header == NULL) {
        local[1] = 0x11D;
        local[2] = fn_8047DC28(0x80000013, size);
        fn_8047DB84(local + 1);
    } else {
        header[0] = flags;
        *(u16 *)(header + 7) = (u16)extraCount;
        *(u16 *)((u8 *)header + 0x1E) = (u16)nameCount;
        header[1] = local[13];
        header[2] = local[14];
        header[3] = local[15];
        header[4] = local[10];
        header[5] = local[11];
        header[6] = local[12];
        u8 *next = (u8 *)(header + 10);
        if (nameCount == 0) {
            header[8] = 0;
        } else {
            next = (u8 *)(((u32)header + 0x37) & ~0xF);
            header[8] = (u32)next;
            next += nameCount * 0x10;
        }
        header[9] = ((flags & 1) == 0) ? 0 : ((u32)next + 1) & ~1;
    }
    if (header == NULL) return NULL;
    if (*(u16 *)((u8 *)header + 0x1E) != 0) {
        u32 bytes = (u32)*(u16 *)((u8 *)header + 0x1E) << 4;
        if (fn_80480D4C(context, (void *)header[8], bytes) != bytes) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(header);
            return NULL;
        }
        u8 *entry = (u8 *)header[8];
        for (u32 i = 0; i < *(u16 *)((u8 *)header + 0x1E); i++) {
            fn_8047D2C4(entry + 2, 2);
            fn_8047D2C4(entry + 10, 2);
            fn_8047D254(entry + 4, 4);
            fn_8047D254(entry + 12, 4);
            entry += 0x10;
        }
    }
    if ((header[0] & 1) != 0 &&
        fn_8047D684(context, (void *)header[9], (u32)*(u16 *)(header + 7) << 1) == 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(header);
        return NULL;
    }
    return header;
}

void *fn_80440468(u32 *header, void *context) {
    int size = *(u16 *)((u8 *)header + 0x1E) * 0x10 + 0x30;
    if ((header[0] & 1) != 0) size += *(u16 *)(header + 7) * 2;
    if (fn_8047CEF0(context, 0x2C, size, 0x37002, 0x37) == NULL) return NULL;
    if (fn_8047CEF0(context, 1, size - 0xC, 0x37002, 0x37) == NULL) return NULL;
    u32 local[9];
    local[0] = header[0];
    local[1] = header[4];
    local[2] = header[5];
    local[3] = header[6];
    local[4] = header[1];
    local[5] = header[2];
    local[6] = header[3];
    local[7] = *(u16 *)(header + 7);
    local[8] = *(u16 *)((u8 *)header + 0x1E);
    fn_8047D158(local, 0x24);
    if (fn_80480ED0(context, local, 0x24) == 0) return NULL;
    u16 count = *(u16 *)((u8 *)header + 0x1E);
    if (count != 0) {
        u8 *entries = (u8 *)header[8];
        for (u32 i = 0; i < count; i++) {
            u32 entry[4];
            *(u64 *)entry = *(u64 *)(entries + i * 0x10);
            *(u64 *)(entry + 2) = *(u64 *)(entries + i * 0x10 + 8);
            fn_8047D1C8((u8 *)entry + 2, 2);
            fn_8047D1C8((u8 *)entry + 10, 2);
            fn_8047D158(entry + 1, 4);
            fn_8047D158(entry + 3, 4);
            if (fn_80480ED0(context, entry, 0x10) == 0) return NULL;
        }
    }
    if ((header[0] & 1) != 0 &&
        fn_8047D560(context, (void *)header[9], (u32)*(u16 *)(header + 7) << 1) == 0) {
        return NULL;
    }
    return header;
}

int fn_8044043C(u32 *header) {
    int size = *(u16 *)((u8 *)header + 0x1E) * 0x10 + 0x30;
    if ((header[0] & 1) != 0) size += *(u16 *)(header + 7) * 2;
    return size;
}

void fn_80440410(void *object) {
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
}

#pragma optimize_for_size reset
#pragma use_lmw_stmw reset

/* Steps the player's sequence by the time since the last tick and queues its due events. */
void fn_8043FF5C(GcaxPlayer *player) {
    u32 *object = (u32 *)player;
    if ((object[0] & 0x80000000) == 0) return;
    if ((object[1] & 2) == 0) return;
    if ((object[1] & 1) != 0) return;
    u8 *cursor = (u8 *)object[0x23];
    if (cursor == NULL) return;

    s32 elapsed = ((s32 *)G8)[7] - (s32)object[0x1D];
    ((s32 *)G8)[10] = elapsed;
    if (((s32 *)G8)[10] < 0) ((s32 *)G8)[10] = -((s32 *)G8)[10];

restart:
    *(f32 *)(object + 0x1F) = 60.0f / *(f32 *)(object + 0x18);
    u32 clock = *(volatile u32 *)0x800000F8;
    u32 divisor = (clock >> 2) / 125000;
    u32 frames = ((u32 *)G8)[10] * 8 / divisor;
    *(f32 *)(object + 0x17) = *(f32 *)(object + 0x16);
    f32 value = (f32)(s32)frames;
    value *= *(f32 *)(G8 + 0x13A44);
    value *= (f32)object[0x13];
    value *= *(f32 *)(object + 0x18);
    value /= 60000000.0f;
    *(f32 *)(object + 0x20) = value;
    *(f32 *)(object + 0x16) += *(f32 *)(object + 0x20);

    while (true) {
        u8 *scan = cursor;
        int length = 0;
        int delta = 0;
        for (; length < 4; length++) {
            if ((scan[length] & 0x80) == 0) break;
        }
        if (length == 4) {
            delta = -1;
        } else {
            length++;
            for (int i = 0; i < length; i++) {
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

        u8 status;
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
                u32 value = ((u32)status << 24) | ((u32)cursor[0] << 16) |
                            ((u32)cursor[1] << 8) | (object[0x21] & 0xFF);
                fn_8043E004(player, value);
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
                    u32 value = ((u32)status << 24) | ((u32)cursor[0] << 16) |
                                ((u32)cursor[1] << 8) | (object[0x21] & 0xFF);
                    fn_8043E004(player, value);
                    *(u8 *)(object + 0x1E) = status;
                    cursor += 2;
                }
                break;
            case 0xA0:
            case 0xC0:
            case 0xD0: {
                u32 value = ((u32)status << 24) | ((u32)cursor[0] << 16);
                fn_8043E004(player, value);
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
    u32 *global = (u32 *)G8;
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
    if ((player->m_flags & 0x80000000) == 0) {
        return -1;
    }
    u32 *table = (u32 *)lbl_805F1404->m_sequenceBanks[bank].m_data;
    if (table == NULL) {
        return -1;
    }
    u32 *entry;
    for (entry = table; *entry != 0; entry++) {
        if ((*entry >> 24) == id) {
            goto found;
        }
    }
    return -1;
found:
    GcaxChunk *header = (GcaxChunk *)((u8 *)table + (*entry & 0xFFFFFF));
    if (header->m_magic != 0x67636178 || header->m_type != 0x4D53424D) {
        return -1;
    }
    u32 interrupt = OSDisableInterrupts();
    player->m_state = 0;
    player->m_state |= 2;
    if (player->m_groups != NULL) {
        fn_8043B840(&player->m_groups);
        player->m_groups = NULL;
    }
    GcaxSequence *sequence = &player->m_sequence;
    u8 *file = (u8 *)table + (*entry & 0xFFFFFF);
    sequence->m_format = *(u16 *)(file + 0x10);
    memset(sequence, 0, sizeof(GcaxSequence));
    sequence->m_division = *(u16 *)(file + 0x1E);
    sequence->m_lastTime = sequence->m_startTime = lbl_805F1404->m_tickTime;
    sequence->m_tempo = lbl_805FAF68;
    sequence->m_messageFlags = flags;
    sequence->m_start = file + 0x28;
    sequence->m_cursor = file + 0x28;
    OSRestoreInterrupts(interrupt);
    return 0;
}

/* Stops the player's sequence. */
int fn_8043FD70(GcaxPlayer *player) {
    if ((player->m_flags & 0x80000000) == 0) {
        return -1;
    }
    u32 interrupt = OSDisableInterrupts();
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
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    u32 interrupt = OSDisableInterrupts();
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
    if ((player->m_flags & 0x80000000) == 0) {
        return;
    }
    u32 interrupt = OSDisableInterrupts();
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
    if (lbl_805F1404 == NULL) return 0x0D000001;
    u8 *object = (u8 *)(G8 + 0x13F6C);
    for (u32 i = 0; i < 0x40; i++) {
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckLoader(loader);
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckLoader(loader);
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int error = GcaxCheckLoader(loader);
    if (error != 0) {
        return error;
    }
    u32 *file = loader->m_file;
    u32 *command = file + 8;
    u32 *base = command;
    if (file[0] != 0x67636178 || file[1] != 0x4D4C5420 || file[4] != 0x67636178 ||
        file[5] != 0x4D4C544D) {
        return 0x9000001;
    }
    int result = 0;
    int count = file[7] >> 4;
    while (count-- > 0) {
        if (command[1] == 0xFFFFFFFF) {
            continue;
        }
        int status = -1;
        int bank = (command[1] >> 24) & 0xF;
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    GcaxPlayer *player = fn_8043E0E8();
    if (player == NULL) {
        return 0x7000101;
    }
    *out = player;
    return 0;
}

/* Frees a player. */
int fn_8043F60C(GcaxPlayer *player) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    va_list args;
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
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
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
    if (result != 0) {
        return result;
    }
    for (int i = 0; i < 16; i++) {
        fn_8043E228(player, i);
    }
    return 0;
}

int fn_8043F0D4(void) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    for (int i = 0; i < 8; i++) {
        fn_8043F13C(&lbl_805F1404->m_players[i]);
    }
    return 0;
}

/* Starts sequence `id` of sequence bank `bank` on the player. */
int fn_8043EFF0(GcaxPlayer *player, s8 bank, s8 id, s8 flags) {
    if (lbl_805F1404 == NULL) {
        return 0xD000001;
    }
    int result = GcaxCheckPlayer(player);
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
}
