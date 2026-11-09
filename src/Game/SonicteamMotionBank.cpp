/* The motion library's motion banks (no class with RTTI; file name and all names inferred). A bank
 * holds a run of motion ids starting at its base id: each id has a record of its name, its motion
 * data (a header, then a chain of half-float key tracks, see Game/MotionHalfKeyTrack.cpp) and an
 * optional list of events. Registering a bank (fn_80413058) gives it free ids when it asks for
 * them, enters every motion's skeleton through the registry (Game/SonicteamMotionRegistry.cpp)
 * and then moves the bank into ARAM (fn_80413538): from then on its records are only described,
 * and a motion is read back into main memory on demand (fn_80413318) and kept with a reference
 * count until it is released. The event readers (fn_80411818, fn_80411DEC) walk a motion's
 * events one at a time and report the ones the current frame range passes.
 * Other units call these by their fn_ names, so they keep them; the file name is invented and
 * everything here is named from what the code does. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 11 of 18 functions match. fn_80413318 (80%): retail keeps a branch for the event
 * offset (desc->m_eventAddr ? addr & 0x1F : 0) where GC/2.7 always turns it into a subfic/subfe
 * mask (if/else, ternary, "init then if" and an inline helper all tried); that shifts its saved
 * registers too. fn_80412BFC (97%): GC/2.7 folds 4 + 0x20 of the name size into one subfic where
 * retail adds them separately (five spellings tried), and allocates the bare name below the
 * parameters. The rest differ only in register choice: fn_80411818 (99.4%) and fn_80411DEC
 * (98.3%) keep the range start in the event's own float register instead of a copy (ternary,
 * if/else, inline helpers by value and by pointer, a shared variable tried), and fn_80411DEC also
 * gives the hoisted (u16)type the top saved register; fn_80413058 (98.6%) loads the list head
 * through r0 and starts the record offset with li instead of a copy of the counter; fn_804124DC
 * (99%) swaps the bank and base registers of the inlined lookup; fn_80413538 (99%) computes the
 * owned bank's name size in r5 where retail uses r0. */
#include <dolphin/types.h>
#include <string.h>
#include "Game/MotionHalfKeyTrack.h"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp). */
namespace std {
float fabs(float x);
}

/* The frame range, speed and current frame of a motion (as in Game/MotionPlayWork.cpp). */
struct MotionFrames {
    /* 0x0 */ float m_start;
    /* 0x4 */ float m_end;
    /* 0x8 */ float m_speed;
    /* 0xC */ float m_frame;
};

/* An event of a motion: a range (bit 15 of the type) with its start and end frames, a point (bit
 * 14) with its frame, or neither; its parameters follow. A size of -1 ends the list. */
struct MotionBankEvent {
    /* 0x0 */ u16 m_type;
    /* 0x2 */ u16 m_size;
    /* 0x4 */ float m_start;
    /* 0x8 */ float m_end;
};

/* A motion's data (layout from the functions below). */
struct MotionBankData {
    /* 0x00 */ u8 m_version;
    /* 0x01 */ u8 m_bigEndian;
    /* 0x02 */ u8 field_0x2[2];
    /* 0x04 */ void *m_skeleton;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ u8 field_0xC;
    /* 0x0D */ u8 m_flags;
    /* 0x0E */ u8 field_0xE[2];
    /* 0x10 */ u16 m_start;
    /* 0x12 */ u16 m_end;
    /* 0x14 */ char m_name[0x20];
    /* 0x34 */ HalfKeyTrackData m_tracks[1];
};

/* What a bank knows of one motion. */
struct MotionBankRecord {
    /* 0x0 */ char *m_name;
    /* 0x4 */ MotionBankData *m_data;
    /* 0x8 */ MotionBankEvent *m_events;
};
SIZE_ASSERT(MotionBankRecord, 0xC);

/* A motion read back from ARAM, with its reference count. */
struct MotionBankEntry {
    /* 0x00 */ u16 m_id;
    /* 0x02 */ s16 m_refs;
    /* 0x04 */ MotionBankRecord m_record;
    /* 0x10 */ MotionBankEntry *m_next;
};
SIZE_ASSERT(MotionBankEntry, 0x14);

/* Where a motion's name, data and events are in ARAM, and their sizes. */
struct MotionBankAramDesc {
    /* 0x00 */ u32 m_nameAddr;
    /* 0x04 */ u32 m_nameSize;
    /* 0x08 */ u32 m_dataAddr;
    /* 0x0C */ u32 m_dataSize;
    /* 0x10 */ u32 m_eventAddr;
    /* 0x14 */ u32 m_eventSize;
};
SIZE_ASSERT(MotionBankAramDesc, 0x18);

