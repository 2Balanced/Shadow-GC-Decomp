/* SaveCycle::CheckModule::Phase (class name from RTTI): the phase of the save/load cycle that
 * checks for a memory card in slot A. It probes the slot, mounts the card and searches it for the
 * game's file, then reports the outcome through the cycle's result (see Game/SaveCycle.cpp) and
 * whether the card can be used. Its state lives in a CheckModule::Impl (class name inferred). The
 * file name, the Impl's name and all member names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <boost/scoped_ptr.hpp>

#include "Game/MemoryCardCtrl.h"
#include "Game/SaveCycle.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* CARDProbeEx (Dolphin SDK; the symbol is still a placeholder). */
s32 fn_8038FF84(s32 chan, s32 *memSize, s32 *sectorSize);
/* The memory card controller (Game/MemoryCardCtrl.cpp): starting a search, whether a request is
 * running, the card's status, the last request's result, whether the card has room for the file
 * and the number of files the search found. */
bool fn_80299A2C(TMemoryCardCtrl *ctrl);
int fn_80298EBC(TMemoryCardCtrl *ctrl);
int fn_80298E5C(TMemoryCardCtrl *ctrl);
int fn_80298EB4(TMemoryCardCtrl *ctrl);
bool fn_80298E88(TMemoryCardCtrl *ctrl);
int fn_80299950(TMemoryCardCtrl *ctrl);
}

namespace SaveCycle {
namespace CheckModule {

/* The phase's state (class name inferred). */
class Impl {
  public:
    Impl();
    ~Impl() {}

    bool Update();

    /* 0x0 */ s32 m_state; /* 0 probe, 1 mount, 2 wait for the card, 3 search, 4 done, 5 failed */
    /* 0x4 */ s32 m_probe; /* the slot's CARDProbeEx result */
};
SIZE_ASSERT(Impl, 0x8);

bool Impl::Update() {
    TMemoryCardCtrl *ctrl = fn_801E5B80()->m_ctrl;
    if (ctrl == 0) {
        return false;
    }
    switch (m_state) {
    case 0:
        m_probe = fn_8038FF84(0, 0, 0);
        m_state = 1;
    case 1:
        switch (m_probe) {
        case 0:
            if (fn_80299A2C(ctrl)) {
                m_state = 2;
            }
            break;
        case -3:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(1);
            break;
        case -2:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(5);
            break;
        case -128:
        default:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(10);
            break;
        }
        break;
    case 2:
        if (fn_80298EBC(ctrl) != 0) {
            break;
        }
        if (fn_80298E5C(ctrl) == 3) {
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(1);
            break;
        }
        m_state = 3;
    case 3:
        switch (fn_80298EB4(ctrl)) {
        case 3:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(1);
            break;
        case 4:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(5);
            break;
        default:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(6);
            break;
        case 6:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(3);
            break;
        case 11:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(7);
            break;
        case 8:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(8);
            break;
        case 7:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(4);
            break;
        case 9:
            m_state = 5;
            if (fn_80298E88(ctrl)) {
                UnkMemoryCard_802E472C::SetResult(2);
            } else {
                UnkMemoryCard_802E472C::SetResult(4);
            }
            break;
        case 0:
        case 10:
            if (fn_80299950(ctrl) > 0) {
                m_state = 4;
            } else {
                m_state = 5;
                if (fn_80298E88(ctrl)) {
                    UnkMemoryCard_802E472C::SetResult(2);
                } else {
                    UnkMemoryCard_802E472C::SetResult(4);
                }
            }
            break;
        case 12:
            m_state = 5;
            UnkMemoryCard_802E472C::SetResult(9);
            break;
        }
        break;
    case 4:
        UnkMemoryCard_802E472C::SetCardUsable(1);
        UnkMemoryCard_802E472C::SetResult(0);
        return false;
    case 5:
        UnkMemoryCard_802E472C::SetCardUsable(0);
        return false;
    }
    return true;
}

Impl::Impl() : m_state(0), m_probe(0) {}

Phase::Phase() : CycleBase(PHASE_CHECK_MODULE), m_impl(new Impl) {}

Phase::~Phase() {}

bool Phase::Update() {
    return m_impl->Update();
}

} // namespace CheckModule
} // namespace SaveCycle
