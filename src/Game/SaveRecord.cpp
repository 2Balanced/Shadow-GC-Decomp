/* Small helpers on the records of the game's save data and play state (file and struct names
 * unknown; "SaveRecord" is invented). They read and write a player record's fields (ammo, score,
 * lives from rings, the weapon in hand), keep the checkpoints SavePoint stores, and count the
 * hero/dark points. Other units call them by address, so they keep their fn_ names. Member and
 * struct names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). The unit is NonMatching only because of fn_8016F4E8 (see there). */
#include "Game/MathTypes.h"
#include "Game/StageTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long count);

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x90];
    /* 0x090 */ s32 m_frame;  /* (name inferred) */
    /* 0x094 */ float m_time; /* (name inferred) */
    /* 0x098 */ u8 field_0x98[0xA8 - 0x98];
    /* 0x0A8 */ s32 m_stageId; /* (name inferred) */
    /* 0x0AC */ s32 m_stageIndex; /* (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0xC8 - 0xB0];
    /* 0x0C8 */ u8 field_0xC8;
    /* 0x0C9 */ u8 field_0xC9[0xCC - 0xC9];
    /* 0x0CC */ s32 field_0xCC;
    /* 0x0D0 */ u8 field_0xD0[0x125 - 0xD0];
    /* 0x125 */ u8 field_0x125;
    /* 0x126 */ u8 field_0x126[0x128 - 0x126];
    /* 0x128 */ s32 field_0x128;
    /* 0x12C */ u8 field_0x12C[0x150 - 0x12C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The start positions of a stage in the table fn_800CBAA4 returns (struct names unknown). */
struct UnkStageStart_800CBAA4 {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};

struct UnkStageInfo_800CBAA4 {
    /* 0x00 */ UnkStageStart_800CBAA4 m_starts[2];
    /* 0x50 */ u8 field_0x50[0xC0 - 0x50];
};
SIZE_ASSERT(UnkStageInfo_800CBAA4, 0xC0);

struct UnkStageTable_800CBAA4 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ UnkStageInfo_800CBAA4 m_stages[1];
};

/* What fn_800BB74C returns (struct name unknown). */
struct UnkObject_800BB74C {
    /* 0x00 */ u8 field_0x0[0x94];
    /* 0x94 */ s32 field_0x94;
    /* 0x98 */ u8 field_0x98[0xA0 - 0x98];
    /* 0xA0 */ s32 field_0xA0;
};

/* A player's record (struct name unknown; see Game/PlayerShadowWeapon.cpp). */
struct UnkRecord_80178B5C {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ s32 field_0x14; /* the weapon's type */
    /* 0x18 */ s32 field_0x18;
};

/* A value and the frame it last changed (struct name unknown). */
struct UnkCounter {
    /* 0x0 */ s32 m_value;
    /* 0x4 */ s32 m_frame;
};

/* A score and its limit, per player (struct name unknown). */
struct UnkScore {
    /* 0x0 */ s32 m_score;
    /* 0x4 */ s32 m_max;
    /* 0x8 */ s32 m_lock;
};
SIZE_ASSERT(UnkScore, 0xC);

/* A checkpoint (struct name unknown). */
struct UnkCheckpoint {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u8 m_valid;
};
SIZE_ASSERT(UnkCheckpoint, 0x1C);

/* The checkpoints of both players (struct name unknown). */
struct UnkCheckpoints {
    /* 0x000 */ s32 m_last;
    /* 0x004 */ s32 m_frame;
    /* 0x008 */ float m_time;
    /* 0x00C */ UnkCheckpoint m_points[2][32];
};
SIZE_ASSERT(UnkCheckpoints, 0x70C);

/* Three counters and the frames they last changed (struct name unknown). */
struct UnkCounters {
    /* 0x00 */ s32 m_counts[3];
    /* 0x0C */ s32 m_frames[3];
};

/* Two floats (struct name unknown). */
struct UnkFloatPair {
    /* 0x0 */ float m_a;
    /* 0x4 */ float m_b;
};

extern "C" {
UnkObject_800BB74C *fn_800BB74C(void);
UnkStageTable_800CBAA4 *fn_800CBAA4(void);

const s32 *fn_8016F21C(const s32 *value, const s32 *low, const s32 *high);
const s32 *fn_8016F250(const s32 *a, const s32 *b);
const s32 *fn_8016F268(const s32 *a, const s32 *b);
}

/* A limit read by the stage manager at 0x80178B80 (meaning unknown; the second value, 0, is a
 * guess at what fills the 8 bytes). */
extern "C" const float lbl_805F4BF0[2] = {30000.0f, 0.0f};

/* The lower bound of a score (an aggregate initializer keeps it in .sdata). */
static s32 s_zero[1] = {0};

extern "C" void fn_8016F674(UnkFloatPair *pair, float value) {
    pair->m_a = value;
}