/* A bank. A loaded bank file is one, with offsets for pointers until it is registered. */
struct MotionBank {
    /* 0x00 */ u16 m_base;
    /* 0x02 */ u16 m_count;
    /* 0x04 */ u8 m_owned;
    /* 0x05 */ u8 m_bigEndian;
    /* 0x06 */ u8 m_inAram;
    /* 0x07 */ u8 field_0x7;
    /* 0x08 */ void (*m_release)(void *data);
    /* 0x0C */ MotionBank *m_next;
    /* 0x10 */ MotionBankRecord *m_records;
    /* 0x14 */ u32 m_aram;
    /* 0x18 */ MotionBankEntry *m_entries;
    /* 0x1C */ MotionBankAramDesc m_descs[1];
};

extern "C" {
char *fn_803AC6C0(const char *s, const char *sub);                 /* strstr */
char *fn_803ACABC(char *dst, const char *src, unsigned long size); /* strncpy */
/* The library's memory (Game/SonicteamMotionMemory.cpp). */
void *fn_80411134(int kind, u32 size);
void fn_804110FC(void *data);
/* The registry entry called name (Game/SonicteamMotionRegistry.cpp). */
void *fn_804106C0(const char *name);
/* The host's byte order and swapping halfwords and words in place (Game/MotionLib.cpp). */
int fn_804211CC(void);
void fn_8042118C(void *dst, const void *src, int count);
void fn_804210E4(void *dst, const void *src, int count);
/* ARAM: allocate, free, write and read (Game/MotionAram.cpp). */
u32 fn_80421368(u32 size);
void fn_80421300(u32 addr);
void fn_80421270(u32 aram, void *mram, u32 length);
void fn_804211E4(void *mram, u32 aram, u32 length);

int fn_804139B4(MotionBankData *data);
MotionBank *fn_80413538(MotionBank *bank);
void fn_80413318(u16 id);
void fn_80413240(u16 id);
u16 fn_80413058(MotionBank *bank);
void fn_80412D7C(MotionBank *bank);
u16 fn_80412BFC(void *data, const char *path, void (*release)(void *data), int autoId);
u16 fn_80412B74(void *data, void (*release)(void *data), int autoId);
void fn_804129D4(u16 base);
MotionBankData *fn_80412920(u16 id);
char *fn_80412820(u16 id);
u16 fn_80412738(const char *name);
int fn_804125F4(MotionFrames *frames, u16 id);
HalfKeyTrackData *fn_804124DC(u16 id);
void *fn_804123DC(u16 id);
int fn_80411DEC(u16 id, u16 type, MotionFrames *frames, void *params, int stopped);
int fn_80411818(u16 id, u16 *type, MotionFrames *frames, void **params, int stopped);
void fn_80411714(void);
}

/* The registered banks. */
MotionBank *lbl_805F13B0;
/* The event the readers reported last. */
u8 *lbl_805F13B4;
/* The motions the two event readers hold, and two more ids that are forgotten with their bank. */
u16 lbl_805F13B8;
u16 lbl_805F13BA;
u16 lbl_805F13BC;
u16 lbl_805F13BE;

/* A name returned by fn_80412820. */
char lbl_805E11A0[0x100];
/* The frame ranges of the motions the two event readers hold. */
MotionFrames lbl_805E12A0;
MotionFrames lbl_805E12B0;

/* Whether data is in the other byte order. */
#define NEEDS_SWAP(bigEndian) (((bigEndian) && !fn_804211CC()) || (!(bigEndian) && fn_804211CC()))

/* Checks a motion's version and byte-swaps it when it is in the other byte order. */
inline int ConvertData(MotionBankData *data) {
    if (data->m_version >= 3) {
        if (NEEDS_SWAP(data->m_bigEndian)) {
            fn_804210E4(&data->m_skeleton, &data->m_skeleton, 1);
            fn_804210E4(&data->m_size, &data->m_size, 1);
            fn_8042118C(&data->m_start, &data->m_start, 1);
            fn_8042118C(&data->m_end, &data->m_end, 1);
            for (HalfKeyTrackData *track = data->m_tracks; track != 0;
                 track = (s8)track->field_0x7 ? (HalfKeyTrackData *)((u8 *)track + track->m_size) : 0) {
                fn_804210E4(&track->m_size, &track->m_size, 1);
                fn_8042118C(&track->m_count, &track->m_count, 1);
                s32 keySize = track->m_size - 8;
                if (data->m_flags & 1) {
                    fn_8042118C(track->m_keys, track->m_keys, keySize / 2);
                } else {
                    fn_804210E4(track->m_keys, track->m_keys, keySize / 4);
                }
            }
        }
        return 1;
    }
    return 0;
}

