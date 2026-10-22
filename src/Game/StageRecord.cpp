/* The current stage's record in the game state (at 0x8057D748, lbl_8057D6A0 + 0xA8): which stage
 * is played, the flags copied from its stage-table entry, and a few switches the stage code sets
 * and tests. The functions keep their fn_ names, which other units call. A function on another
 * structure (fn_80203A10, never called) follows them. File and struct names inferred. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/StageTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The record (struct name unknown). */
struct UnkStageRecord_8057D748 {
    /* 0x00 */ s32 m_id;    /* the stage id, or -1 */
    /* 0x04 */ s32 m_index; /* its index in the stage table, or -1 */
    /* 0x08 */ u8 field_0x8;
    /* 0x09 */ u8 m_flag1;  /* stage-table flag 0x01 */
    /* 0x0A */ u8 m_flag4;  /* stage-table flag 0x04 */
    /* 0x0B */ u8 m_flag8;  /* stage-table flag 0x08 */
    /* 0x0C */ u8 m_flag20; /* stage-table flag 0x20 */
    /* 0x0D */ u8 field_0xD;
    /* 0x0E */ u8 field_0xE[0x18 - 0xE];
    /* 0x18 */ u8 field_0x18;
    /* 0x19 */ u8 field_0x19;
    /* 0x1A */ u8 field_0x1A[0x20 - 0x1A];
    /* 0x20 */ u8 m_busy; /* set by most stage actions (meaning inferred) */
    /* 0x21 */ u8 field_0x21;
    /* 0x22 */ u8 field_0x22;
    /* 0x23 */ u8 field_0x23;
};

/* The structure fn_80203A10 sets up (struct name unknown). */
struct UnkScaled_80203A10 {
    /* 0x00 */ s32 m_count;
    /* 0x04 */ u8 field_0x4[0xC - 0x4];
    /* 0x0C */ float field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ float field_0x18;
    /* 0x1C */ float field_0x1C;
    /* 0x20 */ float field_0x20;
};

/* Sets the count (at least 1) and scales two values (name and meaning inferred; never called). */
extern "C" void fn_80203A10(UnkScaled_80203A10 *object, int count, float scale) {
    object->m_count = count;
    if (object->m_count < 1) {
        object->m_count = 1;
    }
    object->field_0x1C = scale * object->field_0x10;
    object->field_0x20 = scale * object->field_0x18;
    object->field_0xC = scale * object->field_0x10;
    object->field_0x14 = scale * object->field_0x18;
}

/* Whether either of the bytes at 0x21 and 0x23 is set (name inferred). */
extern "C" bool fn_802039E8(UnkStageRecord_8057D748 *record) {
    return record->field_0x21 || record->field_0x23;
}

extern "C" void fn_802039E0(UnkStageRecord_8057D748 *record, u8 value) {
    record->field_0x21 = value;
}

extern "C" u8 fn_802039D8(UnkStageRecord_8057D748 *record) {
    return record->field_0x22;
}

extern "C" void fn_802039D0(UnkStageRecord_8057D748 *record, u8 value) {
    record->field_0x22 = value;
}

extern "C" u8 fn_802039C8(UnkStageRecord_8057D748 *record) {
    return record->field_0x23;
}

extern "C" void fn_802039C0(UnkStageRecord_8057D748 *record, u8 value) {
    record->field_0x23 = value;
}

/* Sets or clears m_busy; returns whether it changed (name inferred). */
extern "C" bool fn_80203970(UnkStageRecord_8057D748 *record, bool set) {
    if (set && !record->m_busy) {
        record->m_busy = 1;
        return true;
    }
    if (!set && record->m_busy) {
        record->m_busy = 0;
        return true;
    }
    return false;
}

extern "C" void fn_80203890(UnkStageRecord_8057D748 *record);

/* Starts stage id: looks up its entry and copies its flags (name inferred). */
extern "C" bool fn_802038B4(UnkStageRecord_8057D748 *record, s32 id) {
    record->m_id = id;
    record->m_index = fn_80176C04(fn_800B74B4(), record->m_id);
    int index = record->m_index;
    fn_800B74B4();
    const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
    if (info == 0) {
        fn_80203890(record);
        return false;
    }
    record->m_flag1 = info->m_flags & 1;
    record->m_flag4 = (info->m_flags >> 2) & 1;
    record->m_flag8 = (info->m_flags >> 3) & 1;
    record->m_flag20 = (info->m_flags >> 5) & 1;
    record->field_0x8 = record->m_flag1;
    record->field_0x21 = 0;
    record->field_0x23 = 0;
    record->field_0xD = 0;
    record->field_0x18 = 0;
    record->field_0x19 = 0;
    return true;
}

/* Clears the record (name inferred). */
extern "C" void fn_80203890(UnkStageRecord_8057D748 *record) {
    record->m_id = -1;
    record->m_index = -1;
    if (record->m_busy) {
        record->m_busy = 0;
    }
}
