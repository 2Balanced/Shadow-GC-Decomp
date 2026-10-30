/* The save data (struct name unknown; fn_800BB74C returns it): its constructor, its reset, its
 * copies, and the lookups of the sound test entries, the movies and the story routes it unlocks.
 * The holder (fn_801E559C) keeps two copies: the current data and the one last saved. No RTTI names
 * this unit; the file is named after the struct. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 * The bit sets and flags at the end of the unit are out-of-line copies of small inline functions;
 * other units call them by their fn_ names.
 *
 * NonMatching: 26 of 27 functions match. In fn_801E5040 (99.3%) GC/2.7 gives the loaded stage
 * number the volatile register r9 ahead of the two table addresses, retail gives it r12 after them
 * (8 instructions differ only by register numbers); about a dozen source shapes were tried. */
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

/* A time split into minutes, seconds and hundredths (class name unknown; see
 * Game/UnkTime_801E46E4.cpp). */
class UnkTime_801E46E4 {
  public:
    UnkTime_801E46E4();
    UnkTime_801E46E4(s8 minutes, s8 seconds, s8 hundredths) {
        Set(minutes, seconds, hundredths);
    }

    UnkTime_801E46E4 &operator=(const UnkTime_801E46E4 &other) {
        m_time = other.m_time;
        Split(m_time);
        return *this;
    }

    void Set(s8 minutes, s8 seconds, s8 hundredths);
    void Split(float time);

    /* 0x0 */ s32 m_minutes;
    /* 0x4 */ s8 m_seconds;
    /* 0x5 */ s8 m_hundredths;
    /* 0x8 */ float m_time;
};
SIZE_ASSERT(UnkTime_801E46E4, 0xC);

/* A set of 512 bits (class name unknown). */
struct UnkBits512_801E5734 {
    UnkBits512_801E5734();

    /* 0x0 */ u32 m_words[16];
};

/* A set of 256 bits (class name unknown). */
struct UnkBits256_801E57B8 {
    UnkBits256_801E57B8();

    /* 0x0 */ u32 m_words[8];
};

/* Sixteen flags (class name unknown; see Game/GoalAction.cpp). */
struct UnkFlags_801E5810 {
    UnkFlags_801E5810();

    /* 0x0 */ u16 m_bits;
};

/* The best result of one mission of a stage (member names inferred). */
struct UnkMissionSave_800BB74C {
    /* 0x00 */ u8 m_cleared;
    /* 0x04 */ s32 m_rank; /* 0 is the best rank */
    /* 0x08 */ UnkTime_801E46E4 m_time;
    /* 0x14 */ s32 m_score;
};
SIZE_ASSERT(UnkMissionSave_800BB74C, 0x18);

/* Five words, copied as one block. */
struct UnkWords5_800BB74C {
    /* 0x0 */ s32 m_words[5];
};

/* What the save data keeps for one stage (member names inferred). */
struct UnkStageSave_800BB74C {
    /* 0x00 */ s32 m_plays;
    /* 0x04 */ UnkMissionSave_800BB74C m_missions[3];
    /* 0x4C */ UnkWords5_800BB74C m_keys; /* -1 when not found (meaning inferred) */
};
SIZE_ASSERT(UnkStageSave_800BB74C, 0x60);

/* The stages played in a row, copied as one block. */
struct UnkRouteEntries_800BB74C {
    /* 0x00 */ s32 m_entries[24];
};

/* The record of the story (member names inferred; see Game/GoalAction.cpp). */
struct UnkPlayHistory_800BB74C {
    UnkPlayHistory_800BB74C();

    /* 0x0000 */ s32 m_count;
    /* 0x0004 */ UnkRouteEntries_800BB74C m_route;
    /* 0x0064 */ s32 m_lives;
    /* 0x0068 */ s32 m_result;
    /* 0x006C */ s32 m_plays;
    /* 0x0070 */ s32 m_lastLives;
    /* 0x0074 */ UnkFlags_801E5810 m_flags;
    /* 0x0078 */ UnkStageSave_800BB74C m_stages[59];
    /* 0x1698 */ u8 field_0x1698;
    /* 0x1699 */ u8 m_allBest; /* every mission has the best rank */
    /* 0x169C */ s32 m_bestCount;
    /* 0x16A0 */ UnkBits512_801E5734 m_routes;    /* the story routes seen */
    /* 0x16E0 */ UnkFlags_801E5810 m_endings;     /* the endings seen (meaning inferred) */
    /* 0x16E4 */ UnkBits512_801E5734 m_soundTest; /* the sound test entries unlocked */
    /* 0x1724 */ UnkBits256_801E57B8 field_0x1724;
};
SIZE_ASSERT(UnkPlayHistory_800BB74C, 0x1744);

/* The options (member names inferred). */
struct UnkOptions_800BB74C {
    /* 0x00 */ s32 m_language;
    /* 0x1C */ s32 m_voiceLanguage;
    /* 0x20 */ s32 m_soundMode;
    /* 0x0C */ u8 m_vibration;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ s32 field_0x14;
};
SIZE_ASSERT(UnkOptions_800BB74C, 0x18);

/* Two words (struct name unknown). */
struct UnkPair_800BB74C {
    /* 0x0 */ s32 field_0x0;
    /* 0x4 */ s32 field_0x4;
};

