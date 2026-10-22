/* StageSequence::Play2PThree::Phase (class name from RTTI): the stage sequence phase that plays a
 * two-player match of three stages. Its phaseImpl draws the three stages' order at random (after
 * the first match, the stage of the last round is not drawn first), plays them one after the
 * other with a StageManager and draws a new order when the match ends. The file name and the
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/Random.h"
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
    /* 0x010 */ s32 m_matchStages[3]; /* a three-stage match's stages (name inferred) */
    /* 0x01C */ u8 field_0x1C[0x230 - 0x1C];
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

namespace StageSequence {
namespace Play2PThree {

class phaseImpl {
  public:
    phaseImpl();
    ~phaseImpl();

    /* Runs the phase; false once it ended. */
    bool Tick();
    /* Ends the stage early (name inferred). */
    void Skip();
    /* Draws the stages' order and starts the first (name inferred). */
    void StartMatch();

    /* 0x00 */ boost::shared_ptr<StageBase::Base> m_stage;
    /* 0x08 */ s32 m_round; /* the stages played so far */
    /* 0x0C */ s32 m_order[3]; /* indexes into UnkStageInfo_80584720::m_matchStages */
    /* 0x18 */ u8 m_firstMatch;
};

/* Deletes the phaseImpl (name inferred; see checked_delete below). */
inline void DeleteImpl(phaseImpl *impl) {
    delete impl;
}

} // namespace Play2PThree
} // namespace StageSequence

/* Retail keeps boost::checked_delete<phaseImpl> out of line and the destructor at its own place;
 * this specialization gives that order under GC/2.7 (see docs/research-notes.md and
 * Game/EnergyCore.cpp). */
namespace boost {
template <>
inline void checked_delete<StageSequence::Play2PThree::phaseImpl>(StageSequence::Play2PThree::phaseImpl *x) {
    StageSequence::Play2PThree::DeleteImpl(x);
}
} // namespace boost

SIZE_ASSERT(StageSequence::Play2PThree::phaseImpl, 0x1C);
SIZE_ASSERT(StageSequence::Play2PThree::Phase, 0x8);

namespace StageSequence {
namespace Play2PThree {

phaseImpl::phaseImpl() {
    m_firstMatch = 1;
    StartMatch();
}

phaseImpl::~phaseImpl() {}

bool phaseImpl::Tick() {
    switch (m_stage->Tick()) {
    case 2:
        return false;
    case 6:
        StartMatch();
        return true;
    case 3:
    case 4:
    case 5:
        if (m_round < 3) {
            m_stage.reset();
            m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(lbl_80584720.m_matchStages[m_order[m_round]], 0));
            fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
            m_round++;
        } else {
            StartMatch();
        }
        return true;
    case 1:
        return true;
    }
    return false;
}

void phaseImpl::Skip() {
    m_stage->vf_0x10();
}

void phaseImpl::StartMatch() {
    u8 unused[3];
    for (int i = 0; i < 3; i++) {
        unused[i] = 1;
    }
    for (int i = 0; i < 3; i++) {
        int stage;
        int pick;
        int n;
        if (i == 0 && !m_firstMatch) {
            /* Not the stage of the round the last match ended in (meaning inferred). */
            pick = 2.0f * fn_8040EE90();
            for (stage = 0, n = 0; stage < 3; stage++) {
                if (unused[stage] && stage != m_order[lbl_8057D6A0.field_0x130 - 1]) {
                    if (n == pick) {
                        break;
                    }
                    n++;
                }
            }
        } else {
            pick = (3 - i) * fn_8040EE90();
            for (stage = 0, n = 0; stage < 3; stage++) {
                if (unused[stage]) {
                    if (n == pick) {
                        break;
                    }
                    n++;
                }
            }
        }
        m_order[i] = stage;
        unused[stage] = 0;
    }
    m_stage.reset();
    m_stage = boost::shared_ptr<StageBase::Base>(new StageManager(lbl_80584720.m_matchStages[m_order[0]], 0));
    fn_800CBBFC(lbl_8057D6A0.field_0x148, 1, 3);
    m_round = 1;
    lbl_8057D6A0.field_0x12C = 3;
    lbl_8057D6A0.field_0x130 = 0;
    lbl_8057D6A0.field_0x138 = 0;
    lbl_8057D6A0.field_0x134 = 0;
    lbl_8057D6A0.field_0x144 = -2;
    m_firstMatch = 0;
}

Phase::Phase() : m_impl(new phaseImpl) {}

Phase::~Phase() {}

bool Phase::Tick() {
    return m_impl->Tick();
}

void Phase::Skip() {
    m_impl->Skip();
}

} // namespace Play2PThree
} // namespace StageSequence
