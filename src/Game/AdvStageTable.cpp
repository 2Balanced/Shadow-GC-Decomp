/* The stage tables of the front-end's select screens (Game/AdvSelectMode.cpp asks them): the
 * stage at a place of the two story maps (six rows of five) and of the boss list, optionally only
 * when one of the stage's missions has been cleared. The file name and every name are inferred;
 * the functions keep their fn_ names. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

/* The save data (struct name unknown; see Game/GoalAction.cpp). */
struct UnkMissionSave_800BB74C {
    /* 0x00 */ u8 m_cleared;
    /* 0x01 */ u8 field_0x1[0x18 - 0x1];
};

struct UnkStageSave_800BB74C {
    /* 0x00 */ s32 m_plays;
    /* 0x04 */ UnkMissionSave_800BB74C m_missions[3];
    /* 0x4C */ u8 field_0x4C[0x60 - 0x4C];
};
SIZE_ASSERT(UnkStageSave_800BB74C, 0x60);

struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0xA8];
    /* 0x00A8 */ UnkStageSave_800BB74C m_stages[59];
};

extern "C" {
UnkSaveData_800BB74C *fn_800BB74C(void);
}

/* The stages of the first and the second story map, by row and column, and of the boss list
 * (-1 where there is none). */
static const s32 s_storyStages[6][5] = {
    {-1, -1, 5, -1, -1},     {-1, 6, 7, 8, -1},       {-1, 9, 10, 11, -1},
    {12, 13, 14, 15, 16},    {17, 18, 19, 20, 21},    {22, 23, 24, 25, 26},
};
static const s32 s_storyStages2[6][5] = {
    {-1, -1, -1, -1, -1},    {-1, -1, -1, 28, -1},    {-1, 29, -1, -1, -1},
    {-1, 30, -1, 31, 32},    {33, -1, 34, -1, -1},    {-1, -1, -1, -1, -1},
};
static const s32 s_bossStages[10] = {36, 35, 38, 37, 39, 39, 40, 41, 43, 42};

/* The stage at row, column of the first map, or -1; with cleared, -1 also when none of its
 * missions has been cleared. */
extern "C" s32 fn_802BF3C0(int row, int column, bool cleared) {
    if (row < 0 || row >= 6) {
        return -1;
    }
    if (column < 0 || column >= 5) {
        return -1;
    }
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (!cleared) {
        return s_storyStages[row][column];
    }
    for (int i = 0; i < 3; i++) {
        if (save->m_stages[s_storyStages[row][column]].m_missions[i].m_cleared) {
            return s_storyStages[row][column];
        }
    }
    return -1;
}

/* The same for the second map. */
extern "C" s32 fn_802BF2EC(int row, int column, bool cleared) {
    if (row < 0 || row >= 6) {
        return -1;
    }
    if (column < 0 || column >= 5) {
        return -1;
    }
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (!cleared) {
        return s_storyStages2[row][column];
    }
    for (int i = 0; i < 3; i++) {
        if (save->m_stages[s_storyStages2[row][column]].m_missions[i].m_cleared) {
            return s_storyStages2[row][column];
        }
    }
    return -1;
}

/* The same for entry index of the boss list (which the game lets run to 10). */
extern "C" s32 fn_802BF244(int index, bool cleared) {
    if (index < 0 || index > 10) {
        return -1;
    }
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (!cleared) {
        return s_bossStages[index];
    }
    for (int i = 0; i < 3; i++) {
        if (save->m_stages[s_bossStages[index]].m_missions[i].m_cleared) {
            return s_bossStages[index];
        }
    }
    return -1;
}
