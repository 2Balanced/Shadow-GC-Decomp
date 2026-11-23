#ifndef GAME_RW_R1_H
#define GAME_RW_R1_H

/* Types inferred by agent r1 (wave 40) for Game/RwCpp01..07 and RwCpp09.
 *
 * 0x8043AA7C onwards is not RenderWare: it is a GameCube AX sound library that calls itself
 * "AX SDLIB" in its messages (gcaxSndMalloc, gcaxNewAramBank; file tags 'gcax' + 'MSB ', 'MPBW',
 * 'MPBP'). The Gcax* names below are descriptive; only the strings quoted in the sources are
 * retail's. Offsets are from the disassembly. */

#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
u32 OSDisableInterrupts(void);
void OSRestoreInterrupts(u32);
}

/* Header of a block in the library's first-fit heap; the payload follows it. */
struct GcaxHeapNode {
    /* 0x00 */ u16 m_magic; // 'MF' free, 'MB' busy
    /* 0x02 */ u16 m_flags;
    /* 0x04 */ GcaxHeapNode *m_next;
    /* 0x08 */ u8 field_0x8[0x18];
};

#define GCAX_HEAP_FREE 0x4D46
#define GCAX_HEAP_BUSY 0x4D42

/* An ARQRequest (dolphin/arq.h) of the ARAM upload queue. */
struct GcaxArqRequest {
    /* 0x00 */ u8 data[0x20];
};

/* A chunk of a sound bank file: 'gcax', a type tag, and the payload size. */
struct GcaxChunk {
    /* 0x00 */ u32 m_magic; // 'gcax'
    /* 0x04 */ u32 m_type;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 m_size;
    /* 0x10 */ u8 m_data[1];
};

/* A reference inside a program bank ('MPBP' chunk payload): element count and byte offset from
 * the start of the payload. */
typedef u32 GcaxTableRef;
#define GCAX_REF_COUNT(ref) ((ref) >> 24)
#define GCAX_REF_OFFSET(ref) ((ref) & 0xFFFFFF)

struct GcaxProgramBank {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ GcaxTableRef m_first;
    /* 0x8 */ GcaxTableRef m_programs; // GcaxTableRef per program, each naming its layers
};

struct GcaxLayer {
    /* 0x0 */ GcaxTableRef m_regions;
    /* 0x4 */ u8 field_0x4[0xC];
};

struct GcaxRegion {
    /* 0x00 */ u8 field_0x0[8];
    /* 0x08 */ u8 m_keyLow;
    /* 0x09 */ u8 m_keyHigh;
    /* 0x0A */ u8 field_0xA[0x36];
};

struct GcaxAramBank {
    /* 0x0 */ u32 m_address;
    /* 0x4 */ u32 m_size;
};

struct GcaxMemoryBank {
    /* 0x0 */ void *m_data;
    /* 0x4 */ u32 m_size;
};

typedef void (*GcaxCallback)(void *);

/* Singly linked list node; a list is a pointer to the first node. */
struct GcaxListNode {
    /* 0x0 */ GcaxListNode *m_next;
    /* 0x4 */ void *m_item;
};

/* The AXPBMIX block of an AX voice (dolphin/ax.h). */
struct GcaxMix {
    u16 vL;
    u16 vDeltaL;
    u16 vR;
    u16 vDeltaR;
    u16 vAuxAL;
    u16 vDeltaAuxAL;
    u16 vAuxAR;
    u16 vDeltaAuxAR;
    u16 vAuxBL;
    u16 vDeltaAuxBL;
    u16 vAuxBR;
    u16 vDeltaAuxBR;
    u16 vAuxBS;
    u16 vDeltaAuxBS;
    u16 vS;
    u16 vDeltaS;
    u16 vAuxAS;
    u16 vDeltaAuxAS;
};

struct GcaxGroup;

/* An AXVPB (dolphin/ax.h), as far as this library reads it. */
struct GcaxAxVoice {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_userContext;
};

struct GcaxVoiceInfo {
    /* 0x000 */ u8 field_0x0[0x146];
    /* 0x146 */ u16 field_0x146;
};

