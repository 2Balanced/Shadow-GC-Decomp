/* The story's event lists (file name invented; no class of the unit has RTTI): which events follow
 * which stage, the memory-card event list (UnkCard_801D3140, the vector of event records behind
 * fn_801D3140) that is filled from them and read back by the stage sequences and the goal action,
 * the event numbers of the stage voices, and a small registry of the tasks that are on screen, kept
 * in the object fn_80014B90 returns. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first); the vector's
 * members defined out of line come out at the end of the unit. */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_SWAP_GLOBAL
#define MSL_VECTOR_BACK
#define MSL_VECTOR_ERASE_ONE
#define MSL_VECTOR_ERASE_ONE_DATA
#define MSL_ALGORITHM_COPY
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>
#include <vector>

class Task;
class UnkObject_800B74B4;

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x88];
    /* 0x088 */ u32 field_0x88; /* bit 0 picks the voice event (meaning unknown) */
    /* 0x08C */ u8 field_0x8C[0xC0 - 0x8C];
    /* 0x0C0 */ u8 field_0xC0; /* (meaning unknown; see Game/ActStageController.cpp) */
    /* 0x0C1 */ u8 field_0xC1[0xCC - 0xC1];
    /* 0x0CC */ s32 m_mode; /* the event list is used in modes 1 and 7 (meaning inferred) */
    /* 0x0D0 */ u8 field_0xD0; /* set: no events (meaning unknown) */
    /* 0x0D1 */ u8 field_0xD1[0x150 - 0xD1];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The save data fn_800BB74C returns (class unknown; as far as this unit reads it). */
struct UnkSaveData_800BB74C {
    /* 0x00 */ u8 field_0x0[0x98];
    /* 0x98 */ s32 field_0x98; /* (meaning unknown) */
};

/* What fn_80176C3C returns (struct name unknown; as far as this unit reads it). */
struct UnkStage_80176C3C {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 m_flags; /* bit 2: a voice from the second table; bit 3: event 0x39E */
    /* 0x0C */ u8 field_0xC[0x4C - 0xC];
    /* 0x4C */ s32 field_0x4C; /* the column of the voice table */
};

/* An event and the value that goes with it (struct name unknown). */
struct UnkEventPair_8025D280 {
    /* 0x0 */ s32 m_event;
    /* 0x4 */ s32 m_value;
};

/* A memory-card event record (struct name unknown; see Game/GoalAction.cpp): an event pair, and
 * which list of the stage's events it came from (0 or 1). */
struct UnkCardSlot_8025D41C {
    /* 0x0 */ UnkEventPair_8025D280 m_pair;
    /* 0x8 */ s32 field_0x8;
};

/* The events that follow the stage m_to when it is reached from m_from (struct name unknown; m_type
 * 1 and 2 also need the save data's field_0x98 to be 3 and 5). */
struct UnkStageEvents_8025D504 {
    /* 0x00 */ s32 m_from;
    /* 0x04 */ s32 m_to;
    /* 0x08 */ s32 m_type;
    /* 0x0C */ const UnkEventPair_8025D280 *m_first[7];
    /* 0x28 */ const UnkEventPair_8025D280 *m_second[7];
};
SIZE_ASSERT(UnkStageEvents_8025D504, 0x44);

/* A task on screen (struct name unknown). */
struct UnkTaskEntry_8025D1C4 {
    /* 0x00 */ u8 m_used;
    /* 0x04 */ Task *m_task;
    /* 0x08 */ s32 m_kind;
    /* 0x0C */ u8 field_0xC[0x38 - 0xC];
};
SIZE_ASSERT(UnkTaskEntry_8025D1C4, 0x38);

/* The registry of the tasks on screen fn_80014B90 returns (struct name unknown). */
struct UnkTaskRegistry_80014B90 {
    /* 0x0000 */ UnkTaskEntry_8025D1C4 m_entries[128];
    /* 0x1C00 */ s32 m_cursor;
};

extern "C" {
UnkObject_800B74B4 *fn_800B74B4(void);
UnkStage_80176C3C *fn_80176C3C(UnkObject_800B74B4 *object, int index);
UnkSaveData_800BB74C *fn_800BB74C(void);
}

/* The memory-card event list (struct name unknown; see Game/Advertise.cpp). */
struct UnkCard_801D3140 {
    /* 0x0 */ std::vector<UnkCardSlot_8025D41C> m_slots;
};
SIZE_ASSERT(UnkCard_801D3140, 0xC);

