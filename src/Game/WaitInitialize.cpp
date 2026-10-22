/* WaitInitialize (class name from RTTI): the GameSystem that runs while the game starts up. It
 * waits half a second (30 frames) and then hands the system manager over to the system the
 * manager has queued (fn_801E5850 and fn_801E5858; behaviour inferred). The unit also holds two
 * helpers over the players' missions that other units call. File name from the class; names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/GameSystem.h"
#include "Game/Mission.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class UnkPlayerManager_8006D6A8;
class UnkSystemManager_801E2128;

extern "C" {
/* The player manager and a player's mission (see CityBombSmall.cpp). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* The system manager, the system it has queued, and switching to a system (behaviours
 * inferred). */
UnkSystemManager_801E2128 *fn_801E2128(void);
GameSystem *fn_801E5850(UnkSystemManager_801E2128 *manager, int index);
void fn_801E5858(UnkSystemManager_801E2128 *manager, GameSystem *system);
}

class WaitInitialize : public GameSystem {
  public:
    WaitInitialize();
    virtual ~WaitInitialize();
    virtual bool Initialize();
    virtual bool IsFinished();
    virtual bool Finalize();
    virtual void Abort();

    /* 0x4 */ s32 m_frames; /* frames waited (name inferred) */
};
SIZE_ASSERT(WaitInitialize, 0x8);

/* What is left of player index's mission of type 3: its target minus its count, or -1 (name
 * inferred). */
extern "C" int fn_8020D2A8(int index) {
    boost::shared_ptr<Mission::Mission> mission = fn_800740E4(fn_8006D6A8(), index).lock();
    if (mission.get() != 0 && mission->GetType() == 3) {
        return mission->GetTarget() - mission->GetCount(0);
    }
    return -1;
}

/* A bit per player whose mission is of type 3 (name inferred). */
extern "C" u32 fn_8020D1E8(void) {
    UnkPlayerManager_8006D6A8 *manager = fn_8006D6A8();
    u32 mask = 0;
    for (int i = 0; i < 3; i++) {
        boost::shared_ptr<Mission::Mission> mission = fn_800740E4(manager, i).lock();
        if (mission.get() != 0 && mission->GetType() == 3) {
            mask |= 1 << i;
        }
    }
    return mask;
}

WaitInitialize::WaitInitialize() : m_frames(0) {}

WaitInitialize::~WaitInitialize() {}

bool WaitInitialize::Initialize() {
    return true;
}

bool WaitInitialize::IsFinished() {
    m_frames++;
    if (m_frames > 30) {
        GameSystem *next = fn_801E5850(fn_801E2128(), 0);
        fn_801E5858(fn_801E2128(), next);
        return true;
    }
    return false;
}

bool WaitInitialize::Finalize() {
    return true;
}

void WaitInitialize::Abort() {}