struct GcaxVoice {
    /* 0x00 */ u32 m_flags;
    /* 0x04 */ u32 m_index;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float field_0x10; // gain of the region
    /* 0x14 */ float m_baseRatio; // sample rate ratio as 16.16
    /* 0x18 */ int m_bendRangeUp;
    /* 0x1C */ int m_bendRangeDown;
    /* 0x20 */ int field_0x20; // 0..15 level
    /* 0x24 */ int m_pan;
    /* 0x28 */ int field_0x28;
    /* 0x2C */ float m_panLeft;
    /* 0x30 */ float m_panRight;
    /* 0x34 */ union {
        GcaxVoiceInfo *field_0x34;
        GcaxAxVoice *m_axVoice; // the AXVPB
    };
    /* 0x38 */ u8 field_0x38[0x4C];
    /* 0x84 */ GcaxMix m_mix;
    /* 0xA8 */ float m_time;
    /* 0xAC */ float m_level; // envelope level, 0..1
    /* 0xB0 */ float m_decibels; // the level as attenuation
    /* 0xB4 */ u8 field_0xB4[0x10];
    /* 0xC4 */ float m_sustainDecibels;
    /* 0xC8 */ float m_attackRate;
    /* 0xCC */ float m_decayRate;
    /* 0xD0 */ float m_sustainRate;
    /* 0xD4 */ float m_releaseRate;
    /* 0xD8 */ int m_state; // envelope stage: 0 attack, 1 decay, 3 sustain, 5 release
    /* 0xDC */ int field_0xDC;
    /* 0xE0 */ int field_0xE0; // 0..15 level of the aux A send
    /* 0xE4 */ u32 field_0xE4; // 0xE4..0xFC: the arguments fn_8043CA7C started the voice with
    /* 0xE8 */ u32 field_0xE8;
    /* 0xEC */ u32 field_0xEC;
    /* 0xF0 */ u32 field_0xF0;
    /* 0xF4 */ int field_0xF4;
    /* 0xF8 */ u32 field_0xF8;
    /* 0xFC */ u32 field_0xFC;
    /* 0x100 */ int m_velocity;
    /* 0x104 */ u32 field_0x104;
    /* 0x108 */ GcaxGroup *m_group;
};

struct GcaxChannel {
    /* 0x00 */ u32 field_0x0; // bit 0: sustain pedal down
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ int m_bank;
    /* 0x0C */ int m_program;
    /* 0x10 */ u32 m_volume;
    /* 0x14 */ u32 m_expression;
    /* 0x18 */ u32 m_pan;
    /* 0x1C */ u32 m_bendRaw; // 14-bit pitch bend, 0x2000 = centre
    /* 0x20 */ int m_depth; // above 0x40 the voice goes to aux B instead of the main bus
    /* 0x24 */ int field_0x24; // surround span, negative = off
    /* 0x28 */ u32 field_0x28;
    /* 0x2C */ int m_bend; // m_bendRaw - 0x2000
    /* 0x30 */ float m_gainRight;
    /* 0x34 */ float m_gainLeft;
    /* 0x38 */ u32 field_0x38;
    /* 0x3C */ float field_0x3C;
    /* 0x40 */ float field_0x40;
    /* 0x44 */ u32 field_0x44;
};

/* A linear fade of a player value, stepped every 5 ms. */
struct GcaxFade {
    /* 0x0 */ float m_target;
    /* 0x4 */ float m_start;
    /* 0x8 */ float m_duration; // milliseconds
    /* 0xC */ float m_remaining; // negative when idle
};

/* The state of a player's sequence (a standard MIDI file track): cleared when a sequence starts
 * or stops. Times are in ticks unless named otherwise. */
struct GcaxSequence {
    /* 0x00 */ u16 m_format;
    /* 0x04 */ u32 m_division; // ticks per quarter note
    /* 0x08 */ float m_eventTick; // tick of the next event
    /* 0x0C */ float m_previousEventTick;
    /* 0x10 */ float m_tick; // where the player is
    /* 0x14 */ float m_previousTick;
    /* 0x18 */ float m_tempo; // quarter notes per minute
    /* 0x1C */ float m_loopTempo;
    /* 0x20 */ s64 m_startTime; // OSGetTime() ticks
    /* 0x28 */ s64 m_lastTime;
    /* 0x30 */ u8 m_status; // running status
    /* 0x34 */ float m_beatSeconds;
    /* 0x38 */ float m_step; // ticks advanced by the last update
    /* 0x3C */ u32 m_messageFlags; // low byte goes into every queued message
    /* 0x40 */ u8 *m_start;
    /* 0x44 */ u8 *m_cursor; // NULL at the end
    /* 0x48 */ u8 *m_loopCursor;
    /* 0x4C */ float m_loopEventTick;
    /* 0x50 */ float m_loopTick;
    /* 0x54 */ int m_loopCount;
};