/* The record of a motion id, or 0. */
inline MotionBankRecord *FindRecord(u16 id) {
    if (lbl_805F13B0 == 0) {
        return 0;
    }
    for (MotionBank *bank = lbl_805F13B0; bank != 0; bank = bank->m_next) {
        if (bank->m_base <= (int)id && bank->m_base + bank->m_count > (int)id) {
            int index = id - bank->m_base;
            if (bank->m_inAram) {
                for (MotionBankEntry *entry = bank->m_entries; entry != 0; entry = entry->m_next) {
                    if (entry->m_id == id) {
                        return &entry->m_record;
                    }
                }
                return 0;
            }
            return &bank->m_records[index];
        }
    }
    return 0;
}

/* Drops a reference to a motion read back from ARAM, freeing it with the last one. */
inline void ReleaseRecord(u16 id) {
    MotionBank *bank = lbl_805F13B0;
    if (id == 0) {
        return;
    }
    for (; bank != 0; bank = bank->m_next) {
        if (bank->m_base <= id && id < bank->m_base + bank->m_count) {
            if (!bank->m_inAram) {
                return;
            }
            MotionBankEntry *prev = 0;
            for (MotionBankEntry *entry = bank->m_entries; entry != 0; entry = entry->m_next) {
                if (entry->m_id == id) {
                    if (--entry->m_refs == 0) {
                        if (prev != 0) {
                            prev->m_next = entry->m_next;
                        } else {
                            bank->m_entries = entry->m_next;
                        }
                        fn_804110FC(entry);
                    }
                    return;
                }
                prev = entry;
            }
            return;
        }
    }
}

/* Forgets the held motions that belong to bank. */
inline void ForgetHeld(MotionBank *bank) {
    if (lbl_805F13B8 >= bank->m_base && lbl_805F13B8 < bank->m_base + bank->m_count) {
        lbl_805F13B8 = 0;
    }
    if (lbl_805F13BA >= bank->m_base && lbl_805F13BA < bank->m_base + bank->m_count) {
        lbl_805F13BA = 0;
    }
    if (lbl_805F13BC >= bank->m_base && lbl_805F13BC < bank->m_base + bank->m_count) {
        lbl_805F13BC = 0;
    }
    if (lbl_805F13BE >= bank->m_base && lbl_805F13BE < bank->m_base + bank->m_count) {
        lbl_805F13BE = 0;
    }
}

/* Rounds a size up to a multiple of 32. */
#define ROUND_UP_32(size)                                                                          \
    {                                                                                              \
        s32 rem = (size) % 32;                                                                     \
        if (rem != 0) {                                                                            \
            (size) += 32 - rem;                                                                    \
        }                                                                                          \
    }

/* Checks a motion's data (converting it to the host's byte order); it must have all three
 * channels. */
int fn_804139B4(MotionBankData *data) {
    int result = 1;
    if (!ConvertData(data)) {
        return 0;
    }
    if (!(data->m_flags & 1)) {
        result = 0;
    }
    if (!(data->m_flags & 2)) {
        result = 0;
    }
    if (!(data->m_flags & 4)) {
        result = 0;
    }
    return result;
}

/* Moves a bank's names, data and events to ARAM; returns the bank that replaces it (bank itself
 * when that fails). */