extern "C" void fn_8016F66C(UnkFloatPair *pair, float value) {
    pair->m_b = value;
}

extern "C" void fn_8016F62C(UnkCounters *counters) {
    counters->m_counts[0] = 0;
    counters->m_counts[1] = 0;
    counters->m_counts[2] = 0;
    memset(counters->m_frames, 0, sizeof(counters->m_frames));
}

extern "C" void fn_8016F5B8(UnkCounters *counters, int kind, int count) {
    if (count == 0) {
        return;
    }
    switch (kind) {
    case 0:
        counters->m_counts[0] += count;
        break;
    case 1:
        counters->m_counts[1] += count;
        break;
    case 2:
        counters->m_counts[2] += count;
        break;
    }
    counters->m_frames[kind] = lbl_8057D6A0.m_frame;
}

/* Resets the checkpoints to the stage's start positions. NonMatching (one register): retail keeps
 * the stage id in r30, the register the entry offset takes next; GC/2.7 puts it in r28. About 25
 * shapes were tried (declaration order, blocks, inline helpers, a chained store). */
extern "C" void fn_8016F4E8(UnkCheckpoints *points) {
    points->m_last = 0;
    points->m_frame = 0;
    points->m_time = 0.0f;
    int id = lbl_8057D6A0.m_stageId;
    UnkStageInfo_800CBAA4 *stage = &fn_800CBAA4()->m_stages[fn_80176C04(fn_800B74B4(), id)];
    for (int i = 0; i < 2; i++) {
        fn_800091D0(&points->m_points[i][0].m_position, &stage->m_starts[i].m_position);
        points->m_points[i][0].m_rotation.x = stage->m_starts[i].m_rotation.x;
        points->m_points[i][0].m_rotation.y = stage->m_starts[i].m_rotation.y;
        points->m_points[i][0].m_rotation.z = stage->m_starts[i].m_rotation.z;
        for (int j = 0; j < 32; j++) {
            points->m_points[i][j].m_valid = 0;
        }
    }
}

extern "C" int fn_8016F3D4(UnkCheckpoints *points, const Vec3f *position, Vec3f *rotation, int index) {
    if (index >= 32) {
        return 0;
    }
    points->m_frame = lbl_8057D6A0.m_frame;
    points->m_time = lbl_8057D6A0.m_time;
    for (int i = 0; i < 2; i++) {
        fn_800091D0(&points->m_points[i][index].m_position, position);
        points->m_points[i][index].m_rotation.x = rotation->x;
        points->m_points[i][index].m_rotation.y = rotation->y;
        points->m_points[i][index].m_rotation.z = rotation->z;
        points->m_points[i][index].m_position.x += 0.1f * i;
        points->m_points[i][index].m_valid = 1;
    }
    if (points->m_last < index) {
        points->m_last = index;
    }
    return 1;
}

extern "C" int fn_8016F3A0(UnkCheckpoints *points) {
    int count = 0;
    for (int i = 0; i < 32; i++) {
        if (points->m_points[0][i].m_valid) {
            count++;
        }
    }
    return count;
}

extern "C" Vec3f *fn_8016F380(UnkCheckpoints *points, int player) {
    return &points->m_points[player][points->m_last].m_position;
}

extern "C" Vec3f *fn_8016F360(UnkCheckpoints *points, int player) {
    return &points->m_points[player][points->m_last].m_rotation;
}

extern "C" Vec3f *fn_8016F344(UnkCheckpoints *points, int index, int player) {
    return &points->m_points[player][index].m_position;
}

extern "C" void fn_8016F2DC(s32 *bonus) {
    int index = lbl_8057D6A0.m_stageIndex;
    fn_800B74B4();
    if (reinterpret_cast<const UnkStageInfo_804C4A18 *>(reinterpret_cast<const u8 *>(lbl_804C4A18) + index * sizeof(UnkStageInfo_804C4A18))->m_flags & 8) {
        *bonus = 50;
    } else {
        *bonus = 0;
    }
}

/* Adds rings; every 1000 make the returned count of extra lives (behaviour inferred). */
extern "C" int fn_8016F288(s32 *rings, int count) {
    *rings += count;
    s32 value = *rings;
    if (value >= 1000) {
        *rings = value % 1000;
        return value / 1000;
    } else if (value < 0) {
        *rings = 0;
        return 0;
    }
    return 0;
}

extern "C" s32 fn_8016F280(s32 *rings) {
    return *rings;
}

extern "C" const s32 *fn_8016F268(const s32 *a, const s32 *b) {
    return (*a < *b) ? a : b;
}

extern "C" const s32 *fn_8016F250(const s32 *a, const s32 *b) {
    return (*a < *b) ? b : a;
}