/* Empties the event list (name inferred). */
extern "C" void fn_8025D6FC(UnkCard_801D3140 *card) {
    card->m_slots.clear();
}

/* Reported when no events follow a stage (name inferred; it does nothing). */
inline void ReportNoEvents(UnkObject_800B74B4 *a, UnkObject_800B74B4 *b) {}

/* The event pairs (0x805F77F8.., in .sdata2). */
static const UnkEventPair_8025D280 s_pair00 = {0x3E9, 0x0};
static const UnkEventPair_8025D280 s_pair01 = {0x44D, 0x388};
static const UnkEventPair_8025D280 s_pair02 = {0x7D1, 0x64};
static const UnkEventPair_8025D280 s_pair03 = {0x835, 0x64};
static const UnkEventPair_8025D280 s_pair04 = {0x836, 0xC9};
static const UnkEventPair_8025D280 s_pair05 = {0x899, 0x64};
static const UnkEventPair_8025D280 s_pair06 = {0x8A3, 0xD2};
static const UnkEventPair_8025D280 s_pair07 = {0x8FD, 0x386};
static const UnkEventPair_8025D280 s_pair08 = {0xBB9, 0x385};
static const UnkEventPair_8025D280 s_pair09 = {0xBC3, 0x136};
static const UnkEventPair_8025D280 s_pair10 = {0xC1D, 0x12D};
static const UnkEventPair_8025D280 s_pair11 = {0xC81, 0x12E};
static const UnkEventPair_8025D280 s_pair12 = {0xFA1, 0x387};
static const UnkEventPair_8025D280 s_pair13 = {0xFA2, 0x190};
static const UnkEventPair_8025D280 s_pair14 = {0x1005, 0x0};
static const UnkEventPair_8025D280 s_pair15 = {0x1006, 0x0};
static const UnkEventPair_8025D280 s_pair16 = {0x100F, 0x19A};
static const UnkEventPair_8025D280 s_pair17 = {0x1069, 0x0};
static const UnkEventPair_8025D280 s_pair18 = {0x106A, 0x5};
static const UnkEventPair_8025D280 s_pair19 = {0x10CD, 0x385};
static const UnkEventPair_8025D280 s_pair20 = {0x10D7, 0x19B};
static const UnkEventPair_8025D280 s_pair21 = {0x1131, 0x388};
static const UnkEventPair_8025D280 s_pair22 = {0x113B, 0x19C};
static const UnkEventPair_8025D280 s_pair23 = {0x1389, 0x38A};
static const UnkEventPair_8025D280 s_pair24 = {0x1393, 0x1FE};
static const UnkEventPair_8025D280 s_pair25 = {0x139D, 0x190};
static const UnkEventPair_8025D280 s_pair26 = {0x139E, 0x388};
static const UnkEventPair_8025D280 s_pair27 = {0x139F, 0x387};
static const UnkEventPair_8025D280 s_pair28 = {0x13A0, 0x0};
static const UnkEventPair_8025D280 s_pair29 = {0x13ED, 0x386};
static const UnkEventPair_8025D280 s_pair30 = {0x1451, 0x1F6};
static const UnkEventPair_8025D280 s_pair31 = {0x145B, 0x1FF};
static const UnkEventPair_8025D280 s_pair32 = {0x14B5, 0x1F6};
static const UnkEventPair_8025D280 s_pair33 = {0x14B6, 0x38A};
static const UnkEventPair_8025D280 s_pair34 = {0x14B5, 0x1F6};
static const UnkEventPair_8025D280 s_pair35 = {0x151A, 0x38A};
static const UnkEventPair_8025D280 s_pair36 = {0x151B, 0x389};
static const UnkEventPair_8025D280 s_pair37 = {0x1771, 0x388};
static const UnkEventPair_8025D280 s_pair38 = {0x17D5, 0x389};
static const UnkEventPair_8025D280 s_pair39 = {0x17D6, 0x38C};
static const UnkEventPair_8025D280 s_pair40 = {0x17D7, 0x1F5};
static const UnkEventPair_8025D280 s_pair41 = {0x17D8, 0x1F6};
static const UnkEventPair_8025D280 s_pair42 = {0x1839, 0x1F5};
static const UnkEventPair_8025D280 s_pair43 = {0x183A, 0x1F6};
static const UnkEventPair_8025D280 s_pair44 = {0x183B, 0x1F7};
static const UnkEventPair_8025D280 s_pair45 = {0x189D, 0x1FE};
static const UnkEventPair_8025D280 s_pair46 = {0x189E, 0x38B};
static const UnkEventPair_8025D280 s_pair47 = {0x189F, 0x1FE};
static const UnkEventPair_8025D280 s_pair48 = {0x1901, 0x389};
static const UnkEventPair_8025D280 s_pair49 = {0x1902, 0x1FE};
static const UnkEventPair_8025D280 s_pair50 = {0x1B59, 0x263};
static const UnkEventPair_8025D280 s_pair51 = {0x1B63, 0x263};
static const UnkEventPair_8025D280 s_pair52 = {0x1BBD, 0x262};
static const UnkEventPair_8025D280 s_pair53 = {0x1BC7, 0x262};
static const UnkEventPair_8025D280 s_pair54 = {0x1C21, 0x265};
static const UnkEventPair_8025D280 s_pair55 = {0x1C2B, 0x265};
static const UnkEventPair_8025D280 s_pair56 = {0x1C85, 0x264};
static const UnkEventPair_8025D280 s_pair57 = {0x1C8F, 0x264};
static const UnkEventPair_8025D280 s_pair58 = {0x1CE9, 0x266};
static const UnkEventPair_8025D280 s_pair59 = {0x1CF3, 0x266};
static const UnkEventPair_8025D280 s_pair60 = {0x1D4D, 0x266};
static const UnkEventPair_8025D280 s_pair61 = {0x1D57, 0x266};
static const UnkEventPair_8025D280 s_pair62 = {0x1DB1, 0x267};
static const UnkEventPair_8025D280 s_pair63 = {0x1DBB, 0x267};
static const UnkEventPair_8025D280 s_pair64 = {0x1E15, 0x268};
static const UnkEventPair_8025D280 s_pair65 = {0x1E1F, 0x268};
static const UnkEventPair_8025D280 s_pair66 = {0x1E79, 0x26A};
static const UnkEventPair_8025D280 s_pair67 = {0x1E83, 0x26A};
static const UnkEventPair_8025D280 s_pair68 = {0x1EDD, 0x269};
static const UnkEventPair_8025D280 s_pair69 = {0x1EE7, 0x269};
static const UnkEventPair_8025D280 s_pair70 = {0x1F41, 0x64};
static const UnkEventPair_8025D280 s_pair71 = {0x1F42, 0x38D};
static const UnkEventPair_8025D280 s_pair72 = {0x1F43, 0x0};
static const UnkEventPair_8025D280 s_pair73 = {0x1F44, 0x38D};
static const UnkEventPair_8025D280 s_pair74 = {0x1F45, 0x38B};
static const UnkEventPair_8025D280 s_pair75 = {0x1F46, 0x38D};
static const UnkEventPair_8025D280 s_pair76 = {0x1FA5, 0x38E};
static const UnkEventPair_8025D280 s_pair77 = {0x1FA6, 0x389};
static const UnkEventPair_8025D280 s_pair78 = {0x1FA7, 0x38E};
static const UnkEventPair_8025D280 s_pair79 = {0x1FA8, 0x388};
static const UnkEventPair_8025D280 s_pair80 = {0x1FA9, 0x38D};
static const UnkEventPair_8025D280 s_pair81 = {0x1FAA, 0x38E};
static const UnkEventPair_8025D280 s_pair82 = {0x2009, 0x0};
static const UnkEventPair_8025D280 s_pair83 = {0x200A, 0x38F};
static const UnkEventPair_8025D280 s_pair84 = {0x200B, 0x388};
static const UnkEventPair_8025D280 s_pair85 = {0x200C, 0x389};