MotionBank *fn_80413538(MotionBank *bank) {
    int i;
    MotionBank *aram = (MotionBank *)fn_80411134(1, bank->m_count * sizeof(MotionBankAramDesc) + 0x1C);
    if (aram == 0) {
        return bank;
    }
    MotionBankAramDesc *descs = aram->m_descs;
    if (bank->m_owned) {
        if (bank->m_count != 1) {
            return bank;
        }
        u32 nameStart = (u32)bank->m_records->m_name;
        if (nameStart & 0x1F) {
            nameStart -= nameStart & 0x1F;
        }
        descs->m_nameSize = strlen(bank->m_records->m_name) + 1;
        descs->m_nameAddr = (u32)bank->m_records->m_name - nameStart;
        s32 nameLength = (u32)bank->m_records->m_name + descs->m_nameSize - nameStart;
        ROUND_UP_32(nameLength);
        u32 dataStart = (u32)bank->m_records->m_data;
        if (dataStart & 0x1F) {
            dataStart -= dataStart & 0x1F;
        }
        descs->m_dataSize = bank->m_records->m_data->m_size;
        descs->m_dataAddr = nameLength + ((u32)bank->m_records->m_data - dataStart);
        s32 dataLength = (u32)bank->m_records->m_data + descs->m_dataSize - dataStart;
        ROUND_UP_32(dataLength);
        aram->m_aram = fn_80421368(nameLength + dataLength);
        if (aram->m_aram == 0) {
            fn_804110FC(aram);
            return bank;
        }
        fn_80421270(aram->m_aram, (void *)nameStart, nameLength);
        fn_80421270(aram->m_aram + nameLength, (void *)dataStart, dataLength);
    } else {
        u32 start = (u32)bank->m_records[0].m_name;
        if (start & 0x1F) {
            start -= start & 0x1F;
        }
        i = 0;
        u32 end = 0;
        for (; i < bank->m_count; i++) {
            if (bank->m_records[i].m_name != 0) {
                descs[i].m_nameSize = strlen(bank->m_records[i].m_name) + 1;
                descs[i].m_nameAddr = (u32)bank->m_records[i].m_name - start;
                u32 last = (u32)bank->m_records[i].m_name + descs[i].m_nameSize;
                if (end < last) {
                    end = last;
                }
            }
            if (bank->m_records[i].m_data != 0) {
                descs[i].m_dataSize = bank->m_records[i].m_data->m_size;
                descs[i].m_dataAddr = (u32)bank->m_records[i].m_data - start;
                u32 last = (u32)bank->m_records[i].m_data + descs[i].m_dataSize;
                if (end < last) {
                    end = last;
                }
            }
            MotionBankEvent *events = bank->m_records[i].m_events;
            if (events != 0) {
                descs[i].m_eventSize = 4;
                for (MotionBankEvent *event = events; (s16)event->m_size != -1;
                     event = (MotionBankEvent *)((u8 *)event + event->m_size)) {
                    descs[i].m_eventSize += event->m_size;
                }
                descs[i].m_eventAddr = (u32)bank->m_records[i].m_events - start;
                u32 last = (u32)bank->m_records[i].m_events + descs[i].m_eventSize;
                if (end < last) {
                    end = last;
                }
            }
        }
        s32 length = end - start;
        ROUND_UP_32(length);
        aram->m_aram = fn_80421368(length);
        if (aram->m_aram == 0) {
            fn_804110FC(aram);
            return bank;
        }
        fn_80421270(aram->m_aram, (void *)start, length);
    }
    aram->m_base = bank->m_base;
    aram->m_count = bank->m_count;
    aram->m_owned = bank->m_owned;
    aram->m_bigEndian = bank->m_bigEndian;
    aram->m_inAram = bank->m_inAram;
    aram->field_0x7 = bank->field_0x7;
    aram->m_release = bank->m_release;
    aram->m_next = bank->m_next;
    aram->m_records = bank->m_records;
    aram->m_records = 0;
    aram->m_inAram = 1;
    aram->m_owned = 0;
    aram->m_release = fn_804110FC;
    for (i = 0; i < aram->m_count; i++) {
        descs[i].m_nameAddr = aram->m_aram + descs[i].m_nameAddr;
        descs[i].m_dataAddr = aram->m_aram + descs[i].m_dataAddr;
        if (descs[i].m_eventAddr != 0) {
            descs[i].m_eventAddr = aram->m_aram + descs[i].m_eventAddr;
        }
    }
    for (MotionBank *other = lbl_805F13B0; other != 0; other = other->m_next) {
        if (other->m_next == bank) {
            other->m_next = aram;
            break;
        }
    }
    if (lbl_805F13B0 == bank) {
        lbl_805F13B0 = aram;
    }
    void (*release)(void *data) = bank->m_release;
    if (bank->m_owned) {
        if (release != 0) {
            release(bank->m_records->m_data);
        }
        fn_804110FC(bank);
    } else if (release != 0) {
        release(bank);
    }
    return aram;
}

