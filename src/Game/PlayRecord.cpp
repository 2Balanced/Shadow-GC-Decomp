/* The play state the game keeps per stage (struct name unknown; fn_80079808 returns the one
 * instance): the players' checkpoints, scores and records, whose helpers are in
 * Game/SaveRecord.cpp. This unit holds its constructor and the resets other units call; they keep
 * their fn_ names. The file name and all names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x90];
    /* 0x090 */ s32 m_frame;  /* (name inferred) */
    /* 0x094 */ float m_time; /* (name inferred) */
    /* 0x098 */ u8 field_0x98[0x150 - 0x98];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The structs of Game/SaveRecord.cpp (names unknown). */
struct UnkRecord_80178B5C {
    /* 0x00 */ s32 field_0x0[7];
};

struct UnkCounter {
    /* 0x0 */ s32 m_value;
    /* 0x4 */ s32 m_frame;
};

struct UnkScore {
    /* 0x0 */ s32 m_score;
    /* 0x4 */ s32 m_max;
    /* 0x8 */ s32 m_lock;
};
SIZE_ASSERT(UnkScore, 0xC);

/* A checkpoint; its rotation starts at zero. */
struct UnkCheckpoint {
    UnkCheckpoint() : m_rotation(0.0f, 0.0f, 0.0f) {}

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u8 m_valid;
};
SIZE_ASSERT(UnkCheckpoint, 0x1C);

/* The checkpoints of both players. Retail keeps the constructor out of line; the call of an
 * (empty) inline function does that under GC/2.7, whose inliner otherwise expands it. */
struct UnkCheckpoints {
    UnkCheckpoints() {
        Init();
    }
    void Init() {}

    /* 0x000 */ s32 m_last;
    /* 0x004 */ s32 m_frame;
    /* 0x008 */ float m_time;
    /* 0x00C */ UnkCheckpoint m_points[2][32];
};
SIZE_ASSERT(UnkCheckpoints, 0x70C);

struct UnkCounters {
    /* 0x00 */ s32 m_counts[3];
    /* 0x0C */ s32 m_frames[3];
};

struct UnkFloatPair {
    /* 0x0 */ float m_a;
    /* 0x4 */ float m_b;
};

/* A player's record (struct name unknown). */
struct UnkPlayerRecord_80178B5C {
    /* 0x00 */ UnkRecord_80178B5C m_record;
    /* 0x1C */ s32 m_ammo;
    /* 0x20 */ UnkCounter m_lives;
    /* 0x28 */ s32 m_rings;
    /* 0x2C */ UnkCounters m_counters;
    /* 0x44 */ UnkFloatPair m_pair;
};
SIZE_ASSERT(UnkPlayerRecord_80178B5C, 0x4C);

/* The play state (struct name unknown). */
struct UnkObject_80079808 {
    UnkObject_80079808();

    /* The record of player, or 0. */
    UnkPlayerRecord_80178B5C *GetRecord(int player) {
        if (player >= 0) {
            return &m_records[player];
        }
        return 0;
    }

    /* 0x000 */ UnkCheckpoints m_checkpoints;
    /* 0x70C */ UnkScore m_scores[2];
    /* 0x724 */ u32 field_0x724;
    /* 0x728 */ UnkPlayerRecord_80178B5C m_records[2];
};
SIZE_ASSERT(UnkObject_80079808, 0x7C0);

extern "C" {
/* Game/SaveRecord.cpp. */
extern const float lbl_805F4BF0[2];
void fn_8016F674(UnkFloatPair *pair, float value);
void fn_8016F62C(UnkCounters *counters);
void fn_8016F4E8(UnkCheckpoints *points);
void fn_8016F2DC(s32 *rings);
int fn_8016F288(s32 *rings, int count);
void fn_8016F1C8(UnkScore *scores, int max);
void fn_8016EFE8(UnkCounter *counter);
s32 fn_8016EF8C(UnkCounter *counter, int count);
s32 fn_8016EF84(UnkCounter *counter);
void fn_8016EF74(s32 *ammo, int count);
void fn_8016EF10(UnkRecord_80178B5C *record);
void fn_80335808(u32 *arg0);

void fn_80178C54(UnkObject_80079808 *data, u8 first);
}

UnkObject_80079808::UnkObject_80079808() {
    fn_80178C54(this, 1);
}

/* Resets the play state, for a new stage when first is set (name inferred). */
extern "C" void fn_80178C54(UnkObject_80079808 *data, u8 first) {
    fn_8016F4E8(&data->m_checkpoints);
    fn_8016F1C8(data->m_scores, (int)lbl_805F4BF0[0]);
    fn_80335808(&data->field_0x724);
    for (int i = 0; i < 2; i++) {
        UnkPlayerRecord_80178B5C *record = data->GetRecord(i);
        fn_8016EF10(&record->m_record);
        fn_8016EF74(&record->m_ammo, 0);
        fn_8016F2DC(&record->m_rings);
        fn_8016F62C(&record->m_counters);
        fn_8016F674(&record->m_pair, 0.0f);
        if (first) {
            fn_8016EFE8(&record->m_lives);
        }
    }
    if (!first) {
        if (fn_8016EF84(&data->m_records[0].m_lives) > 0) {
            fn_8016EF8C(&data->m_records[0].m_lives, -1);
        } else {
            fn_8016EF8C(&data->m_records[0].m_lives, -fn_8016EF84(&data->m_records[0].m_lives));
            fn_8016EF8C(&data->m_records[0].m_lives, 5);
        }
    }
}

/* Restarts the stage from the last checkpoint (name inferred). */
extern "C" void fn_80178B80(UnkObject_80079808 *data) {
    fn_8016F1C8(data->m_scores, (int)lbl_805F4BF0[0]);
    lbl_8057D6A0.m_frame = data->m_checkpoints.m_frame;
    lbl_8057D6A0.m_time = data->m_checkpoints.m_time;
    if (fn_8016EF84(&data->m_records[0].m_lives) > 0) {
        fn_8016EF8C(&data->m_records[0].m_lives, -1);
    }
    for (int i = 0; i < 2; i++) {
        UnkPlayerRecord_80178B5C *record = data->GetRecord(i);
        fn_8016EF10(&record->m_record);
        fn_8016EF74(&record->m_ammo, 0);
        fn_8016F2DC(&record->m_rings);
        fn_8016F62C(&record->m_counters);
        fn_8016F674(&record->m_pair, 0.0f);
    }
}

extern "C" UnkPlayerRecord_80178B5C *fn_80178B5C(UnkObject_80079808 *data, int player) {
    return data->GetRecord(player);
}

/* Adds count rings to a record and the lives they earn (name inferred). */
extern "C" void fn_80178B20(UnkPlayerRecord_80178B5C *record, int count) {
    fn_8016EF8C(&record->m_lives, fn_8016F288(&record->m_rings, count));
}
