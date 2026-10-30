#ifndef GAME_AIRWINGCAMERA_H
#define GAME_AIRWINGCAMERA_H

#include "Game/BirdBase.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

namespace Vehicle {

class AirWingCameraImpl;

namespace BirdBase {

/* The object at BirdBase's 0x100 (class name unknown; Game/VehicleAirWingCamera.cpp): the camera a
 * bird's rider sees through while the bird follows its path, a pimpl wrapper around a
 * Vehicle::AirWingCameraImpl (class name from the RTTI of its bound callbacks), so probably
 * Vehicle::AirWingCamera in the original. Constructor 0x801B3148, destructor 0x801B308C. */
struct UnkBirdTrail_801B3148 {
    UnkBirdTrail_801B3148(TVehicleBase *vehicle, UnkBirdPath_801B446C *path);
    ~UnkBirdTrail_801B3148();

    /* 0x0 */ boost::scoped_ptr<AirWingCameraImpl> m_impl;
};
SIZE_ASSERT(UnkBirdTrail_801B3148, 0x4);

} // namespace BirdBase
} // namespace Vehicle

#endif
