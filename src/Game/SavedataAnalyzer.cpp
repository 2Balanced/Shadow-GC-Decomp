/* SavedataAnalyzer::StoryCheck (class name from the RTTI of its shared_ptr control block in
 * Game/AdvSelectMode.cpp; Game/SavedataAnalyzer.h): what the select-mode screens work out from the
 * save data. For every stage played it marks the stage, and for every line of the story chart that
 * a cleared mission leads along, or that the story went along, it keeps a record in a
 * std::map<std::string, ...> keyed by "line %3d-%3d" (stand-in in MSL_C++). The destructor is
 * emitted elsewhere (0x800BBC04). File name from the class's namespace; the other functions keep their
 * address names, which the select-mode units call. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * NonMatching: 33 of 34 functions and the data match. In fn_801ABDB8's second loop retail keeps the
 * index of the next stage in the register of the previous one (r23); GC/2.7 puts it in r20, for
 * every source form tried (a reused or a new variable, the index as the lookup's argument, a
 * pointer to the element, other declaration orders). */
#define MSL_LESS_CAST
#define MSL_PAIR_COPY_CALL
#define MSL_STRING_COPY
#define MSL_STRING_LESS
#define MSL_STRING_COMPARE_INLINE
#define MSL_TREE_END_NODE_NAME __tree_end_node_StoryCheck
#include <dolphin/types.h>
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>
#define SAVEDATA_ANALYZER_LINES
#include "Game/SavedataAnalyzer.h"
#include "Game/StageTable.h"
#include "size_assert.h"

using SavedataAnalyzer::StoryCheck;
using SavedataAnalyzer::UnkStoryLine_801ABDB8;

/* The save data as this unit reads it (struct name unknown; see Game/AdvSelectMode.cpp). */
struct UnkMissionSave_801ABDB8 {
    /* 0x00 */ u8 m_cleared;
    /* 0x01 */ u8 field_0x1[0x18 - 0x1];
};

struct UnkStageSave_801ABDB8 {
    /* 0x00 */ s32 m_plays;
    /* 0x04 */ UnkMissionSave_801ABDB8 m_missions[3];
    /* 0x4C */ u8 field_0x4C[0x60 - 0x4C];
};
SIZE_ASSERT(UnkStageSave_801ABDB8, 0x60);

/* The way the story went: the stages played, as indices in the stage table, the last one being the
 * current one (struct name unknown; meaning inferred). */
struct UnkStoryPath_800BB74C {
    /* 0x00 */ s32 m_count; /* the lines gone along */
    /* 0x04 */ s32 m_stages[29];
};

struct UnkSaveData_800BB74C {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ UnkStoryPath_800BB74C m_path;
    /* 0xA8 */ UnkStageSave_801ABDB8 m_stages[59];
};

extern "C" {
/* The save data in use. */
UnkSaveData_800BB74C *fn_800BB74C(void);
}

/* The stage table's entry at index (name inferred; the table is reached through its owner, which
 * the lookup ignores). */
inline const UnkStageInfo_804C4A18 &StageInfo(int index) {
    fn_800B74B4();
    return lbl_804C4A18[index];
}

typedef std::map<std::string, UnkStoryLine_801ABDB8> LineMap;

/* The key of a line (name inferred). */
static char s_lineName[32];

StoryCheck::StoryCheck(UnkSaveData_800BB74C *save) {
    m_save = save;
}

s32 fn_801AC1A4(StoryCheck *check, int stage) {
    return check->m_stages[stage];
}

bool fn_801ABDB8(StoryCheck *check) {
    UnkSaveData_800BB74C *save;
    if (check->m_save != 0) {
        save = check->m_save;
    } else {
        UnkSaveData_800BB74C *current = fn_800BB74C();
        save = current;
        if (current == 0) {
            return false;
        }
    }
    check->m_lines.clear();
    memset(check->m_stages, 0, sizeof(check->m_stages));
    for (int i = 0; i < 59; i++) {
        UnkStageSave_801ABDB8 *stage = &save->m_stages[i];
        if (stage->m_plays != 0) {
            check->m_stages[i] |= 1;
            for (int j = 0; j < 3; j++) {
                if (stage->m_missions[j].m_cleared) {
                    UnkStoryLine_801ABDB8 line;
                    const UnkStageInfo_804C4A18 &info = StageInfo(i);
                    line.m_from = info.m_id;
                    line.m_to = StageInfo(info.m_missions[j].field_0x0).m_id;
                    line.m_flags |= 1;
                    sprintf(s_lineName, "line %3d-%3d", line.m_from, line.m_to);
                    std::string name = std::string(s_lineName);
                    LineMap::iterator it = check->m_lines.find(name);
                    if (it == check->m_lines.end()) {
                        check->m_lines.insert(
                            LineMap::value_type(name, line));
                    }
                }
            }
        }
    }
    int k;
    UnkStoryPath_800BB74C *path = &save->m_path;
    for (k = 0; k < path->m_count; k++) {
        check->m_stages[path->m_stages[k]] |= 2;
        UnkStoryLine_801ABDB8 line;
        int index = path->m_stages[k];
        line.m_from = StageInfo(index).m_id;
        index = path->m_stages[k + 1];
        line.m_to = StageInfo(index).m_id;
        line.m_flags |= 2;
        sprintf(s_lineName, "line %3d-%3d", line.m_from, line.m_to);
        std::string name = std::string(s_lineName);
        LineMap::iterator it = check->m_lines.find(name);
        if (it == check->m_lines.end()) {
            check->m_lines.insert(LineMap::value_type(name, line));
        } else {
            (*it).second.m_flags |= 2;
        }
    }
    check->m_stages[path->m_stages[path->m_count]] |= 4;
    return true;
}

#include <map_impl>
