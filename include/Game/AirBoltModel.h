#ifndef GAME_AIRBOLTMODEL_H
#define GAME_AIRBOLTMODEL_H

#include "Game/BirdModelBase.h"

namespace Vehicle {

/* Vehicle::AirBoltModel (class name from RTTI; Game/AirBoltModel.cpp): the model of
 * Vehicle::BirdBase::AirBolt. */
class AirBoltModel : public BirdBase::BirdModelBase {
  public:
    AirBoltModel(CharInfo *owner);
    virtual ~AirBoltModel();

    /* Loads the model's resources (name inferred; AirBolt's task initializer calls it). */
    static void Load();
};
SIZE_ASSERT(AirBoltModel, 0x8);

} // namespace Vehicle

#endif