/* A sequence player. */
struct GcaxPlayer {
    /* 0x00 */ u32 m_flags; // bit 31: in use
    /* 0x04 */ u32 m_state; // bit 0: paused, bit 1: playing
    /* 0x08 */ float m_volume;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ GcaxFade m_volumeFade;
    /* 0x24 */ float m_speed;
    /* 0x28 */ float field_0x28;
    /* 0x2C */ GcaxFade m_speedFade;
    /* 0x3C */ float field_0x3C; // added to every voice's aux A send
    /* 0x40 */ s64 m_pauseTime;
    /* 0x48 */ GcaxSequence m_sequence;
    /* 0xA0 */ GcaxListNode *m_groups;
    /* 0xA4 */ GcaxChannel m_channels[16];
    /* 0x524 */ u32 m_queue[0x400]; // pending messages, see fn_8043E004
    /* 0x1524 */ int m_queueRead;
    /* 0x1528 */ int m_queueWrite;
    /* 0x152C */ int m_queueCount;
};

/* A handle for loading the banks a 'gcax' 'MLT ' file lists (fn_8043F70C). */
struct GcaxLoader {
    /* 0x00 */ u8 m_used;
    /* 0x01 */ s8 m_request; // ARAM request being waited for, or -1
    /* 0x04 */ u32 *m_file;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ GcaxCallback m_callback;
    /* 0x10 */ void *m_callbackData;
};

/* A group of voices started together (one of 64); list nodes of an owner carry these. */
struct GcaxGroup {
    /* 0x00 */ u8 m_used;
    /* 0x01 */ u8 m_voiceCount;
    /* 0x02 */ u8 m_channel;
    /* 0x03 */ u8 m_key;
    /* 0x04 */ u8 m_velocity;
    /* 0x08 */ int m_priority;
    /* 0x0C */ GcaxPlayer *m_owner;
    /* 0x10 */ GcaxListNode *m_voices;
};

/* The four AX aux effects the library runs (the Dolphin SDK's AXFX reverb, high quality reverb,
 * chorus and delay), as far as their parameters go; the rest is the effect's own state. */
struct GcaxReverbStd {
    /* 0x000 */ u8 m_state[0x13C];
    /* 0x13C */ u8 m_tempDisableFX;
    /* 0x140 */ float m_coloration;
    /* 0x144 */ float m_mix;
    /* 0x148 */ float m_time;
    /* 0x14C */ float m_damping;
    /* 0x150 */ float m_preDelay;
};

struct GcaxReverbHi {
    /* 0x000 */ u8 m_state[0x1C4];
    /* 0x1C4 */ u8 m_tempDisableFX;
    /* 0x1C8 */ float m_coloration;
    /* 0x1CC */ float m_mix;
    /* 0x1D0 */ float m_time;
    /* 0x1D4 */ float m_damping;
    /* 0x1D8 */ float m_preDelay;
    /* 0x1DC */ float m_crosstalk;
};

struct GcaxChorus {
    /* 0x00 */ u8 m_state[0x90];
    /* 0x90 */ u32 m_baseDelay;
    /* 0x94 */ u32 m_variation;
    /* 0x98 */ u32 m_period;
};

struct GcaxDelay {
    /* 0x00 */ u8 m_state[0x3C];
    /* 0x3C */ u32 m_delay[3];
    /* 0x48 */ u32 m_feedback[3];
    /* 0x54 */ u32 m_output[3];
};