extern "C" const s32 *fn_8016F21C(const s32 *value, const s32 *low, const s32 *high) {
    return fn_8016F268(fn_8016F250(value, low), high);
}

extern "C" void fn_8016F1C8(UnkScore *scores, int max) {
    memset(scores, 0, sizeof(UnkScore) * 2);
    for (int i = 0; i < 2; i++) {
        scores[i].m_max = max;
    }
}

extern "C" s32 fn_8016F1C0(UnkScore *score) {
    return score->m_max;
}

extern "C" void fn_8016F178(UnkScore *scores, int player, int value) {
    UnkScore *score = &scores[player];
    score->m_score = value;
    score->m_score = *fn_8016F21C(&score->m_score, s_zero, &score->m_max);
}

extern "C" void fn_8016F0CC(UnkScore *scores, int player, int points) {
    if (points > 0) {
        for (int i = 0; i < 2; i++) {
            if (scores[i].m_score >= scores[i].m_max) {
                return;
            }
        }
    }
    if (scores[player].m_lock > 0) {
        return;
    }
    if (points <= 0 && lbl_8057D6A0.field_0xC8) {
        return;
    }
    UnkScore *score = &scores[player];
    score->m_score += points;
    score->m_score = *fn_8016F21C(&score->m_score, s_zero, &score->m_max);
}

extern "C" void fn_8016F0B4(UnkScore *scores, int player) {
    scores[player].m_lock++;
}

extern "C" void fn_8016F094(UnkScore *scores, int player) {
    if (scores[player].m_lock > 0) {
        scores[player].m_lock--;
    }
}

extern "C" void fn_8016EFE8(UnkCounter *counter) {
    if (lbl_8057D6A0.field_0x125) {
        counter->m_value = fn_800BB74C()->field_0xA0;
    } else if (lbl_8057D6A0.field_0xCC == 1) {
        if (fn_80176C3C(fn_800B74B4(), -1)->m_id == 0x2C6) {
            counter->m_value = lbl_8057D6A0.field_0x128;
        } else {
            counter->m_value = fn_800BB74C()->field_0x94;
        }
    } else {
        counter->m_value = 5;
    }
    counter->m_frame = lbl_8057D6A0.m_frame;
}

extern "C" s32 fn_8016EF8C(UnkCounter *counter, int count) {
    if (count == 0) {
        return counter->m_value;
    }
    counter->m_value += count;
    if (counter->m_value > 99) {
        counter->m_value = 99;
    } else if (counter->m_value < 0) {
        counter->m_value = 0;
    }
    counter->m_frame = lbl_8057D6A0.m_frame;
    return counter->m_value;
}

extern "C" s32 fn_8016EF84(UnkCounter *counter) {
    return counter->m_value;
}

extern "C" s32 fn_8016EF7C(UnkCounter *counter) {
    return counter->m_frame;
}

extern "C" void fn_8016EF74(s32 *ammo, int count) {
    *ammo = count;
}

extern "C" s32 fn_8016EF38(s32 *ammo, int count) {
    *ammo += count;
    if (*ammo >= 999) {
        *ammo = 999;
    } else if (*ammo < 0) {
        *ammo = 0;
    }
    return *ammo;
}

extern "C" s32 fn_8016EF30(s32 *ammo) {
    return *ammo;
}

extern "C" void fn_8016EF10(UnkRecord_80178B5C *record) {
    record->field_0x0 = 0;
    record->field_0x4 = 0;
    record->field_0x8 = 0;
    record->field_0x10 = 0;
    record->field_0x14 = 0;
    record->field_0x18 = 0;
}

extern "C" void fn_8016EF08(UnkRecord_80178B5C *record, s32 value) {
    record->field_0x0 = value;
}

extern "C" void fn_8016EF00(UnkRecord_80178B5C *record, s32 value) {
    record->field_0x4 = value;
}

extern "C" void fn_8016EEF8(UnkRecord_80178B5C *record, s32 value) {
    record->field_0x8 = value;
}

extern "C" void fn_8016EEF0(UnkRecord_80178B5C *record, s32 value) {
    record->field_0xC = value;
}

extern "C" void fn_8016EEE8(UnkRecord_80178B5C *record, s32 value) {
    record->field_0x10 = value;
}

extern "C" void fn_8016EEE0(UnkRecord_80178B5C *record, s32 value) {
    record->field_0x14 = value;
}

extern "C" void fn_8016EED8(UnkRecord_80178B5C *record, s32 value) {
    record->field_0x18 = value;
}

extern "C" s32 fn_8016EED0(UnkRecord_80178B5C *record) {
    return record->field_0x0;
}

extern "C" s32 fn_8016EEC8(UnkRecord_80178B5C *record) {
    return record->field_0x8;
}

extern "C" s32 fn_8016EEC0(UnkRecord_80178B5C *record) {
    return record->field_0xC;
}
