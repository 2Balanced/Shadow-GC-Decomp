/* StageSequence::PlayEvent::Phase (class name from RTTI): the stage sequence phase that plays one
 * stage, the one the stage info names for it (an event stage; meaning inferred), with a
 * StageManager until the StageManager stops running. The file name and the names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/StageManager.h"
#include "Game/StageSequence.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The stages to play (struct name unknown). */
struct UnkStageInfo_80584720 {
    /* 0x000 */ u8 field_0x0[0x8];
    /* 0x008 */ s32 m_eventStageId; /* (name inferred) */
    /* 0x00C */ u8 field_0xC[0x230 - 0xC];
};
SIZE_ASSERT(UnkStageInfo_80584720, 0x230);

extern UnkStageInfo_80584720 lbl_80584720;

namespace StageSequence {
namespace PlayEvent {

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

} // namespace PlayEvent
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::PlayEvent::phaseImpl>(StageSequence::PlayEvent::phaseImpl *x) {
    StageSequence::PlayEvent::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::PlayEvent::phaseImpl, 0x8);
SIZE_ASSERT(StageSequence::PlayEvent::Phase, 0x8);

namespace StageSequence {
namespace PlayEvent {

phaseImpl::phaseImpl()
    : m_stage(boost::shared_ptr<StageBase::Base>(new StageManager(lbl_80584720.m_eventStageId, 0))) {}

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

} // namespace PlayEvent
} // namespace StageSequence