/* The start of the save data (struct name unknown; member names inferred). */
struct UnkSaveHeader_800BB74C {
    /* 0x00 */ UnkPair_800BB74C field_0x0;
    /* 0x08 */ UnkPair_800BB74C field_0x8;
    /* 0x10 */ UnkPair_800BB74C field_0x10;
    /* 0x18 */ UnkOptions_800BB74C m_options;
};
SIZE_ASSERT(UnkSaveHeader_800BB74C, 0x30);

/* The end of the save data (struct name unknown; contents unknown). */
struct UnkSaveBlock_800BB74C {
    /* 0x000 */ s32 field_0x0;
    /* 0x004 */ u8 field_0x4[0x7DC];
    /* 0x7E0 */ UnkPair_800BB74C field_0x7E0;
};
SIZE_ASSERT(UnkSaveBlock_800BB74C, 0x7E8);

struct UnkSaveData_800BB74C {
    UnkSaveData_800BB74C();

    /* 0x0000 */ UnkSaveHeader_800BB74C m_header;
    /* 0x0030 */ UnkPlayHistory_800BB74C m_history;
    /* 0x1774 */ UnkSaveBlock_800BB74C field_0x1774;
};
SIZE_ASSERT(UnkSaveData_800BB74C, 0x1F5C);

/* The current save data and the copy last saved (member names inferred). */
struct UnkSaveHolder_801E559C {
    UnkSaveHolder_801E559C();

    /* 0x0000 */ UnkSaveData_800BB74C m_current;
    /* 0x1F5C */ UnkSaveData_800BB74C m_saved;
};

/* An entry of the story route table: which of the route's stage choices each of its seven stages
 * took (struct name unknown). */
struct UnkRouteInfo_801E515C {
    /* 0x0 */ s8 m_choices[7];
};

/* A file whose playing unlocks a sound test entry. */
struct UnkSoundUnlock_801E4F70 {
    /* 0x0 */ const char *m_file;
    /* 0x4 */ s32 m_entry;
};

/* The game state (struct name unknown; see Game/GameSystem.cpp), as far as this unit uses it. */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x58];
    /* 0x58 */ s32 m_language;
    /* 0x5C */ s32 m_voiceLanguage;
    /* 0x60 */ u8 field_0x60[0x78 - 0x60];
    /* 0x78 */ s32 m_soundMode;
    /* 0x7C */ u8 m_vibration;
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
UnkFlags_801E5810 *fn_801E5828(UnkFlags_801E5810 *flags, u32 bit);
UnkFlags_801E5810 *fn_801E5810(UnkFlags_801E5810 *flags, u32 bit);
UnkFlags_801E5810 *fn_801E5804(UnkFlags_801E5810 *flags, const UnkFlags_801E5810 *other);
UnkBits256_801E57B8 *fn_801E57D8(UnkBits256_801E57B8 *bits, const UnkBits256_801E57B8 *other);
UnkBits512_801E5734 *fn_801E5798(UnkBits512_801E5734 *bits, u32 bit);
bool fn_801E5774(const UnkBits512_801E5734 *bits, u32 bit);
UnkBits512_801E5734 *fn_801E5754(UnkBits512_801E5734 *bits, const UnkBits512_801E5734 *other);
}

/* Sets a flag. */
extern "C" UnkFlags_801E5810 *fn_801E5828(UnkFlags_801E5810 *flags, u32 bit) {
    flags->m_bits |= 1 << bit;
    return flags;
}

/* Clears a flag. */
extern "C" UnkFlags_801E5810 *fn_801E5810(UnkFlags_801E5810 *flags, u32 bit) {
    flags->m_bits &= ~(1 << bit);
    return flags;
}

extern "C" UnkFlags_801E5810 *fn_801E5804(UnkFlags_801E5810 *flags, const UnkFlags_801E5810 *other) {
    flags->m_bits = other->m_bits;
    return flags;
}

UnkFlags_801E5810::UnkFlags_801E5810() {
    m_bits = 0;
}

extern "C" UnkBits256_801E57B8 *fn_801E57D8(UnkBits256_801E57B8 *bits, const UnkBits256_801E57B8 *other) {
    for (int i = 0; i < 8; i++) {
        bits->m_words[i] = other->m_words[i];
    }
    return bits;
}

UnkBits256_801E57B8::UnkBits256_801E57B8() {
    for (int i = 0; i < 8; i++) {
        m_words[i] = 0;
    }
}

/* Sets a bit. */
extern "C" UnkBits512_801E5734 *fn_801E5798(UnkBits512_801E5734 *bits, u32 bit) {
    bits->m_words[bit >> 5] |= 1 << (bit & 0x1F);
    return bits;
}

/* Whether a bit is set. */
extern "C" bool fn_801E5774(const UnkBits512_801E5734 *bits, u32 bit) {
    return (bits->m_words[bit >> 5] & (1 << (bit & 0x1F))) != 0;
}

extern "C" UnkBits512_801E5734 *fn_801E5754(UnkBits512_801E5734 *bits, const UnkBits512_801E5734 *other) {
    for (int i = 0; i < 16; i++) {
        bits->m_words[i] = other->m_words[i];
    }
    return bits;
}

