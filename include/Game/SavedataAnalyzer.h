#ifndef GAME_SAVEDATAANALYZER_H
#define GAME_SAVEDATAANALYZER_H

/* SavedataAnalyzer::StoryCheck (class name from the RTTI of its shared_ptr control block in
 * Game/AdvSelectMode.cpp): what the front-end works out from the save data while its menus are
 * open (0x100 bytes). Its constructor (0x801AC1B4), destructor (emitted in Game/folderStoryRoute.cpp,
 * 0x800BBC04) and other functions keep their fn_ names; their meanings and the member names
 * are inferred. */

#include <dolphin/types.h>
#include "size_assert.h"

/* The save data (struct name unknown; see Game/GoalAction.cpp). */
struct UnkSaveData_800BB74C;

namespace SavedataAnalyzer {

#ifdef SAVEDATA_ANALYZER_LINES
/* A line between two stages on the story chart, keyed by "line %3d-%3d" (Game/SavedataAnalyzer.cpp;
 * struct name unknown, member names inferred). */
struct UnkStoryLine_801ABDB8 {
    /* 0x0 */ s32 m_from;  /* stage id */
    /* 0x4 */ s32 m_to;    /* stage id */
    /* 0x8 */ u32 m_flags; /* bit 0: a cleared mission leads along it, bit 1: the story went along it */
};
#endif

class StoryCheck {
  public:
    /* Built from save (Game/SavedataAnalyzer.cpp; 0x801AC1B4). */
    StoryCheck(UnkSaveData_800BB74C *save);
    /* Destroys the lines (Game/folderStoryRoute.cpp). */
    ~StoryCheck();

    /* 0x000 */ UnkSaveData_800BB74C *m_save;
#ifdef SAVEDATA_ANALYZER_LINES
    /* 0x004 */ std::map<std::string, UnkStoryLine_801ABDB8> m_lines; /* built by fn_801AC1F0 */
#else
    /* 0x004 */ u8 field_0x4[0x14 - 0x4]; /* built by fn_801AC1F0, destroyed by its destructor */
#endif
    /* 0x014 */ s32 m_stages[59];         /* a word per stage; bit 0 shows its picture */
};
SIZE_ASSERT(StoryCheck, 0x100);

} // namespace SavedataAnalyzer

extern "C" {
/* Works the save data out; false without save data (behaviour inferred). */
bool fn_801ABDB8(SavedataAnalyzer::StoryCheck *check);
/* The word of stage (m_stages[stage]). */
s32 fn_801AC1A4(SavedataAnalyzer::StoryCheck *check, int stage);
}

#endif