/* The pairs in event order (fn_8025D280). */
static const UnkEventPair_8025D280 *s_pairs[86] = {
    &s_pair00, &s_pair01, &s_pair02, &s_pair03, &s_pair04, &s_pair05,
    &s_pair06, &s_pair07, &s_pair08, &s_pair09, &s_pair10, &s_pair11,
    &s_pair12, &s_pair13, &s_pair14, &s_pair15, &s_pair16, &s_pair17,
    &s_pair18, &s_pair19, &s_pair20, &s_pair21, &s_pair22, &s_pair23,
    &s_pair24, &s_pair25, &s_pair26, &s_pair27, &s_pair28, &s_pair29,
    &s_pair30, &s_pair31, &s_pair32, &s_pair33, &s_pair34, &s_pair35,
    &s_pair36, &s_pair37, &s_pair38, &s_pair39, &s_pair40, &s_pair41,
    &s_pair42, &s_pair43, &s_pair44, &s_pair45, &s_pair46, &s_pair47,
    &s_pair48, &s_pair49, &s_pair50, &s_pair51, &s_pair52, &s_pair53,
    &s_pair54, &s_pair55, &s_pair56, &s_pair57, &s_pair58, &s_pair59,
    &s_pair60, &s_pair61, &s_pair62, &s_pair63, &s_pair64, &s_pair65,
    &s_pair66, &s_pair67, &s_pair68, &s_pair69, &s_pair70, &s_pair71,
    &s_pair72, &s_pair73, &s_pair74, &s_pair75, &s_pair76, &s_pair77,
    &s_pair78, &s_pair79, &s_pair80, &s_pair81, &s_pair82, &s_pair83,
    &s_pair84, &s_pair85,
};