/* The library's state block (lbl_805F1404 points at it). */
struct GcaxGlobals {
    /* 0x0000 */ int m_initialized;
    /* 0x0004 */ u32 field_0x4;
    /* 0x0008 */ int m_lock; // above 1 while an API call is in progress: the tick is skipped
    /* 0x000C */ int m_activeVoices;
    /* 0x0010 */ int m_activeGroups;
    /* 0x0014 */ u32 field_0x14;
    /* 0x0018 */ s64 m_tickTime;
    /* 0x0020 */ s64 m_lastTickTime;
    /* 0x0028 */ u32 field_0x28;
    /* 0x002C */ float m_masterDecibels;
    /* 0x0030 */ u8 field_0x30[8];
    /* 0x0038 */ int m_voiceLimit;
    /* 0x003C */ int m_axVoiceCount; // voices that hold an AX voice
    /* 0x0040 */ int m_ticks; // ticks run from the AX callback
    /* 0x0044 */ int m_pauseVoices; // 1: pausing a player pauses its voices instead of freeing them
    /* 0x0048 */ u8 field_0x48[4];
    /* 0x004C */ int m_nextGroup; // where the search for a group to free starts
    /* 0x0050 */ int m_nextVoice;
    /* 0x0054 */ GcaxVoice m_voices[64];
    /* 0x4354 */ GcaxGroup m_groups[64];
    /* 0x4854 */ GcaxHeapNode *m_heap;
    /* 0x4858 */ u32 m_heapSize;
    /* 0x485C */ GcaxArqRequest m_requests[64];
    /* 0x505C */ volatile u8 m_requestBusy[64];
    /* 0x509C */ u32 m_arqOwner;
    /* 0x50A0 */ GcaxCallback m_requestCallbacks[64];
    /* 0x51A0 */ void *m_requestData[64];
    /* 0x52A0 */ u32 field_0x52A0;
    /* 0x52A4 */ u32 m_aramSize; // ARAM the library was given
    /* 0x52A8 */ int m_aramFree;
    /* 0x52AC */ u32 m_aramNext;
    /* 0x52B0 */ u32 m_aramBase;
    /* 0x52B4 */ u32 m_aramZero; // 0x500 bytes of silence in ARAM
    /* 0x52B8 */ u32 m_config[16];
    /* 0x52F8 */ GcaxAramBank m_aramBanks[16];
    /* 0x5378 */ GcaxMemoryBank m_programBanks[16];
    /* 0x53F8 */ GcaxMemoryBank m_sequenceBanks[16];
    /* 0x5478 */ GcaxPlayer m_players[8];
    /* 0xFDF8 */ u8 field_0xFDF8[2];
    /* 0xFDFA */ u8 m_mono;
    /* 0xFDFB */ u8 m_surround;
    /* 0xFDFC */ GcaxGroup *m_currentGroup; // argument of the list predicate fn_8043B810
    /* 0xFE00 */ u8 field_0xFE00[4];
    /* 0xFE04 */ GcaxReverbStd m_reverbStd;
    /* 0xFF58 */ GcaxReverbHi m_reverbHi;
    /* 0x10138 */ GcaxChorus m_chorus;
    /* 0x101D4 */ GcaxDelay m_delay;
    /* 0x10234 */ u8 field_0x10234[0x3804];
    /* 0x13A38 */ u8 field_0x13A38;
    /* 0x13A39 */ u8 field_0x13A39;
    /* 0x13A3A */ u8 field_0x13A3A;
    /* 0x13A3B */ u8 m_missedTicks; // ticks that found the library locked
    /* 0x13A3C */ u8 field_0x13A3C[8];
    /* 0x13A44 */ float field_0x13A44; // sequence speed factor
    /* 0x13A48 */ u8 field_0x13A48[0x524]; // holds a 32-byte aligned 0x500-byte ARAM upload buffer
    /* 0x13F6C */ GcaxLoader m_loaders[64];
    /* 0x1446C */ u8 m_loaderWait; // the wait flag the loaders pass to the bank loaders
};

SIZE_ASSERT(GcaxVoice, 0x10C);
SIZE_ASSERT(GcaxChannel, 0x48);
SIZE_ASSERT(GcaxSequence, 0x58);
SIZE_ASSERT(GcaxPlayer, 0x1530);
SIZE_ASSERT(GcaxLoader, 0x14);
SIZE_ASSERT(GcaxGroup, 0x14);
SIZE_ASSERT(GcaxReverbStd, 0x154);
SIZE_ASSERT(GcaxReverbHi, 0x1E0);
SIZE_ASSERT(GcaxChorus, 0x9C);
SIZE_ASSERT(GcaxDelay, 0x60);
SIZE_ASSERT(GcaxGlobals, 0x14470);

