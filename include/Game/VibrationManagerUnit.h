#ifndef GAME_VIBRATIONMANAGERUNIT_H
#define GAME_VIBRATIONMANAGERUNIT_H

#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Vibration {

class Request;
/* The requests and the server of one pad (class name unknown; Game/VibrationManagerUnit.cpp). */
class ManagerUnitImpl;

/* Vibration::ManagerUnit (class name from the RTTI of its shared_ptr control block): the vibration
 * of one pad, which the vibration manager (Game/VibrationManagerTask.cpp) keeps for each pad. Its
 * implementation lives in Game/VibrationManagerUnit.cpp. Member names are inferred. */
class ManagerUnit {
  public:
    ManagerUnit(int pad);
    ~ManagerUnit();
    /* Drops the requests and stops the motor. */
    void Reset();
    /* Plays the requests for dt seconds; mode is the manager's state. */
    void Update(float dt, int mode);
    /* The player the pad belongs to, or -1. */
    void SetPlayer(int player);
    void Add(boost::shared_ptr<Request> request);
    void Remove(boost::shared_ptr<Request> request);
    void Stop();

    /* 0x0 */ boost::scoped_ptr<ManagerUnitImpl> m_impl;
};
SIZE_ASSERT(ManagerUnit, 0x4);

/* How many requests a handle has room for at first: one for each pad (Game/VibrationManagerTask.cpp;
 * name inferred). */
extern const u32 kPadCount;

} // namespace Vibration

#endif