UnkBits512_801E5734::UnkBits512_801E5734() {
    for (int i = 0; i < 16; i++) {
        m_words[i] = 0;
    }
}

/* The keys of a stage that were found (name inferred). */
extern "C" int fn_801E5700(UnkStageSave_800BB74C *stage) {
    int count = 0;
    for (int i = 0; i < 5; i++) {
        if (stage->m_keys.m_words[i] != -1) {
            count++;
        }
    }
    return count;
}

UnkPlayHistory_800BB74C::UnkPlayHistory_800BB74C() {}

UnkSaveData_800BB74C::UnkSaveData_800BB74C() {}

extern "C" void fn_801E51E4(UnkSaveHolder_801E559C *holder, const UnkSaveData_800BB74C *source);

/* The holder's constructor: both copies start reset. */
UnkSaveHolder_801E559C::UnkSaveHolder_801E559C() {
    fn_801E51E4(this, 0);
}

extern "C" UnkMissionSave_800BB74C *fn_801E5540(UnkMissionSave_800BB74C *mission, const UnkMissionSave_800BB74C *other) {
    mission->m_cleared = other->m_cleared;
    mission->m_rank = other->m_rank;
    mission->m_time = other->m_time;
    mission->m_score = other->m_score;
    return mission;
}

extern "C" UnkStageSave_800BB74C *fn_801E54B8(UnkStageSave_800BB74C *stage, const UnkStageSave_800BB74C *other) {
    stage->m_plays = other->m_plays;
    fn_801E5540(&stage->m_missions[0], &other->m_missions[0]);
    fn_801E5540(&stage->m_missions[1], &other->m_missions[1]);
    fn_801E5540(&stage->m_missions[2], &other->m_missions[2]);
    stage->m_keys = other->m_keys;
    return stage;
}

extern "C" UnkPlayHistory_800BB74C *fn_801E53C4(UnkPlayHistory_800BB74C *history, const UnkPlayHistory_800BB74C *other) {
    history->m_count = other->m_count;
    history->m_route = other->m_route;
    history->m_lives = other->m_lives;
    history->m_result = other->m_result;
    history->m_plays = other->m_plays;
    history->m_lastLives = other->m_lastLives;
    fn_801E5804(&history->m_flags, &other->m_flags);
    UnkStageSave_800BB74C *stage = history->m_stages;
    const UnkStageSave_800BB74C *otherStage = other->m_stages;
    do {
        fn_801E54B8(stage, otherStage);
        stage++;
        otherStage++;
    } while (stage < history->m_stages + 59);
    history->field_0x1698 = other->field_0x1698;
    history->m_allBest = other->m_allBest;
    history->m_bestCount = other->m_bestCount;
    fn_801E5754(&history->m_routes, &other->m_routes);
    fn_801E5804(&history->m_endings, &other->m_endings);
    fn_801E5754(&history->m_soundTest, &other->m_soundTest);
    fn_801E57D8(&history->field_0x1724, &other->field_0x1724);
    return history;
}

extern "C" UnkSaveData_800BB74C *fn_801E52F0(UnkSaveData_800BB74C *save, const UnkSaveData_800BB74C *other) {
    save->m_header = other->m_header;
    fn_801E53C4(&save->m_history, &other->m_history);
    save->field_0x1774 = other->field_0x1774;
    return save;
}

/* Resets the save data, or copies it from source, and takes it as saved (name inferred). */
extern "C" void fn_801E51E4(UnkSaveHolder_801E559C *holder, const UnkSaveData_800BB74C *source) {
    UnkSaveData_800BB74C *save = &holder->m_current;
    if (source) {
        memcpy(save, source, sizeof(UnkSaveData_800BB74C));
    } else {
        memset(save, 0, sizeof(UnkSaveData_800BB74C));
        save->m_history.m_route.m_entries[0] = 5;
        save->m_header.m_options.m_language = lbl_8057D6A0.m_language;
        save->m_header.m_options.m_voiceLanguage = lbl_8057D6A0.m_voiceLanguage;
        save->m_header.m_options.m_soundMode = lbl_8057D6A0.m_soundMode;
        save->m_header.m_options.m_vibration = lbl_8057D6A0.m_vibration;
    }
    for (int i = 0; i < 59; i++) {
        for (int j = 0; j < 3; j++) {
            save->m_history.m_stages[i].m_missions[j].m_rank = 4;
            UnkTime_801E46E4 time(99, 59, 99);
            save->m_history.m_stages[i].m_missions[j].m_time = time;
        }
        for (int j = 0; j < 5; j++) {
            save->m_history.m_stages[i].m_keys.m_words[j] = -1;
        }
    }
    fn_801E52F0(&holder->m_saved, save);
}

