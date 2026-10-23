/* StageSequence::PlayHard::Phase (class name from RTTI): the stage sequence phase that plays the
 * stages of a fixed list one after another (the hard mode; meaning inferred), keeping the position
 * in the list in the save data. After the last stage it plays a staff roll (StageStaffroll, ending
 * 4) and saves. The file name and the names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/Render.h"
#include "Game/SaveCycle.h"
#include "Game/StageManager.h"
#include "Game/StageSequence.h"
#include "Game/StageTable.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x125];
    /* 0x125 */ u8 field_0x125; /* meaning unknown */
    /* 0x126 */ u8 field_0x126[0x148 - 0x126];
    /* 0x148 */ u8 field_0x148[3]; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown; see Game/StageSequencePlayOnce.cpp). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ s32 m_hardMode; /* 0 start the list over, 1 continue it (meanings inferred) */
    /* 0x024 */ u8 field_0x24[0x230 - 0x24];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

/* The save data fn_800BB74C returns (class unknown; see Game/StageSequencePlayStory.cpp). */
struct UnkSaveData_800BB74C {
    /* 0x00 */ u8 field_0x0[0x9C];
    /* 0x9C */ u32 m_hardStage; /* the position in s_stages (name inferred) */
    /* 0xA0 */ s32 field_0xA0;  /* meaning unknown */
};

/* A task the save waits for (class unknown; see Game/StageSequencePlayStory.cpp); the halfword at
 * 0x4 holds its flags. */
struct UnkTask_802E3DBC {
    /* 0x0 */ u8 field_0x0[0x4];
    /* 0x4 */ u16 m_flags;
};

/* The message source (class name unknown; see Game/StageManager.cpp). */
class UnkHintTarget_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x64];
    /* 0x64 */ float m_scale;
};

/* TMessageDisp (class name from the RTTI of its shared_ptr control block; see
 * Game/StageManager.cpp). */
class TMessageDisp : public UnkHintTarget_801FB180 {
  public:
    /* 0x68 */ u8 field_0x68[0x74 - 0x68];
};

extern "C" {
/* Sets count bytes at dest to value (behaviour inferred). */
void fn_800CBBFC(void *dest, int value, int count);
UnkSaveData_800BB74C *fn_800BB74C(void);
TMessageDisp *fn_8004ABD0(TMessageDisp *message, const char *name, int arg2);
/* The message manager (see Game/StageSequencePlayStory.cpp). */
void *fn_80117514(void);
void fn_802E5958(void *manager);
void fn_802E597C(void *manager, const boost::weak_ptr<UnkHintTarget_801FB180> &target);
bool fn_802E5810(void *manager);
bool fn_802B8DF0(UnkTask_802E3DBC *task);
}

/* The stages of the list, as stage table indexes (name inferred). */
static s32 s_stages[23] = {5,    6,    7,    8,    9,    0xA,  0xB,  0xC,  0xD,  0xE,  0xF,  0x10,
                           0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B};

/* The stage table entry at index (name inferred; the lookup ignores the object it is called on). */
inline const UnkStageInfo_804C4A18 *StageEntry(int index) {
    fn_800B74B4();
    return &lbl_804C4A18[index];
}

namespace StageSequence {
namespace PlayHard {

class phaseImpl {
  public:
    phaseImpl();
    ~phaseImpl();

    /* Runs the phase; false once it ended. */
    bool Tick();
    /* Ends the stage early (name inferred). */
    void Skip();

    /* The states (names inferred). */
    void PlayStage();
    void PlayStaffroll();
    void WaitSave();

    /* 0x00 */ s32 m_state;
    /* 0x04 */ boost::shared_ptr<UnkHintTarget_801FB180> m_message;
    /* 0x0C */ UnkTask_802E3DBC *m_saveTask;
    /* 0x10 */ boost::shared_ptr<StageBase::Base> m_stage;
};

/* Deletes the phaseImpl (name inferred; see checked_delete below). */
inline void DeleteImpl(phaseImpl *impl) {
    delete impl;
}

} // namespace PlayHard
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::PlayHard::phaseImpl>(StageSequence::PlayHard::phaseImpl *x) {
    StageSequence::PlayHard::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::PlayHard::phaseImpl, 0x18);
SIZE_ASSERT(StageSequence::PlayHard::Phase, 0x8);

namespace StageSequence {
namespace PlayHard {

phaseImpl::phaseImpl() : m_saveTask(0) {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    switch (lbl_80584720.m_hardMode) {
    case 0: {
        save->m_hardStage = 0;
        save->field_0xA0 = 0;
        UnkSaveData_800BB74C *data = fn_800BB74C();
        data->field_0xA0 = 3;
        break;
    }
    case 1:
        if (save->m_hardStage >= 23) {
            save->m_hardStage = 0;
            save->field_0xA0 = 0;
        }
        break;
    }
    lbl_8057D6A0.field_0x125 = 1;
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
    m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(StageEntry(s_stages[save->m_hardStage])->m_id, 0));
    m_state = 0;
}

phaseImpl::~phaseImpl() {}

bool phaseImpl::Tick() {
    switch (m_state) {
    case 0:
        PlayStage();
        break;
    case 1:
        PlayStaffroll();
        break;
    case 2:
        WaitSave();
        break;
    case 3:
        return false;
    }
    return true;
}

void phaseImpl::PlayStage() {
    switch (m_stage->Tick()) {
    case 1:
        break;
    case 2:
        m_state = 3;
        break;
    case 3:
    case 4:
    case 5:
    case 6: {
        UnkSaveData_800BB74C *save = fn_800BB74C();
        if (save->m_hardStage < 23) {
            m_stage.reset();
            fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
            m_stage = boost::shared_ptr<StageBase::Base>(
                new StageManager(StageEntry(s_stages[save->m_hardStage])->m_id, 0));
        } else {
            m_stage.reset();
            m_stage = boost::shared_ptr<StageBase::Base>(new StageStaffroll(4, 1));
            m_state = 1;
            save->m_hardStage = 0;
        }
        break;
    }
    }
}

void phaseImpl::PlayStaffroll() {
    if (m_stage->Tick() == 8) {
        TMessageDisp *message = (TMessageDisp *)operator new(0x74);
        if (message != 0) {
            message = fn_8004ABD0(message, "Save", 1);
        }
        m_message = boost::shared_ptr<UnkHintTarget_801FB180>(message);
        fn_802E597C(fn_80117514(), boost::weak_ptr<UnkHintTarget_801FB180>(m_message));
        m_saveTask = (UnkTask_802E3DBC *)new SaveCycle::CycleTask(
            (Task *)fn_80046178()->field_0x40, SaveCycle::PHASE_SAVE);
        m_state = 2;
    }
}

void phaseImpl::WaitSave() {
    if (m_saveTask != 0) {
        if (fn_802B8DF0(m_saveTask)) {
            m_saveTask->m_flags |= 1;
            m_saveTask = 0;
            fn_802E5958(fn_80117514());
            m_state = 3;
        }
    } else if (fn_802E5810(fn_80117514())) {
        m_state = 3;
    }
}

void phaseImpl::Skip() {
    m_stage->vf_0x10();
}

Phase::Phase() : m_impl(new phaseImpl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

void Phase::Skip() {
    m_impl->Skip();
}

} // namespace PlayHard
} // namespace StageSequence
