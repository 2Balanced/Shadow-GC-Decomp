/* StageSequence::Manager (class name from RTTI; see Game/StageSequence.h): the GameSystem that
 * runs the stage sequence. Initialize creates the phase that SetNext chose and records its
 * number as the current sequence; IsFinished runs the phase until it ends. The file name and the
 * names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/StageSequence.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xCC];
    /* 0x0CC */ s32 m_sequence; /* the stage sequence playing (name inferred) */
    /* 0x0D0 */ u8 field_0xD0[0x150 - 0xD0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* Called when the sequence ends (behaviour unknown); fn_801E5840 takes fn_801E2128's result. */
void *fn_801E2128(void);
void fn_801E5840(void *arg0);
}

SIZE_ASSERT(StageSequence::Manager, 0x10);

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for StageSequenceBase (boost 1.32's definition, as in Coaster.cpp). */
template <class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

namespace StageSequence {

/* The sequence the next Initialize starts, and the one playing (names inferred). */
static s32 s_next = -1;
static s32 s_current = -1;

Manager::Manager() : m_sequence(-1) {}

Manager::~Manager() {}

void Manager::SetNext(s32 sequence) {
    s_next = sequence;
}

s32 Manager::GetCurrent() {
    return s_current;
}

bool Manager::Initialize() {
    switch (s_next) {
    case -1:
        m_phase.reset();
        break;
    case 0:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new PlayOnce::Phase);
        break;
    case 1:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new PlayStory::Phase);
        break;
    case 2:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new PlayEvent::Phase);
        break;
    case 3:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new Play2POnce::Phase);
        break;
    case 4:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new Play2PThree::Phase);
        break;
    case 5:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new MovieTest::Phase);
        break;
    case 6:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new PlayHard::Phase);
        break;
    case 7:
        m_phase.reset();
        m_sequence = s_next;
        s_current = m_sequence;
        lbl_8057D6A0.m_sequence = m_sequence;
        m_phase = boost::shared_ptr<StageSequenceBase>(new PlayFlashback::Phase);
        break;
    }
    return true;
}

bool Manager::IsFinished() {
    if (m_phase && m_phase->Tick()) {
        return false;
    }
    return true;
}

bool Manager::Finalize() {
    s_next = -1;
    fn_801E5840(fn_801E2128());
    return true;
}

void Manager::Abort() {
    if (m_phase) {
        m_phase->Skip();
    }
}

} // namespace StageSequence
