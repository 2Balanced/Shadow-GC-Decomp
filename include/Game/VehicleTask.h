#ifndef GAME_VEHICLETASK_H
#define GAME_VEHICLETASK_H

#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Vehicle {

class TVehicleBase;

/* Vehicle::VehicleTask (class name from RTTI; vtable at 0x8053B860; Game/TVehicleBase.cpp): the
 * task that runs a vehicle (Vehicle::TVehicleBase) every frame and deletes it when it is left alone
 * too long or too far from the cameras. Game/VehicleTable.cpp creates one for each vehicle it
 * creates. Member names are inferred. */
class VehicleTask : public Task {
  public:
    VehicleTask(Task *parent, const boost::shared_ptr<TVehicleBase> &vehicle);
    virtual ~VehicleTask();
    virtual void Tick(float dt);

    /* 0x28 */ boost::shared_ptr<TVehicleBase> m_vehicle;
    /* 0x30 */ float m_range; /* deleted beyond it from the cameras when let go; -1 never */
    /* 0x34 */ u32 pad_0x34;
};
SIZE_ASSERT(VehicleTask, 0x38);

} // namespace Vehicle

#endif
