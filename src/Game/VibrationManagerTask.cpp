/* Vibration::ManagerTask (class name from RTTI; the task's name is "VibrationManager") and the
 * vibration manager (class name unknown; the function-local static behind fn_80177F38, known to
 * Game/StageManager.cpp as UnkVibrationManager_80177F38). The manager keeps a Vibration::ManagerUnit
 * (Game/VibrationManagerUnit.h) for each pad and hands it the requests the rumble functions
 * (Game/VibrationRequest.cpp) make through fn_8034AF24 and fn_8034B0A8, each of them registered
 * with the handle the rumble function returns. File name from the class; all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: every function and all data match, but GC/2.7 lays out three things differently:
 * retail emits the RequestHandle shared_ptr instances (0x8034AD40..0x8034AF24) ahead of
 * fn_8034AF24 instead of after it, emits dispose/get_deleter of the Request control block ahead of
 * the RequestHandle one (0x8034BCB0, 0x8034BD24), and orders the checked_deleter RTTI records in
 * .sdata the same way (Request before RequestHandle). No source order tried changed either. */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_AT
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST
#include <vector>

#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "Game/VibrationManagerUnit.h"
#include "Game/VibrationPattern.h"
#include "Game/VibrationRequest.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using namespace Vibration;

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x7C];
    /* 0x7C */ u8 m_vibration; /* the vibration option (meaning inferred) */
    /* 0x7D */ u8 field_0x7D[0x150 - 0x7D];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The game's pad state (see Game/GamePeripheralTask.cpp). */
struct UnkPads_80051734 {
    /* 0x000 */ u8 field_0x0[0x11C];
    /* 0x11C */ u32 m_playerPads[2];
};

/* The stage's settings (struct name unknown; only the flags at 0x8 are read here). */
struct UnkStageInfo_80176C3C {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ u32 m_flags; /* 4: a 2P stage (meaning inferred) */
};

extern "C" {
UnkPads_80051734 *fn_80051734(void);
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
void *fn_800B74B4(void);
UnkStageInfo_80176C3C *fn_80176C3C(void *table, int stage);
}

namespace Vibration {

/* Vibration::ManagerTask (class name from RTTI): updates the manager every frame. */
class ManagerTask : public Task {
  public:
    ManagerTask(Task *parent);
    virtual ~ManagerTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(ManagerTask, 0x28);

} // namespace Vibration

/* The vibration manager (class name unknown; see Game/StageManager.cpp). */
class UnkVibrationManager_80177F38 {
  public:
    UnkVibrationManager_80177F38();
    ~UnkVibrationManager_80177F38();
    void Update(float dt);
    u32 GetPad(int player);
    boost::shared_ptr<ManagerUnit> &GetUnit(int pad);
    bool IsValidPlayer(int player);
    void AddRequest(float duration, boost::shared_ptr<RequestHandle> handle, int pad, int kind,
                    const Pattern *pattern, int mode, int arg7);
    void AddRequestWithPos(float duration, boost::shared_ptr<RequestHandle> handle, int pad, int kind,
                           const Pattern *pattern, const Vec3f *position, int mode, int arg8);