/* The events that follow each stage. */
static UnkStageEvents_8025D504 s_stageEvents[65] = {
    {-1, 5, 0, {&s_pair00}},
    {5, 6, 0, {&s_pair01, &s_pair02}},
    {5, 7, 0, {&s_pair01, &s_pair03, &s_pair04}},
    {5, 8, 0, {&s_pair01, &s_pair05}},
    {6, 9, 0, {&s_pair07, &s_pair08}},
    {6, 10, 0, {&s_pair07, &s_pair10}},
    {7, 9, 0, {&s_pair07, &s_pair08}},
    {7, 10, 0, {&s_pair07, &s_pair10}},
    {7, 11, 0, {&s_pair07, &s_pair11}},
    {8, 28, 0, {&s_pair06}},
    {28, 10, 0, {&s_pair07, &s_pair10}},
    {28, 11, 0, {&s_pair07, &s_pair11}},
    {9, 29, 0, {&s_pair09}},
    {29, 12, 0, {&s_pair12, &s_pair13}},
    {29, 13, 0, {&s_pair15}},
    {29, 14, 0, {&s_pair17, &s_pair18}},
    {10, 13, 0, {&s_pair14}},
    {10, 14, 0, {&s_pair17, &s_pair18}},
    {10, 15, 0, {&s_pair19}},
    {11, 14, 0, {&s_pair17, &s_pair18}},
    {11, 15, 0, {&s_pair19}},
    {11, 16, 0, {&s_pair21}},
    {12, 17, 0, {&s_pair23}},
    {12, 18, 0, {&s_pair29}},
    {13, 30, 0, {&s_pair16}},
    {30, 17, 0, {&s_pair23}},
    {30, 18, 0, {&s_pair29}},
    {30, 19, 0, {&s_pair30}},
    {14, 18, 0, {&s_pair29}},
    {14, 19, 0, {&s_pair30}},
    {14, 20, 0, {&s_pair32, &s_pair33}},
    {15, 31, 0, {&s_pair20}},
    {31, 19, 0, {&s_pair30}},
    {31, 20, 0, {&s_pair32, &s_pair33}},
    {31, 21, 0, {&s_pair34, &s_pair35, &s_pair36}},
    {16, 32, 0, {&s_pair22}},
    {32, 20, 0, {&s_pair32, &s_pair33}},
    {32, 21, 0, {&s_pair34, &s_pair35, &s_pair36}},
    {17, 33, 0, {&s_pair24}},
    {33, 22, 0, {&s_pair25, &s_pair26, &s_pair27, &s_pair28, &s_pair37}},
    {33, 23, 0, {&s_pair25, &s_pair26, &s_pair27, &s_pair38, &s_pair39}},
    {18, 22, 0, {&s_pair37}},
    {18, 23, 0, {&s_pair40, &s_pair39}},
    {18, 24, 0, {&s_pair42}},
    {19, 34, 0, {&s_pair31}},
    {34, 23, 0, {&s_pair41, &s_pair39}},
    {34, 24, 0, {&s_pair43}},
    {34, 25, 0, {&s_pair45, &s_pair46, &s_pair47}},
    {20, 24, 0, {&s_pair44}},
    {20, 25, 0, {&s_pair45, &s_pair46, &s_pair47}},
    {20, 26, 0, {&s_pair48, &s_pair49}},
    {21, 25, 0, {&s_pair45, &s_pair46, &s_pair47}},
    {21, 26, 0, {&s_pair48, &s_pair49}},
    {22, 36, 0, {&s_pair50}, {&s_pair51}},
    {22, 35, 0, {&s_pair52}, {&s_pair53}},
    {23, 38, 0, {&s_pair54}, {&s_pair55}},
    {23, 37, 0, {&s_pair56}, {&s_pair57}},
    {24, 39, 1, {&s_pair58}, {&s_pair59}},
    {24, 39, 0, {&s_pair60}, {&s_pair61}},
    {25, 40, 0, {&s_pair62}, {&s_pair63}},
    {25, 41, 0, {&s_pair64}, {&s_pair65}},
    {26, 43, 0, {&s_pair66}, {&s_pair67}},
    {26, 42, 0, {&s_pair68}, {&s_pair69}},
    {-1, 27, 0, {&s_pair70, &s_pair71, &s_pair72, &s_pair73, &s_pair74, &s_pair75}},
    {27, 44, 0, {&s_pair76, &s_pair77, &s_pair78, &s_pair79, &s_pair80, &s_pair81}, {&s_pair82, &s_pair83, &s_pair84, &s_pair85}},
};