/* Takes a reference to a motion, reading it back from ARAM when it is not in memory. */
void fn_80413318(u16 id) {
    MotionBankEntry *entry;
    s32 eventOffset;
    MotionBank *bank;
    MotionBankEntry *prev;
    MotionBankAramDesc *desc;
    s32 nameOffset;
    s32 nameLength;
    s32 dataOffset;
    s32 dataLength;
    s32 eventLength;
    u8 *buffer;
    u8 *cursor;
    MotionBankAramDesc *descs;
    MotionBankEntry *other;

    bank = lbl_805F13B0;
    if (id == 0) {
        return;
    }
    for (; bank != 0; bank = bank->m_next) {
        if (bank->m_base <= id && id < bank->m_base + bank->m_count) {
            if (!bank->m_inAram) {
                return;
            }
            descs = bank->m_descs;
            prev = 0;
            for (other = bank->m_entries; other != 0; other = other->m_next) {
                if (other->m_id == id) {
                    other->m_refs++;
                    return;
                }
                prev = other;
            }
            desc = &descs[id - bank->m_base];
            nameOffset = desc->m_nameAddr & 0x1F;
            dataOffset = desc->m_dataAddr & 0x1F;
            nameLength = nameOffset + desc->m_nameSize;
            dataLength = dataOffset + desc->m_dataSize;
            if (desc->m_eventAddr != 0) {
                eventOffset = desc->m_eventAddr & 0x1F;
            } else {
                eventOffset = 0;
            }
            eventLength = desc->m_eventSize != 0 ? eventOffset + desc->m_eventSize : 0;
            ROUND_UP_32(nameLength);
            ROUND_UP_32(dataLength);
            ROUND_UP_32(eventLength);
            entry = (MotionBankEntry *)fn_80411134(1, nameLength + dataLength + eventLength + 0x40);
            if (entry == 0) {
                return;
            }
            buffer = (u8 *)entry + 0x20;
            entry->m_refs = 1;
            entry->m_id = id;
            if ((u32)buffer & 0x1F) {
                buffer += 0x20 - ((u32)buffer & 0x1F);
            }
            fn_804211E4(buffer, desc->m_nameAddr - nameOffset, nameLength);
            entry->m_record.m_name = (char *)(buffer + nameOffset);
            cursor = buffer + nameLength;
            fn_804211E4(cursor, desc->m_dataAddr - dataOffset, dataLength);
            entry->m_record.m_data = (MotionBankData *)(cursor + dataOffset);
            if (desc->m_eventAddr != 0) {
                cursor += dataLength;
                fn_804211E4(cursor, desc->m_eventAddr - eventOffset, eventLength);
                entry->m_record.m_events = (MotionBankEvent *)(cursor + eventOffset);
            }
            if (prev != 0) {
                prev->m_next = entry;
            } else {
                bank->m_entries = entry;
            }
            return;
        }
    }
}

/* Drops a reference to a motion. */
void fn_80413240(u16 id) {
    ReleaseRecord(id);
}

/* Registers a bank (giving it the first free run of ids when its base is 0) and moves it to
 * ARAM; returns its base id, or 0. */
u16 fn_80413058(MotionBank *bank) {
    int i;
    MotionBank *last = lbl_805F13B0;
    if (last != 0) {
        for (MotionBank *other = last; other != 0; other = other->m_next) {
            if (other == bank) {
                return 0;
            }
        }
    }
    if (bank->m_base != 0) {
        if (last != 0) {
            for (MotionBank *other = last; other != 0; other = other->m_next) {
                last = other;
                if (other->m_base > bank->m_base && other->m_base - bank->m_count < bank->m_base) {
                    return 0;
                }
                if (other->m_base < bank->m_base && other->m_base + other->m_count > bank->m_base) {
                    return 0;
                }
            }
            last->m_next = bank;
        } else {
            lbl_805F13B0 = bank;
        }
    } else if (last != 0) {
        int id = 1;
        u16 used = 0;
        for (;;) {
            MotionBank *other;
            for (other = last; other != 0; other = other->m_next) {
                if (other->m_base <= id && other->m_base + other->m_count > id) {
                    id = used = other->m_base + other->m_count - 1;
                    break;
                }
            }
            if (other == 0 && id - used >= bank->m_count) {
                bank->m_base = used + 1;
                break;
            }
            if (id == 0xFFFF) {
                return 0;
            }
            id++;
        }
        last = lbl_805F13B0;
        while (last->m_next != 0) {
            last = last->m_next;
        }
        last->m_next = bank;
    } else {
        bank->m_base = 1;
        lbl_805F13B0 = bank;
    }
    for (i = 0; i < bank->m_count; i++) {
        bank->m_records[i].m_data->m_skeleton = fn_804106C0(bank->m_records[i].m_data->m_name);
    }
    return fn_80413538(bank)->m_base;
}

/* Turns a loaded bank file's offsets into pointers, byte-swapping what is in the other byte
 * order. */
