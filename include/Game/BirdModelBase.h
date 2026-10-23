#ifndef GAME_BIRDMODELBASE_H
#define GAME_BIRDMODELBASE_H

#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The motion table a bird's model is built with (layout in Game/AirBoltModel.cpp; the motion
 * object at 0x8022F058 reads it). */
struct BirdMotionTable;

namespace Vehicle {
namespace BirdBase {

/* Vehicle::BirdBase::BirdModelBase (class name from RTTI; Game/BirdModelBase.cpp): the model and
 * motion of a flying vehicle (Vehicle::AirBoltModel, Vehicle::AirWingModel). Its state lives in an
 * Impl that picks the flying motion from the owner's heading. Member and function names are
 * inferred. */
class BirdModelBase {
  public:
    class Impl;

    BirdModelBase(CharInfo *owner, const BirdMotionTable *table);
    virtual ~BirdModelBase();

    /* Passes arg on to the motion object (0x8022EC24; meaning unknown). */
    void SetMotionArg(void *arg);
    /* Pauses the motion and drops a pending pause request. */
    void Stop();
    /* Asks the motion to pause or resume on the next Update. */
    void RequestPause();
    /* The motion's current time (meaning inferred). */
    float GetMotionTime();
    /* The model's frame number index (meaning inferred). */
    void *GetFrame(int index);
    /* Places the model. */
    void SetPlacement(const Vec3f *position, const Vec3f *rotation);
    /* Switches the flying state (meanings inferred from the motions each one plays). */
    void SetState(int state);
    void Update(float dt);

    /* 0x4 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(BirdModelBase, 0x8);

} // namespace BirdBase
} // namespace Vehicle

#endif