/* The event of each stage voice, by voice and column (0: event 0x384). */
static s32 s_voiceEvents[13][8] = {
    {0x384, 0, 0x320, 0x321, 0x385, 0x322, 0, 0x323},
    {0x386, 0, 0x329, 0x32A, 0x387, 0x32B, 0x32C, 0x32D},
    {0x39A, 0, 0, 0, 0x39B, 0x335, 0, 0},
    {0x392, 0, 0, 0x331, 0x393, 0, 0x332, 0x333},
    {0x388, 0, 0, 0, 0x389, 0, 0, 0},
    {0x398, 0, 0, 0x32E, 0x399, 0, 0x32F, 0x330},
    {0x396, 0, 0, 0, 0x397, 0x336, 0x337, 0x338},
    {0x39C, 0, 0, 0, 0x39D, 0, 0x339, 0x33A},
    {0x390, 0, 0, 0, 0x391, 0, 0, 0},
    {0x394, 0, 0, 0, 0x395, 0, 0, 0},
    {0x38A, 0, 0, 0, 0x38B, 0, 0, 0},
    {0x38E, 0, 0, 0, 0x38F, 0, 0x334, 0},
    {0x38C, 0, 0x324, 0x325, 0x38D, 0x326, 0x327, 0x328},
};

/* The voice events of the second kind of stage (by field_0x88 bit 0; only column 1 is read). */
static s32 s_bossVoiceEvents[2][2] = {
    {0x3B6, 0x3C0},
    {0x3B7, 0x3C1},
};

/* Adds the events that follow stage to to the event list (name inferred). */
extern "C" void fn_8025D504(UnkCard_801D3140 *card, int from, int to) {
    if (lbl_8057D6A0.field_0xD0 != 0) {
        return;
    }
    if (lbl_8057D6A0.m_mode == 1 || lbl_8057D6A0.m_mode == 7) {
        const s32 *type;
        UnkStageEvents_8025D504 *events;
        int k;
        u32 i;
        int j;
        for (i = 0; i < 65; i++) {
            UnkStageEvents_8025D504 *entry = &s_stageEvents[i];
            if (from == entry->m_from && to == entry->m_to) {
                type = &entry->m_type;
                if (*type == 0) {
                    break;
                }
                if (*type == 1 && fn_800BB74C()->field_0x98 == 3) {
                    break;
                }
                if (*type == 2 && fn_800BB74C()->field_0x98 == 5) {
                    break;
                }
            }
        }
        if (i == 65) {
            ReportNoEvents(fn_800B74B4(), fn_800B74B4());
            return;
        }
        events = &s_stageEvents[i];
        for (j = 0; j < 7; j++) {
            const UnkEventPair_8025D280 *pair = events->m_first[j];
            if (pair != 0) {
                UnkCardSlot_8025D41C slot;
                slot.m_pair.m_event = pair->m_event;
                slot.m_pair.m_value = pair->m_value;
                slot.field_0x8 = 0;
                card->m_slots.push_back(slot);
            }
        }
        for (k = 0; k < 7; k++) {
            const UnkEventPair_8025D280 *pair = events->m_second[k];
            if (pair != 0) {
                UnkCardSlot_8025D41C slot;
                slot.m_pair.m_event = pair->m_event;
                slot.m_pair.m_value = pair->m_value;
                slot.field_0x8 = 1;
                card->m_slots.push_back(slot);
            }
        }
    }
}