void fn_80412D7C(MotionBank *bank) {
    int i;
    int swap = 0;
    if (NEEDS_SWAP(bank->m_bigEndian)) {
        swap = 1;
        fn_8042118C(&bank->m_base, &bank->m_base, 1);
        fn_8042118C(&bank->m_count, &bank->m_count, 1);
        fn_804210E4(&bank->m_records, &bank->m_records, 1);
    }
    bank->m_records = (MotionBankRecord *)((u32)bank->m_records + (u32)bank);
    for (i = 0; i < bank->m_count; i++) {
        if (swap) {
            fn_804210E4(&bank->m_records[i].m_name, &bank->m_records[i].m_name, 1);
            fn_804210E4(&bank->m_records[i].m_data, &bank->m_records[i].m_data, 1);
            fn_804210E4(&bank->m_records[i].m_events, &bank->m_records[i].m_events, 1);
        }
        bank->m_records[i].m_name = (char *)((u32)bank + (u32)bank->m_records[i].m_name);
        bank->m_records[i].m_data = (MotionBankData *)((u32)bank + (u32)bank->m_records[i].m_data);
        ConvertData(bank->m_records[i].m_data);
        if (bank->m_records[i].m_events != 0) {
            bank->m_records[i].m_events = (MotionBankEvent *)((u32)bank + (u32)bank->m_records[i].m_events);
            if (swap) {
                MotionBankEvent *event = bank->m_records[i].m_events;
                fn_8042118C(event, event, 2);
                while ((s16)event->m_size != -1) {
                    if (event->m_type & 0x8000) {
                        fn_8042118C(&event->m_start, &event->m_start, 2);
                    } else if (event->m_type & 0x4000) {
                        fn_8042118C(&event->m_start, &event->m_start, 1);
                    }
                    event = (MotionBankEvent *)((u8 *)event + event->m_size);
                    fn_8042118C(event, event, 2);
                }
            }
        }
    }
}

/* Registers one motion held in memory as a bank of its own, named after the file name of path
 * (without directory and extension); returns its id, or 0 (release frees the data then). */
u16 fn_80412BFC(void *data, const char *path, void (*release)(void *data), int autoId) {
    const char *separator = "/";
    if (!fn_804139B4((MotionBankData *)data)) {
        if (release != 0) {
            release(data);
        }
        return 0;
    }
    const char *name = path;
    char *found = fn_803AC6C0(path, separator);
    while (found != 0) {
        name = found + strlen(separator);
        found = fn_803AC6C0(name, separator);
    }
    char *dot = fn_803AC6C0(name, ".");
    int length;
    if (dot != 0) {
        length = dot - name + 1;
    } else {
        length = strlen(name) + 1;
    }
    MotionBank *bank = (MotionBank *)fn_80411134(1, length + (0x20 + (4 - length % 4)));
    bank->m_count = 1;
    bank->m_owned = 1;
    bank->m_release = release;
    bank->m_records = (MotionBankRecord *)&bank->m_aram;
    bank->m_records->m_name = (char *)(bank->m_records + 1);
    bank->m_records->m_data = (MotionBankData *)data;
    fn_803ACABC(bank->m_records->m_name, name, length - 1);
    if (autoId) {
        bank->m_base = 0;
    }
    u16 id = fn_80413058(bank);
    if (id == 0) {
        if (release != 0) {
            release(data);
        }
        fn_804110FC(bank);
    }
    return id;
}

/* Registers a loaded bank file; returns its base id, or 0 (release frees the file then). */
u16 fn_80412B74(void *data, void (*release)(void *data), int autoId) {
    MotionBank *bank = (MotionBank *)data;
    if (bank == 0) {
        return 0;
    }
    fn_80412D7C(bank);
    bank->m_release = release;
    if (autoId) {
        bank->m_base = 0;
    }
    u16 id = fn_80413058(bank);
    if (id == 0 && release != 0) {
        release(data);
    }
    return id;
}

/* Unregisters the bank whose base id is base. */
void fn_804129D4(u16 base) {
    if (lbl_805F13B0 == 0) {
        return;
    }
    MotionBank *prev = 0;
    for (MotionBank *bank = lbl_805F13B0; bank != 0; prev = bank, bank = bank->m_next) {
        if (bank->m_base == base) {
            if (prev != 0) {
                prev->m_next = bank->m_next;
            } else {
                lbl_805F13B0 = bank->m_next;
            }
            void (*release)(void *data) = bank->m_release;
            if (bank->m_inAram) {
                MotionBankEntry *entry = bank->m_entries;
                fn_80421300(bank->m_aram);
                while (entry != 0) {
                    MotionBankEntry *next = entry->m_next;
                    fn_804110FC(entry);
                    entry = next;
                }
                ForgetHeld(bank);
            }
            if (bank->m_owned) {
                if (release != 0) {
                    release(bank->m_records->m_data);
                }
                fn_804110FC(bank);
            } else if (release != 0) {
                release(bank);
            }
            return;
        }
    }
}

/* The data of a motion in memory, or 0. */
MotionBankData *fn_80412920(u16 id) {
    MotionBankRecord *record = FindRecord(id);
    if (record != 0) {
        return record->m_data;
    }
    return 0;
}

/* The name of a motion (in a buffer the next call overwrites), or 0. */
char *fn_80412820(u16 id) {
    char *name = 0;
    fn_80413318(id);
    MotionBankRecord *record = FindRecord(id);
    if (record != 0) {
        name = record->m_name;
    }
    if (name != 0) {
        strcpy(lbl_805E11A0, name);
        name = lbl_805E11A0;
    }
    return name;
}

