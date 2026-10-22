/* StageSequence::Play2POnce::Phase (class name from RTTI): the stage sequence phase that plays a
 * two-player stage. Its phaseImpl plays a StageManager for the stage chosen in the stage info,
 * and starts the stage again while the StageManager asks for a restart. The file name and the
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/StageManager.h"
#include "Game/StageSequence.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x12C];
    /* 0x12C */ s32 field_0x12C; /* meaning unknown */
    /* 0x130 */ s32 field_0x130; /* meaning unknown */
    /* 0x134 */ s32 field_0x134; /* meaning unknown */
    /* 0x138 */ s32 field_0x138; /* meaning unknown */
    /* 0x13C */ u8 field_0x13C[0x144 - 0x13C];
    /* 0x144 */ s32 field_0x144; /* meaning unknown */
    /* 0x148 */ u8 field_0x148[3]; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stage to play (struct name unknown). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0xC];
    /* 0x00C */ s32 m_stageId; /* (name inferred) */
    /* 0x010 */ u8 field_0x10[0x230 - 0x10];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

extern "C" {
/* Sets count bytes at dest to value (behaviour inferred). */
void fn_800CBBFC(void *dest, int value, int count);
}

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for StageBase::Base (boost 1.32's definition, as in Coaster.cpp). */
template <class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

/* Clears the game state's stage results before a stage starts (name and meaning inferred). */
inline void ResetResults() {
    lbl_8057D6A0.field_0x12C = 1;
    lbl_8057D6A0.field_0x130 = 0;
    lbl_8057D6A0.field_0x138 = 0;
    lbl_8057D6A0.field_0x134 = 0;
    lbl_8057D6A0.field_0x144 = -2;
}

namespace StageSequence {
namespace Play2POnce {

class phaseImpl {
  public:
    phaseImpl();
    ~phaseImpl();

    /* Runs the phase; false once it ended. */
    bool Tick();
    /* Ends the stage early (name inferred). */
    void Skip();

    /* 0x0 */ boost::shared_ptr<StageBase::Base> m_stage;
};

/* Deletes the phaseImpl (name inferred; see checked_delete below). */
inline void DeleteImpl(phaseImpl *impl) {
    delete impl;
}

} // namespace Play2POnce
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::Play2POnce::phaseImpl>(StageSequence::Play2POnce::phaseImpl *x) {
    StageSequence::Play2POnce::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::Play2POnce::phaseImpl, 0x8);
SIZE_ASSERT(StageSequence::Play2POnce::Phase, 0x8);

namespace StageSequence {
namespace Play2POnce {

phaseImpl::phaseImpl() {
    m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(lbl_80584720.m_stageId, 0));
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
    ResetResults();
}

phaseImpl::~phaseImpl() {}

bool phaseImpl::Tick() {
    switch (m_stage->Tick()) {
    case 2:
        return false;
    case 3:
    case 4:
    case 5:
    case 6:
        m_stage.reset();
        m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(lbl_80584720.m_stageId, 0));
        ResetResults();
        return true;
    case 1:
        return true;
    }
    return false;
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

} // namespace Play2POnce
} // namespace StageSequence