/* Takes the first event of type (2: any) from the event list into slot (name inferred). */
extern "C" bool fn_8025D41C(UnkCard_801D3140 *card, UnkCardSlot_8025D41C *slot, int type, bool pop) {
    if (lbl_8057D6A0.field_0xD0 != 0) {
        return false;
    }
    if (lbl_8057D6A0.m_mode == 1 || lbl_8057D6A0.m_mode == 7) {
        if (card->m_slots.empty()) {
            return false;
        }
        UnkCardSlot_8025D41C &first = *card->m_slots.begin();
        if (type == 2 || type == first.field_0x8) {
            if (slot != 0) {
                *slot = first;
            }
            if (pop) {
                card->m_slots.erase(&first);
            }
            return true;
        }
    }
    return false;
}

/* Sets the list each event came from to value (name inferred). */
extern "C" void fn_8025D38C(UnkCard_801D3140 *card, int value) {
    for (UnkCardSlot_8025D41C *it = card->m_slots.begin(); it != card->m_slots.end(); it++) {
        it->field_0x8 = value;
    }
}

/* The event of stage voice id (name inferred). */
extern "C" s32 fn_8025D2C4(UnkCard_801D3140 *card, s32 id) {
    if (id < 0 || id > 12) {
        id = 0;
    }
    UnkStage_80176C3C *stage = fn_80176C3C(fn_800B74B4(), -1);
    if (stage != 0) {
        if (stage->m_flags & 4) {
            return s_bossVoiceEvents[lbl_8057D6A0.field_0x88 & 1][1];
        }
        if (stage->m_flags & 8) {
            return 0x39E;
        }
        s32 column = fn_80176C3C(fn_800B74B4(), -1)->field_0x4C;
        s32 event = s_voiceEvents[id][column];
        if (event == 0) {
            event = 0x384;
        }
        return event;
    }
    return 0;
}

/* The index of event in the pair table, or -1 (name inferred). */
extern "C" int fn_8025D280(int event) {
    for (int i = 0; i < 86; i++) {
        if (event == s_pairs[i]->m_event) {
            return i;
        }
    }
    return -1;
}

/* Set while the registry is being changed (name inferred). */
static u8 s_registryBusy;

/* Clears the registry (name inferred). */
extern "C" UnkTaskRegistry_80014B90 *fn_8025D240(UnkTaskRegistry_80014B90 *registry) {
    registry->m_cursor = 0;
    memset(registry, 0, sizeof(registry->m_entries));
    return registry;
}

/* Adds task to the registry (name inferred). */
extern "C" bool fn_8025D1C4(UnkTaskRegistry_80014B90 *registry, Task *task, int kind, const char *name) {
    s_registryBusy = 1;
    s32 start = registry->m_cursor;
    do {
        if (registry->m_entries[registry->m_cursor].m_used == 0) {
            registry->m_entries[registry->m_cursor].m_used = 1;
            UnkTaskEntry_8025D1C4 *entry = &registry->m_entries[registry->m_cursor];
            entry->m_task = task;
            entry->m_kind = kind;
            s_registryBusy = 0;
            return true;
        }
        registry->m_cursor++;
        if (registry->m_cursor >= 128) {
            registry->m_cursor = 0;
        }
    } while (registry->m_cursor != start);
    return false;
}

/* Removes task from the registry (name inferred). */
extern "C" bool fn_8025D144(UnkTaskRegistry_80014B90 *registry, Task *task) {
    s_registryBusy = 1;
    s32 start = registry->m_cursor;
    do {
        UnkTaskEntry_8025D1C4 *entry = &registry->m_entries[registry->m_cursor];
        if (entry->m_used != 0 && entry->m_task == task) {
            entry->m_used = 0;
            s_registryBusy = 0;
            return true;
        }
        registry->m_cursor++;
        if (registry->m_cursor > 128) {
            registry->m_cursor = 0;
        }
    } while (registry->m_cursor != start);
    s_registryBusy = 0;
    return false;
}

/* Whether a task of kind is on screen (name inferred). */
extern "C" bool fn_8025D104(UnkTaskRegistry_80014B90 *registry, int kind) {
    for (int i = 0; i < 128; i++) {
        if (registry->m_entries[i].m_used != 0 && kind == registry->m_entries[i].m_kind) {
            return true;
        }
    }
    return false;
}
