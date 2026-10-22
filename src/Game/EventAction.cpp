/* StageAction::EventAction (class name from RTTI): the stage action while an event plays. It stops
 * the characters and the stage, locks the players' scores, switches the hint manager to the event
 * layout and hides the HUD; its destructor undoes all of that. Execute does nothing. File name
 * inferred. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/StageAction.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C; /* meaning unknown */
    /* 0x020 */ u8 field_0x20[0x60 - 0x20];
    /* 0x060 */ s32 field_0x60; /* meaning unknown */
    /* 0x064 */ u8 field_0x64[0xA8 - 0x64];
    /* 0x0A8 */ u8 m_stageRecord[0x24]; /* the current stage (see Game/StageRecord.cpp) */
    /* 0x0CC */ u8 field_0xCC[0x148 - 0xCC];
    /* 0x148 */ u8 m_hudVisible; /* meaning inferred */
    /* 0x149 */ u8 field_0x149[0x150 - 0x149];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The hint manager (class name unknown; see Game/UnkHintManager.h). */
class UnkHintManager_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u8 m_taskActive;
    /* 0x2D */ u8 m_altLayout;
};

/* The out-of-line copy of the inline accessor Game/HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

/* An object with a word at 0x30 (class unknown; see Game/PlayAction.cpp). */
struct UnkObject_805EF9F4;

extern "C" {
void fn_801E2F28(UnkObject_805EF9F4 *object);
void fn_801E2EDC(UnkObject_805EF9F4 *object);
}

struct UnkObject_805EF9F4 {
    /* Stops the object (name inferred). */
    void Stop() {
        fn_801E2F28(this);
        fn_801E2EDC(this);
        field_0x30 = 0;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 field_0x30;
};

extern UnkObject_805EF9F4 *lbl_805EF9F4;

/* The players' scores (struct name unknown; see Game/SaveRecord.cpp). */
struct UnkScore;

/* The stage data fn_80079808 returns (class unknown). */
struct UnkStageData_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_scores[4]; /* the players' UnkScore records */
};

/* What fn_801723E0 returns (class unknown; see Game/PjsEventActionFade.cpp). */
struct UnkEventHolder_801723E0 {
    /* 0x0 */ void *m_task;
};

struct UnkCharaList_80074194;
struct UnkSound_80053164;

extern "C" {
UnkCharaList_80074194 *fn_80074194(void);
/* Stops and restarts the characters (behaviour inferred). */
void fn_800A8CF4(UnkCharaList_80074194 *list);
void fn_800A8D9C(UnkCharaList_80074194 *list);
void fn_800A8DFC(UnkCharaList_80074194 *list);
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(void *record, bool set);
UnkEventHolder_801723E0 *fn_801723E0(void);
/* The sound manager, and its event switches (behaviour inferred). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D4E20(UnkSound_80053164 *sound, s32 arg1);
void fn_801D4DF0(UnkSound_80053164 *sound, s32 arg1);
/* Unknown resets (behaviour unknown). */
void *fn_800522A8(void);
void fn_80058B38(void *object);
UnkStageData_80079808 *fn_80079808(void);
/* Locks and unlocks a player's score (Game/SaveRecord.cpp). */
void fn_8016F0B4(UnkScore *scores, int player);
void fn_8016F094(UnkScore *scores, int player);
/* The hint manager's event mode (behaviour inferred). */
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
}

/* Whether no event task was running when the action started (name and meaning inferred). */
static s32 s_noEvent;

namespace StageAction {

EventAction::EventAction() {
    fn_800A8CF4(fn_80074194());
    fn_80203970(lbl_8057D6A0.m_stageRecord, true);
    lbl_8057D6A0.field_0x1C = 0.0f;
    lbl_8057D6A0.field_0x60 = 0;
    if (lbl_805EF9F4 != 0) {
        lbl_805EF9F4->Stop();
    }
    s_noEvent = (fn_801723E0()->m_task == 0) ? 1 : 0;
    fn_801D4E20(fn_80053164(), s_noEvent);
    fn_80058B38(fn_800522A8());
    fn_800A8D9C(fn_80074194());
    UnkScore *scores = reinterpret_cast<UnkScore *>(fn_80079808()->m_scores);
    fn_8016F0B4(scores, 0);
    fn_8016F0B4(scores, 1);
    UnkHintManager_801FB180 *hints = GetHintManager();
    hints->m_taskActive = 1;
    fn_801FAAB4(GetHintManager());
    hints = GetHintManager();
    hints->m_altLayout = 1;
    m_hudVisible = lbl_8057D6A0.m_hudVisible;
    lbl_8057D6A0.m_hudVisible = 0;
}

EventAction::~EventAction() {
    UnkScore *scores = reinterpret_cast<UnkScore *>(fn_80079808()->m_scores);
    fn_8016F094(scores, 0);
    fn_8016F094(scores, 1);
    fn_801D4DF0(fn_80053164(), s_noEvent);
    lbl_8057D6A0.field_0x1C = 1.0f;
    fn_800A8DFC(fn_80074194());
    fn_801FAAA4(GetHintManager());
    UnkHintManager_801FB180 *hints = GetHintManager();
    hints->m_altLayout = 0;
    lbl_8057D6A0.m_hudVisible = m_hudVisible;
}

void EventAction::Execute() {}

} // namespace StageAction
