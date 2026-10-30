#ifndef GAME_AIRWINGMODEL_H
#define GAME_AIRWINGMODEL_H

#include "Game/BirdModelBase.h"

namespace Vehicle {

/* Vehicle::AirWingModel (class name from RTTI; Game/AirWingModel.cpp): the model of
 * Vehicle::BirdBase::AirWing. Constructor 0x801B429C. */
class AirWingModel : public BirdBase::BirdModelBase {
  public:
    AirWingModel(CharInfo *owner);
    virtual ~AirWingModel();
    /* Loads the model's resources (0x801B4204; name inferred, as for Vehicle::AirBoltModel). */
    static void Load();
};
SIZE_ASSERT(AirWingModel, 0x8);

} // namespace Vehicle

#endif