/* The story routes: for each route, the choice taken at each of its seven stages. */
const UnkRouteInfo_801E515C lbl_804D1690[0x146] = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 1, 2},
    {0, 0, 0, 0, 0, 1, 3},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 1},
    {0, 0, 0, 0, 1, 1, 2},
    {0, 0, 0, 0, 1, 1, 3},
    {0, 0, 0, 0, 1, 2, 4},
    {0, 0, 0, 0, 1, 2, 5},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 1},
    {0, 0, 0, 1, 0, 1, 2},
    {0, 0, 0, 1, 0, 1, 3},
    {0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 1},
    {0, 0, 0, 1, 1, 1, 2},
    {0, 0, 0, 1, 1, 1, 3},
    {0, 0, 0, 1, 1, 2, 4},
    {0, 0, 0, 1, 1, 2, 5},
    {0, 0, 0, 1, 2, 1, 2},
    {0, 0, 0, 1, 2, 1, 3},
    {0, 0, 0, 1, 2, 2, 4},
    {0, 0, 0, 1, 2, 2, 5},
    {0, 0, 0, 1, 2, 3, 6},
    {0, 0, 0, 1, 2, 3, 7},
    {0, 0, 0, 2, 1, 0, 0},
    {0, 0, 0, 2, 1, 0, 1},
    {0, 0, 0, 2, 1, 1, 2},
    {0, 0, 0, 2, 1, 1, 3},
    {0, 0, 0, 2, 1, 2, 4},
    {0, 0, 0, 2, 1, 2, 5},
    {0, 0, 0, 2, 2, 1, 2},
    {0, 0, 0, 2, 2, 1, 3},
    {0, 0, 0, 2, 2, 2, 4},
    {0, 0, 0, 2, 2, 2, 5},
    {0, 0, 0, 2, 2, 3, 6},
    {0, 0, 0, 2, 2, 3, 7},
    {0, 0, 0, 2, 3, 2, 4},
    {0, 0, 0, 2, 3, 2, 5},
    {0, 0, 0, 2, 3, 3, 6},
    {0, 0, 0, 2, 3, 3, 7},
    {0, 0, 0, 2, 3, 4, 8},
    {0, 0, 0, 2, 3, 4, 9},
    {0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 1},
    {0, 0, 1, 1, 0, 1, 2},
    {0, 0, 1, 1, 0, 1, 3},
    {0, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 1},
    {0, 0, 1, 1, 1, 1, 2},
    {0, 0, 1, 1, 1, 1, 3},
    {0, 0, 1, 1, 1, 2, 4},
    {0, 0, 1, 1, 1, 2, 5},
    {0, 0, 1, 1, 2, 1, 2},
    {0, 0, 1, 1, 2, 1, 3},
    {0, 0, 1, 1, 2, 2, 4},
    {0, 0, 1, 1, 2, 2, 5},
    {0, 0, 1, 1, 2, 3, 6},
    {0, 0, 1, 1, 2, 3, 7},
    {0, 0, 1, 2, 1, 0, 0},
    {0, 0, 1, 2, 1, 0, 1},
    {0, 0, 1, 2, 1, 1, 2},
    {0, 0, 1, 2, 1, 1, 3},
    {0, 0, 1, 2, 1, 2, 4},
    {0, 0, 1, 2, 1, 2, 5},
    {0, 0, 1, 2, 2, 1, 2},
    {0, 0, 1, 2, 2, 1, 3},
    {0, 0, 1, 2, 2, 2, 4},
    {0, 0, 1, 2, 2, 2, 5},
    {0, 0, 1, 2, 2, 3, 6},
    {0, 0, 1, 2, 2, 3, 7},
    {0, 0, 1, 2, 3, 2, 4},
    {0, 0, 1, 2, 3, 2, 5},
    {0, 0, 1, 2, 3, 3, 6},
    {0, 0, 1, 2, 3, 3, 7},
    {0, 0, 1, 2, 3, 4, 8},
    {0, 0, 1, 2, 3, 4, 9},
    {0, 0, 1, 3, 2, 1, 2},
    {0, 0, 1, 3, 2, 1, 3},
    {0, 0, 1, 3, 2, 2, 4},
    {0, 0, 1, 3, 2, 2, 5},
    {0, 0, 1, 3, 2, 3, 6},
    {0, 0, 1, 3, 2, 3, 7},
    {0, 0, 1, 3, 3, 2, 4},
    {0, 0, 1, 3, 3, 2, 5},
    {0, 0, 1, 3, 3, 3, 6},
    {0, 0, 1, 3, 3, 3, 7},
    {0, 0, 1, 3, 3, 4, 8},
    {0, 0, 1, 3, 3, 4, 9},
    {0, 0, 1, 3, 4, 3, 6},
    {0, 0, 1, 3, 4, 3, 7},
    {0, 0, 1, 3, 4, 4, 8},
    {0, 0, 1, 3, 4, 4, 9},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 0, 1, 2},
    {0, 1, 0, 0, 0, 1, 3},
    {0, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 1},
    {0, 1, 0, 0, 1, 1, 2},
    {0, 1, 0, 0, 1, 1, 3},
    {0, 1, 0, 0, 1, 2, 4},
    {0, 1, 0, 0, 1, 2, 5},
    {0, 1, 0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0, 0, 1},
    {0, 1, 0, 1, 0, 1, 2},
    {0, 1, 0, 1, 0, 1, 3},
    {0, 1, 0, 1, 1, 0, 0},
    {0, 1, 0, 1, 1, 0, 1},
    {0, 1, 0, 1, 1, 1, 2},
    {0, 1, 0, 1, 1, 1, 3},
    {0, 1, 0, 1, 1, 2, 4},
    {0, 1, 0, 1, 1, 2, 5},
    {0, 1, 0, 1, 2, 1, 2},
    {0, 1, 0, 1, 2, 1, 3},
    {0, 1, 0, 1, 2, 2, 4},
    {0, 1, 0, 1, 2, 2, 5},
    {0, 1, 0, 1, 2, 3, 6},
    {0, 1, 0, 1, 2, 3, 7},
    {0, 1, 0, 2, 1, 0, 0},
    {0, 1, 0, 2, 1, 0, 1},
    {0, 1, 0, 2, 1, 1, 2},
    {0, 1, 0, 2, 1, 1, 3},
    {0, 1, 0, 2, 1, 2, 4},
    {0, 1, 0, 2, 1, 2, 5},
    {0, 1, 0, 2, 2, 1, 2},
    {0, 1, 0, 2, 2, 1, 3},
    {0, 1, 0, 2, 2, 2, 4},
    {0, 1, 0, 2, 2, 2, 5},
    {0, 1, 0, 2, 2, 3, 6},
    {0, 1, 0, 2, 2, 3, 7},
    {0, 1, 0, 2, 3, 2, 4},
    {0, 1, 0, 2, 3, 2, 5},
    {0, 1, 0, 2, 3, 3, 6},
    {0, 1, 0, 2, 3, 3, 7},
    {0, 1, 0, 2, 3, 4, 8},
    {0, 1, 0, 2, 3, 4, 9},
    {0, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 1},
    {0, 1, 1, 1, 0, 1, 2},
    {0, 1, 1, 1, 0, 1, 3},
    {0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 1},
    {0, 1, 1, 1, 1, 1, 2},
    {0, 1, 1, 1, 1, 1, 3},
    {0, 1, 1, 1, 1, 2, 4},
    {0, 1, 1, 1, 1, 2, 5},
    {0, 1, 1, 1, 2, 1, 2},
    {0, 1, 1, 1, 2, 1, 3},
    {0, 1, 1, 1, 2, 2, 4},
    {0, 1, 1, 1, 2, 2, 5},
    {0, 1, 1, 1, 2, 3, 6},
    {0, 1, 1, 1, 2, 3, 7},
    {0, 1, 1, 2, 1, 0, 0},
    {0, 1, 1, 2, 1, 0, 1},
    {0, 1, 1, 2, 1, 1, 2},
    {0, 1, 1, 2, 1, 1, 3},
    {0, 1, 1, 2, 1, 2, 4},
    {0, 1, 1, 2, 1, 2, 5},
    {0, 1, 1, 2, 2, 1, 2},
    {0, 1, 1, 2, 2, 1, 3},
    {0, 1, 1, 2, 2, 2, 4},
    {0, 1, 1, 2, 2, 2, 5},
    {0, 1, 1, 2, 2, 3, 6},
    {0, 1, 1, 2, 2, 3, 7},
    {0, 1, 1, 2, 3, 2, 4},
    {0, 1, 1, 2, 3, 2, 5},
    {0, 1, 1, 2, 3, 3, 6},
    {0, 1, 1, 2, 3, 3, 7},
    {0, 1, 1, 2, 3, 4, 8},
    {0, 1, 1, 2, 3, 4, 9},
    {0, 1, 1, 3, 2, 1, 2},
    {0, 1, 1, 3, 2, 1, 3},
    {0, 1, 1, 3, 2, 2, 4},
    {0, 1, 1, 3, 2, 2, 5},
    {0, 1, 1, 3, 2, 3, 6},
    {0, 1, 1, 3, 2, 3, 7},
    {0, 1, 1, 3, 3, 2, 4},
    {0, 1, 1, 3, 3, 2, 5},
    {0, 1, 1, 3, 3, 3, 6},
    {0, 1, 1, 3, 3, 3, 7},
    {0, 1, 1, 3, 3, 4, 8},
    {0, 1, 1, 3, 3, 4, 9},
    {0, 1, 1, 3, 4, 3, 6},
    {0, 1, 1, 3, 4, 3, 7},
    {0, 1, 1, 3, 4, 4, 8},
    {0, 1, 1, 3, 4, 4, 9},
    {0, 1, 2, 2, 1, 0, 0},
    {0, 1, 2, 2, 1, 0, 1},
    {0, 1, 2, 2, 1, 1, 2},
    {0, 1, 2, 2, 1, 1, 3},
    {0, 1, 2, 2, 1, 2, 4},
    {0, 1, 2, 2, 1, 2, 5},
    {0, 1, 2, 2, 2, 1, 2},
    {0, 1, 2, 2, 2, 1, 3},
    {0, 1, 2, 2, 2, 2, 4},
    {0, 1, 2, 2, 2, 2, 5},
    {0, 1, 2, 2, 2, 3, 6},
    {0, 1, 2, 2, 2, 3, 7},
    {0, 1, 2, 2, 3, 2, 4},
    {0, 1, 2, 2, 3, 2, 5},
    {0, 1, 2, 2, 3, 3, 6},
    {0, 1, 2, 2, 3, 3, 7},
    {0, 1, 2, 2, 3, 4, 8},
    {0, 1, 2, 2, 3, 4, 9},
    {0, 1, 2, 3, 2, 1, 2},
    {0, 1, 2, 3, 2, 1, 3},
    {0, 1, 2, 3, 2, 2, 4},
    {0, 1, 2, 3, 2, 2, 5},
    {0, 1, 2, 3, 2, 3, 6},
    {0, 1, 2, 3, 2, 3, 7},
    {0, 1, 2, 3, 3, 2, 4},
    {0, 1, 2, 3, 3, 2, 5},
    {0, 1, 2, 3, 3, 3, 6},
    {0, 1, 2, 3, 3, 3, 7},
    {0, 1, 2, 3, 3, 4, 8},
    {0, 1, 2, 3, 3, 4, 9},
    {0, 1, 2, 3, 4, 3, 6},
    {0, 1, 2, 3, 4, 3, 7},
    {0, 1, 2, 3, 4, 4, 8},
    {0, 1, 2, 3, 4, 4, 9},
    {0, 1, 2, 4, 3, 2, 4},
    {0, 1, 2, 4, 3, 2, 5},
    {0, 1, 2, 4, 3, 3, 6},
    {0, 1, 2, 4, 3, 3, 7},
    {0, 1, 2, 4, 3, 4, 8},
    {0, 1, 2, 4, 3, 4, 9},
    {0, 1, 2, 4, 4, 3, 6},
    {0, 1, 2, 4, 4, 3, 7},
    {0, 1, 2, 4, 4, 4, 8},
    {0, 1, 2, 4, 4, 4, 9},
    {0, 2, 1, 1, 0, 0, 0},
    {0, 2, 1, 1, 0, 0, 1},
    {0, 2, 1, 1, 0, 1, 2},
    {0, 2, 1, 1, 0, 1, 3},
    {0, 2, 1, 1, 1, 0, 0},
    {0, 2, 1, 1, 1, 0, 1},
    {0, 2, 1, 1, 1, 1, 2},
    {0, 2, 1, 1, 1, 1, 3},
    {0, 2, 1, 1, 1, 2, 4},
    {0, 2, 1, 1, 1, 2, 5},
    {0, 2, 1, 1, 2, 1, 2},
    {0, 2, 1, 1, 2, 1, 3},
    {0, 2, 1, 1, 2, 2, 4},
    {0, 2, 1, 1, 2, 2, 5},
    {0, 2, 1, 1, 2, 3, 6},
    {0, 2, 1, 1, 2, 3, 7},
    {0, 2, 1, 2, 1, 0, 0},
    {0, 2, 1, 2, 1, 0, 1},
    {0, 2, 1, 2, 1, 1, 2},
    {0, 2, 1, 2, 1, 1, 3},
    {0, 2, 1, 2, 1, 2, 4},
    {0, 2, 1, 2, 1, 2, 5},
    {0, 2, 1, 2, 2, 1, 2},
    {0, 2, 1, 2, 2, 1, 3},
    {0, 2, 1, 2, 2, 2, 4},
    {0, 2, 1, 2, 2, 2, 5},
    {0, 2, 1, 2, 2, 3, 6},
    {0, 2, 1, 2, 2, 3, 7},
    {0, 2, 1, 2, 3, 2, 4},
    {0, 2, 1, 2, 3, 2, 5},
    {0, 2, 1, 2, 3, 3, 6},
    {0, 2, 1, 2, 3, 3, 7},
    {0, 2, 1, 2, 3, 4, 8},
    {0, 2, 1, 2, 3, 4, 9},
    {0, 2, 1, 3, 2, 1, 2},
    {0, 2, 1, 3, 2, 1, 3},
    {0, 2, 1, 3, 2, 2, 4},
    {0, 2, 1, 3, 2, 2, 5},
    {0, 2, 1, 3, 2, 3, 6},
    {0, 2, 1, 3, 2, 3, 7},
    {0, 2, 1, 3, 3, 2, 4},
    {0, 2, 1, 3, 3, 2, 5},
    {0, 2, 1, 3, 3, 3, 6},
    {0, 2, 1, 3, 3, 3, 7},
    {0, 2, 1, 3, 3, 4, 8},
    {0, 2, 1, 3, 3, 4, 9},
    {0, 2, 1, 3, 4, 3, 6},
    {0, 2, 1, 3, 4, 3, 7},
    {0, 2, 1, 3, 4, 4, 8},
    {0, 2, 1, 3, 4, 4, 9},
    {0, 2, 2, 2, 1, 0, 0},
    {0, 2, 2, 2, 1, 0, 1},
    {0, 2, 2, 2, 1, 1, 2},
    {0, 2, 2, 2, 1, 1, 3},
    {0, 2, 2, 2, 1, 2, 4},
    {0, 2, 2, 2, 1, 2, 5},
    {0, 2, 2, 2, 2, 1, 2},
    {0, 2, 2, 2, 2, 1, 3},
    {0, 2, 2, 2, 2, 2, 4},
    {0, 2, 2, 2, 2, 2, 5},
    {0, 2, 2, 2, 2, 3, 6},
    {0, 2, 2, 2, 2, 3, 7},
    {0, 2, 2, 2, 3, 2, 4},
    {0, 2, 2, 2, 3, 2, 5},
    {0, 2, 2, 2, 3, 3, 6},
    {0, 2, 2, 2, 3, 3, 7},
    {0, 2, 2, 2, 3, 4, 8},
    {0, 2, 2, 2, 3, 4, 9},
    {0, 2, 2, 3, 2, 1, 2},
    {0, 2, 2, 3, 2, 1, 3},
    {0, 2, 2, 3, 2, 2, 4},
    {0, 2, 2, 3, 2, 2, 5},
    {0, 2, 2, 3, 2, 3, 6},
    {0, 2, 2, 3, 2, 3, 7},
    {0, 2, 2, 3, 3, 2, 4},
    {0, 2, 2, 3, 3, 2, 5},
    {0, 2, 2, 3, 3, 3, 6},
    {0, 2, 2, 3, 3, 3, 7},
    {0, 2, 2, 3, 3, 4, 8},
    {0, 2, 2, 3, 3, 4, 9},
    {0, 2, 2, 3, 4, 3, 6},
    {0, 2, 2, 3, 4, 3, 7},
    {0, 2, 2, 3, 4, 4, 8},
    {0, 2, 2, 3, 4, 4, 9},
    {0, 2, 2, 4, 3, 2, 4},
    {0, 2, 2, 4, 3, 2, 5},
    {0, 2, 2, 4, 3, 3, 6},
    {0, 2, 2, 4, 3, 3, 7},
    {0, 2, 2, 4, 3, 4, 8},
    {0, 2, 2, 4, 3, 4, 9},
    {0, 2, 2, 4, 4, 3, 6},
    {0, 2, 2, 4, 4, 3, 7},
    {0, 2, 2, 4, 4, 4, 8},
    {0, 2, 2, 4, 4, 4, 9},
};