/* The id of the motion called name, or 0. */
u16 fn_80412738(const char *name) {
    if (name == 0) {
        return 0;
    }
    if (lbl_805F13B0 == 0) {
        return 0;
    }
    int i;
    for (MotionBank *bank = lbl_805F13B0; bank != 0; bank = bank->m_next) {
        for (i = 0; i < bank->m_count; i++) {
            if (bank->m_inAram) {
                if (strcmp(name, fn_80412820(bank->m_base + i)) == 0) {
                    return i + bank->m_base;
                }
            } else if (strcmp(name, bank->m_records[i].m_name) == 0) {
                return i + bank->m_base;
            }
        }
    }
    return 0;
}

/* The frame range of a motion; 0 when it is not known. */
int fn_804125F4(MotionFrames *frames, u16 id) {
    int found = 0;
    fn_80413318(id);
    MotionBankData *data = fn_80412920(id);
    if (data != 0) {
        found = 1;
        frames->m_start = frames->m_frame = data->m_start;
        frames->m_end = data->m_end;
    }
    ReleaseRecord(id);
    return found;
}

/* The first key track of a motion, then (with id 0) each next one, then 0. */
HalfKeyTrackData *fn_804124DC(u16 id) {
    static HalfKeyTrackData *s_track = 0;
    if (id != 0) {
        MotionBankRecord *record = FindRecord(id);
        if (record != 0) {
            s_track = record->m_data->m_tracks;
        } else {
            s_track = 0;
        }
    } else if (s_track != 0) {
        if ((s8)s_track->field_0x7 != 0) {
            s_track = (HalfKeyTrackData *)((u8 *)s_track + s_track->m_size);
        } else {
            s_track = 0;
        }
    }
    return s_track;
}

/* The skeleton of a motion, or 0. */
void *fn_804123DC(u16 id) {
    void *skeleton = 0;
    fn_80413318(id);
    MotionBankData *data = fn_80412920(id);
    if (data != 0) {
        skeleton = data->m_skeleton;
    }
    ReleaseRecord(id);
    return skeleton;
}

/* A frame, or the default when it is negative. */
inline float FrameOr(float frame, const float *fallback) {
    if (frame < 0.0f) {
        return *fallback;
    }
    return frame;
}

/* Whether the frames passed the point time this step (or stand on it when stopped). */
inline int PassesPoint(MotionFrames *frames, MotionBankEvent *event, int stopped) {
    int hit = 0;
    if (std::fabs(frames->m_speed) <= 0.0f) {
        if (stopped && std::fabs(frames->m_frame - event->m_start) < 0.0001f) {
            hit = 1;
        }
    } else if (frames->m_speed > 0.0f) {
        if (frames->m_speed >= frames->m_end - frames->m_start) {
            if (frames->m_start <= event->m_start && frames->m_end >= event->m_start) {
                hit = 1;
            }
        } else {
            if ((frames->m_frame - frames->m_speed) < frames->m_start) {
                if (frames->m_start <= event->m_start && frames->m_frame >= event->m_start) {
                    hit = 1;
                } else if (frames->m_end - (frames->m_start - (frames->m_frame - frames->m_speed)) < event->m_start && frames->m_end >= event->m_start) {
                    hit = 1;
                }
            } else if ((frames->m_frame - frames->m_speed) < event->m_start && frames->m_frame >= event->m_start) {
                hit = 1;
            }
        }
    } else {
        if (frames->m_speed <= frames->m_start - frames->m_end) {
            if (frames->m_start <= event->m_start && frames->m_end >= event->m_start) {
                hit = 1;
            }
        } else {
            if ((frames->m_frame - frames->m_speed) > frames->m_end) {
                if (frames->m_frame <= event->m_start && frames->m_end >= event->m_start) {
                    hit = 1;
                } else {
                    float wrapped = frames->m_start + ((frames->m_frame - frames->m_speed) - frames->m_end);
                    if (frames->m_start <= event->m_start && wrapped > event->m_start) {
                        hit = 1;
                    }
                }
            } else if (frames->m_frame <= event->m_start && (frames->m_frame - frames->m_speed) > event->m_start) {
                hit = 1;
            }
        }
    }
    return hit;
}

/* The next event of type type of a motion (a new id starts the motion's list) that the frames
 * pass, with its parameters copied to params; 0 at the end of the list. */
