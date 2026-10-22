/* StageSequence::PlayOnce::Phase (class name from RTTI): the stage sequence phase that plays one
 * stage, the first the stage info names (meaning inferred), with a StageManager until the
 * StageManager stops running. The file name and the names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/StageManager.h"
#include "Game/StageSequence.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x148];
    /* 0x148 */ u8 field_0x148[3]; /* meaning unknown */
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages to play (struct name unknown). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ s32 m_stageId; /* (name inferred) */
    /* 0x004 */ u8 field_0x4[0x230 - 0x4];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

extern "C" {
/* Sets count bytes at dest to value (behaviour inferred). */
void fn_800CBBFC(void *dest, int value, int count);
}

namespace StageSequence {
namespace PlayOnce {

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

} // namespace PlayOnce
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::PlayOnce::phaseImpl>(StageSequence::PlayOnce::phaseImpl *x) {
    StageSequence::PlayOnce::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::PlayOnce::phaseImpl, 0x8);
SIZE_ASSERT(StageSequence::PlayOnce::Phase, 0x8);

namespace StageSequence {
namespace PlayOnce {

phaseImpl::phaseImpl() : m_stage(boost::shared_ptr<StageBase::Base>(new StageManager(lbl_80584720.m_stageId, 0))) {
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
}

phaseImpl::~phaseImpl() {}

bool phaseImpl::Tick() {
    return (u8)(m_stage->Tick() == 1);
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

} // namespace PlayOnce
} // namespace StageSequence