    /* 0x00 */ s32 m_state;       /* 1 while a stage's players have their pads */
    /* 0x04 */ s32 m_playerCount; /* 1 or 2 */
    /* 0x08 */ ManagerTask *m_task;
    /* 0x0C */ std::vector<boost::shared_ptr<ManagerUnit> > m_units;
};
SIZE_ASSERT(UnkVibrationManager_80177F38, 0x18);

extern "C" UnkVibrationManager_80177F38 *fn_80177F38(void);

const u32 Vibration::kPadCount = 2;

namespace Vibration {

ManagerTask::ManagerTask(Task *parent) : Task(parent) {
    m_name = "VibrationManager";
    m_flags |= 0x100;
}

ManagerTask::~ManagerTask() {}

void ManagerTask::Tick(float dt) {
    fn_80177F38()->Update(dt);
}

} // namespace Vibration

UnkVibrationManager_80177F38::UnkVibrationManager_80177F38() : m_state(0), m_playerCount(1), m_task(0) {
    m_task = new ManagerTask(fn_801EB304(fn_8001C4F8(), 4));
    m_units.reserve(2);
    for (int i = 0; i < 2; i++) {
        m_units.push_back(boost::shared_ptr<ManagerUnit>(new ManagerUnit(i)));
    }
}

UnkVibrationManager_80177F38::~UnkVibrationManager_80177F38() {
    m_units.clear();
    if (m_task != 0) {
        delete m_task;
    }
    m_task = 0;
}

/* Resets the pads' requests (name inferred). */
extern "C" void fn_8034B4F4(UnkVibrationManager_80177F38 *manager) {
    for (std::vector<boost::shared_ptr<ManagerUnit> >::iterator it = manager->m_units.begin();
         it != manager->m_units.end(); ++it) {
        (*it)->Reset();
    }
}

extern "C" void fn_8034B390(UnkVibrationManager_80177F38 *manager);

/* Gives the stage's players their pads (name inferred). */
extern "C" void fn_8034B454(UnkVibrationManager_80177F38 *manager) {
    fn_8034B390(manager);
    if (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 4) {
        manager->m_playerCount = 2;
    } else {
        manager->m_playerCount = 1;
    }
    for (int i = 0; i < manager->m_playerCount; i++) {
        manager->GetUnit(fn_80051734()->m_playerPads[i])->SetPlayer(i);
    }
    manager->m_state = 1;
}

/* Takes the pads back from the players (name inferred). */
extern "C" void fn_8034B3E4(UnkVibrationManager_80177F38 *manager) {
    for (std::vector<boost::shared_ptr<ManagerUnit> >::iterator it = manager->m_units.begin();
         it != manager->m_units.end(); ++it) {
        (*it)->SetPlayer(-1);
    }
    fn_8034B390(manager);
    manager->m_state = 0;
    manager->m_playerCount = 1;
}

/* Stops every pad (name inferred). */
extern "C" void fn_8034B390(UnkVibrationManager_80177F38 *manager) {
    for (std::vector<boost::shared_ptr<ManagerUnit> >::iterator it = manager->m_units.begin();
         it != manager->m_units.end(); ++it) {
        (*it)->Stop();
    }
}

void UnkVibrationManager_80177F38::Update(float dt) {
    for (std::vector<boost::shared_ptr<ManagerUnit> >::iterator it = m_units.begin(); it != m_units.end();
         ++it) {
        (*it)->Update(dt, m_state);
    }
}

u32 UnkVibrationManager_80177F38::GetPad(int player) {
    if (m_state == 1) {
        UnkPads_80051734 *pads = fn_80051734();
        return pads->m_playerPads[player];
    }
    return player;
}

boost::shared_ptr<ManagerUnit> &UnkVibrationManager_80177F38::GetUnit(int pad) {
    return m_units.at(pad);
}

bool UnkVibrationManager_80177F38::IsValidPlayer(int player) {
    if (lbl_8057D6A0.m_vibration) {
        if (player == -2) {
            return true;
        }
        if (player >= 0 && player < m_playerCount) {
            return true;
        }
    }
    return false;
}

/* Plays pattern on the pad of player (-2: every pad) (name inferred). */
extern "C" boost::weak_ptr<RequestHandle> fn_8034B0A8(UnkVibrationManager_80177F38 *manager, int kind,
                                                      const Pattern *pattern, int mode, int arg4, int player,
                                                      float duration) {
    boost::shared_ptr<RequestHandle> handle;
    if (manager->IsValidPlayer(player)) {
        handle = boost::shared_ptr<RequestHandle>(new RequestHandle);
        if (player == -2) {
            for (int i = 0; i < manager->m_playerCount; i++) {
                manager->AddRequest(duration, handle, manager->GetPad(i), kind, pattern, mode, arg4);
            }
        } else {
            manager->AddRequest(duration, handle, manager->GetPad(player), kind, pattern, mode, arg4);
        }
    }
    return handle;
}

/* The same at a position (only while the players have their pads) (name inferred). */
extern "C" boost::weak_ptr<RequestHandle> fn_8034AF24(UnkVibrationManager_80177F38 *manager, int kind,
                                                      const Pattern *pattern, const Vec3f *position, int mode,
                                                      int arg5, int player, float duration) {
    boost::shared_ptr<RequestHandle> handle;
    if (manager->IsValidPlayer(player) && manager->m_state == 1) {
        handle = boost::shared_ptr<RequestHandle>(new RequestHandle);
        if (player == -2) {
            for (int i = 0; i < manager->m_playerCount; i++) {
                manager->AddRequestWithPos(duration, handle, manager->GetPad(i), kind, pattern, position, mode,
                                           arg5);
            }
        } else {
            manager->AddRequestWithPos(duration, handle, manager->GetPad(player), kind, pattern, position, mode,
                                       arg5);
        }
    }
    return handle;
}

void UnkVibrationManager_80177F38::AddRequest(float duration, boost::shared_ptr<RequestHandle> handle, int pad,
                                              int kind, const Pattern *pattern, int mode, int arg7) {
    boost::shared_ptr<Request> request =
        boost::shared_ptr<Request>(new Request(duration, handle, pad, 0, kind, pattern, mode, arg7));
    GetUnit(pad)->Add(request);
    handle->Add(request);
}

void UnkVibrationManager_80177F38::AddRequestWithPos(float duration, boost::shared_ptr<RequestHandle> handle,
                                                     int pad, int kind, const Pattern *pattern,
                                                     const Vec3f *position, int mode, int arg8) {
    boost::shared_ptr<Request> request = boost::shared_ptr<Request>(
        new RequestWithPos(duration, handle, pad, kind, pattern, position, mode, arg8));
    GetUnit(pad)->Add(request);
    handle->Add(request);
}

/* Stops the requests of a handle (name inferred). */
extern "C" void fn_8034A418(UnkVibrationManager_80177F38 *manager, boost::shared_ptr<RequestHandle> &handle) {
    for (std::vector<boost::weak_ptr<Request> >::iterator it = handle->m_requests.begin();
         it != handle->m_requests.end(); ++it) {
        if (!it->expired()) {
            boost::shared_ptr<Request> request = it->lock();
            manager->GetUnit(request->m_pad)->Remove(request);
        }
    }
}

#include <vector_impl>