int fn_80411DEC(u16 id, u16 type, MotionFrames *frames, void *params, int stopped) {
    static u8 *s_cursor;
    MotionBankEvent *event;
    int found = 0;
    lbl_805F13B4 = 0;
    if (id != 0) {
        s_cursor = 0;
        fn_80413318(id);
        MotionBankRecord *record = FindRecord(id);
        if (record != 0) {
            fn_804125F4(&lbl_805E12A0, id);
            s_cursor = (u8 *)record->m_events;
            ReleaseRecord(lbl_805F13B8);
            lbl_805F13B8 = id;
        }
    }
    if (s_cursor == 0) {
        return 0;
    }
    for (event = (MotionBankEvent *)s_cursor; (s16)event->m_size != -1;
         event = (MotionBankEvent *)(s_cursor += event->m_size)) {
        if (event->m_type == type) {
            if (type & 0x8000) {
                if (frames == 0) {
                    break;
                }
                float start = FrameOr(event->m_start, &lbl_805E12A0.m_start);
                float end = event->m_end;
                if (end < 0.0f) {
                    end = lbl_805E12A0.m_end;
                }
                if (frames->m_frame >= start && frames->m_frame <= end) {
                    if (event->m_size != 12 && params != 0) {
                        memcpy(params, s_cursor + 12, event->m_size - 12);
                    }
                    found = 1;
                    lbl_805F13B4 = s_cursor;
                    s_cursor += event->m_size;
                    break;
                }
            } else if (type & 0x4000) {
                if (frames == 0) {
                    break;
                }
                if (PassesPoint(frames, event, stopped)) {
                    if (event->m_size != 8 && params != 0) {
                        memcpy(params, s_cursor + 8, event->m_size - 8);
                    }
                    found = 1;
                    lbl_805F13B4 = s_cursor;
                    s_cursor += event->m_size;
                    break;
                }
            } else {
                if (event->m_size != 4 && params != 0) {
                    memcpy(params, s_cursor + 4, event->m_size - 4);
                }
                found = 1;
                lbl_805F13B4 = s_cursor;
                s_cursor += event->m_size;
                break;
            }
        }
    }
    if (!found) {
        s_cursor = 0;
        ReleaseRecord(lbl_805F13B8);
        lbl_805F13B8 = 0;
    }
    return found;
}

/* The next event of a motion (a new id starts the motion's list) that the frames pass, with its
 * type and a pointer to its parameters (0 when it has none); 0 at the end of the list. */
int fn_80411818(u16 id, u16 *type, MotionFrames *frames, void **params, int stopped) {
    static u8 *s_cursor;
    MotionBankEvent *event;
    int found = 0;
    lbl_805F13B4 = 0;
    if (id != 0) {
        s_cursor = 0;
        fn_80413318(id);
        MotionBankRecord *record = FindRecord(id);
        if (record != 0) {
            fn_804125F4(&lbl_805E12B0, id);
            s_cursor = (u8 *)record->m_events;
            ReleaseRecord(lbl_805F13BA);
            lbl_805F13BA = id;
        }
    }
    if (s_cursor == 0) {
        return 0;
    }
    for (event = (MotionBankEvent *)s_cursor; (s16)event->m_size != -1;
         event = (MotionBankEvent *)(s_cursor += event->m_size)) {
        *type = event->m_type;
        if (*type & 0x8000) {
            if (frames == 0) {
                break;
            }
            float start = FrameOr(event->m_start, &lbl_805E12B0.m_start);
            float end = event->m_end;
            if (end < 0.0f) {
                end = lbl_805E12B0.m_end;
            }
            if (frames->m_frame >= start && frames->m_frame <= end) {
                *params = 0;
                if (event->m_size != 12) {
                    *params = s_cursor + 12;
                }
                found = 1;
                lbl_805F13B4 = s_cursor;
                s_cursor += event->m_size;
                break;
            }
        } else if (*type & 0x4000) {
            if (frames == 0) {
                break;
            }
            if (PassesPoint(frames, event, stopped)) {
                *params = 0;
                if (event->m_size != 8) {
                    *params = s_cursor + 8;
                }
                found = 1;
                lbl_805F13B4 = s_cursor;
                s_cursor += event->m_size;
                break;
            }
        } else {
            *params = 0;
            if (event->m_size != 4) {
                *params = s_cursor + 4;
            }
            found = 1;
            lbl_805F13B4 = s_cursor;
            s_cursor += event->m_size;
            break;
        }
    }
    if (!found) {
        s_cursor = 0;
        ReleaseRecord(lbl_805F13BA);
        lbl_805F13BA = 0;
    }
    return found;
}

/* Frees every motion read back from ARAM. */
void fn_80411714(void) {
    for (MotionBank *bank = lbl_805F13B0; bank != 0; bank = bank->m_next) {
        if (bank->m_inAram) {
            MotionBankEntry *entry = bank->m_entries;
            while (entry != 0) {
                MotionBankEntry *next = entry->m_next;
                fn_804110FC(entry);
                entry = next;
            }
            bank->m_entries = 0;
            ForgetHeld(bank);
        }
    }
}


