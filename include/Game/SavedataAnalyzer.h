#ifndef GAME_SAVEDATAANALYZER_H
#define GAME_SAVEDATAANALYZER_H

/* SavedataAnalyzer::StoryCheck (class name from the RTTI of its shared_ptr control block in
 * Game/AdvSelectMode.cpp): what the front-end works out from the save data while its menus are
 * open (0x100 bytes). Its constructor (fn_801AC1B4), destructor (fn_800BBC04, which also frees it
 * when free is set) and other functions keep their fn_ names; their meanings and the member names
 * are inferred. */

#include <dolphin/types.h>
#include "size_assert.h"

/* The save data (struct name unknown; see Game/GoalAction.cpp). */
struct UnkSaveData_800BB74C;

namespace SavedataAnalyzer {

class StoryCheck {
  public:
    /* 0x000 */ UnkSaveData_800BB74C *m_save;
    /* 0x004 */ u8 field_0x4[0x14 - 0x4]; /* built by fn_801AC1F0, destroyed by fn_800BBC54 */
    /* 0x014 */ s32 m_stages[59];         /* a word per stage; bit 0 shows its picture */
};
SIZE_ASSERT(StoryCheck, 0x100);

} // namespace SavedataAnalyzer

extern "C" {
SavedataAnalyzer::StoryCheck *fn_801AC1B4(SavedataAnalyzer::StoryCheck *check,
                                          UnkSaveData_800BB74C *save);
void fn_800BBC04(SavedataAnalyzer::StoryCheck *check, int free);
/* Works the save data out (behaviour inferred). */
void fn_801ABDB8(SavedataAnalyzer::StoryCheck *check);
/* The word of stage (m_stages[stage]). */
s32 fn_801AC1A4(SavedataAnalyzer::StoryCheck *check, int stage);
}

#endif
