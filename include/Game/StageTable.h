#ifndef GAME_STAGETABLE_H
#define GAME_STAGETABLE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The stage table (Game/StageTable.cpp): one 0x50-byte entry per stage, with its id, name, flags,
 * callbacks and missions (struct names unknown; Game/Mission.cpp declares its own view of it).
 * Member names are inferred. */
/* A mission of a stage entry (struct name unknown). */
struct UnkStageMission_804C4A18 {
    /* 0x0 */ s32 field_0x0;
    /* 0x4 */ s32 m_id;
    /* 0x8 */ s32 field_0x8;
};

/* A stage's entry in the stage table (struct name unknown; see Game/Mission.cpp). */
struct UnkStageInfo_804C4A18 {
    /* 0x00 */ s32 m_id;
    /* 0x04 */ const char *m_name;
    /* 0x08 */ u32 m_flags;
    /* 0x0C */ void (*m_callbacks[4])(void);
    /* 0x1C */ UnkStageMission_804C4A18 m_missions[3];
    /* 0x40 */ s32 m_missionId;
    /* 0x44 */ s32 field_0x44;
    /* 0x48 */ s32 field_0x48;
    /* 0x4C */ s32 field_0x4C;
};
SIZE_ASSERT(UnkStageInfo_804C4A18, 0x50);

/* The object the lookups belong to (class unknown; fn_800B74B4 returns it). They ignore it. */
class UnkObject_800B74B4;

/* The table (name inferred; other units reach it as lbl_804C4A18). */
extern const UnkStageInfo_804C4A18 lbl_804C4A18[59];

extern "C" {
UnkObject_800B74B4 *fn_800B74B4(void);
/* The stage entry of id, or of the current stage when id is -1. */
UnkStageInfo_804C4A18 *fn_80176C3C(UnkObject_800B74B4 *object, int id);
/* The index of the stage entry of id, or -1. */
int fn_80176C04(UnkObject_800B74B4 *object, int id);
/* The mission id of a player in the stage entry at index. */
s32 fn_80176BE4(UnkObject_800B74B4 *object, int index, int player);
}

#endif
