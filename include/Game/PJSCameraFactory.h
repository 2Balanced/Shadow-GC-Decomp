#ifndef GAME_PJSCAMERAFACTORY_H
#define GAME_PJSCAMERAFACTORY_H

/* Units that include this header define MSL_LIST_ACCESSORS before it (see <list>). */
#include "Game/CameraManagerUnit.h"
#include "size_assert.h"

namespace PJSCamera {

/* PJSCameraFactory (class name and base from RTTI; vtable 0x8053C4AC; Game/PJSCameraFactory.cpp):
 * the game's camera factory. Its constructor registers the game's camera modes and adjusts with
 * their names; its targets are PJSCameraTargetInfo. The PJS camera tasks' object makes one and
 * hands it to the camera manager (0x801D0A78). */
class PJSCameraFactory : public SonicteamUSA::System::Camera::CameraFactory {
  public:
    PJSCameraFactory();
    virtual SonicteamUSA::System::Camera::CameraTargetInfo *CreateTarget();
    virtual ~PJSCameraFactory();
};
SIZE_ASSERT(PJSCameraFactory, 0x24);

} // namespace PJSCamera

#endif
