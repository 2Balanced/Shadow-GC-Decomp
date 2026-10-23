/* StageAction::FlashbackAction (class name from RTTI): the stage action after a flashback stage of
 * the story mode. It stops the characters and the stage like Game/EventAction.cpp, then, once no
 * event task runs, takes the front of the flashback queue: if the entry is the current stage it
 * plays the entry's event and pops it, otherwise (or when the queue is empty) it ends the stage
 * (state 16). File name inferred. Names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/FlashbackQueue.h"
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
    /* 0x0A8 */ s32 m_stageRecord[0x24 / 4]; /* the current stage (see Game/StageRecord.cpp); its
                                              * first word is the stage id */
    /* 0x0CC */ u8 field_0xCC[0x150 - 0xCC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown; see Game/StageSequencePlayOnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ UnkFlashbackQueue_803546C0 m_flashbacks;
    /* 0x228 */ u8 field_0x228[0x230 - 0x228];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

/* The hint manager (class name unknown; see Game/UnkHintManager.h). */
class UnkHintManager_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u8 m_taskActive;
    /* 0x2D */ u8 m_altLayout;
};

/* The out-of-line copy of the inline accessor Game/HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

/* An object with a word at 0x30 (class unknown; see Game/EventAction.cpp). */
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

/* What fn_801723E0 returns (class unknown; see Game/GoalAction.cpp). */
struct UnkEventHolder_801723E0 {
    /* 0x0 */ void *m_task;
};

struct UnkCharaList_80074194;
struct UnkSound_80053164;

extern "C" {
UnkCharaList_80074194 *fn_80074194(void);
/* Stops and restarts the characters (behaviour inferred). */
void fn_800A8D48(UnkCharaList_80074194 *list);
void fn_800A8D9C(UnkCharaList_80074194 *list);
void fn_800A8DFC(UnkCharaList_80074194 *list);
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(void *record, bool set);
/* The event holder, and starting an event through it (see Game/GoalAction.cpp). */
UnkEventHolder_801723E0 *fn_801723E0(void);
void fn_8017430C(UnkEventHolder_801723E0 *holder, s32 event);
/* Whether the event can start (behaviour inferred). */
bool fn_80177CB8(void);
/* The sound manager and its event switches (behaviour inferred; see Game/EndAction.cpp). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D4E78(UnkSound_80053164 *sound);
void fn_801D4E50(UnkSound_80053164 *sound);
/* Unknown resets (behaviour unknown). */
void *fn_800522A8(void);
void fn_80058B38(void *object);
/* The hint manager's event mode (behaviour inferred). */
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
}

namespace StageAction {

FlashbackAction::FlashbackAction() {
    m_state = 0;
    fn_800A8D48(fn_80074194());
    fn_80203970(lbl_8057D6A0.m_stageRecord, true);
    lbl_8057D6A0.field_0x1C = 0.0f;
    lbl_8057D6A0.field_0x60 = 0;
    if (lbl_805EF9F4 != 0) {
        lbl_805EF9F4->Stop();
    }
    fn_801D4E78(fn_80053164());
    fn_80058B38(fn_800522A8());
    fn_800A8D9C(fn_80074194());
    UnkHintManager_801FB180 *hints = GetHintManager();
    hints->m_taskActive = 1;
    fn_801FAAB4(GetHintManager());
    hints = GetHintManager();
    hints->m_altLayout = 1;
}

FlashbackAction::~FlashbackAction() {
    fn_801D4E50(fn_80053164());
    lbl_8057D6A0.field_0x1C = 1.0f;
    fn_800A8DFC(fn_80074194());
    fn_801FAAA4(GetHintManager());
    UnkHintManager_801FB180 *hints = GetHintManager();
    hints->m_altLayout = 0;
}

void FlashbackAction::Execute() {
    switch (m_state) {
    case 0:
        if (fn_801723E0()->m_task == 0) {
            m_entry = lbl_80584720.m_flashbacks.Front();
            if (m_entry != 0 && m_entry->m_stageId != 0) {
                if (m_entry->m_stageId == lbl_8057D6A0.m_stageRecord[0]) {
                    m_state = 1;
                } else {
                    fn_80175EEC(fn_80074470(), 16, true);
                }
            } else {
                fn_80175EEC(fn_80074470(), 16, true);
            }
        }
        break;
    case 1:
        if (fn_80177CB8()) {
            fn_8017430C(fn_801723E0(), m_entry->m_event);
            lbl_80584720.m_flashbacks.Pop();
            m_state = 0;
        }
        break;
    }
}

} // namespace StageAction