/* The stage table index of each choice of the seven stages of a route. */
s32 lbl_8053D9B0[7][10] = {
    {5, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {6, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {12, 13, 14, 15, 16, -1, -1, -1, -1, -1},
    {17, 18, 19, 20, 21, -1, -1, -1, -1, -1},
    {22, 23, 24, 25, 26, -1, -1, -1, -1, -1},
    {36, 35, 38, 37, 39, 39, 40, 41, 43, 42},
};

/* The files of the sound test entries. */
const char *lbl_8053DAC8[0x38] = {
    "sng_vox01.adx",         "sng_vox02.adx",     "sng_vox03.adx",     "sng_vox04.adx",
    "sng_vox05.adx",         "sng_vox06.adx",     "sng_stg0100.adx",   "sng_stg0200.adx",
    "sng_stg0201.adx",       "sng_stg0202.adx",   "sng_stg0210.adx",   "sng_stg0300.adx",
    "sng_stg0301.adx",       "sng_stg0302.adx",   "sng_stg0310.adx",   "sng_stg0400.adx",
    "sng_stg0401.adx",       "sng_stg0402.adx",   "sng_stg0403.adx",   "sng_stg0404.adx",
    "sng_stg0410.adx",       "sng_stg0500.adx",   "sng_stg0501.adx",   "sng_stg0502.adx",
    "sng_stg0503.adx",       "sng_stg0504.adx",   "sng_stg0510.adx",   "sng_stg0600.adx",
    "sng_stg0601.adx",       "sng_stg0602.adx",   "sng_stg0603.adx",   "sng_stg0604.adx",
    "sng_stg0610.adx",       "sng_stg0611.adx",   "sng_stg0612.adx",   "sng_stg0700.adx",
    "sng_stg0710.adx",       "sng_stg0710b.adx",  "sng_stg0800.adx",   "sng_jin_cctrl.adx",
    "sng_jin_ccslow.adx",    "sng_jin_invincible.adx", "sng_EV_music1.adx", "sng_sys01.adx",
    "sng_sys02.adx",         "sng_sys03.adx",     "sng_sys04.adx",     "sng_sys05.adx",
    "sng_E1001.adx",         "sng_E4101.adx",     "sng_E4201.adx",     "sng_E5024.adx",
    "sng_E8003.adx",         "sng_E8201.adx",     "sng_EV_music2.adx", 0,
};

/* The files whose playing unlocks a sound test entry. */
const UnkSoundUnlock_801E4F70 lbl_804D2470[0x4E] = {
    {"sng_vox01.adx", 0},
    {"Op.sfd", 0},
    {"sng_vox02.adx", 1},
    {"sng_vox03.adx", 2},
    {"sng_vox04.adx", 3},
    {"sng_vox05.adx", 4},
    {"sng_vox06.adx", 5},
    {"sng_EV_music3.adx", 5},
    {"sng_stg0100.adx", 6},
    {"sng_stg0200.adx", 7},
    {"sng_stg0201.adx", 8},
    {"sng_stg0202.adx", 9},
    {"sng_stg0210.adx", 10},
    {"sng_stg0412.adx", 10},
    {"sng_stg0300.adx", 11},
    {"sng_stg0301.adx", 12},
    {"sng_stg0302.adx", 13},
    {"sng_stg0310.adx", 14},
    {"sng_stg0411.adx", 14},
    {"sng_stg0511.adx", 14},
    {"sng_stg0400.adx", 15},
    {"sng_stg0401.adx", 16},
    {"sng_stg0402.adx", 17},
    {"sng_stg0403.adx", 18},
    {"sng_stg0404.adx", 19},
    {"sng_stg0410.adx", 20},
    {"sng_stg0500.adx", 21},
    {"sng_stg0501.adx", 22},
    {"sng_stg0502.adx", 23},
    {"sng_stg0503.adx", 24},
    {"sng_stg0504.adx", 25},
    {"sng_stg0510.adx", 26},
    {"sng_stg0600.adx", 27},
    {"sng_stg0601.adx", 28},
    {"sng_stg0602.adx", 29},
    {"sng_stg0603.adx", 30},
    {"sng_stg0604.adx", 31},
    {"sng_stg0610.adx", 32},
    {"sng_stg0616.adx", 32},
    {"sng_stg0617.adx", 32},
    {"sng_stg0611.adx", 33},
    {"sng_stg0613.adx", 33},
    {"sng_stg0618.adx", 33},
    {"sng_stg0612.adx", 34},
    {"sng_stg0614.adx", 34},
    {"sng_stg0615.adx", 34},
    {"sng_stg0700.adx", 35},
    {"sng_stg0710.adx", 36},
    {"sng_stg0800.adx", 38},
    {"sng_jin_cctrl_e.adx", 39},
    {"sng_jin_cctrl_j.adx", 39},
    {"sng_jin_cctrl.adx", 39},
    {"sng_jin_ccslow_e.adx", 40},
    {"sng_jin_ccslow_j.adx", 40},
    {"sng_jin_ccslow.adx", 40},
    {"sng_jin_invincible.adx", 41},
    {"sng_jin_roundclear.adx", 42},
    {"sng_EV_music1.adx", 42},
    {"sng_sys01.adx", 43},
    {"sng_sys02.adx", 44},
    {"sng_sys03.adx", 45},
    {"sng_sys04.adx", 46},
    {"sng_sys05.adx", 47},
    {"sng_E1001.adx", 48},
    {"E1001.sfd", 48},
    {"sng_E4101.adx", 49},
    {"E4101.sfd", 49},
    {"E4102.sfd", 49},
    {"sng_E4201.adx", 50},
    {"E4201.sfd", 50},
    {"sng_E5024.adx", 51},
    {"E5024.sfd", 51},
    {"sng_E8003.adx", 52},
    {"E8003.sfd", 52},
    {"sng_E8201.adx", 53},
    {"E8201.sfd", 53},
    {"sng_EV_music2.adx", 54},
    {"sng_EV_music3.adx", 54},
};

/* Fills info with a story route, and returns whether it was seen (name inferred). */
extern "C" bool fn_801E515C(UnkSaveData_800BB74C *save, UnkRouteInfo_801E515C *info, int route) {
    if (route < 0 || route >= 0x146) {
        return false;
    }
    info->m_choices[0] = lbl_804D1690[route].m_choices[0];
    info->m_choices[1] = lbl_804D1690[route].m_choices[1];
    info->m_choices[2] = lbl_804D1690[route].m_choices[2];
    info->m_choices[3] = lbl_804D1690[route].m_choices[3];
    info->m_choices[4] = lbl_804D1690[route].m_choices[4];
    info->m_choices[5] = lbl_804D1690[route].m_choices[5];
    info->m_choices[6] = lbl_804D1690[route].m_choices[6];
    return fn_801E5774(&save->m_history.m_routes, route);
}

/* The stage table index of a route's choice at one of its stages (name inferred). */
extern "C" s32 fn_801E5140(int stage, int choice) {
    return lbl_8053D9B0[stage][choice];
}

/* The story route the history took, or -1 (name inferred). */
extern "C" int fn_801E5040(UnkSaveData_800BB74C *save) {
    UnkPlayHistory_800BB74C *history = &save->m_history;
    for (int i = 0; i < 0x146; i++) {
        const UnkRouteInfo_801E515C *route = &lbl_804D1690[i];
        for (int j = 0; j < 7; j++) {
            bool found = false;
            for (int k = 0; k <= history->m_count; k++) {
                if (lbl_8053D9B0[j][route->m_choices[j]] == history->m_route.m_entries[k]) {
                    if (history->m_route.m_entries[k] == 0x27) {
                        if (history->m_result == 3 && route->m_choices[j] == 4) {
                            found = true;
                            break;
                        }
                        if (history->m_result == 5 && route->m_choices[j] == 5) {
                            found = true;
                            break;
                        }
                    } else {
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                goto next;
            }
        }
        return i;
    next:;
    }
    return -1;
}

/* The number of sound test entries. */
extern "C" int fn_801E5038(UnkSaveData_800BB74C *save) {
    return 0x37;
}

/* The file of a sound test entry when it is unlocked, or 0 (name inferred). */
extern "C" const char *fn_801E4FE0(UnkSaveData_800BB74C *save, u32 entry) {
    if (entry < 0x37 && fn_801E5774(&save->m_history.m_soundTest, entry)) {
        return lbl_8053DAC8[entry];
    }
    return 0;
}

/* Unlocks the sound test entries of a file that was played (name inferred). */
extern "C" void fn_801E4F70(UnkSaveData_800BB74C *save, const char *file) {
    for (u32 i = 0; i < 0x4E; i++) {
        if (strcmp(file, lbl_804D2470[i].m_file) == 0) {
            fn_801E5798(&save->m_history.m_soundTest, lbl_804D2470[i].m_entry);
        }
    }
}