extern GcaxGlobals *lbl_805F1404;

/* Error code for a player handle: null, not one of the eight players, or not in use. */
static inline int GcaxCheckPlayer(GcaxPlayer *player) {
    int result;
    if (player == NULL) {
        result = 0x7000103;
    } else {
        for (int i = 0; i < 8; i++) {
            if (player == &lbl_805F1404->m_players[i]) {
                if ((int)player->m_flags != 0) {
                    result = 0;
                    goto done;
                }
                break;
            }
        }
        result = 0x7000102;
    }
done:
    return result;
}

/* The same for a loader handle. */
static inline int GcaxCheckLoader(GcaxLoader *loader) {
    int result;
    if (loader == NULL) {
        result = 0x7000103;
    } else {
        for (int i = 0; i < 64; i++) {
            if (loader == &lbl_805F1404->m_loaders[i]) {
                if (loader->m_used != 0) {
                    result = 0;
                    goto done;
                }
                break;
            }
        }
        result = 0x7000102;
    }
done:
    return result;
}

/* While the lock count is above 1 the 5 ms tick only counts a missed tick. */
static inline void GcaxLock() {
    u32 interrupt = OSDisableInterrupts();
    lbl_805F1404->m_lock++;
    OSRestoreInterrupts(interrupt);
}

static inline void GcaxUnlock() {
    u32 interrupt = OSDisableInterrupts();
    lbl_805F1404->m_lock--;
    OSRestoreInterrupts(interrupt);
}

/* RenderWare (from 0x80440410 on): the public API's basic types, as far as these units use them. */
struct R1V3d {
    float x;
    float y;
    float z;
};

struct R1Sphere {
    R1V3d center;
    float radius;
};

struct R1BBox {
    R1V3d sup;
    R1V3d inf;
};

/* RenderWare is C: assigning an RwBBox copies its 24 bytes flat, two words at a time, where C++
 * copies the two vectors member by member. Box copies go through this to keep C's code. */
struct R1BBoxBits {
    float f[6];
};
#define R1_COPY_BBOX(dst, src) (*(R1BBoxBits *)&(dst) = *(const R1BBoxBits *)&(src))

/* The same for a sphere. */
struct R1SphereBits {
    float f[4];
};

struct R1MorphTarget {
    /* 0x00 */ void *m_parentGeometry;
    /* 0x04 */ R1Sphere m_boundingSphere;
    /* 0x14 */ R1V3d *m_vertices;
    /* 0x18 */ R1V3d *m_normals;
};

/* The two objects plugin 0x11D extends (fn_80441028), as far as these units read them: a
 * geometry (object type 8) and a world sector. */
struct R1Geometry {
    /* 0x00 */ u8 m_type;
    /* 0x01 */ u8 field_0x1[0x5B];
    /* 0x5C */ R1MorphTarget *m_morphTarget;
};

/* A frame hierarchy (plugin 0x11E, fn_80447200), as far as these units read it. */
struct R1HierarchyNode {
    /* 0x0 */ int m_id;
    /* 0x4 */ int m_index;
    /* 0x8 */ int m_flags;
    /* 0xC */ void *m_frame;
};

struct R1Hierarchy;
struct R1Hierarchy {
    /* 0x00 */ int m_flags;
    /* 0x04 */ int m_nodeCount;
    /* 0x08 */ void *m_matrices;
    /* 0x0C */ void *m_matricesUnaligned;
    /* 0x10 */ R1HierarchyNode *m_nodes;
    /* 0x14 */ void *m_parentFrame;
    /* 0x18 */ R1Hierarchy *m_parentHierarchy;
    /* 0x1C */ int m_rootParentOffset;
    /* 0x20 */ void *m_interpolator;
};

/* Plugin 0x11E's data in a frame. */
struct R1FrameHierarchyData {
    /* 0x0 */ int m_id;
    /* 0x4 */ R1Hierarchy *m_hierarchy;
};

struct R1WorldSector {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ R1BBox m_boundingBox;
};

#endif
